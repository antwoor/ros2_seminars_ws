# qr_decoder.py

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2
from pyzbar import pyzbar
from std_msgs.msg import String

class QRDecoder(Node):
    def __init__(self):
        super().__init__('qr_decoder')
        self.subscription = self.create_subscription(
            Image,
            '/camera/image_raw',
            self.image_callback,
            10)
        self.publisher = self.create_publisher(String, 'qr_codes', 10)
        self.bridge = CvBridge()

    def image_callback(self, msg):
        cv_image = self.bridge.imgmsg_to_cv2(msg, "bgr8")
        barcodes = pyzbar.decode(cv_image)
        for barcode in barcodes:
            barcode_data = barcode.data.decode('utf-8')
            self.get_logger().info(f"QR Code detected: {barcode_data}")
            qr_msg = String()
            qr_msg.data = barcode_data
            self.publisher.publish(qr_msg)

def main(args=None):
    rclpy.init(args=args)
    node = QRDecoder()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
