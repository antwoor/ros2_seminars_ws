#include <geometry_msgs/msg/twist.hpp>      // Сообщение для задания линейной и угловой скорости
#include <memory>
#include <rclcpp/rclcpp.hpp>                // Базовый API ROS 2 (C++)
#include <sensor_msgs/msg/laser_scan.hpp>   // Сообщение лидара (LaserScan)
#include <algorithm>                        // Для std::min_element

// Конечный автомат состояний робота
// Робот всегда находится в одном из этих состояний
enum class RobotState {
  MOVING_STRAIGHT,   // Движение вперёд
  TURNING_LEFT,      // Поворот влево
  TURNING_RIGHT,     // Поворот вправо
  TURNING_AROUND,    // Разворот на 180° (новое состояние)
  RETURNING_TO_MAZE, // Возврат в лабиринт после разворота (новое состояние)
  OUT_OF_MAZE        // Выход из лабиринта (остановка) — больше не используется активно
};

class MazeSolving : public rclcpp::Node {
public:
  // Конструктор узла
  MazeSolving() : Node("maze_solver"), state_(RobotState::MOVING_STRAIGHT) {

    // Publisher: отправляет команды скорости в топик /cmd_vel
    // Именно эти команды управляют движением робота
    publisher_ =
        this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);

    // Subscriber: подписка на данные лидара из топика /scan
    // При получении нового скана вызывается функция lidarCallback
    subscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
        "/scan", 10,
        std::bind(&MazeSolving::lidarCallback, this, std::placeholders::_1));
  }

