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
		}

		void train_system( const cires::Params& param, const std::string srcdir, const std::string& outconf )
		{
			retriever.initConfig(param);
			load_images(srcdir);

			cv::FileStorage fs(outconf, cv::FileStorage::WRITE);
			retriever.store(fs);
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
			"{help h ? usage   |  NULL  |      | print help message     }"
			"{type             | string |      | build or service (MUST)}"
			"{vocabulary       | string |      | vocabulary file path   }"
			"{configure        | string |      | vocabulary file path   }"
			"{vocabulary-len   | digits |      | vocabulary size        }"
			"{hamming-len      | digits |      | hamming binary bit num }"
			"{src-dir          | string |      | image directory        }"
			"{db-dir           | string |      | image db directory     }"
			"{tmp-dir          | string |      | temporary directory    }"
			"{port             | digits | 4560 | listening port         }"
			"{hessian-response | digits | 700  | hessian response (SURF)}";

	cv::CommandLineParser parser(argc, argv, keys.c_str());

	const std::string type = parser.get<std::string>("type");
	if (parser.has("help") || type.empty() || (type != "build" && type != "service")) {
		parser.printParams();
		return 1;
	}

	Main job;
	cires::Params param;

	if (parser.has("vocabulary")) {
		const std::string vocfile = parser.get<std::string>("vocabulary");
		cv::Mat voc = Main::read_matrix(vocfile, "vocabulary");
		if (not voc.empty()) {
			param.vocabulary = voc;
		}
	}
	if (parser.has("hessian-response")) {
		const float hessianrep = parser.get<float>("hessian-response");
		if (hessianrep < 200)
			std::cerr << "Warning: too small hessian response, use default value instead.\n";
		else
			param.hessian = hessianrep;
	}
	if (parser.has("db-dir")) {
		const std::string db_dir = parser.get<std::string>("db-dir");
		param.dbdir = db_dir;
	}
	if (parser.has("tmp-dir")) {
		const std::string tmp_dir = parser.get<std::string>("db-dir");
		param.tmpdir = tmp_dir;
	}

	const std::string src_dir = parser.get<std::string>("src-dir");

	if (type == "build") {

		if (not parser.has("src-dir") || not parser.has("tmp-dir"))
			throw std::runtime_error("Error: src-dir and tmp-dir should both be set.");


		job.train_system(param, src_dir, "out.yaml");

	}
	else {
		// start retrieval service.
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
