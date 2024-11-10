import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from sensor_msgs.msg import LaserScan
from std_msgs.msg import String

class Navigation(Node):
    def __init__(self):
        super().__init__('navigation')
        self.publisher_ = self.create_publisher(Twist, 'cmd_vel', 10)
        self.scan_subscriber_ = self.create_subscription(
            LaserScan,
            'scan',
            self.scan_callback,
            10)
        self.qr_subscriber_ = self.create_subscription(
            String,
            'qr_code',
            self.qr_callback,
            10)

        self.safe_distance = 0.5  # Минимальное расстояние до препятствия, м
        self.twist_msg = Twist()
        self.found_qr_code = False

    def scan_callback(self, scan_msg):
        # Логика избегания препятствий на основе данных лидара
        min_distance = min(scan_msg.ranges)
        if min_distance < self.safe_distance:
            self.get_logger().info(f'Obstacle detected at {min_distance} meters. Turning...')
            # Если обнаружено препятствие, робот поворачивает налево
            self.twist_msg.linear.x = 0.0
            self.twist_msg.angular.z = 0.5
        else:
            # Если препятствий нет, робот движется вперед
            self.twist_msg.linear.x = 0.2
            self.twist_msg.angular.z = 0.0
        
        self.publisher_.publish(self.twist_msg)

    def qr_callback(self, qr_msg):
        # Логика обработки QR-кодов
        self.get_logger().info(f'QR Code detected: {qr_msg.data}')
        self.found_qr_code = True
        # Допустим, при обнаружении QR-кода, робот останавливается для считывания
        self.twist_msg.linear.x = 0.0
        self.twist_msg.angular.z = 0.0
        self.publisher_.publish(self.twist_msg)

    def main(self):
        rclpy.spin(self)
        self.destroy_node()

def main(args=None):
    rclpy.init(args=args)
    navigator = Navigation()
    navigator.main()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
