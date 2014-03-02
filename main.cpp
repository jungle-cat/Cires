/*
 * main.cpp
 *
 *  Created on: 2014年2月24日
 *      Author: Feng
 */


#include <iostream>
#include <memory>

#include <opencv/cv.h>

#include <common/Algnum.hpp>

int main()
{
	 cv::Mat m = (cv::Mat_<double>(5, 3) << 12,-51,4,6,167,-68,-4,24,-41,-1,1,0,2,0,3);

//	cv::Mat m = (cv::Mat_<double>(3,3) << 12,-51,4,6,167,-68,-4,24,-41);
	std::cout << m << std::endl;

	std::tuple<cv::Mat, cv::Mat> ret = cires::Algnum::QR(m);
	auto q = std::get<0>(ret);
	auto r = std::get<1>(ret);
	std::cout << q << std::endl;
	std::cout << r << std::endl;
	std::cout << q*r << std::endl;
	return 0;
}
