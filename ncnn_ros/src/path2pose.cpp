#include "opencv2/opencv.hpp"
#include "ros/ros.h"
#include "nav_msgs/Path.h"
#include <geometry_msgs/PoseStamped.h>
#include "tf2/LinearMath/Quaternion.h"

double positionX, positionY, positionZ;

double bias_x_last = 0;
double bias_y_last = 0;

double pose_x = 0;
double pose_y = 0;
double pose_z = 2;

int i = 0;
double x;
double y;
double fixedOrientation = 0.0;
double orientationX = fixedOrientation ;
double orientationY = fixedOrientation ;
double orientationZ = fixedOrientation ;
double orientationW = sqrt(1.0 - 3.0*fixedOrientation*fixedOrientation); 
ros::Publisher chatter_pub;


void send_pose(const nav_msgs::Path& pt)
{
    i = 0;
    ROS_INFO("%d",pt.poses.size());
    int index = pt.poses.size() / 2;
    geometry_msgs::PoseStamped msg;
    msg = pt.poses[pt.poses.size() - 1];
    ROS_INFO("发送的消息：%f, %f, %f,", msg.pose.position.x, msg.pose.position.y, msg.pose.position.z);
    msg.pose.position.z = 2.0;
    // msg.pose.orientation.x=orientationX;
    // msg.pose.orientation.y=orientationY;
    // msg.pose.orientation.z=orientationZ;
    // msg.pose.orientation.w=orientationW;
    while(i<10){
        i++;
        chatter_pub.publish(msg);
        ros::Duration(0.01).sleep();
    }
}


int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    // 2.初始化ros节点
    ros::init(argc,argv, "pub_pose");
    // 3.创建ros节点句柄
    ros::NodeHandle nh;
    // 4.创建订阅对象
    ros::Subscriber sub = nh.subscribe("/move_base/TrajectoryPlannerROS/local_plan", 1, send_pose);
    chatter_pub = nh.advertise<geometry_msgs::PoseStamped>("/ardrone/command/pose", 5);
    
    while (ros::ok())
    {
        ros::spin();
    }
    
}









