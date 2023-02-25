#include "ros/ros.h"
#include "ncnn_ros/box.h"
#include "geometry_msgs/PoseStamped.h"
#include <string>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include "ncnn_ros/pose_addAction.h"
#include "actionlib/client/simple_action_client.h"
#include "ncnn_ros/goalAction.h"
ros::Publisher chatter_pub;
double bias_x_sum = 0;
double bias_y_sum = 0;

double pose_x = 0;
double pose_y = 0;

double x = 0;
double y = 0;
int i = 0;

int config = 0;
std::string goal[4];
ros::ServiceClient client;
ros::ServiceClient client_add;
std::string default_goal[4] = {"H", "W", "Car", "Ring"};
float goal_x[4] = {0.0, 1.5, 1.5, 0.0};
float goal_y[4] = {-1.0, -1.0, 1.0, 1.0};
int sequence[4] = {0, 0, 0, 0};
double theta = 0.0;
double pitch, roll;//定义存储r\p\y的容器

ncnn_ros::pose_addGoal add_goal;
ncnn_ros::goalGoal goal_goal;
bool send_add = false;
bool finish_add = true;
bool finish_goal = true;
bool next_goal = true;
bool add_run = false;

void down_up(){
    geometry_msgs::PoseStamped msg;
    msg.header.frame_id = "map";
    msg.header.stamp = ros::Time::now();
    msg.pose.position.x = pose_x;
    msg.pose.position.y = pose_y;
    msg.pose.position.z = 0.5;
    tf2::Quaternion qtn;
    qtn.setRPY(0,0,theta);
    msg.pose.orientation.x = qtn.getX();
    msg.pose.orientation.y = qtn.getY();
    msg.pose.orientation.z = qtn.getZ();
    msg.pose.orientation.w = qtn.getW();
    i = 0;
    while(i<5)
    {
        i++;
        // ROS_INFO("接受的消息：%s  %s  %s ", argv[1], argv[2], argv[3]);
        chatter_pub.publish(msg);
        ros::spinOnce();
        ros::Duration(0.01).sleep();
    }
    ros::Duration(3).sleep();
    msg.pose.position.x = pose_x;
    msg.pose.position.y = pose_y;
    msg.pose.position.z = 2;
    i = 0;
    while(i<5)
    {
        i++;
        // ROS_INFO("接受的消息：%s  %s  %s ", argv[1], argv[2], argv[3]);
        chatter_pub.publish(msg);
        ros::spinOnce();
        ros::Duration(0.01).sleep();
    }
    ros::Duration(3).sleep();
    
}

void do_box_msg(const ncnn_ros::box::ConstPtr& box_data)
{
    if(box_data->label == goal[config])
    {
        double x_t = box_data->bias_x;
        double y_t = box_data->bias_y;
        double x_pid = x_t*cos(theta) - y_t*sin(theta);
        double y_pid = x_t*sin(theta) + y_t*cos(theta);
        bias_x_sum+=x_pid;
        bias_y_sum+=y_pid;
        if(bias_x_sum>2000){bias_x_sum=2000;}
        if(bias_x_sum<-2000){bias_x_sum=-2000;}
        if(bias_y_sum>2000){bias_y_sum=2000;}
        if(bias_y_sum<-2000){bias_y_sum=-2000;}
        x = x_pid + 0.005 * bias_x_sum;
        y = y_pid + 0.005 * bias_y_sum;
        if(x > -20 && x < 20){x = 0;}
        if(y > -20 && y < 20){y = 0;}
        if(x == 0 && y == 0){config++;next_goal = true;}
        double temp;
        temp = pose_x + x/500;
        if(temp > -0.7 && temp < 2)
        {add_goal.x_add = x/500;}
        temp = pose_y + y/500;
        if(temp > -1.3 && temp < 1.5)
        {add_goal.y_add = y/500;}
        ROS_INFO("x_pid:%f, y_pid:%f, bias_x_sum:%f, bias_y_sum:%f, x:%f, y:%f, theta:%f", x_pid, y_pid, bias_x_sum, bias_y_sum, x, y, theta);
        send_add = true;
    }
    else{send_add = false;}
}

// 响应成功
void done_cd(const actionlib::SimpleClientGoalState &status, const ncnn_ros::pose_addResultConstPtr &result)
{   
    if(status.state_ == status.SUCCEEDED)
    {
        ROS_INFO("成功%f", result->response);
    }
    else{ROS_WARN("相应失败");}
    finish_add = true;
}

