/*
 * main.cpp
 *
 *  Created on: 2014年3月31日
 *      Author: Feng
 */

#include <pipeline/image_retriever.h>
#include <pipeline/multimedia_server.h>

#include <boost/filesystem.hpp>

#include <opencv/cv.h>
#include <opencv/highgui.h>

namespace filesystem = boost::filesystem;



class Main
{
	public:
		typedef std::tuple<std::string, float> candidate_type;


		void start_retriever( const cires::Params& param )
		{
			retriever.initConfig(param);
		}

		void start_retriever( const std::string& configure )
		{
			cv::FileStorage fs(configure, cv::FileStorage::READ);
			if (!fs.isOpened())
				throw std::runtime_error("Error: failed to load configuration.");
			retriever.rebuild();
		}

		void train_system( const std::string& srcdir, const std::string& outconf, const cires::Params& preconf )
		{
			std::cerr << "Starting traning ... \n";
			cires::Params param = retriever.train(srcdir, preconf);
			std::cerr << "Finished trianing .... \n";

			cv::FileStorage fs(outconf, cv::FileStorage::WRITE);
			param.store(fs);
		}


		/*
		 *  Load images for a given directory.
		 *  @param dir The directory of the image set.
		 */
		void load_images( const std::string& dir )
		{
			filesystem::path directory(dir);

			if (not filesystem::is_directory(directory)) {
				std::cerr << "Error: failed to locate image directory.\n";
				return;
			}

			int num = 0;
			for (auto it = filesystem::directory_iterator(directory);
					it != filesystem::directory_iterator(); ++it, ++num) {
				const filesystem::path imagepath = directory / (*it);
				cv::Mat image = cv::imread(imagepath.string(), cv::IMREAD_GRAYSCALE);
				retriever.addImage(image, imagepath.string());
			}
		}

		/*
		 *  Query an image by its full path and desired number of retrieved results.
		 *  @param name
		 *  @param max_len
		 */
		std::vector<candidate_type> query( const std::string& name, int max_len )
		{
			cv::Mat image = cv::imread(name, cv::IMREAD_GRAYSCALE);
			if (image.empty())
				return std::vector<candidate_type>();
			else
				return retriever.queryImage(image, name);
		}

		static cv::Mat read_matrix( const std::string& file, const std::string& key )
		{
			cv::FileStorage fs(file, cv::FileStorage::READ);
			cv::Mat m;
			if (fs.isOpened()) {
				fs[key] >> m;
			}
			return m;
		}


		static std::string make_message( const std::vector<candidate_type>& lists )
		{
			std::stringstream ss;
			for( std::size_t i = 0; i < lists.size(); ++i ) {
				auto c = lists[i];
				i == 0 ? ss : ss << ";";
				ss << std::get<0>(c) << " " << "0 0";
			}
			return ss.str() + "\r\n\r\n";
		}


		cires::ImageRetriever retriever;
};


int main(int argc, char** argv)
{
	const std::string keys =
			"{help h ? usage   |  NULL  |       | print help message     }"
			"{type             | string | build | build or service (MUST)}"
			"{vocabulary       | string |       | vocabulary file path   }"
			"{configure        | string |       | vocabulary file path   }"
			"{voclen           | digits |       | vocabulary size        }"
			"{binarylen        | digits |       | hamming binary bit num }"
			"{srcdir           | string |       | image directory        }"
			"{port             | digits |  4560 | listening port         }"
			"{hessian          | digits |  700  | hessian response (SURF)}";

	cv::CommandLineParser parser(argc, argv, keys.c_str());

	const std::string type = parser.get<std::string>("type");
	if (parser.has("help") || type.empty() || (type != "build" && type != "service")) {
		parser.printParams();
		return 1;
	}

	cires::Params param;

	// if user provided configure file for retrieving, read params
	const std::string confname = parser.has("configure") ? parser.get<std::string>("configure") : "";
	if (!confname.empty() && type == "build") {
		cv::FileStorage fs(confname, cv::FileStorage::READ);
		RuntimeCheck(fs.isOpened(), "Error: failed to load configure file.");
		param.load(fs.root());
	}

	// user provided settings.
	if (parser.has("vocabulary")) {
		const std::string vocfile = parser.get<std::string>("vocabulary");
		cv::Mat voc = Main::read_matrix(vocfile, "vocabulary");
		if (not voc.empty()) {
			param.vocabulary = voc;
		}
	}
	if (parser.has("hessian")) {
		const float hessianrep = parser.get<float>("hessian");
		if (hessianrep < 200)
			std::cerr << "Warning: too small hessian response, use default value instead.\n";
		else
			param.hessian = hessianrep;
	}


	RuntimeCheck(parser.has("srcdir"), "Error: no srcdir provided.")
	const std::string srcdir = parser.get<std::string>("srcdir");

	Main job;

	if (type == "build") {
		job.train_system(srcdir, confname, param);
	}
	else if (type == "service") {
		const int port = parser.get<int>("port");
		MultimediaServer server(port);
		job.start_retriever(param);

		auto caller = [&]( const std::string& cmd ) {
			std::stringstream ss(cmd);

			std::string action;
			ss >> action;

			if (action == "query") {
				std::string name;
				int max_len = 0;
				ss >> name >> max_len;

				auto candidates = job.query(name, max_len);
				return Main::make_message(candidates);
			}
		};

		server.run(caller);
	}

	return 0;
}
