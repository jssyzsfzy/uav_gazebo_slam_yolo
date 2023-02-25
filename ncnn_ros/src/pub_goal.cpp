#include "opencv2/opencv.hpp"
#include "ros/ros.h"
#include "ncnn_ros/box.h"
#include <sensor_msgs/image_encodings.h>
#include "move_base_msgs/MoveBaseGoal.h"
#include "tf2/LinearMath/Quaternion.h"
#include "move_base_msgs/MoveBaseActionGoal.h"
#include "ncnn_ros/goal.h"
#include "ncnn_ros/pose_addAction.h"
#include "ncnn_ros/goalAction.h"
#include "actionlib/server/simple_action_server.h"

int i = 0;
double pose_x = 0;
double pose_y = 0;
double pose_z = 2;
double fixedOrientation = 0.0;
double orientationX = fixedOrientation ;
double orientationY = fixedOrientation ;
double orientationZ = fixedOrientation ;
double orientationW = sqrt(1.0 - 3.0*fixedOrientation*fixedOrientation); 

typedef actionlib::SimpleActionServer<ncnn_ros::pose_addAction> Server;
typedef actionlib::SimpleActionServer<ncnn_ros::goalAction> Server_goal;

ros::Publisher pub;

void do_pose(const geometry_msgs::PoseStamped& msg)
{
    pose_x = msg.pose.position.x;
    pose_y = msg.pose.position.y;
}
// action操作
void do_action(const ncnn_ros::pose_addGoalConstPtr &msg, Server* server_add){
    move_base_msgs::MoveBaseActionGoal goal;
    goal.header.stamp = ros::Time::now();
    goal.goal.target_pose.header.frame_id = "map";
    goal.goal.target_pose.header.stamp = ros::Time::now();
    
    float x_goal = pose_x + msg->x_add;
    float y_goal = pose_y + msg->y_add;
    goal.goal.target_pose.pose.position.x = x_goal;
    goal.goal.target_pose.pose.position.y = y_goal;
    goal.goal.target_pose.pose.position.z = 2.0;
    goal.goal.target_pose.pose.orientation.x = orientationX;
    goal.goal.target_pose.pose.orientation.y = orientationY;
    goal.goal.target_pose.pose.orientation.z = orientationZ;
    goal.goal.target_pose.pose.orientation.w = orientationW;
    i = 0;
    while(i<10){
        i++;
        pub.publish(goal);
        ros::Duration(0.01).sleep();
    }
    // a. 提交的目标值
    ros::Rate rate(2);
    // b. 产生连续反馈
    int result;
    float distance = sqrt(pow(x_goal-pose_x,2) +pow(y_goal-pose_y, 2));
    // ROS_INFO("得到的位置信息：pose_x:%f, pose_y:%f, x_goal:%f, y_goal:%f, distance:%f", pose_x, pose_y, x_goal, y_goal, distance);
    while(distance>0.12)
    {
        pub.publish(goal);
        ROS_INFO("得到的位置信息：pose_x:%f, pose_y:%f, x_goal:%f, y_goal:%f, distance:%f", pose_x, pose_y, x_goal, y_goal, distance);
        ncnn_ros::pose_addFeedback fb;
        fb.progress_bar = distance;
        rate.sleep();
        server_add->publishFeedback(fb);
        distance = sqrt(pow(x_goal-pose_x,2) +pow(y_goal-pose_y, 2));
    }
    ncnn_ros::pose_addResult r;
    r.response = distance;
    server_add->setSucceeded(r);
}

void goal_action(const ncnn_ros::goalGoalConstPtr &msg, Server_goal* server_goal){
    move_base_msgs::MoveBaseActionGoal goal;
    goal.header.stamp = ros::Time::now();
    goal.goal.target_pose.header.frame_id = "map";
    goal.goal.target_pose.header.stamp = ros::Time::now();
    
    float x_goal = msg->x_goal;
    float y_goal = msg->y_goal;
    goal.goal.target_pose.pose.position.x = x_goal;
    goal.goal.target_pose.pose.position.y = y_goal;
    goal.goal.target_pose.pose.position.z = 2.0;
    goal.goal.target_pose.pose.orientation.x = orientationX;
    goal.goal.target_pose.pose.orientation.y = orientationY;
    goal.goal.target_pose.pose.orientation.z = orientationZ;
    goal.goal.target_pose.pose.orientation.w = orientationW;
    i = 0;
    while(i<10){
        i++;
        pub.publish(goal);
        ros::Duration(0.01).sleep();
    }
    // a. 提交的目标值
    ros::Rate rate(2);
    // b. 产生连续反馈
    int result;
    float distance = sqrt(pow(x_goal-pose_x,2) +pow(y_goal-pose_y, 2));
    // ROS_INFO("得到的位置信息：pose_x:%f, pose_y:%f, x_goal:%f, y_goal:%f, distance:%f", pose_x, pose_y, x_goal, y_goal, distance);
    while(distance>0.15)
    {
        pub.publish(goal);
        ROS_INFO("得到的位置信息：pose_x:%f, pose_y:%f, x_goal:%f, y_goal:%f, distance:%f", pose_x, pose_y, x_goal, y_goal, distance);
        ncnn_ros::goalFeedback fb;
        fb.progress_bar = distance;
        rate.sleep();
        server_goal->publishFeedback(fb);
        distance = sqrt(pow(x_goal-pose_x,2) +pow(y_goal-pose_y, 2));
    }
    ncnn_ros::goalResult r;
    r.response = distance;
    server_goal->setSucceeded(r);
}

int main(int argc, char *argv[])
{   
    
    setlocale(LC_ALL, "");
    // 2.初始化ros节点
    ros::init(argc,argv, "pub_goal");
    // 3.创建ros节点句柄
    ros::NodeHandle nh;
	pub = nh.advertise<move_base_msgs::MoveBaseActionGoal>("/move_base/goal", 10);
    Server server_add(nh, "add_pose", boost::bind(&do_action, _1, &server_add), false);
    Server_goal server_goal(nh, "goal_pose", boost::bind(&goal_action, _1, &server_goal), false);
    ros::Subscriber sub = nh.subscribe("/ardrone/command/pose", 1, do_pose);
    ros::Rate rate(30);
    server_add.start();
    server_goal.start();
    while(ros::ok())
    {
        rate.sleep();
        ros::spinOnce();
    }
    
    return 0;
}
