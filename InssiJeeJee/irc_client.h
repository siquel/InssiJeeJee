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
	std::string login;
	std::string nick;
	
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::socket socket;

	boost::thread read_thread;
	boost::thread send_thread;

	// methods
	void start_read();
	void read();
	void handle_line(std::string& s);
	void receive(boost::system::error_code const& error, std::size_t bytes);
	void send_raw(std::string const& msg);

	boost::asio::streambuf buffer;
	// signals
	boost::signals2::signal<void(std::string)> sig_recv;
public:
	irc_client(std::string const& nick, std::string const& ident);
	boost::signals2::signal<void(std::string)>& sig_receive();
	~irc_client();
	void connect(std::string const& hostname, std::string const& port);
	void join_channel(std::string const& channel);
	void send_message(std::string channel, std::string message);
};

