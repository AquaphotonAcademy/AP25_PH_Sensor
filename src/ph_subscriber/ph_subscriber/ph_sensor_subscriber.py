#!usr/bin/env python3
import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32


class PhSensorNode(Node):
    def __init__(self):
        super().__init__("ph_subscriber") 
        self.subscription_=self.create_subscription(Float32, "ph_value", self.ph_callback,10)

    
    def ph_callback(self, msg):
        self.get_logger().info("Received ph value: %.2f", msg.data)


def main(args=None):
    rclpy.init(args=args)
    node = PhSensorNode()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == "__main__":
    main()