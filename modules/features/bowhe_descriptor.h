/*
 * bowhe_descriptor.h
 *
 *  Created on: 2014年3月31日
 *      Author: Feng
 */

#ifndef __BOWHE_DESCRIPTOR_H__
#define __BOWHE_DESCRIPTOR_H__

#include <common/common.h>
#include "he_quantizer.h"
#include "bow_feature.h"
#include "local_descriptors.h"

namespace cires {


struct BoWHENode
{
	typedef HEQuantizer::hash_type hash_type;
	typedef id_type                word_type;

	word_type	word_id;
	float       orientation;
	float       logscale;
	hash_type   signature;

	BoWHENode( word_type wid, float o, float s, hash_type b )
		: word_id(wid), orientation(o), logscale(s), signature(b)
	{}

};

template <typename LocalFeatureType>
class BoWHEFeatures
{
	public:
		typedef HEQuantizer::hash_type hash_type;
		typedef id_type                word_type;
		typedef BoWHENode              fenode_type;

		typedef LocalFeatureType       lfextractor_type;


		BoWHEFeatures() = default;

		template <typename ...Args>
		BoWHEFeatures(const Mat& voc, const Mat& p, const Mat& th, Args... args)
			: m_bow(voc), m_he(p, th), m_lfextractor(forward<Args>(args)...)
		{
		}

		tuple<Mat, Mat> train( const Mat& desc, const Mat& vocab, int bitnum )
		{
			RuntimeCheck(desc.cols == vocab.cols && desc.type() == DataType<float>::type,
					"Error: descriptors and vocabulary do not match.");

			const size_t dims = desc.cols;
			const size_t voc_size = vocab.rows;


			m_bow = BoWFeatures(vocab);
			Mat_<int> indice = m_bow.assign(desc);
			vector<vector<int>> indices(voc_size);
			for (int r = 0; r < indice.rows; ++r) {
				int idx = indice(r);
				auto& label = indices[idx];
				label.push_back(r);
			}

			vector<Mat> descs;
			descs.reserve(indices.size());
			for (int i = 0; i < indices.size(); ++i) {
				auto& label = indices[i];
				const std::size_t num = label.size();

				// merge descriptor of same word
				cv::Mat_<float> desc_word(num, dims);
				for (int j = 0; j < label.size(); ++j) {
					int l = label[j];
					desc.row(l).copyTo(desc_word.row(j));
				}

				descs.push_back(desc_word);
			}

			return m_he.train(descs, bitnum);
		}

		tuple<Mat, Mat, Mat> train( const Mat& descriptors, int wordnum, int bitnum )
		{
			Mat vocab = m_bow.train(descriptors, wordnum);
			auto ret = train(descriptors, vocab, bitnum);
			return make_tuple(vocab, std::get<0>(ret), std::get<1>(ret));
		}

		/*!
		 * @param keypoints
		 * @param descriptors
		 * @return
		 */
		std::vector<fenode_type> compute( const std::vector<cv::KeyPoint>& keypoints, const cv::Mat& descriptors )
		{
			if (keypoints.size() != descriptors.rows || descriptors.channels() != 1 || descriptors.type() != cv::DataType<float>::type)
				throw std::invalid_argument("Keypoints and descriptors don't match and only single channel float descriptors are supported.");

			const std::size_t num = keypoints.size();
			cv::Mat_<int> labels = m_bow.assign(descriptors);

			std::vector<fenode_type> info;
			info.reserve(num);

			for(int i = 0; i < num; ++i) {
				const int label = labels(i);
				auto desc = descriptors.row(i);
				auto keypoint = keypoints[i];

				hash_type signature = m_he.compute(desc, label);
				info.emplace_back(label, keypoint.angle, keypoint.size, signature);
			}

			return std::move(info);
		}

		/*!
		 *  Compute the descriptors for a given grayscale image.
		 *  @param image The single channel 8-bit length image.
		 *  @return
		 */
		std::vector<fenode_type> compute( const cv::Mat& image )
		{
			auto ret = m_lfextractor.compute(image);
			auto keypoints = std::get<0>(ret);
			auto descriptors = std::get<1>(ret);

			return compute( keypoints, descriptors );
		}

		lfextractor_type& lfextractor()
		{
			return m_lfextractor;
		}

		void load( const cv::FileNode& fn )
		{
		}

		void store( cv::FileStorage& fs )
		{
		}

	private:
		BoWFeatures      m_bow;
		HEQuantizer      m_he;
		lfextractor_type m_lfextractor;
};

}
#endif /* __BOWHE_DESCRIPTOR_H__ */
