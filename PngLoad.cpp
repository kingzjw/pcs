/*
* author : Jiawei Zhou
* head file : pic toolkit implemented by opencv
* time: 6/14.2018  
*/
#include "zjw_pngLoad.h"

PngLoad::PngLoad(string _path)
{
	path = _path;
}

PngLoad::~PngLoad()
{
}

bool PngLoad::loadPngPic()
{
	img = imread(path, cv::IMREAD_COLOR);
	assert(!img.empty());
	//if (img.empty())                      // Check for invalid input
	//{
	//	cout << "Could not open or find the image" << std::endl;
	//	return false;
	//}
	return true;
}

void PngLoad::separateChannel()
{
	// Split the image into different channels  
	std::vector<cv::Mat> rgbChannels(3);
	split(img, rgbChannels);

	// Show individual channels  
	cv::Mat blank_ch, fin_img;
	blank_ch = cv::Mat::zeros(cv::Size(img.cols, img.rows), CV_8UC1);

	// Showing Red Channel  
	// G and B channels are kept as zero matrix for visual perception  
	std::vector<cv::Mat> channels_r;
	channels_r.push_back(blank_ch);
	channels_r.push_back(blank_ch);
	channels_r.push_back(rgbChannels[2]);

	/// Merge the three channels  
	cv::merge(channels_r, fin_img);
	cv::imshow("R", fin_img);
	imwrite("./R.jpg", fin_img);

	// Showing Green Channel  
	std::vector<cv::Mat> channels_g;
	channels_g.push_back(blank_ch);
	channels_g.push_back(rgbChannels[1]);
	channels_g.push_back(blank_ch);
	cv::merge(channels_g, fin_img);
	cv::imshow("G", fin_img);
	imwrite("./G.jpg", fin_img);

	// Showing Blue Channel  
	std::vector<cv::Mat> channels_b;
	channels_b.push_back(rgbChannels[0]);
	channels_b.push_back(blank_ch);
	channels_b.push_back(blank_ch);
	cv::merge(channels_b, fin_img);
	cv::imshow("B", fin_img);
	imwrite("./B.jpg", fin_img);

	cv::waitKey(0);
	return ;
}

void PngLoad::traverse()
{
	for (int row_it = 0; row_it < img.rows; row_it++)
	{
		for (int col_it = 0; col_it < img.cols; col_it++)
		{
			//读取
			int b = img.at<cv::Vec3b>(col_it, row_it)[0]; // b = image.at<uchar>(x,y*3);
			cout << b << " ";
			int g = img.at<cv::Vec3b>(col_it, row_it)[1]; //g = image.at<uchar>(x, y * 3 + 1);
			cout << g << " ";
			int r = img.at<cv::Vec3b>(col_it, row_it)[2]; //r = image.at<uchar>(x, y * 3 + 1);
			cout << r << endl;
		}
	}
}

//返回的bgr
cv::Vec3b PngLoad::getPixel(int row, int col)
{
	//读取
	return img.at<cv::Vec3b>(row, col); // b = image.at<uchar>(x,y*3);
}

void PngLoad::setPixel(int row, int col, cv::Vec3b rgb)
{
	//b
	img.at<cv::Vec3b>(row, col)[0] = rgb[2];
	//g
	img.at<cv::Vec3b>(row, col)[1] = rgb[1];
	//r
	img.at<cv::Vec3b>(row, col)[2] = rgb[0];
}

