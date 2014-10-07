#include "irc_client.h"
#include <iostream>
void handle_line(irc_client& client, std::string line) {
	std::cout << line << std::endl;
}

int main(int argc, char** argv) {
	std::string port = "6667";
	std::string hostname = "irc.quakenet.org";

	irc_client client(hostname, port);
	client.sig_receive().connect(boost::bind(handle_line, boost::ref(client), _1));
	client.connect();
	
	// dirty hack
	while (true) { }

	return 0;
}