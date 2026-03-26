#include "line_follower.h"

LineFollower::LineFollower() : Node("LineFollowingNode") {
    this->declare_parameter<std::string>("camera_topic", "/camera/image_raw");
    std::string camera_topic = this->get_parameter("camera_topic").as_string();
    
    // ================= НАСТРАИВАЕМЫЕ ПАРАМЕТРЫ =================
    // Данные коэффициенты необходимо подобрать для устойчивого следования по линии.
    
    this->declare_parameter<int>("kP", 50);                 // 1–10
                                     // Коэффициент пропорциональной составляющей ПИД-регулятора.
                                     // Слишком большой (>12) → робот "болтается" из стороны в сторону.
                                     // Слишком маленький (<1) → вялая реакция, съезд с линии на поворотах.
    
    this->declare_parameter<int>("kI", 50);                 // 1–10
                                     // Коэффициент интегральной составляющей ПИД-регулятора.
                                     // Слишком большой (>12) → накопление ошибки, перерегулирование.
                                     // Слишком маленький (<3) → медленное возвращение на линию после отклонения.
    
    this->declare_parameter<int>("lower_threshold", 300);   // 50–60
                                     // Нижний порог фильтра Кэнни (Canny edge detection).
                                     // Слишком низкий (<45) → много шума, ложные срабатывания.
                                     // Слишком высокий (>65) → пропуск краёв линии, особенно на поворотах.
    
    this->declare_parameter<int>("upper_threshold", 500);  // 90–100
                                     // Верхний порог фильтра Кэнни.
                                     // Слишком низкий (<85) → обрывы линии на изгибах.
                                     // Слишком высокий (>110) → захват посторонних объектов и теней.
    // ===========================================================
    
    publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
    subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
      camera_topic, 10,
      std::bind(&LineFollower::cameraCallback, this, std::placeholders::_1));
    RCLCPP_INFO(this->get_logger(), "\n------ Узел следования по линии запущен -----\n");
}

void LineFollower::cameraCallback(const sensor_msgs::msg::Image::SharedPtr cameraMsg){
    
    auto velocityMsg = geometry_msgs::msg::Twist();
    static cv_bridge::CvImagePtr cvPtr; // Указатель на изображение OpenCV
    cvPtr = cv_bridge::toCvCopy(cameraMsg, "bgr8"); // Преобразуем ROS-сообщение в изображение OpenCV
    cv::Mat grayImage, cannyImage; // Матрицы для хранения изображений в оттенках серого и после фильтра Кэнни
    cv::cvtColor(cvPtr->image, grayImage, cv::COLOR_BGR2GRAY); // Преобразуем в оттенки серого

    // Получаем параметры для фильтра Кэнни из настроек узла
    static int lowerThreshold = this->get_parameter("lower_threshold").as_int();
    static int upperThreshold = this->get_parameter("upper_threshold").as_int();
    double p = (float)this->get_parameter("kP").as_int();
    double i = (float)this->get_parameter("kI").as_int();
    p *= 0.01;  // Масштабируем коэффициенты для удобства настройки
    i *= 0.0001; // Масштабируем интегральную составляющую

    // Применяем фильтр Кэнни для обнаружения краёв линии
    cv::Canny(grayImage, cannyImage, lowerThreshold, upperThreshold);
    
    // Извлекаем область интереса (ROI) — прямоугольник, где ожидается линия
    // row_ и column_ задают координаты левого верхнего угла ROI (настраиваются в line_follower.h)
    cv::Mat roi = cannyImage(cv::Range(row_, row_ + 200), cv::Range(column_, column_ + 500));

    // Сканируем ROI для поиска белых пикселей (краёв линии)
    static std::vector<int> edges; // Вектор для хранения координат обнаруженных краёв
    for (int i = 0; i < 500; ++i) {
        // Сканируем горизонтальную линию на высоте Y=160 внутри ROI (относительная координата)
        if (roi.at<uchar>(160, i) == 255) {
            edges.push_back(i); // Сохраняем координату белого пикселя
        }
    }

    static int midArea;
    static int midPoint;
    static int robotMidPoint;
    
    // ===== ГИСТЕРЕЗИС ПРИ ПОТЕРЕ ЛИНИИ =====
    // Защита от ложных остановок на резких поворотах:
    // робот останавливается ТОЛЬКО после последовательных кадров без линии
    static int lineLostCounter = 0;
    const int LINE_LOST_THRESHOLD = 60;  // 15 кадров ≈ 0.5 сек при частоте 30 Гц
    
    if (!edges.empty()) {
        lineLostCounter = 0;  // Сбрасываем счётчик при обнаружении линии
        
        // Вычисляем центр обнаруженного отрезка линии
        midArea = edges.back() - edges.front();
        midPoint = edges.front() + midArea / 2;
        robotMidPoint = 500 / 2;  // Центр ROI (250 пикселей)

        // Вычисляем ошибку: разница между центром робота и центром линии
        // Положительная ошибка → линия слева от центра → поворот налево (положительная угловая скорость)
        // Отрицательная ошибка → линия справа от центра → поворот направо (отрицательная угловая скорость)
        double error = robotMidPoint - midPoint;
        integral += error * dt_;
        
        // Устанавливаем скорость робота
        velocityMsg.linear.x = 0.15;  // Линейная скорость (м/с)
        velocityMsg.angular.z = angularVel_ * (p * error + i * integral);
        
        publisher_->publish(velocityMsg);
        
        RCLCPP_INFO(this->get_logger(), "→ СЛЕДОВАНИЕ | Ошибка: %.1f пикс | Угл.скор: %.3f рад/с", 
                    error, velocityMsg.angular.z);
    }
    else {
        lineLostCounter++;
        double error = robotMidPoint - midPoint;
        
        if (lineLostCounter >= LINE_LOST_THRESHOLD) {
            // Подтверждённая потеря линии → останавливаем робота
            velocityMsg.linear.x = 0.0;
            velocityMsg.angular.z = 0.0;
            publisher_->publish(velocityMsg);
            
            RCLCPP_INFO(this->get_logger(), "⏹ ЛИНИЯ ПОТЕРЯНА (после %d кадров) → ОСТАНОВКА", 
                        lineLostCounter);
        }
        else {
            // Временная потеря линии (например, на резком повороте) → продолжаем движение прямо
            velocityMsg.linear.x = 0.15;
            velocityMsg.angular.z = angularVel_ * (p * error + i * integral);
            publisher_->publish(velocityMsg);
            
            RCLCPP_INFO(this->get_logger(), "⚠️  Линия временно потеряна (%d/%d кадров)", 
                        lineLostCounter, LINE_LOST_THRESHOLD);
        }
    }

    // Визуализация: рисуем кружки на центре обнаруженной линии и центре робота
    if (!edges.empty()) {
        cv::circle(roi, cv::Point(midPoint, 160), 2, cv::Scalar(255, 255, 255), -1);
        cv::circle(roi, cv::Point(robotMidPoint, 160), 5, cv::Scalar(255, 255, 255), -1);
    }
    cv::imshow("Следование по линии", roi);
    cv::waitKey(1);
    
    // Очищаем вектор после обработки, чтобы предотвратить утечку памяти
    edges.clear();
}