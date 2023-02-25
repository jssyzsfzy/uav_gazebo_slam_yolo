
//#include "src/include/yolo-fastestv2.h"

// #include <opencv2\opencv.hpp>
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;
int main()
{
	//string path = "Resources/test_video.mp4";
	cv::VideoCapture cap(0);
	Mat img;
	while (true) {
		cap.read(img);
		cv::imshow("Image", img);
		cv::waitKey(10);
	}
	return 0;
}

/*
#include<iostream>
using namespace std;

int main(){

    cout<<"hello world"<<endl;
    return 0;
}
*/









