/*
 * HEQuantizer.hpp
 *
 *  Created on: 2014年3月1日
 *      Author: Feng
 */

#ifndef __HEQUANTIZER_HPP__
#define __HEQUANTIZER_HPP__

#include "precomp.hpp"

CIRES_BEGIN_NAMESPACE

class HEQuantizer
{
	public:
		HEQuantizer( const cv::Mat& proj )
		{
		}

		void train( const cv::Mat descriptors )
		{
			const int d = descriptors.cols;
			const int r = 20;

			// random matrix generation
			cv::Mat m = Algnum::Gaussian(d, d);
			cv::Mat p = std::get<0>(Algnum::QR(m));
			p = p.rowRange(0, r);

			// descriptor projection and assignment
			cv::Mat z = descriptors * p.t();

			// median
		}

		void load( cv::FileNode& ns )
		{
		}

		void store( cv::FileStorage& fs )
		{
		}

		void compute( const cv::Mat& descriptors )
		{
		}

	private:
};

CIRES_END_NAMESPACE
#endif // __HEQUANTIZER_HPP__
