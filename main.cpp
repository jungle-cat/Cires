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


#define VERSION_STR "1.2"


class Main
{
	public:
		typedef std::tuple<std::string, float> candidate_type;


		void start_retriever( const cires::Params& param, const std::string& confname, const std::string& srcdir)
		{
			// load appropriate parameter.
			cv::FileStorage fs(confname, cv::FileStorage::READ);
			RuntimeCheck(fs.isOpened(), "Error: failed to load configuration.");
			cires::Params settings;
			settings.load(fs.root());

			if (param.hessian != settings.hessian) settings.hessian = param.hessian;
			if (param.voclen != settings.voclen) settings.voclen = param.voclen;
			if (param.binarylen != settings.binarylen) settings.binarylen = param.binarylen;
			if (param.imgmaxlen != settings.imgmaxlen) settings.imgmaxlen = param.imgmaxlen;
			if (param.binthresh != settings.binthresh) settings.binthresh = param.binthresh;

			if (not param.vocabulary.empty()) settings.vocabulary = param.vocabulary;
			if (not param.projection.empty()) settings.projection = param.projection;
			if (not param.thresholds.empty()) settings.thresholds = param.thresholds;

			if (settings.vocabulary.empty() ||
					settings.projection.empty() ||
					settings.thresholds.empty())
				throw std::runtime_error("Error: settings in partially empty.");

			std::cout << "Initializing retrieval system ...\n";
			retriever.initConfig(settings);
			std::cout << "Loading images ...\n";

			load_images(srcdir);
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
				const filesystem::path imagepath = it->path();
				cv::Mat image = cv::imread(imagepath.string(), cv::IMREAD_GRAYSCALE);
				if (image.empty())
					std::cerr << "\tWarning: failed to load image " << imagepath.string() << "\n";
				else
					retriever.addImage(image, imagepath.leaf().string());
			}
		}

		/*
		 *  Query an image by its full path and desired number of retrieved results.
		 *  @param name
		 *  @param max_len
		 */
		std::vector<candidate_type> query( const std::string& name, int max_len )
		{
			std::string leafname = filesystem::path(name).leaf().string();
			cv::Mat image = cv::imread(name, cv::IMREAD_GRAYSCALE);
			if (image.empty())
				return std::vector<candidate_type>();
			else
				return retriever.queryImage(image, leafname);
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
			"{help             ||       | print help message     }"
			"{type             || build | build or service (MUST)}"
			"{vocabulary       ||       | vocabulary file path   }"
			"{configure        ||       | configure file path    }"
			"{voclen           ||       | vocabulary size        }"
			"{binarylen        ||       | hamming binary bit num }"
			"{srcdir           ||       | image directory        }"
			"{port             ||  4560 | listening port         }"
			"{hessian          ||  700  | hessian response (SURF)}"
			"{version          ||       | print the version      }";

	cires::CommandLineParser parser(argc, argv, keys.c_str());

	if (parser.has("version")) {
		std::cout << "cires version : \n\t" << VERSION_STR << std::endl;
		return 0;
	}

	const std::string type = parser.get<std::string>("type");
	if (parser.has("help") || type.empty() || (type != "build" && type != "service")) {
		parser.printParams();
		return 1;
	}

	cires::Params param;

	std::cout << "system settings: \n"
			  << "\t- type: " << type << "\n";
	// if user provided configure file for retrieving, read params
	const std::string confname = parser.has("configure") ? parser.get<std::string>("configure") : "";
	if (!confname.empty() && type == "service") {
		std::cout << "\t- configure:  " << confname << "\n";
		cv::FileStorage fs(confname, cv::FileStorage::READ);
		RuntimeCheck(fs.isOpened(), "Error: failed to load configure file.");
		param.load(fs.root());
	}
	// user provided settings.
	if (parser.has("vocabulary")) {
		const std::string vocfile = parser.get<std::string>("vocabulary");
		std::cout << "\t- vocabulary: " << vocfile << "\n";
		cv::Mat voc = vocfile.empty() ? cv::Mat() : Main::read_matrix(vocfile, "vocabulary");
		if (not voc.empty()) {
			param.vocabulary = voc;
		}
	}
	if (parser.has("hessian")) {
		const float hessianrep = parser.get<float>("hessian");
		std::cout << "\t- hessian: " << hessianrep << "\n";
		if (hessianrep < 200)
			std::cerr << "\t  Warning: too small hessian response, use default value instead.\n";
		else
			param.hessian = hessianrep;
	}
	if (parser.has("voclen")) {
		const int voclen = parser.get<int>("voclen");
		std::cout << "\t- voclen: " << voclen << "\n";
		param.voclen = voclen;
	}
	if (parser.has("binarylen")) {
		const int binarylen = parser.get<int>("binarylen");
		std::cout << "\t- binarylen: " << binarylen << "\n";
		param.binarylen = binarylen;
	}


	RuntimeCheck(parser.has("srcdir"), "Error: no srcdir provided.")
	const std::string srcdir = parser.get<std::string>("srcdir");
	std::cout << "\t- srcdir: " << srcdir << "\n";


	Main job;

	if (type == "build") {
		std::cout << "\n\n";
		job.train_system(srcdir, confname, param);
	}
	else if (type == "service") {
		int binthresh = parser.get<int>("binthresh");
		RuntimeCheck(binthresh > 0 && binthresh < 65, "Error: binary threshold should be in range [0,64]");
		std::cout << "\t- binthresh: " << binthresh << "\n";
		param.binthresh = binthresh;

		const int port = parser.get<int>("port");
		RuntimeCheck(port > 1024, "Error: port should be set greater than 1024.");
		std::cout << "\t- port: " << port << "\n";

		std::cout << "\n\n";

		RuntimeCheck(confname.empty(), "Error: configure file should be set.");

		// start retriever.
		MultimediaServer server(port);
		job.start_retriever(param, confname, srcdir);

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
