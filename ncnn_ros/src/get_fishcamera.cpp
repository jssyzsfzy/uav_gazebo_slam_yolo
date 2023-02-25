#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <stdio.h>
#include <cv_bridge/cv_bridge.h>
cv::Mat cvImg1;
cv::Mat cvImg2;
int get_img1(const sensor_msgs::ImageConstPtr& msg)
{
    ROS_INFO("img1:%lf",(double)(msg->header.stamp.toSec()));
	cvImg1 = cv_bridge::toCvShare(msg, "bgr8")->image;
	// cv::imwrite("/home/fuhua/desktop/img/gx_" + std::to_string(num) + ".jpg", cvImg);
	cv::imshow("output1", cvImg1);
	cv::waitKey(1);
	return 1;
}
int get_img2(const sensor_msgs::ImageConstPtr& msg)
{
    ROS_INFO("img2:%lf",(double)(msg->header.stamp.toSec()));
	cvImg2 = cv_bridge::toCvShare(msg, "bgr8")->image;
	// cv::imwrite("/home/fuhua/desktop/img/gx_" + std::to_string(num) + ".jpg", cvImg);
	cv::imshow("output2", cvImg2);
	cv::waitKey(1);
	return 1;
}
int main(int argc, char** argv)
{
    ros::init(argc, argv, "camera_image_get");
    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub1 = it.subscribe("/camera/fisheye1/image_raw", 1, get_img1);
    image_transport::Subscriber sub2 = it.subscribe("/camera/fisheye2/image_raw", 1, get_img2);
    
    ros::Rate r(30);
    while(ros::ok()){
        ros::spinOnce();
        r.sleep();
    }
    return 0;
}
// #include <librealsense2/rs.hpp>
// #include <opencv2/opencv.hpp>

// int main(){

//     rs2::config cfg;
//     cfg.enable_stream(RS2_STREAM_FISHEYE,1, RS2_FORMAT_Y8);
//     cfg.enable_stream(RS2_STREAM_FISHEYE,2, RS2_FORMAT_Y8);
//     rs2::pipeline pipe;
//     pipe.start(cfg);

//     rs2::frameset data;

//     while (1){
//         data = pipe.wait_for_frames();
//         rs2::frame image_left = data.get_fisheye_frame(1);
//         rs2::frame image_right = data.get_fisheye_frame(2);

//         if (!image_left || !image_right)
//             break;

//         cv::Mat cv_image_left(cv::Size(848, 800), CV_8U, (void*)image_left.get_data(), cv::Mat::AUTO_STEP);
//         cv::Mat cv_image_right(cv::Size(848, 800), CV_8U, (void*)image_right.get_data(), cv::Mat::AUTO_STEP);

//         cv::imshow("left", cv_image_left);
//         cv::imshow("right", cv_image_right);
//         cv::waitKey(1);
//     }

//     return 0;
// }
