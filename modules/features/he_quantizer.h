/*
 * he_quantizer.h
 *
 *  Created on: 2014年3月30日
 *      Author: Feng
 */

#ifndef HAMMING_EMBEDDING_H_
#define HAMMING_EMBEDDING_H_

#include <common/common.h>

namespace cires {

class HEQuantizer
{
	public:
		typedef unsigned long long  hash_type;
		typedef id_type   word_type;

		HEQuantizer() = default;

		HEQuantizer( const Mat& proj )
			: m_proj(proj)
		{}

		void train( const vector<Mat>& descs, size_t bitlen );
		hash_type compute( const Mat& descr, word_type word_id ) const;

	private:
		Mat  m_proj;
		Mat  m_threshs;
};

}
#endif /* HAMMING_EMBEDDING_H_ */
