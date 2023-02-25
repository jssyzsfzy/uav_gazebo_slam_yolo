#include "geometry_msgs/PoseStamped.h"
#include "ros/ros.h"

double positionX, positionY, positionZ;
double orientationX, orientationY, orientationZ, orientationW;

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    if(argc != 4){
        ROS_INFO("参数个数不对");
        return 1;
    }
    ros::init(argc,argv, "pub_position_control");
    // 3.创建ros节点句柄
    ros::NodeHandle nh;
    ros::Publisher chatter_pub = nh.advertise<geometry_msgs::PoseStamped>("/ardrone/command/pose", 10);
    geometry_msgs::PoseStamped msg;
    int i = 0;
    ros::Rate r(20);
    positionX = std::atof(argv[1]);
    positionY = std::atof(argv[2]);
    positionZ = std::atof(argv[3]);
    // ROS_INFO("接受的消息：%f, %f, %f,", positionX, positionY, positionZ);
    msg.pose.position.x = positionX;
    msg.pose.position.y = positionY;
    msg.pose.position.z = positionZ;
    // ROS_INFO("发送的消息：%f, %f, %f,", msg.pose.position.x, msg.pose.position.y, msg.pose.position.z);
    double fixedOrientation = 0.0;
    orientationX = fixedOrientation ;
    orientationY = fixedOrientation ;
    orientationZ = fixedOrientation ;
    orientationW = sqrt(1.0 - 3.0*fixedOrientation*fixedOrientation); 
    msg.pose.orientation.x = orientationX;
    msg.pose.orientation.y = orientationY;
    msg.pose.orientation.z = orientationZ;
    msg.pose.orientation.w = orientationW;
    while(i<10)
    {
        i++;
        // ROS_INFO("接受的消息：%s  %s  %s ", argv[1], argv[2], argv[3]);
        chatter_pub.publish(msg);
        ros::spinOnce();
        r.sleep();
    }
    // msg.pose.position.x = positionX;
    // msg.pose.position.y = positionY;
    // msg.pose.position.z = positionZ+1;
    // // 2.初始化ros节点
    // ros::Duration(2).sleep();
    // while(i<20)
    // {
    //     i++;
    //     chatter_pub.publish(msg);
    //     r.sleep();
    // }
    
    return 0;
}


