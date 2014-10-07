#include "irc_client.h"
using boost::asio::ip::tcp;

irc_client::irc_client(std::string const& hostname, std::string const& port) : 
	hostname(hostname), port(port), io_service(),  socket(io_service) {

}


irc_client::~irc_client()
{
	socket.close();
	read_thread.join();
	send_thread.join();
}

void irc_client::connect() {
	using namespace boost::asio::ip;
	
	try {
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(hostname, port);
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

		boost::asio::connect(socket, endpoint_iterator);
	}
	catch (boost::system::system_error& error) {
		// TODO handle
	}
	start_read();
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


void irc_client::receive(boost::system::error_code const& error, std::size_t bytes) {
	std::istream is(&buffer);
	std::string line;

	while (std::getline(is, line))
	{
		sig_receive()(line);
	}

	read();
}

boost::signals2::signal<void(std::string)>& irc_client::sig_receive() {
	return sig_recv;
}
