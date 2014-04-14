/*
 * ImageRetriever.hpp
 *
 *  Created on: 2014年3月2日
 *      Author: Feng
 */

#ifndef __IMAGERETRIEVER_HPP__
#define __IMAGERETRIEVER_HPP__

#include <common/common.h>

#include <features/bowhe_descriptor.h>
#include <features/local_descriptors.h>
#include <index/inverted_index.h>
#include <index/metainfo.h>


namespace cires {

struct Params
{
	float hessian = 400;  //!< the default hessian setting for surf detector.
	int voclen = 5000;    //!< the vocabulary length.
	int binarylen = 64;   //!< the binary length for hamming embedding.
	Mat vocabulary;

	std::string dbdir = ".";
	std::string tmpdir;
};

class ImageRetriever
{
	public:

		typedef BoWHEFeatures<LocalFeatureExtractor<DefaultLFE> > descext_type;
		typedef ImageMetaInfoIndex                                minfoindex_type;
		typedef InvertedIndex                                     invindex_type;

		typedef typename minfoindex_type::id_type                 imageid_type;

		ImageRetriever();
		void initConfig( const Params& param );


		void load( const FileNode& fn );
		void store( FileStorage& fs );

		void addImage( const cv::Mat& image, const std::string& refname );

		vector<tuple<string, float>> queryImage( const Mat& image, const string& refname = "" );

	private:
		Mat reformed( const cv::Mat& image );

	private:
		double          m_maximglen;
		minfoindex_type m_metainfo;
		descext_type    m_bwextractor;
		invindex_type   m_invindexer;
};

}
#endif /* __IMAGERETRIEVER_HPP__ */
