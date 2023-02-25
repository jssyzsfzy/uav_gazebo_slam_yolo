#include "ros/ros.h"
#include "ncnn_ros/box.h"
#include "geometry_msgs/PoseStamped.h"



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
bool config = 1;

double posex[37] = {-0.15, -0.3, -0.45, -0.6, -0.3, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 
                    1.4, 1.6, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.6, 1.4,
                    1.2, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.0, 0.0, 0.0, 0.0};
double posey[37] = {-0.2, -0.4, -0.6, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, -0.8, 
                    -0.8, -0.8, -0.8, -0.8, -0.6, -0.4, -0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.8, 0.6, 0.4, 0};
int main(int argc, char *argv[])
{

    setlocale(LC_ALL, "");
    // 2.初始化ros节点
    ros::init(argc, argv, "travel");
    ros::NodeHandle nh;
    // mySubAndPub sap;
    // 6.spin()
    chatter_pub = nh.advertise<geometry_msgs::PoseStamped>("/ardrone/command/pose", 5);
    int status = 0;
    while(ros::ok()){
        if(config == 1){
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
            status++;
            if(status >= 37){status = 0;}
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
        ros::spinOnce();
    }
    return 0;
}

