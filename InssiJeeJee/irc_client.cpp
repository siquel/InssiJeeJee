#include "irc_client.h"
#include <boost/algorithm/string/predicate.hpp>
using boost::asio::ip::tcp;

irc_client::irc_client(std::string const& nick, std::string const& ident) : 
	nick(nick), login(ident), io_service(),  socket(io_service) {

}


irc_client::~irc_client()
{
	socket.close();
	read_thread.join();
	send_thread.join();
}

void irc_client::connect(std::string const& hostname, std::string const& port) {
	using namespace boost::asio::ip;
	this->hostname = hostname;
	this->port = port;
	try {
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(hostname, port);
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

		boost::asio::connect(socket, endpoint_iterator);
	}
	catch (boost::system::system_error& error) {
		// TODO handle
	}
	
	send_raw(std::string("NICK ") + nick + std::string("\r\n"));
	send_raw(std::string("USER ") + login + std::string(" 8 * :realname\r\n"));
	
	boost::system::error_code error;
	while (true) {
		boost::asio::read_until(socket, buffer, "\r\n");
		std::istream is(&buffer);
		std::string line;

		while (std::getline(is, line)) {
			std::cout << line << std::endl;
			if (boost::starts_with(line, "PING ")) {
				send_raw(std::string("PONG ") + line.substr(5) + std::string("\r\n"));
			}
			int space = line.find(" ", 0);
			int secondSpace = line.find(" ", space + 1);
			if (secondSpace >= 0) {
				// we are connected to server
				std::string code = line.substr(space + 1, secondSpace - space);
				if (code.find( "004", 0) != std::string::npos) {
					start_read();
					return;
				}
			}
		}
	}

	
}

void irc_client::send_raw(std::string const& msg) {
	std::cout << "Sending >> " << msg << std::endl;
	try {
		boost::asio::write(socket,
			boost::asio::buffer(msg.data(), msg.size()), boost::asio::transfer_all());
	}
	catch (boost::system::system_error& error) {
		// todo handle
	}
}

// start reading and start reading thread
void irc_client::start_read() {
	read();
	read_thread = boost::thread(boost::bind(&boost::asio::io_service::run, &io_service));
	
}

// reads socket and calls receive with data which was read
void irc_client::read() {
	try {
		boost::asio::async_read_until(socket, buffer, "\r\n", boost::bind(&irc_client::receive, this, _1, _2));
	}
	catch (boost::system::system_error& error) {
		// TODO handle
	}
}

void irc_client::join_channel(std::string const& channel) {
	send_raw(std::string("JOIN ") + channel + std::string("\r\n"));
}


void irc_client::receive(boost::system::error_code const& error, std::size_t bytes) {
	std::istream is(&buffer);
	std::string line;

	while (std::getline(is, line))
	{
		handle_line(line);
	}

	read();
}

void irc_client::handle_line(std::string &line) {
	std::cout << line << std::endl;
	if (boost::starts_with(line, "PING ")) {
		send_raw(std::string("PONG ") + line.substr(5) + std::string("\r\n"));
	}
}

boost::signals2::signal<void (std::string)>& irc_client::sig_receive() {
	return sig_recv;
}
