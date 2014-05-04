/*
 * he_quantizer.cpp
 *
 *  Created on: 2014年3月30日
 *      Author: Feng
 */

#include "he_quantizer.h"
#include <common/algnum.h>

using namespace cires;

static
Mat initProj(int dims, int bitlen, int dtype)
{
	Mat m(dims, dims, dtype);
	cv::randn(m, cv::Scalar::all(0), cv::Scalar::all(1));
	auto p = std::get<0>(qr(m));
	return p.rowRange(0, bitlen);
}

tuple<Mat, Mat> HEQuantizer::train( const vector<Mat>& descs, size_t bitlen )
{
	RuntimeCheck(!descs.empty(), "No descriptors are provided for training.");

	const int dims = descs[0].cols;
	if (m_proj.empty())
		m_proj = initProj(dims, bitlen, DataType<float>::type).t();
	RuntimeCheck(m_proj.rows == dims, "Error: projection matrix and descriptors do not match.");

	const int wordnum = descs.size();
	m_threshs.create(wordnum, bitlen, DataType<float>::type);

	for ( int i = 0; i < wordnum; ++i ) {
		auto& desc = descs[i];
		Mat z = desc * m_proj;
		RuntimeCheck(z.cols == bitlen, "Error: transformed features.");
		const int num = desc.rows;

		// find median value
		Mat_<float> thresh = m_threshs.row(i);
		const int mid_idx = num / 2;
		for (size_t j = 0; j < bitlen; ++j) {
			Mat zc = z.col(j);
			std::vector<float> vals;
			vals.reserve(zc.cols);
			for (int k = 0; k < zc.rows; ++k) {
				vals.push_back(zc.at<float>(k));
			}

			std::nth_element(vals.begin(), vals.begin() + mid_idx, vals.end());
			thresh(j) = vals[mid_idx];
		}
	} //

	return make_tuple(m_proj, m_threshs);
}

HEQuantizer::hash_type HEQuantizer::compute( const Mat& desc, HEQuantizer::word_type word ) const
{
	RuntimeCheck(desc.cols == m_proj.rows, "Error: descriptors and projection matrix do not match.");
	RuntimeCheck(m_proj.cols <= 64, "Error: maximally 64 bit hashing is supported.");

	const size_t bitlen = m_proj.cols;
	Mat_<unsigned char> cr = ( desc * m_proj ) > m_threshs.row(word);

	hash_type signature = 0x0;
	for ( int i = 0; i < cr.cols; ++i) {
		unsigned char c = cr(i);
		if (c > 0)
			signature |= hash_type(0x01) << i;
	}
	return signature;
}
