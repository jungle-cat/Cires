/*
 * BoWHEFeatures.hpp
 *
 *  Created on: 2014年3月2日
 *      Author: Feng
 */

#ifndef __BOWHEFEATURES_HPP__
#define __BOWHEFEATURES_HPP__

#include "precomp.hpp"

CIRES_BEGIN_NAMESPACE


template <typename LocalFeatureType>
class BoWHEFeatures
{
	public:
		typedef HEQuantizer::hash_type hash_type;
		typedef unsigned int           word_type;
		typedef std::tuple<word_type, cv::KeyPoint, hash_type> keypoint_type;

		BoWHEFeatures()
		{
		}

		void train( const cv::Mat& descriptors, const cv::Mat& vocab, int bitnum )
		{
			if (descriptors.cols != vocab.cols || descriptors.type() != vocab.type())
				throw std::invalid_argument("Descriptors and vocabulary don't match.");
			if (descriptors.type() != cv::DataType<float>::type)
				throw std::invalid_argument("Only single channel float matrix is supported.");

			const std::size_t dims = descriptors.cols;
			const std::size_t voc_size = vocab.rows;

			cv::Mat proj = HEQuantizer::initProj(dims, bitnum);

			m_bow = BoWFeatures(vocab);
			cv::Mat_<int> indice = m_bow.assign(descriptors);
			std::vector<std::vector<int>> indices(voc_size);
			for (int r = 0; r < indice.rows; ++r) {
				int idx = indice(r);
				auto& label = indices[idx];
				label.push_back(r);
			}

			std::vector<cv::Mat> descs;
			descs.reserve(indices.size());
			for (int i = 0; i < indices.size(); ++i) {
				auto& label = indices[i];
				const std::size_t num = label.size();

				// merge descriptor of same word
				cv::Mat_<float> desc_word(num, dims);
				for (int j = 0; j < label.size(); ++j) {
					int l = label[j];
					descriptors.row(l).copyTo(desc_word.row(j));
				}

				descs.push_back(desc_word);
			}

			m_he.train(descs, bitnum);
		}

		std::tuple<cv::Mat, cv::Mat> train( const cv::Mat& descriptors, int wordnum, int bitnum )
		{
			cv::Mat vocab = m_bow.train(descriptors, wordnum);
			cv::Mat median = train(descriptors, vocab, bitnum);
			return std::make_tuple(vocab, median);
		}

		std::vector<keypoint_type> compute( const std::vector<cv::KeyPoint>& keypoints, const cv::Mat& descriptors )
		{
			if (keypoints.size() != descriptors.rows || descriptors.channels() != 1 || descriptors.type() != cv::DataType<float>::type)
				throw std::invalid_argument("Keypoints and descriptors don't match and only single channel float descriptors are supported.");

			const std::size_t num = keypoints.size();
			cv::Mat_<int> labels = m_bow.assign(descriptors);

			std::vector<keypoint_type> info;
			info.reserve(num);

			for(int i = 0; i < num; ++i) {
				const int label = labels(i);
				auto desc = descriptors.row(i);
				auto keypoint = keypoints[i];

				hash_type signature = m_he.compute(desc, label);
				info.push_back(std::make_tuple(label, keypoint, signature));
			}

			return std::move(info);
		}

		std::vector<keypoint_type> compute( const cv::Mat& image )
		{
			auto ret = m_lfextractor.compute(image);
			auto keypoints = std::get<0>(ret);
			auto descriptors = std::get<1>(ret);

			return compute( keypoints, descriptors );
		}

		void load( const cv::FileNode& fn )
		{
		}

		void store( cv::FileStorage& fs )
		{
		}

	private:
		BoWFeatures   m_bow;
		HEQuantizer   m_he;
		LocalFeatureType m_lfextractor;
};


CIRES_END_NAMESPACE
#endif // __BOWHEFEATURES_HPP__
