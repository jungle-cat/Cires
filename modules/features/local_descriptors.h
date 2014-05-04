/*
 * local_descriptors.h
 *
 *  Created on: 2014年3月31日
 *      Author: Feng
 */

#ifndef __LOCAL_DESCRIPTORS_H__
#define __LOCAL_DESCRIPTORS_H__

#include <common/common.h>

namespace cires {

typedef cv::SIFT DefaultLFE;

template <typename LFEImpl = DefaultLFE>
class LocalFeatureExtractor
{
	public:
		template <typename ...Args>
		LocalFeatureExtractor(Args... args)
			: m_extractor(forward<Args>(args)...)
		{
		}

		tuple<vector<KeyPoint>, Mat>
		compute( const Mat& image )
		{
			vector<KeyPoint> keypoints;
			Mat descriptors;
			m_extractor(image, Mat(), keypoints, descriptors);

			return make_tuple(move(keypoints), descriptors);
		}

		int size() const
		{
			return m_extractor.descriptorSize();
		}

	private:
		LFEImpl m_extractor;
};


}
#endif /* __LOCAL_DESCRIPTORS_H__ */
