#pragma once
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/signals2/signal.hpp>
#include <string>
class irc_client
{
private:
	std::string hostname;
	std::string port;
	
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::socket socket;

	boost::thread read_thread;
	boost::thread send_thread;

	// methods
	void start_read();
	void read();
	void handle_line(std::string& s);
	void receive(boost::system::error_code const& error, std::size_t bytes);

	boost::asio::streambuf buffer;
public:
	irc_client(std::string const& hostname, std::string const& port);
	~irc_client();
	void connect();
};