private:
  // Callback-функция, вызывается при получении каждого нового сообщения LaserScan
  void lidarCallback(const sensor_msgs::msg::LaserScan::SharedPtr lidarMsg) {

    // Выбираем минимальное расстояние в определённых секторах лидара.
    // Индексы соответствуют угловым диапазонам:
    //  - справа
    //  - спереди
    //  - слева
    // Мы берём минимум, чтобы учитывать ближайшее препятствие в секторе.
    float rightObstacle = *std::min_element(
        lidarMsg->ranges.begin() + 260,
        lidarMsg->ranges.begin() + 280);

    float frontObstacle = *std::min_element(
        lidarMsg->ranges.begin() + 340,
        lidarMsg->ranges.begin() + 360);

    float leftObstacle = *std::min_element(
        lidarMsg->ranges.begin() + 80,
        lidarMsg->ranges.begin() + 100);

    // Вывод текущих расстояний в консоль (для отладки)
    RCLCPP_INFO(this->get_logger(), "Front: %f, Right: %f, Left: %f",
                frontObstacle, rightObstacle, leftObstacle);

    // === ЗАЩИТА: пропускаем логику выбора направления во время разворота/возврата ===
    if (state_ != RobotState::TURNING_AROUND && 
        state_ != RobotState::RETURNING_TO_MAZE) {

      // === ВАША ИСХОДНАЯ ЛОГИКА ВЫБОРА НАПРАВЛЕНИЯ (БЕЗ ИЗМЕНЕНИЙ!) ===
      // 1) Если спереди свободно — едем прямо
      // 2) Если справа больше пространства — поворачиваем направо
      // 3) Если слева больше пространства — поворачиваем налево
      // 4) Иначе едем прямо
      if ((frontObstacle > leftObstacle || frontObstacle > rightObstacle) &&
          frontObstacle > frontThreshold_)
      {
        state_ = RobotState::MOVING_STRAIGHT;
      }
      else if (rightObstacle > frontObstacle &&
               rightObstacle > rightThreshold_)
      {
        state_ = RobotState::TURNING_RIGHT;
      }
      else if (leftObstacle > frontObstacle &&
               leftObstacle > leftThreshold_)
      {
        state_ = RobotState::TURNING_LEFT;
      }
      else
      {
        state_ = RobotState::MOVING_STRAIGHT;
      }
    }

    // === ОБНАРУЖЕНИЕ ВЫХОДА ИЗ ЛАБИРИНТА ===
    // Срабатывает ТОЛЬКО если робот в состоянии движения (не разворачивается/не возвращается)
    if ((state_ == RobotState::MOVING_STRAIGHT || 
         state_ == RobotState::TURNING_LEFT || 
         state_ == RobotState::TURNING_RIGHT) &&
        frontObstacle > frontThreshold_ &&
        rightObstacle > 1.5 &&
        leftObstacle > 1.5) {
      
      state_ = RobotState::TURNING_AROUND;  // Начинаем разворот вместо остановки
      turnStartTime_ = this->now();
    }

    geometry_msgs::msg::Twist command;  // Сообщение для управления скоростью

    // Реализация поведения в зависимости от текущего состояния
    switch (state_) {

    case RobotState::MOVING_STRAIGHT: {
      command.linear.x = linearVel_;  // Линейная скорость вперёд (м/с)
      command.angular.z = 0.0;        // Без вращения
      RCLCPP_INFO(this->get_logger(), "Moving STRAIGHT");
      break;
    }

    case RobotState::TURNING_LEFT: {
      command.linear.x = 0.0;         // Останавливаем поступательное движение
      command.angular.z = angularVel_; // Положительная угловая скорость (рад/с)
      RCLCPP_INFO(this->get_logger(), "Moving LEFT");
      break;
    }

    case RobotState::TURNING_RIGHT: {
      command.linear.x = 0.0;
      command.angular.z = -angularVel_; // Отрицательная угловая скорость
      RCLCPP_INFO(this->get_logger(), "Moving RIGHT");
      break;
    }

    case RobotState::TURNING_AROUND: {
      command.linear.x = 0.0;
      command.angular.z = -angularVel_; // Вращаемся по часовой стрелки
      
      // Время разворота на 180° (π радиан) с запасом 10%
      float turnDuration = 3.14159f / angularVel_ * 1.1f;
      
      if ((this->now() - turnStartTime_).seconds() > turnDuration) {
        state_ = RobotState::RETURNING_TO_MAZE;
        turnStartTime_ = this->now();
      }
      RCLCPP_INFO(this->get_logger(), "TURNING AROUND");
      break;
    }

    case RobotState::RETURNING_TO_MAZE: {
      command.linear.x = linearVel_;  // Едем вперёд в лабиринт
      command.angular.z = 0.0;
      
      // Время возврата в лабиринт
      float returnDuration = 2.0f;
      
      if ((this->now() - turnStartTime_).seconds() > returnDuration) {
        state_ = RobotState::MOVING_STRAIGHT;  // Возвращаемся к обычной логике
      }
      RCLCPP_INFO(this->get_logger(), "RETURNING TO MAZE");
      break;
    }

    case RobotState::OUT_OF_MAZE: {
      // Это состояние больше не используется активно,
      // но оставлено для обратной совместимости
      command.linear.x = 0.0;
      command.angular.z = 0.0;
      RCLCPP_INFO(this->get_logger(), "OUT OF MAZE (idle)");
      break;
    }
    }

    // Публикация команды движения
    publisher_->publish(command);
  }

  // ================= НАСТРОЕННЫЕ ПАРАМЕТРЫ =================
  float frontThreshold_ = 1.5f;  // Порог расстояния спереди (м)

  float rightThreshold_ = 1.5f;  // Порог расстояния справа (м)

  float leftThreshold_ = 1.5f;   // Порог расстояния слева (м)

  float angularVel_ = 0.4f;      // Угловая скорость поворота (рад/с)

  float linearVel_ = 0.8f;       // Линейная скорость (м/с)

  RobotState state_;             // Текущее состояние конечного автомата
  rclcpp::Time turnStartTime_;   // Время начала разворота/возврата

  // ROS 2 интерфейсы
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscription_;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);                       // Инициализация ROS 2
  rclcpp::spin(std::make_shared<MazeSolving>());  // Запуск узла
  rclcpp::shutdown();                             // Завершение работы
  return 0;
}