// 激活回调
void active_cd(){
    ROS_INFO("建立连接");
}
// 连续反馈
void feed_cb(const ncnn_ros::pose_addFeedbackConstPtr &feedback)
{
    ROS_INFO("pose_add当前进度:%.2f", feedback->progress_bar);

}

void done_goal_cd(const actionlib::SimpleClientGoalState &status, const ncnn_ros::goalResultConstPtr &result)
{   
    if(status.state_ == status.SUCCEEDED)
    {
        ROS_INFO("成功%f", result->response);
    }
    else{ROS_WARN("相应失败");}
    add_run = true;
    finish_goal = true;
}
// 连续反馈
void feed_goal_cb(const ncnn_ros::goalFeedbackConstPtr &feedback)
{
    ROS_INFO("goal当前进度:%.2f", feedback->progress_bar);

}
void do_pose(const geometry_msgs::PoseStamped::ConstPtr& pose_data)
{
    pose_x = pose_data->pose.position.x;
    pose_y = pose_data->pose.position.y;
    tf2::Quaternion imu_quat(
            pose_data->pose.orientation.x,
            pose_data->pose.orientation.y,
            pose_data->pose.orientation.z,
            pose_data->pose.orientation.w);
    
    tf2::Matrix3x3 m(imu_quat);
    m.getRPY(roll, pitch, theta);
}

int main(int argc, char *argv[])
{

    setlocale(LC_ALL, "");
    // 2.初始化ros节点
    ros::init(argc, argv, "get_box");
    ros::NodeHandle nh;
    if(argc != 5){
        ROS_INFO("参数个数不对");
        return 1;
    }
    goal[0] = argv[1];
    goal[1] = argv[2];
    goal[2] = argv[3];
    goal[3] = argv[4];
    int temp_2 = 0;
    for(int temp_1=0;temp_1<4;temp_1++)
    {
        for(int temp=0;temp<4;temp++)
        {
            if(goal[temp_1] == default_goal[temp])
            {
                sequence[temp_2] = temp;
                temp_2+=1;
            }
        }
    }
    
    chatter_pub = nh.advertise<geometry_msgs::PoseStamped>("/ardrone/command/pose", 10);    
    ROS_INFO("sequence: %d, %d, %d, %d", sequence[0], sequence[1], sequence[2], sequence[3]);
    ros::Subscriber sub = nh.subscribe("box_data", 0, do_box_msg);
    actionlib::SimpleActionClient<ncnn_ros::pose_addAction> client_add(nh, "add_pose"); 
    actionlib::SimpleActionClient<ncnn_ros::goalAction> client_goal(nh, "goal_pose"); 
    ros::Subscriber sub_pose = nh.subscribe<geometry_msgs::PoseStamped>("/ardrone/command/pose", 1, do_pose);
    client_add.waitForServer();
    client_goal.waitForServer();
    ROS_INFO("start");
    ros::Rate rate(2);
    while(ros::ok()){
        if(send_add == true && finish_add == true && add_run == true)
        {
            client_goal.cancelAllGoals();
            ROS_INFO("send_add x_add:%f, y_add:%f, config:%d", add_goal.x_add, add_goal.y_add, config);
            client_add.sendGoal(add_goal, &done_cd, &active_cd, &feed_cb);
            finish_add = false;
        }
        ROS_INFO("config:%d, send_add:%d, finish_add:%d, add_run:%d, finish_goal:%d, next_goal:%d", 
                config, send_add, finish_add, add_run, finish_goal, next_goal);
        if(finish_goal == true && next_goal == true)
        {
            
            ROS_INFO("send_goal x_goal:%f, y_goal:%f, config:%d", goal_goal.x_goal, goal_goal.y_goal, config);
            client_add.cancelAllGoals();
            if(config >= 4)
            {   
                ROS_INFO("sucess");
                return 1;
            }
            goal_goal.x_goal = goal_x[sequence[config]];
            goal_goal.y_goal = goal_y[sequence[config]];
            client_goal.sendGoal(goal_goal, &done_goal_cd, &active_cd, &feed_goal_cb);
            finish_goal = false;
            next_goal = false;
            add_run = false;
        }
        
        rate.sleep();
        ros::spinOnce();
    }
    return 0;
}

