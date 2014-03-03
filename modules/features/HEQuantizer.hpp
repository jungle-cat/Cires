/*
 * HEQuantizer.hpp
 *
 *  Created on: 2014年3月1日
 *      Author: Feng
 */

#ifndef __HEQUANTIZER_HPP__
#define __HEQUANTIZER_HPP__

#include "precomp.hpp"

CIRES_BEGIN_NAMESPACE


class HEQuantizer
{
	public:
		typedef unsigned long long hash_type;

		HEQuantizer( const cv::Mat& proj )
		{
		}


		void train( const std::vector<cv::Mat> descriptors, std::size_t bitnum )
		{
			if (descriptors.empty())
				throw std::invalid_argument("No descriptors is provided for training.");
			if (descriptors[0].type() != cv::DataType<float>::type || descriptors[0].channels() != 1)
				throw std::invalid_argument("Only single channel float descriptors are supported.");

			const int dims = descriptors[0].cols;

			if (m_proj.empty())
				m_proj = initProj(dims, bitnum);
			if (m_proj.cols != dims)
				throw std::invalid_argument("Projection matrix and descriptors don't match.");

			int wordnum = descriptors.size();
			m_threshs.resize(wordnum);

			for ( int i = 0; i < descriptors.size(); ++i ) {
				auto desc = descriptors[i];
				auto z = desc * m_proj.t();
				const int num = desc.rows;

				auto thresh = m_threshs[i];
				thresh.create(1, dims);

				cv::Mat_<float> _desc(dims, num);
				static_cast<cv::Mat_<float> >(z.t()).copyTo(_desc);
				const int mid_idx = num / 2;

				for ( int j = 0; j < dims; ++j) {
					float* ptr = _desc[j];
					std::nth_element(ptr, ptr+mid_idx, ptr+num);

					float mid = ptr[mid_idx];
					thresh(j) = mid;
				}
			}
		}

		/*!
		 * @brief Generate a hash bit for single row descriptor of the specified word.
		 * @param descriptor Single row descriptor of type CV_32FC1.
		 * @param widx The word index of the given descriptor.
		 * @return The hash code of maximumly 64bit.
		 */
		hash_type compute( const cv::Mat& descriptor, std::size_t widx )
		{
			if (descriptor.rows != 1 || descriptor.type() != cv::DataType<float>::type)
				throw std::invalid_argument("Only CV_32FC1 and single row descriptor is supported.");

			const std::size_t bitnum = m_proj.rows;
			if (bitnum > 64)
				throw std::invalid_argument("Maximumly 64bit is supproted for Hamming Embedding.");

			cv::Mat z = descriptor * m_proj.t();
			cv::Mat_<unsigned char> cr = z > m_threshs[widx];

			hash_type signature = 0;
			for (int i = 0; i < cr.cols; cr++) {
				unsigned char c = cr(i);

				hash_type b = hash_type(0x01) << i;
				if (c > 0)
					signature |= b;
			}
			return signature;
		}

		/*!
		 * @brief Get the projection matrix.
		 * @return Projection matrix with type CV_32FC1.
		 */
		cv::Mat projMatrix() const
		{
			return m_proj;
		}

		/*!
		 * @brief Get the hash code length of the Hamming Embedding.
		 * @return The length of hash code.
		 */
		std::size_t hash_size() const
		{
			return m_proj.rows;
		}

		void load( cv::FileNode& ns )
		{
		}

		void store( cv::FileStorage& fs )
		{
		}

	private:
		/*!
		 * @brief Initial projection matrix by QR factorization of random Gaussian matrix.
		 * @param dims
		 * @param dd
		 * @return Projection matrix of CV_32FC1.
		 */
		cv::Mat initProj( int dims, int dd )
		{
			cv::Mat m = Algnum::Gaussian(dims, dims);
			cv::Mat p = std::get<0>(Algnum::QR(m));
			p = p.rowRange(0, dd);
			return p;
		}


	private:
		cv::Mat_<float>               m_proj;
		std::vector<cv::Mat_<float> > m_threshs;
};

CIRES_END_NAMESPACE
#endif // __HEQUANTIZER_HPP__
