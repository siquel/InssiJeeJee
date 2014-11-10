#include "irc_client.h"
#include <iostream>
#include <boost/algorithm/string/predicate.hpp>

irc_client* client;

static const std::string prefix = "/";

int main(int argc, char** argv) {
	std::string port = "6667";
	std::string hostname = "irc.quakenet.org";

	client = new irc_client("InssiJeeJee", "Pidginzz");
//	client.sig_receive().connect(boost::bind(handle_line, boost::ref(client), _1));
	client->connect(hostname, port);
	//client->join_channel("#ttv14");
	//client->send_message("#ttv14", "Pidgin <3");
	std::string line;
	while (getline(std::cin, line)) {
		// TODO management console
		if (!boost::starts_with(line, prefix)) continue;

		std::string command = line.substr(1);
		if (command == "q" || command == "quit")
			exit(0);
	}
	return 0;
}