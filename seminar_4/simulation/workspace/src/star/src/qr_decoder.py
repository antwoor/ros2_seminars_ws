#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2
from pyzbar.pyzbar import decode
import sys

class QRCodeReader(Node):
    def __init__(self):
        super().__init__('qr_code_reader')
        
        # Объявляем параметр для имени топика (по умолчанию /camera/image_raw)
        self.declare_parameter('camera_topic', '/camera/image_raw')
        camera_topic = self.get_parameter('camera_topic').get_parameter_value().string_value
        
        self.bridge = CvBridge()
        self.subscription = self.create_subscription(
            Image,
            camera_topic,
            self.image_callback,
            10  # глубина очереди
        )
        self.subscription  # предотвращаем удаление подписки сборщиком мусора
        self.get_logger().info(f'Подписка на топик: {camera_topic}')

    def image_callback(self, msg):
        try:
            # Преобразуем ROS Image в OpenCV (bgr8 по умолчанию)
            cv_image = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
        except Exception as e:
            self.get_logger().error(f'Ошибка преобразования изображения: {e}')
            return

        # Преобразуем в оттенки серого для улучшения детекции QR
        gray = cv2.cvtColor(cv_image, cv2.COLOR_BGR2GRAY)

        # Ищем QR-коды
        qr_codes = decode(gray)

        for qr in qr_codes:
            # Извлекаем данные (может быть bytes)
            data = qr.data.decode('utf-8', errors='ignore')
            self.get_logger().info(f'Найден QR-код: {data}')

            # Опционально: рисуем рамку вокруг QR (только для отладки)
            # Можно раскомментировать, если нужно визуализировать
            # pts = [qr.polygon[i] for i in range(len(qr.polygon))]
            # cv2.polylines(cv_image, [np.array(pts, dtype=np.int32)], True, (0,255,0), 3)
            # cv2.imshow('QR', cv_image)
            # cv2.waitKey(1)

def main(args=None):
    rclpy.init(args=args)
    node = QRCodeReader()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.get_logger().info('Завершение работы...')
    finally:
        node.destroy_node()
        rclpy.shutdown()
        cv2.destroyAllWindows()  # если использовали imshow

if __name__ == '__main__':
    main()
