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
	//���� img
	bool loadPngPic();
	//����rgbͨ��
	void separateChannel();
	void traverse();
	//����bgr
	cv::Vec3b getPixel(int row, int col);
	//����һ��Pixel �������rgb
	void setPixel(int row, int col, cv::Vec3b rgb);
};