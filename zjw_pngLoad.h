#pragma once

#include <opencv2/core/core.hpp>   
#include <opencv2/highgui/highgui.hpp>   
#include <iostream>    
#include <string>
using namespace std;

class PngLoad
{
private:
	string path;
	cv::Mat img;
public :
	PngLoad(string _path);
	~PngLoad();

	cv::Mat getImg() { return img; }
	//加载 img
	bool loadPngPic();
	//分离rgb通道
	void separateChannel();
	void traverse();
	//返回bgr
	cv::Vec3b getPixel(int row, int col);
	//设置一个Pixel 传入的是rgb
	void setPixel(int row, int col, cv::Vec3b rgb);
};