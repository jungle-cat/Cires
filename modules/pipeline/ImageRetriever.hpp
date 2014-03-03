/*
 * ImageRetriever.hpp
 *
 *  Created on: 2014年3月2日
 *      Author: Feng
 */

#ifndef IMAGERETRIEVER_HPP_
#define IMAGERETRIEVER_HPP_


#include "precomp.hpp"

CIRES_BEGIN_NAMESPACE

class ImageRetriever
{
	public:

		void initConfig( const cv::FileNode& fn )
		{
		}

		void addImage( const cv::Mat& image, const std::string& refname )
		{
			auto id = m_metainfo.queryIDByName(refname);
			typedef ImageMetaInfoIndex<ImageMetaInfo> metainfo_t;
			typedef typename BoWHEFeatures<LocalFeatureExtractor<DefaultLFE> >::keypoint_type keypoint_t;
			if (id != metainfo_t::nindex)
				return;
			else {
				cv::Mat img = reformed(image);
				id = m_metainfo.insert({refname, image, img.size()});
				std::vector<keypoint_t> info = m_bwextractor.compute(img);
			}
		}

		void queryImage( const cv::Mat& image, const std::string& refname = "" )
		{
			cv::Mat img = reformed(image);
		}

	private:
		cv::Mat reformed( const cv::Mat& image )
		{
			int maxlen = std::max(image.rows, image.cols);
			cv::Mat resized;
			if (maxlen > m_maximglen) {
				double ratio = m_maximglen / static_cast<double>(maxlen);
				cv::resize(image, resized, cv::Size(), ratio, ratio);
			}
			else
				resized = image;

			cv::Mat gray;
			if (resized.channels() != 1)
				cv::cvtColor(resized, gray, cv::COLOR_BGR2GRAY);
			else
				gray = resized;

			cv::Mat equalized;
			cv::equalizeHist(gray, equalized);

			return equalized;
		}


	private:
		double   m_maximglen;
		ImageMetaInfoIndex<ImageMetaInfo>                 m_metainfo;
		BoWHEFeatures<LocalFeatureExtractor<DefaultLFE> > m_bwextractor;

};

CIRES_END_NAMESPACE
#endif /* IMAGERETRIEVER_HPP_ */
