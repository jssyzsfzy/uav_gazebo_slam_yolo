#include "opencv2/opencv.hpp"
#include "ros/ros.h"
#include "ncnn_ros/box.h"
#include <sensor_msgs/image_encodings.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include "geometry_msgs/PoseStamped.h"
cv::Mat cvImg;

ros::Publisher chatter_pub;
int num = 0;
image_transport::Subscriber sub;
double posex[37] = {-0.15, -0.3, -0.45, -0.6, -0.3, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 
                    1.4, 1.6, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.6, 1.4,
                    1.2, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.0, 0.0, 0.0, 0.0};
double posey[37] = {-0.2, -0.4, -0.6, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, 
                    -0.8, -0.8, -0.8, -0.8, -0.6, -0.4, -0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.8, 0.6, 0.4, 0};
double pose_x = 0;
double pose_y = 0;
double pose_z = 2;
double fixedOrientation = 0.0;
double orientationX = fixedOrientation ;
double orientationY = fixedOrientation ;
double orientationZ = fixedOrientation ;
double orientationW = sqrt(1.0 - 3.0*fixedOrientation*fixedOrientation); 
int status = 0;
int i = 0;

int get_img(const sensor_msgs::ImageConstPtr& image)
{
    status++;
    if(status >= 37){return 0;}
    std::string image_time_=std::to_string(num);
	cvImg = cv_bridge::toCvShare(image, "bgr8")->image;
	cv::imwrite("/home/fuhua/desktop/img/gx_" + image_time_ + ".jpg", cvImg);
    ROS_INFO("RECEIVED %d", num);
    num++;

    pose_x = posex[status];
    pose_y = posey[status];
    geometry_msgs::PoseStamped msg;
    msg.pose.position.x = pose_x;
    msg.pose.position.y = pose_y;
    msg.pose.position.z = pose_z;
    msg.pose.orientation.x = orientationX;
    msg.pose.orientation.y = orientationY;
    msg.pose.orientation.z = orientationZ;
    msg.pose.orientation.w = orientationW;
    ROS_INFO("发送的消息：%f, %f, %f,", msg.pose.position.x, msg.pose.position.y, msg.pose.position.z);
    while(i<5){
        i++;
        chatter_pub.publish(msg);
        ros::Duration(0.01).sleep();
        ROS_INFO("Goal published %d", i);
    }
    i = 0;
    ros::Duration(0.5).sleep();
}

int main(int argc, char *argv[])
{   
    
    setlocale(LC_ALL, "");
    // 2.初始化ros节点
    ros::init(argc,argv, "pub_box");
    // 3.创建ros节点句柄
    ros::NodeHandle nh;
    chatter_pub = nh.advertise<geometry_msgs::PoseStamped>("/ardrone/command/pose", 5);
	image_transport::ImageTransport it(nh);
    // 4.创建订阅对象
    sub = it.subscribe("/ardrone/camera/image_raw", 1, get_img);
    
	while(ros::ok()){

        ros::spinOnce();
    }
    
    return 0;
}


