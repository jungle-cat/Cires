/*M
 *
 * File Name     :  MultimediaServer.hpp
 *
 * Creation Date :  May 8, 2013
 *
 * Last Modified :  May 8, 2013
 *
 * Created By    :  feng weiguo (ustcrevolutionary@gmail.com)
 * 
 * Copyright     :  Any kinds use of the code is only legal with author's permission.
 *
 */

#ifndef __MULTIMEDIASERVER_HPP__
#define __MULTIMEDIASERVER_HPP__

#include <ctime>
#include <memory>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>



using boost::asio::ip::tcp;

typedef std::function<std::string(const std::string&)> CallableType;

class ProcessSession
	: public std::enable_shared_from_this<ProcessSession>
{
	public:
		enum : unsigned int {
			MAX_LEN = 1024
		};

		ProcessSession(boost::asio::io_service& ioservice)
			:m_socket(ioservice)
		{
			std::memset(data_,'\0',sizeof(data_));
		}

		~ProcessSession()
		{
			std::cout << "destruct the client session: " << m_time << std::endl;
		}

		tcp::socket& socket()
		{
			return m_socket;
		}

		void start(CallableType caller)
		{
			m_socket.async_read_some(
					boost::asio::buffer(data_,MAX_LEN),
					std::bind( &ProcessSession::handle_read,
							   shared_from_this(),
							   std::placeholders::_1,
							   std::placeholders::_2,
							   caller));
		}
	private:

		void handle_write( std::shared_ptr<std::string> pmsg,
						   const boost::system::error_code& error,
				  	  	   std::size_t bytes_transferred)
		{
			if (error)
				std::cerr << "Error: failed in sending!\n";
			else
				std::cerr << "Succeed: sending ok!\n";
		}

		void handle_read( const boost::system::error_code& error,
						  std::size_t bytes_transferred,
						  CallableType caller)
		{
			if(!error)
			{
				// analyze the recived bytes
				data_[bytes_transferred] = '\0';
				m_cmdline.append(data_);

				// check if msg has been recieved
				auto pos = m_cmdline.find("\r\n\r\n");
				if (pos != std::string::npos) {
					std::string cmdline = m_cmdline.substr(0, pos);
					m_cmdline.erase(0, cmdline.length());
					std::string ret = caller(cmdline);

					std::shared_ptr<std::string> pmsg(new std::string(ret));
					m_socket.async_write_some(
						boost::asio::buffer(*pmsg),
						std::bind( &ProcessSession::handle_write,
								   shared_from_this(),
								   pmsg,
								   std::placeholders::_1,
								   std::placeholders::_2));
				}
				m_socket.async_read_some(
									boost::asio::buffer(data_, MAX_LEN),
									std::bind( &ProcessSession::handle_read,
											   shared_from_this(),
											   std::placeholders::_1,
											   std::placeholders::_2,
											   caller));
			}

		}
	private:
		tcp::socket m_socket;
		char data_[MAX_LEN+1];
		std::string m_cmdline;
		std::string m_time;
};


class serverApp
{
	public:
		typedef CallableType caller_type;

		serverApp( boost::asio::io_service& ioservice,
				   tcp::endpoint& endpoint,
				   caller_type caller)
			: m_ioservice(ioservice)
			, m_acceptor(ioservice,endpoint)
			, m_caller(caller)
		{
			auto new_session = std::make_shared<ProcessSession>(m_ioservice);
			m_acceptor.async_accept(
					new_session->socket(),
					std::bind( &serverApp::handle_accept,
							   this,
							   std::placeholders::_1,
							   new_session));
		}

		~serverApp()
		{
		}

	private:
		void handle_accept( const boost::system::error_code& error,
							std::shared_ptr<ProcessSession>& session )
		{
			if(!error)
			{
				std::cerr << "accepted\n";
				session->start(m_caller);

				auto new_session = std::make_shared<ProcessSession>(m_ioservice);
				m_acceptor.async_accept(
						new_session->socket(),
						std::bind( &serverApp::handle_accept,
								   this,
								   std::placeholders::_1,
								   new_session));
			}
		}

	private:
		boost::asio::io_service& m_ioservice;
		tcp::acceptor 			 m_acceptor;
		caller_type              m_caller;
};


class MultimediaServer
{
	public:
		MultimediaServer(int port)
			: m_port(port)
			, m_ioservice()
		{
		}

		void run(CallableType caller)
		{
			if (m_port > 0) {
				tcp::endpoint endPoint(tcp::v4(), m_port);
				m_server.reset(new serverApp(m_ioservice, endPoint, caller));
			}
			if (m_server) {
				m_ioservice.run();
			}
		}
	private:
		int                         m_port;
		boost::asio::io_service     m_ioservice;
		std::shared_ptr<serverApp>  m_server;
};
#endif // __MULTIMEDIASERVER_HPP__
