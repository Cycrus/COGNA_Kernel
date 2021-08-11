/**
 * @file networking_sender.cpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief Implementation of networking_sender class
 *
 * @date 2021-05-27
 *
 */

#include "networking_sender.hpp"
#include <chrono>
#include <iostream>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif // BUFFER_SIZE

namespace utils{

//----------------------------------------------------------------------------------------------------------------------
//
networking_sender::networking_sender(std::string ip, int port){
	_sender = new udp_client_server::udp_client(ip, port);
}

//----------------------------------------------------------------------------------------------------------------------
//
networking_sender::~networking_sender(){
	delete _sender;
}

//----------------------------------------------------------------------------------------------------------------------
//
std::string networking_sender::get_ip(){
	return _sender->get_addr();
}

//----------------------------------------------------------------------------------------------------------------------
//
int networking_sender::get_port(){
	return _sender->get_port();
}

//----------------------------------------------------------------------------------------------------------------------
//
void networking_sender::add_data(std::string key, float value){
	std::lock_guard<std::mutex> guard(_payload_mutex);
	if(_payload.find(key) == _payload.end()){
		_payload[key] = value;
	}
	else{
		_payload[key] += value;
	}
}
void networking_sender::add_data(std::string key, int value){
	std::lock_guard<std::mutex> guard(_payload_mutex);
	if(_payload.find(key) == _payload.end()){
		_payload[key] = value;
	}
	else{
		_payload[key] += value;
	}
}
void networking_sender::add_data(std::string key, std::string value){
	std::lock_guard<std::mutex> guard(_payload_mutex);
	_payload[key] = value;
}

//----------------------------------------------------------------------------------------------------------------------
//
void networking_sender::remove_data(std::string key){
	_payload.erase(key);
}

//----------------------------------------------------------------------------------------------------------------------
//
void networking_sender::clear_payload(){
	_payload.clear();
}

//----------------------------------------------------------------------------------------------------------------------
//
std::string networking_sender::stringify_payload(int indent){
	return _payload.dump(indent);
}

//----------------------------------------------------------------------------------------------------------------------
//
void networking_sender::send_payload(){
	auto time_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	std::cout << "TIME = " << (long long)time_in_ms << std::endl;
	_payload["time"] = (long long)time_in_ms;
	std::string stringified_payload = _payload.dump();
	std::cout << stringified_payload << std::endl;
	_sender->send(stringified_payload.c_str(), stringified_payload.size());
	clear_payload();
}

} //namespace utils
