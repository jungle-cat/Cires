/*
 * main.cpp
 *
 *  Created on: 2014年2月24日
 *      Author: Feng
 */


#include <iostream>
#include <memory>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <features/LocalFeatures.hpp>
#include <features/BagofWords.hpp>

int main()
{
	cires::LocalFeatureExtractor<> detector(400);

	std::string name = "C:/Users/Feng/Pictures/original_5xuj_08e10001e492125f.jpg";
	cv::Mat img = cv::imread(name).clone();
	if (img.empty())
		std::cerr << "Error: failed to load image.\n";

	auto ret = detector.compute(img);

	std::vector<cv::KeyPoint> keypoints = std::get<0>(ret);
	cv::Mat descriptors = std::get<1>(ret);

	cires::BoWFeatures bow(descriptors);

	cv::Mat feature = descriptors.rowRange(1,7);
	cv::Mat label = bow.assign(feature,9);
	std::cout << label << std::endl;

std::cout << detector.size() << std::endl;
std::cout << descriptors.rows << std::endl;
	cv::Mat draw;
	cv::drawKeypoints(img, keypoints, draw, cv::Scalar(0,0,255));
	cv::imshow("test", draw);
	cv::waitKey();
	return 0;
}
