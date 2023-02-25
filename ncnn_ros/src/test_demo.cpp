#include "ros/ros.h"
#include "ncnn_ros/box.h"
#include "geometry_msgs/PoseStamped.h"
#include <string>
#include "ncnn_ros/goal.h"
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include "ncnn_ros/pose_addAction.h"
#include "actionlib/client/simple_action_client.h"
double bias_x_last = 0;
double bias_y_last = 0;

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
ncnn_ros::goal pub_goal;
ncnn_ros::pose_addGoal add_goal;
bool send_add = false;
bool finish_add = true;
bool finish_goal = true;


void do_box_msg(const ncnn_ros::box::ConstPtr& box_data)
{
    if(box_data->label == goal[config])
    {
        double x_t = 0.8*box_data->bias_x + 0.2*bias_x_last;
        double y_t = 0.8*box_data->bias_y + 0.2*bias_y_last;
        bias_x_last = box_data->bias_x;
        bias_y_last = box_data->bias_y;
        x = x_t*cos(theta) - y_t*sin(theta);
        y = x_t*sin(theta) + y_t*cos(theta);
        if(x > -20 && x < 20){x = 0;}
        if(y > -20 && y < 20){y = 0;}
        if(x == 0 && y == 0){config++;finish_goal = true;}
        double temp;
        temp = pose_x + x/1000;
        if(temp > -0.7 && temp < 2)
        {add_goal.x_add = x/1000;}
        temp = pose_y + y/1000;
        if(temp > -1.2 && temp < 1.5)
        {add_goal.y_add = y/1000;}
        ROS_INFO("x_t:%f, y_t:%f, x:%f, y:%f, theta:%f", x_t, y_t, x, y, theta);
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
    ROS_INFO("当前进度:%.2f", feedback->progress_bar);

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
    
        
    ROS_INFO("sequence: %d, %d, %d, %d", sequence[0], sequence[1], sequence[2], sequence[3]);
    ros::Subscriber sub = nh.subscribe("box_data", 0, do_box_msg);
    client = nh.serviceClient<ncnn_ros::goal>("ser_goal");
    actionlib::SimpleActionClient<ncnn_ros::pose_addAction> client_add(nh, "add_pose"); 
    ros::Subscriber sub_pose = nh.subscribe<geometry_msgs::PoseStamped>("/ardrone/command/pose", 1, do_pose);
    client_add.waitForServer();
    ROS_INFO("start");
    ros::Rate rate(2);
    while(ros::ok()){
        if(abs(pub_goal.request.x - pose_x)<0.1 && abs(pub_goal.request.y - pose_y)<0.1){
            finish_goal = false;
        }
        else{finish_goal = true;}
        if(send_add == true && finish_add == true)
        {
            ROS_INFO("x_add:%f, y_add:%f, config:%d", add_goal.x_add, add_goal.y_add, config);
            client_add.sendGoal(add_goal, &done_cd, &active_cd, &feed_cb);
            finish_add = false;
            finish_goal = false;
        }
        if(config >= 1)
        {ROS_INFO("config:%d", config);}
        //  && finish_goal == true+
        if(send_add == false && finish_goal == true)
        {
            client_add.cancelAllGoals();
            pub_goal.request.x = goal_x[sequence[config]];
            pub_goal.request.y = goal_y[sequence[config]];
            pub_goal.request.z = 2.0;
            bool flag = client.call(pub_goal);
            if (flag)
            {
                ROS_INFO("发送四个点的位置信息并反馈：config:%d, 响应结果:%s",config, pub_goal.response.response.c_str());
            }
            else
            {
                ROS_ERROR("请求处理失败....");
            }
        }
        rate.sleep();
        ros::spinOnce();
    }
    return 0;
}

