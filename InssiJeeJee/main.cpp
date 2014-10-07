#include "irc_client.h"

int main(int argc, char** argv) {
	std::string port = "6667";
	std::string hostname = "irc.quakenet.org";

	irc_client client(hostname, port);
	client.connect();
	
	// dirty hack
	while (true) { }

	return 0;
}