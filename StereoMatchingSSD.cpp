#include "stdafx.h"
int sum(const cv::Mat& img1, const cv::Mat& img2);
cv::Mat disparity(const cv::Mat& img1, const cv::Mat& img2);
cv::Mat disp_img(const cv::Mat& img1, const cv::Mat& img2,const int blockSize, const int pixelRange, const int gauss);

int main()
{
	
	cv::Mat img1 = cv::imread("./im2.png", 0);
	cv::Mat img2 = cv::imread("./im6.png", 0);
	
	if (img1.cols != img2.cols || img1.rows != img2.rows)
	{
		cout << "wrong dimensions" << endl;
		return -1;
	}
	//cv::copyMakeBorder(img1, img1, 1, 1, 1, 1, cv::BORDER_CONSTANT, 0);
	//cv::copyMakeBorder(img2, img2, 1, 1, 1, 1, cv::BORDER_CONSTANT, 0);
	cv::Mat disp = cv::Mat::zeros(img1.rows, img1.cols, CV_8UC1);
	cout << disp.channels() << img1.channels()<<endl;
	//Block size must be odd
	//disp = disp_img(img1, img2, 9, 75,2);
	//disp = disparity(img1,img2);
	
	//cv::imshow("das", disp);
	//cv::waitKey(0);
	
	return 0;
}

int sum(const cv::Mat& img1, const cv::Mat& img2) //Sum of Squared difference
{
	cv::Mat src = abs(img1 - img2);
	int sum=0;
	for (int i = 0; i < img1.cols; i++)
	{
		for (int j = 0; j < img1.rows; j++)
		{
			src.at<uchar>(i, j) = src.at<uchar>(i, j)*src.at<uchar>(i, j);
			sum += src.at<uchar>(i, j);
		}
	}
	return sum;
}

cv::Mat disp_img(const cv::Mat& img1, const cv::Mat& img2, const int blockSize,const int pixelRange,const int gauss)
{	

	cv::Mat blur1, blur2;
	cv::GaussianBlur(img1, blur1,cv::Size(1 + 2 * gauss, 1 + 2 * gauss), cv::BORDER_DEFAULT);
	cv::GaussianBlur(img2, blur2, cv::Size(1 + 2 * gauss, 1 + 2 * gauss), cv::BORDER_DEFAULT);

	cv::Mat disp = cv::Mat::zeros(img1.rows, img1.cols, CV_8UC1);
	int offset_adjust = int(255 / pixelRange);//Map depth map to 0-255 range
	int halfBlockSize = int(blockSize / 2);
	for (int y = halfBlockSize; y < img1.rows - halfBlockSize; y++)
	{
		for (int x = halfBlockSize;x < img1.cols - halfBlockSize; x++)
		{
			int best_offset = 0;
			int prev_ssd = 65534;
			cv::Mat roi = blur1(cv::Rect(x - halfBlockSize, y- halfBlockSize, blockSize, blockSize));
			for (int range = x-pixelRange; range < x; range++)
			{
				if (range < (blockSize- halfBlockSize)) { range = blockSize - halfBlockSize; } // to avoid over-flow error while slicing right view image into blocks
				cv::Mat img2_roi = blur2(cv::Rect(range - halfBlockSize, y - halfBlockSize, blockSize, blockSize));
				int ssd= sum(roi, img2_roi);
				if (ssd < prev_ssd)
				{
					prev_ssd = ssd;
					best_offset = range;
				}
			}	
			disp.at<uchar>(y, x) = (x-best_offset)*offset_adjust;
		}
	}
	return disp;
}

/*
cv::Mat disparity(const cv::Mat& img1, const cv::Mat& img2)
{
	int SSD = 0;
	//Store disparity map and SSD
	cv::Mat disp = cv::Mat::zeros(img1.rows, img1.cols, CV_8UC1);
	double offset = 255 / 50; //Map depth map to 0-255 range
	for (int i = 1; i < img1.rows - 1; i++)
	{
		for (int j = 1; j < img1.cols - 1; j++)
		{
			cv::Mat roi = img1(cv::Rect(j - 1, i - 1, 3, 3));
			int match_left = 0;
			int min_ssdlft = 10000;
			for (int idx = j - 50; idx<j; ++idx) //search on 1 side
			{
			if (idx < 1) { idx = 1; }            // to avoid over-flow error while slicing right view image into blocks

				cv::Mat img2_roi = img2(cv::Rect(idx - 1, i - 1, 3, 3));
				SSD = sum(roi, img2_roi);
				if (SSD < min_ssdlft) //compare to find least SSD
				{
					min_ssdlft = SSD;
					match_left = idx;
				}
			}
			disp.at<uchar>(i, j) = offset*(j - match_left); //disparity
		}
	}
	return disp;
}*/