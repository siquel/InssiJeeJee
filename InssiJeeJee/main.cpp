#include "irc_client.h"
#include <iostream>


int main(int argc, char** argv) {
	std::string port = "6667";
	std::string hostname = "irc.quakenet.org";

	irc_client client("InssiJeeJee", "Pidginzz");
//	client.sig_receive().connect(boost::bind(handle_line, boost::ref(client), _1));
	client.connect(hostname, port);
	client.join_channel("#ttv14");
	
	// dirty hack
	while (true) { }

	return 0;
}