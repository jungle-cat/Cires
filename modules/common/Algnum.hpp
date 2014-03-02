/*
 * Algnum.hpp
 *
 *  Created on: 2014年3月1日
 *      Author: Feng
 */

#ifndef ALGNUM_HPP_
#define ALGNUM_HPP_

#include "precomp.hpp"
CIRES_BEGIN_NAMESPACE


class Algnum
{
	public:
		enum : int{
			ROW = 0,
			COL = 1
		};

		static
		std::tuple<cv::Mat, cv::Mat> QR( const cv::Mat& data )
		{
			cv::Mat q, r;
			return std::make_tuple(q, r);
		}

		static
		cv::Mat Gaussian( int rows, int cols, float mu = 0, float sigma = 1 )
		{
			cv::Mat g;

			return g;
		}

		static
		cv::Mat median( const cv::Mat& data, int type = ROW)
		{
			cv::Mat g;

			return g;
		}


	private:
};

CIRES_END_NAMESPACE
#endif /* ALGNUM_HPP_ */
