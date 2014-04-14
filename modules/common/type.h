/*
 * type.h
 *
 *  Created on: 2014年3月30日
 *      Author: Feng
 */

#ifndef __TYPE_H__
#define __TYPE_H__

#include <vector>
#include <deque>
#include <string>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <memory>


#include <opencv/cv.h>
#include <opencv2/nonfree/features2d.hpp>

namespace cires {

using std::size_t;
using std::vector;
using std::deque;
using std::string;
using std::unordered_map;
using std::tuple;
using std::make_tuple;
using std::sqrt;
using std::sort;
using std::max;
using std::min;
using std::max_element;
using std::stringstream;
using std::invalid_argument;
using std::runtime_error;
using std::out_of_range;
using std::strlen;
using std::nullptr_t;
using std::shared_ptr;
using std::forward;
using std::move;

typedef cv::Size size;
using cv::Mat;
using cv::Mat_;
using cv::KeyPoint;
using cv::DataType;
using cv::SURF;
using cv::FileStorage;
using cv::FileNode;
using cv::CommandLineParser;

/*
 * Cires specific typedef
 */

typedef unsigned int id_type;
typedef unsigned long long hash_type;

}
#endif /* __TYPE_H__ */
