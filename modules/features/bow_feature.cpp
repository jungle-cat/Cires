/*
 * bow_feature.cpp
 *
 *  Created on: 2014年3月31日
 *      Author: Feng
 */


#include "bow_feature.h"

using namespace cires;

BoWFeatures::BoWFeatures( const Mat& voc )
{
	if (!voc.empty()) {
		RuntimeCheck(voc.type() == DataType<float>::type, "Error: must be single channel float matrix.");
		m_vocab = voc;
		m_pindex.reset(new indexer_type(m_vocab, cvflann::KDTreeIndexParams()));
	}
}

bool BoWFeatures::empty() const
{
	return m_vocab.empty();
}

size_t BoWFeatures::size() const
{
	return m_vocab.rows;
}

size_t BoWFeatures::dims() const
{
	return m_vocab.cols;
}

Mat BoWFeatures::train( const Mat& descs, size_t num )
{
	RuntimeCheck(descs.type() == DataType<float>::type, "Error: must be single channel float matrix.");

	Mat labels, centers;
	cv::kmeans( descs,
				num,
				labels,
				cv::TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0),
				3,
				cv::KMEANS_PP_CENTERS,
				centers);

	if (not centers.empty())
		m_vocab = centers;
	return centers;
}

Mat_<int> BoWFeatures::assign( const Mat& descs, size_t max_matches )
{
	RuntimeCheck(!empty() && descs.cols == dims() && descs.type() == DataType<float>::type,
			"Error: descriptors do not match the vacabulary.");
	const int n = descs.rows;
	const int maxnum = min(int(max_matches), n);
	Mat indices = Mat_<int>(n, maxnum), dists = Mat_<float>(n, maxnum);
	m_pindex->knnSearch(descs, indices, dists, maxnum, cvflann::SearchParams());

	return indices;
}
