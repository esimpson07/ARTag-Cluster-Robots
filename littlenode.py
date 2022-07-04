#!/usr/bin/python3.8
from telnetlib import Telnet
import roslib; roslib.load_manifest('wsjs')
import rospy
import tf.transformations
from geometry_msgs.msg import Twist
x = 0
y = 0
z = 0

tn = Telnet('192.168.1.1',23)

def callback(msg):
    rospy.loginfo("Received a /cmd_vel message!")
    rospy.loginfo("Linear Components: [%f, %f, %f]"%(msg.linear.x, msg.linear.y, msg.linear.z))
    rospy.loginfo("Angular Components: [%f, %f, %f]"%(msg.angular.x, msg.angular.y, msg.angular.z))
    x = msg.linear.x
    y = msg.angular.z
    z = msg.linear.z
    ld = x - y
    rd = x + y
    m = max(abs(ld),abs(rd))
    if(m > 1):
    	ld = ld / m
    	rd = rd / m
    cats = bytes(str(str(int(255 * (ld + 1)) + 255) + str(int(255 * (rd + 1)) + 255)), 'utf-8')
    tn.write(cats)
    rospy.loginfo("Telnet interacting!:[%s]"%cats)
    

def listener():
    rospy.init_node('cmd_vel_listener')
    rospy.Subscriber("/input_joy/cmd_vel", Twist, callback)
    tn = Telnet('192.168.1.1',23)

    
    rospy.spin()

if __name__ == '__main__':
    listener()
