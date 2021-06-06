#include "networking_client.hpp"
#include "json.hpp"
#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h> //sleep() for Linux systems
#include <thread>

int main(){
	std::string ip = "0.0.0.0";
	int port = 40001;
	std::cout << "Starting Network test..." << std::endl;

	utils::networking_client *json_worker = new utils::networking_client(ip, port, true);
	utils::networking_client *hello_worker = new utils::networking_client(ip, 5006, false);
	std::thread json_udp_thread(&utils::networking_client::receive_message, json_worker);
	std::thread hello_udp_thread(&utils::networking_client::receive_message, hello_worker);
	std::string temp_json;
	std::string temp_string;
	nlohmann::json j;

	usleep(10000);

	while (true){
		json_worker->store_message();
		hello_worker->store_message();

		float cast_test = (float)json_worker->get_json_value("eins") + (float)json_worker->get_json_value("zwei");
		float null_test = (float)json_worker->get_json_value("invalid_key");
		null_test = (float)json_worker->get_json_value("invalid_key");

		std::cout << cast_test << std::endl;
		std::cout << null_test << std::endl;

		std::cout << json_worker->get_message(4) << std::endl;

		std::cout << "out 'incrementor': " << json_worker->get_json_value("incrementor") << std::endl;
		std::cout << "out: " << hello_worker->get_message() << std::endl;
		std::cout << std::endl;

		usleep(1000000);
	}

	delete json_worker;
	delete hello_worker;

	return 0;
}
