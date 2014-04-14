/*
 * bow_feature.h
 *
 *  Created on: 2014年3月31日
 *      Author: Feng
 */

#ifndef __BOW_FEATURE_H__
#define __BOW_FEATURE_H__

#include <common/common.h>


namespace cires {

class BoWFeatures
{
	public:
		typedef cv::flann::GenericIndex<cvflann::L2<float> > indexer_type;
		typedef typename indexer_type::ElementType           element_type;

		BoWFeatures( const Mat& voc = Mat() );
		bool empty() const;

		size_t size() const;
		size_t dims() const;

		Mat train( const Mat& descs, size_t num );
		Mat_<int> assign( const Mat& desc, size_t max_match = 1);


	private:
		Mat                    m_vocab;
		shared_ptr<indexer_type> m_pindex;
};

}
#endif /* __BOW_FEATURE_H__ */
