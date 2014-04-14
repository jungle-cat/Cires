/*
 * image_retriever.cpp
 *
 *  Created on: 2014年3月31日
 *      Author: Feng
 */

#include "image_retriever.h"

using namespace cires;

ImageRetriever::ImageRetriever()
	: m_maximglen(720)
{}

void ImageRetriever::initConfig( const Params& param )
{
}


void ImageRetriever::addImage( const Mat& image, const string& name )
{
	imageid_type id = m_metainfo.get_id_by_name(name);
	if (id != minfoindex_type::nindex)
		return;

	Mat img = reformed(image);
	id = m_metainfo.insert(name, image, img.size());
	auto descinfo = m_bwextractor.compute(img);

	m_invindexer.add(id, descinfo.begin(), descinfo.end());
}

vector<tuple<string, float>> ImageRetriever::queryImage( const Mat& image, const string& name)
{
	Mat img = reformed(image);
	auto descinfo = m_bwextractor.compute(img);
	auto rets = m_invindexer.query(descinfo.begin(), descinfo.end());

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


Mat ImageRetriever::reformed( const Mat& image )
{
	int maxlen = std::max(image.rows, image.cols);
	cv::Mat resized;
	if (maxlen > m_maximglen) {
		double ratio = m_maximglen / static_cast<double>(maxlen);
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
