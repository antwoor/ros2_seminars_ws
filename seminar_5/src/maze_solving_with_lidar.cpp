#include <geometry_msgs/msg/twist.hpp>      // Сообщение для задания линейной и угловой скорости
#include <memory>
#include <rclcpp/rclcpp.hpp>                // Базовый API ROS 2 (C++)
#include <sensor_msgs/msg/laser_scan.hpp>   // Сообщение лидара (LaserScan)

// Конечный автомат состояний робота
// Робот всегда находится в одном из этих состояний
enum class RobotState {
  MOVING_STRAIGHT,   // Движение вперёд
  TURNING_LEFT,      // Поворот влево
  TURNING_RIGHT,     // Поворот вправо
  OUT_OF_MAZE        // Выход из лабиринта (остановка)
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

    // Условие "выхода из лабиринта":
    // если со всех сторон достаточно свободного пространства,
    // считаем, что лабиринт пройден
    if (frontObstacle > frontThreshold_ &&
        rightObstacle > 1.2 &&
        leftObstacle > 1.2) {
      state_ = RobotState::OUT_OF_MAZE;
    }

    geometry_msgs::msg::Twist command;  // Сообщение для управления скоростью

    // Логика выбора состояния (реактивное управление):
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

    // Реализация поведения в зависимости от текущего состояния
    switch (state_) {

    case RobotState::MOVING_STRAIGHT:
      command.linear.x = linearVel_;  // Линейная скорость вперёд (м/с)
      command.angular.z = 0.0;        // Без вращения
      RCLCPP_INFO(this->get_logger(), "Moving STRAIGHT");
      break;

    case RobotState::TURNING_LEFT:
      command.linear.x = 0.0;         // Останавливаем поступательное движение
      command.angular.z = angularVel_; // Положительная угловая скорость (рад/с)
      RCLCPP_INFO(this->get_logger(), "Moving LEFT");
      break;

    case RobotState::TURNING_RIGHT:
      command.linear.x = 0.0;
      command.angular.z = -angularVel_; // Отрицательная угловая скорость
      RCLCPP_INFO(this->get_logger(), "Moving RIGHT");
      break;

    case RobotState::OUT_OF_MAZE:
      command.linear.x = 0.0;         // Полная остановка
      command.angular.z = 0.0;
      RCLCPP_INFO(this->get_logger(), "OUT OF MAZE");
      break;
    }

    // Публикация команды движения
    publisher_->publish(command);
  }

  // ================= НАСТРАИВАЕМЫЕ ПАРАМЕТРЫ =================
  // Эти коэффициенты необходимо подобрать для устойчивого
  // прохождения лабиринта.

  float frontThreshold_ = ...f;  // 1.0-2.0f
                                 // Порог расстояния спереди (м).
                                 // Если расстояние меньше — считаем,
                                 // что впереди препятствие.

  float rightThreshold_ = ...f;  // 0.5-1.5f
                                 // Порог расстояния справа (м).
                                 // Влияет на принятие решения о повороте направо.

  float leftThreshold_ = ...f;   // 0.5-1.5f
                                 // Порог расстояния слева (м).
                                 // (В текущей логике почти не используется,
                                 // но может влиять при модификации алгоритма.)

  float angularVel_ = ...f;      // 0.1-1.0f
                                 // Угловая скорость поворота (рад/с).
                                 // Слишком большая → резкие развороты.
                                 // Слишком маленькая → робот "застревает".

  float linearVel_ = ...f;       // 0.1-1.0f
                                 // Линейная скорость (м/с).
                                 // Слишком большая → столкновения.
                                 // Слишком маленькая → медленное прохождение.

  RobotState state_;  // Текущее состояние конечного автомата

  // ROS 2 интерфейсы
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscription_;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);                       // Инициализация ROS 2
  rclcpp::spin(std::make_shared<MazeSolving>());  // Запуск узла
  rclcpp::shutdown();                            // Завершение работы
  return 0;
}