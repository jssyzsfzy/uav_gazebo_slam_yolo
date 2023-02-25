#include "yolo-fastestv2.h"
#include "ros/ros.h"
#include "ncnn_ros/box.h"
cv::Mat cvImg;
yoloFastestv2 api;
char text[256];
int num = 0;
static const char* class_names[] = {
        "Car", "H", "Ring", "W",
    };
ros::Publisher pub;
ncnn_ros::box box_item;


int get_img(const sensor_msgs::ImageConstPtr& msg)
{
	cvImg = cv_bridge::toCvShare(msg, "bgr8")->image;
	// cv::imwrite("/home/fuhua/desktop/img/gx_" + std::to_string(num) + ".jpg", cvImg);
    std::vector<TargetBox> boxes;
	api.detection(cvImg, boxes);
	int height = cvImg.rows;
	int width = cvImg.cols;
	if(cvImg.empty()){std::cout<<"error cvImg"<<std::endl;return 0;}
	for (int i = 0; i < boxes.size(); i++) {
		if(boxes[i].cate < 4 && boxes[i].score > 0.85){
			int baseLine = 0;
			sprintf(text, "%s %.1f%%", class_names[boxes[i].cate], boxes[i].score * 100);
			cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
			box_item.bias_x = (height-boxes[i].y1-boxes[i].y2)/2;
			box_item.bias_y = (width-boxes[i].x1-boxes[i].x2)/2;
			box_item.label = class_names[boxes[i].cate];
			ROS_INFO("send_data:bias_x %d,bias_y %d,label %s, width:%d, height:%d, score:%f", box_item.bias_x, box_item.bias_y, class_names[boxes[i].cate], boxes[i].x2 - boxes[i].x1, boxes[i].y2 - boxes[i].y1, boxes[i].score * 100);
			pub.publish(box_item);
			int x = boxes[i].x1;
			int y = boxes[i].y1 - label_size.height - baseLine;
			if (y < 0)
				y = 0;
			if (x + label_size.width > cvImg.cols)
				x = cvImg.cols - label_size.width;

			cv::rectangle(cvImg, cv::Rect(cv::Point(x, y), cv::Size(label_size.width, label_size.height + baseLine)),
						cv::Scalar(255, 255, 255), -1);

			cv::putText(cvImg, text, cv::Point(x, y + label_size.height),
						cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));

			cv::rectangle (cvImg, cv::Point(boxes[i].x1, boxes[i].y1), 
						cv::Point(boxes[i].x2, boxes[i].y2), cv::Scalar(255, 255, 0), 2, 2, 0);
		}

	}
	// ROS_INFO("output");
	cv::imshow("output", cvImg);
	cv::waitKey(1);
	return 1;
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
	pub = nh.advertise<ncnn_ros::box>("box_data", 1);
    api.loadModel("/home/fuhua/desktop/ros/px4_gazebo/src/ncnn_ros/model/gx-opt.param",
                  "/home/fuhua/desktop/ros/px4_gazebo/src/ncnn_ros/model/gx-opt.bin");
	ros::Rate r(20);
    while(ros::ok()){
		ros::spinOnce();
		r.sleep();
	}
    return 0;
}
