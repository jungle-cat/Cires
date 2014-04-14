/*
 * algonum.cpp
 *
 *  Created on: 2014年3月30日
 *      Author: Feng
 */

#include "algnum.h"

using namespace cires;

/*
 * Perform QR decomposition using householder reflection.
 * @param data
 * @param
 */
tuple<Mat, Mat> cires::qr( const Mat& data )
{
	/*
	 * http://en.wikipedia.org/wiki/QR_decomposition#Using_Householder_reflections
	 */
	if (data.channels() != 1)
		throw std::invalid_argument("QR only support single channel matrix.");

	cv::Mat_<double> _data;
	if (data.depth() != CV_64F)
		data.convertTo(_data, cv::DataType<double>::type);
	else
		_data = data;

	const int rows = _data.rows, cols = _data.cols;
	const cv::Mat_<double> I = cv::Mat_<double>::eye(rows, rows);
	cv::Mat_<double> q = I.clone(), r;

	const int itnum = std::min(_data.rows - 1, _data.cols);
	cv::Mat_<double> A = _data.clone(), A2;
	for (int i = 0; i < itnum; ++i) {
		A2 = cv::Mat_<double>::eye(A.size());
		cv::Rect roi = {i, i, cols-i, rows-i};
		A(roi).copyTo(A2(roi));

		auto x = A2.col(i);
		double alpha = cv::norm(x, cv::NORM_L2);
		if (_data(i,i) > 0) alpha = -alpha;

		auto e = I.col(i);
		auto u = x + alpha * e;
		u = u / cv::norm(u, cv::NORM_L2);

		auto Q = I - 2*u*u.t();
		A = Q*A2;

		// remember q
		q = Q*q;
	}

	r = q * _data;
	q = q.t();

	if (data.depth() != CV_64F) {
		cv::Mat q1, r1;
		q.convertTo(q1, data.type());
		r.convertTo(r1, data.type());
		return std::make_tuple(q1, r1);
	}
	else {
		return std::make_tuple(q, r);
	}
}
