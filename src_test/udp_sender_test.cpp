#include "networking_sender.hpp"
#include "json.hpp"
#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h> //sleep() for Linux systems
#include <thread>

int main(){
	std::string ip = "localhost";
	int port = 40002;
	std::cout << "Starting sender test..." << std::endl;

	utils::networking_sender *json_sender = new utils::networking_sender(ip, port);

	int int_variable = 13;
	float float_variable = 8.44;

	json_sender->add_data("float_value", static_cast<float>(0.5));
  json_sender->add_data("float_value2", static_cast<float>(0.6));
	json_sender->add_data("int_value", static_cast<int>(42));
	json_sender->add_data("float_var", float_variable);
	json_sender->add_data("int_var", int_variable);
	json_sender->add_data("eins", static_cast<std::string>("Hello World"));
	std::cout << json_sender->stringify_payload() << std::endl;
	json_sender->send_payload();

	delete json_sender;
	return 0;
}
