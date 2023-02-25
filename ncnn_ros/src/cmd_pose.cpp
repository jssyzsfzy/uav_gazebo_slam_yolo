#include "ros/ros.h"
#include <geometry_msgs/PoseStamped.h>
#include "tf2/LinearMath/Quaternion.h"
#include "geometry_msgs/Twist.h"
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>

double pose_x = 0;
double pose_y = 0;
double pose_z = 2;
double fixedOrientation = 0.0;
double orientationX = fixedOrientation ;
double orientationY = fixedOrientation ;
double orientationZ = fixedOrientation ;
double orientationW = sqrt(1.0 - 3.0*fixedOrientation*fixedOrientation); 
double vx, vy, vw;
ros::Publisher chatter_pub;
double theta = 0.0;
int i = 0;
double pitch, yaw;//定义存储r\p\y的容器
void do_cmd(const geometry_msgs::Twist::ConstPtr& msg_cmd){
    vx = msg_cmd->linear.x;
    vy = msg_cmd->linear.y;
    vw = msg_cmd->angular.z;
    geometry_msgs::PoseStamped msg;
    pose_x += 0.04*(vx*cos(theta) - vy*sin(theta));
    pose_y += 0.04*(vx*sin(theta) + vy*sin(theta));
    msg.pose.position.x = pose_x;
    msg.pose.position.y = pose_y;
    msg.pose.position.z = 2.0;
    theta += 0.04*vw;
    tf2::Quaternion qtn;
    qtn.setRPY(0,0,theta);
    msg.pose.orientation.x = qtn.getX();
    msg.pose.orientation.y = qtn.getY();
    msg.pose.orientation.z = qtn.getZ();
    msg.pose.orientation.w = qtn.getW();
    i = 0;
    chatter_pub.publish(msg);
    while(i<5){
        i++;
        chatter_pub.publish(msg);
        ros::Duration(0.01).sleep();
    }
    ROS_INFO("get vx:%.2f, vy:%.2f, vw:%.2f", vx, vy, vw);
}


int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    ros::init(argc, argv, "cmd_path");
    ros::NodeHandle nh;
    ros::Subscriber sub = nh.subscribe<geometry_msgs::Twist>("/cmd_vel", 1, do_cmd);
    chatter_pub = nh.advertise<geometry_msgs::PoseStamped>("/ardrone/command/pose", 1);
    ros::spin();
    return 0;
}

