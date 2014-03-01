/*
 * LocalFeatures.hpp
 *
 *  Created on: 2014年3月1日
 *      Author: Feng
 */

#ifndef LOCALFEATURES_HPP_
#define LOCALFEATURES_HPP_

#include "precomp.hpp"

typedef cv::SURF DefaultLFE;

template <typename LFEImpl = DefaultLFE>
class LocalFeatureExtractor
{
	public:
		template <typename ...Args>
		LocalFeatureExtractor(Args... args)
			: m_extractor(std::forward<Args...>(args))
		{
		}

		std::tuple<std::vector<cv::KeyPoint>, cv::Mat>
		compute( const cv::Mat& image )
		{
			std::vector<cv::KeyPoint> keypoints;
			cv::Mat descriptors;
			m_extractor.compute(image, keypoints, descriptors);

			return std::make_tuple(std::move(keypoints), descriptors);
		}

		void load( const cv::FileNode& ns )
		{
		}

		void store( const cv::FileStorage fs ) const
		{
		}

		int size() const
		{
			return m_extractor.descriptorSize();
		}

	private:
		LFEImpl m_extractor;
};


#endif /* LOCALFEATURES_HPP_ */
