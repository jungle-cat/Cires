/*
 * algnum.h
 *
 *  Created on: 2014年3月30日
 *      Author: Feng
 */

#ifndef __ALGNUM_H__
#define __ALGNUM_H__

#include "type.h"

namespace cires {

tuple<Mat, Mat> qr( const Mat& data );

Mat median( const Mat& data, int type );

}
#endif /* __ALGNUM_H__ */
