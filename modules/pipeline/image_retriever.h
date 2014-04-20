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
	int imgmaxlen = 720;

	Mat vocabulary;
	Mat projection;
	Mat thresholds;

	void store( FileStorage& fs ) const
	{
		fs << "hessian" << hessian
		   << "voclen" << voclen
		   << "binarylen" << binarylen
		   << "imgmaxlen" << imgmaxlen
		   << "vocabulary" << vocabulary
		   << "projection" << projection
		   << "thresholds" << thresholds;
	}

	void load( const FileNode& fn )
	{
		fn["hessian"] >> hessian;
		fn["voclen"] >> voclen;
		fn["binarylen"] >> binarylen;
		fn["imgmaxlen"] >> imgmaxlen;
		fn["vocabulary"] >> vocabulary;
		fn["projection"] >> projection;
		fn["thresholds"] >> thresholds;
	}
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


		void addImage( const Mat& image, const std::string& refname );

		/*!
		 * @brief Given a set of images, train the retrieval system.
		 * @param dirname The directory which contains a set of images.
		 */
		Params train( const string& dirname, const Params& preconf );

		/*!
		 *  @brief Rebuild the index for the retrieval system.
		 *   	   Only InvertedIndex is affected.
		 */
		void rebuild();

		vector<tuple<string, float>> queryImage( const Mat& image, const string& refname = "" );

	private:
		Mat reformed( const Mat& image, int maxsize );

		Mat reformed( const Mat& image )
		{
			return reformed(image, m_maximglen);
		}

	private:
		double          m_maximglen;
		minfoindex_type m_metainfo;
		descext_type    m_bwextractor;
		invindex_type   m_invindexer;
};

}
#endif /* __IMAGERETRIEVER_HPP__ */
