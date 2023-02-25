#include "opencv2/opencv.hpp"
#include "ros/ros.h"
#include "ncnn_ros/box.h"
#include <sensor_msgs/image_encodings.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
cv::Mat cvImg;

char text[256];
int num = 0;
static const char* class_names[] = {
        "Car", "H", "Ring", "W",
    };


int config = 0;
int get_img(const sensor_msgs::ImageConstPtr& msg)
{
    config = 1;
    std::string image_time_=std::to_string(num);
	cvImg = cv_bridge::toCvShare(msg, "bgr8")->image;
	cv::imwrite("/home/fuhua/desktop/img/gx_" + image_time_ + ".jpg", cvImg);
    ROS_INFO("RECEIVED %d", num);

}



int main(int argc, char *argv[])
{   
    
    setlocale(LC_ALL, "");
    // 2.初始化ros节点
    ros::init(argc,argv, "pub_box");
    // 3.创建ros节点句柄
    ros::NodeHandle nh;
	image_transport::ImageTransport it(nh);
    // 4.创建订阅对象
    image_transport::Subscriber sub = it.subscribe("/ardrone/camera/image_raw", 1, get_img);
    // 5.创建回调函数处理数据
	while(config == 0){
        ros::spinOnce();
        num++;
    }
    
    return 0;
}
