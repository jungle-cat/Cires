/*
 * BagofWords.hpp
 *
 *  Created on: 2014年3月3日
 *      Author: Feng
 */

#ifndef __BAGOFWORDS_HPP__
#define __BAGOFWORDS_HPP__

#include "precomp.hpp"

CIRES_BEGIN_NAMESPACE

class BoWFeatures
{
	public:
		typedef cv::flann::GenericIndex<cv::flann::L2<float>> index_type;
		typedef typename index_type::ElementType element_type;
		BoWFeatures()
		{
		}

		BoWFeatures( const cv::Mat& vocab )
		{
			if (vocab.type() != cv::DataType<float>::type)
				throw std::invalid_argument("Only single channel float features are supported.");

			m_vocab = vocab.clone();
			m_pindex.reset(new index_type(m_vocab, cvflann::KDTreeIndexParams()));
		}

		bool empty() const
		{
			return m_vocab.empty();
		}

		std::size_t size() const
		{
			return m_vocab.rows;
		}

		std::size_t dims() const
		{
			return m_vocab.cols;
		}

		void setVocabulary( const cv::Mat& vocab )
		{
			if (vocab.type() != cv::DataType<float>::type)
				throw std::invalid_argument("Only single channel float features are supported.");
			m_vocab = vocab.clone();
			m_pindex.reset(new index_type(m_vocab, cvflann::KDTreeIndexParams()));
		}

		cv::Mat train( const cv::Mat& descriptors, std::size_t num )
		{
			if (descriptors.type() != cv::DataType<float>::type)
				throw std::invalid_argument("Only single channel float features are supported.");

			cv::Mat labels, centers;
			cv::kmeans( descriptors,
						num,
						labels,
						cv::TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0),
						3,
						cv::KMEANS_PP_CENTERS,
						centers);
			return centers;
		}

		cv::Mat_<int> assign( const cv::Mat& descriptors, std::size_t maxnum = 1)
		{
			if (empty())
				throw std::runtime_error("Indexer and vocabulary are not initialized.");
			if (m_vocab.cols != dims() || m_vocab.type() != descriptors.type())
				throw std::invalid_argument("Descriptors and vocabularies don't match.");

			const std::size_t n = descriptors.rows;
			maxnum = std::min(maxnum, n);

			cv::Mat indices = cv::Mat_<int>(n, maxnum), dists = cv::Mat_<float>(n, maxnum);
			m_pindex->knnSearch(descriptors, indices, dists, maxnum, cvflann::SearchParams());

			return indices;
		}

		void load( const cv::FileNode& fn )
		{
		}

		void store( cv::FileStorage& fs )
		{
		}

	private:
		cv::Mat                      m_vocab;
		std::shared_ptr<index_type>  m_pindex;
};


CIRES_END_NAMESPACE
#endif // __BAGOFWORDS_HPP__
