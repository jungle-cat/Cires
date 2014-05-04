/*
 * image_retriever.cpp
 *
 *  Created on: 2014年3月31日
 *      Author: Feng
 */

#include "image_retriever.h"
#include <opencv/highgui.h>

#include <boost/filesystem.hpp>


using namespace cires;
using namespace boost::filesystem;

using cv::imread;

ImageRetriever::ImageRetriever()
	: m_maximglen(720)
{}

void ImageRetriever::initConfig( const Params& param )
{
	m_bwextractor = descext_type(param.vocabulary,
								 param.projection,
								 param.thresholds,
								 param.hessian);
	m_maximglen = param.imgmaxlen;
	m_binthresh = param.binthresh;
}


void ImageRetriever::addImage( const Mat& image, const string& name )
{
	imageid_type id = m_metainfo.get_id_by_name(name);
	if (id != minfoindex_type::nindex) {
		std::cerr << "\tInfo: image already exists.\n";
		return;
	}

	Mat img = reformed(image);
	id = m_metainfo.insert(name, image, img.size());
	auto descinfo = m_bwextractor.compute(img);

	for (std::size_t i = 0; i < descinfo.size(); ++i) {
		auto& desc = descinfo[i];

		auto& word_id = desc.word_id;
		m_invindexer.add( word_id,           // word id
						  id,                // image id
						  i,                 // descriptor id
						  desc.signature,
						  desc.orientation,
						  desc.logscale);
	}
}

Params ImageRetriever::train( const string& dirname, const Params& preconf )
{
	path dir = dirname;
	RuntimeCheck(is_directory(dir), "Error: failed to load image directory.");

	vector<string> names;
	// load image names
	for (auto it = directory_iterator(dir); it != directory_iterator(); ++it)
		names.push_back((it->path()).string());

	// shuffle the images
	std::random_shuffle(names.begin(), names.end());

	const bool with_vocabulary = not preconf.vocabulary.empty();
	const float hessian = preconf.hessian;
	const int voclen = preconf.voclen;
	const int binlen = preconf.binarylen;
	const int imgmaxlen = preconf.imgmaxlen;

	// load images and extract local feature for training.
	auto lfextractor = m_bwextractor.lfextractor();
	const int maxnum = 1000 * voclen;
	const int dims = lfextractor.size();
	Mat descs(maxnum, dims, DataType<float>::type); //TODO add auto configuration for feature type.
	int count = 0;
	for (auto it = names.begin(); it != names.end(); ++it) {
		std::cout << "\t" << *it << std::endl;
		Mat image = imread(*it);
		image = reformed(image, imgmaxlen);

		auto lfs = lfextractor.compute(image);
		Mat desc = std::get<1>(lfs);
		int num = desc.rows;

		desc.copyTo(descs.rowRange(count, count+num));
		count += num;

		if (count > maxnum)
			break;
	}

	// copy the valid descriptors for training.
	if (maxnum > count)
		descs = descs.rowRange(0, count);

	Params param = preconf;

	// train the parameters and store in Param struct.
	if (with_vocabulary) {
		auto ret = m_bwextractor.train(descs, preconf.vocabulary, binlen);
		param.projection = std::get<0>(ret);
		param.thresholds = std::get<1>(ret);
	}
	else {
		auto ret = m_bwextractor.train(descs, voclen, binlen);
		param.vocabulary = std::get<0>(ret);
		param.projection = std::get<1>(ret);
		param.thresholds = std::get<2>(ret);
	}

	return param;
}

void ImageRetriever::rebuild()
{
}

vector<tuple<string, float>> ImageRetriever::queryImage( const Mat& image, const string& name)
{
	Mat img = reformed(image);
	auto descinfo = m_bwextractor.compute(img);
	auto rets = m_invindexer.query(descinfo.begin(), descinfo.end(), m_binthresh);

	vector<tuple<string, float>> results;
	results.reserve(rets.size());

	for (auto& x : rets) {
		auto image_id = std::get<0>(x);
		auto score = std::get<1>(x);
		string name = m_metainfo.get_name_by_id(image_id);
		results.emplace_back(name, score);
	}
	return move(results);
}


Mat ImageRetriever::reformed( const Mat& image, int maxsize )
{
	int maxlen = std::max(image.rows, image.cols);
	cv::Mat resized;
	if (maxlen > maxsize) {
		double ratio = maxsize / static_cast<double>(maxlen);
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
