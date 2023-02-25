#!/usr/bin/env python3

#导入相关的库文件
#我们这里通过捕获PWM波脉冲宽度获取遥控器的控制信号，进而将其映射为移动小车的速度，
#并通过ROS操作系统以cmd_vel话题twist消息的形式发布出去
import rospy
from std_msgs.msg import String
#导入pigpio库
# import pigpio
import time
from ncnn_ros.msg import control
#建立实体化对象
# pi=pigpio.pi()

# #tick_0用于存储低电平捕获的时间戳
# tick_0=[None,None,None]
# #tick_1用于存储高电平捕获的时间戳
# tick_1=[None,None,None]
# #diff用于存储脉宽
# diff=[0,0,0]
# last_disk = 0

# #定义回调函数
# def in_callback(argu,gpio,level,tick):
#     #如果是低电平，则说明下降沿到来，通过数组diff将两次捕获的时间差存储起来
#     if level==0:
#         tick_0[argu]=tick   
#     else:
#         tick_1[argu]=tick
#     if tick_0[argu] != None and tick_1[argu] != None and 0 < tick_1[argu] - tick_0[argu] < 1000:
#         diff[argu] = tick_1[argu] - tick_0[argu]


# def mycallback1(gpio,level,tick):
#     in_callback(0,gpio,level,tick)

# def mycallback2(gpio,level,tick):
#     in_callback(1,gpio,level,tick)

# def mycallback3(gpio,level,tick):
#     in_callback(2,gpio,level,tick)


 

if __name__=='__main__':
    # cb1=pi.callback(12,pigpio.EITHER_EDGE,mycallback1)
    # cb2=pi.callback(16,pigpio.EITHER_EDGE,mycallback2)
    # cb3=pi.callback(20,pigpio.EITHER_EDGE,mycallback3)
    rospy.init_node("get_control")
    pub = rospy.Publisher("control_signal", control,queue_size=4)
    control_sig = control()
    while not rospy.is_shutdown():
        print("runing")
    	# if diff[0] != 0 and diff[1] != 0 and diff[2] != 0:
        #     control_sig.vx = diff[0]
        #     control_sig.vy = diff[1]
        #     control_sig.vw = diff[2]
        #     pub.publish(control_sig)
