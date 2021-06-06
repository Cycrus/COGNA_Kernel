/**
 * @file networking_client.cpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief Implementation of networking_client class
 *
 * @date 2021-05-27
 *
 */

#include "networking_client.hpp"
#include <iostream>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif //BUFFER_SIZE

namespace utils{

networking_client::networking_client(std::string ip, int port, bool is_json){
	_receiver = new udp_client_server::udp_server(ip, port);
	_is_json = is_json;
}

//----------------------------------------------------------------------------------------------------------------------
//
networking_client::~networking_client(){
	delete _receiver;
}

//----------------------------------------------------------------------------------------------------------------------
//
std::string networking_client::get_ip(){
	return _receiver->get_addr();
}

//----------------------------------------------------------------------------------------------------------------------
//
int networking_client::get_port(){
	return _receiver->get_port();
}

//----------------------------------------------------------------------------------------------------------------------
//
void networking_client::receive_message(){
	while(true){
		char *temp_msg = (char*)malloc(BUFFER_SIZE);
		_receiver->recv(temp_msg, BUFFER_SIZE);
		_msg = temp_msg;
		free(temp_msg);
		temp_msg = NULL;
	}
}

//----------------------------------------------------------------------------------------------------------------------
//
void networking_client::store_message(){
	if(_is_json){
		try{
			_hashtable = nlohmann::json::parse(_msg);
		}
		catch(...){
			std::cout << "[ERROR] Could not parse message to json hashtable." << std::endl;
		}
	}

	_stored_message = _msg;
}

//----------------------------------------------------------------------------------------------------------------------
//
std::string networking_client::get_message(int indent){
	if(_is_json && indent > -1){
		try{
			return _hashtable.dump(indent);
		}
		catch(...){
			return _stored_message;
		}
	}

	return _stored_message;
}

//----------------------------------------------------------------------------------------------------------------------
//
nlohmann::json networking_client::get_json_value(std::string key){
	if(_is_json){
		auto return_value = _hashtable[key];
		if(return_value.is_null()){
			return_value = 0;
		}
		return return_value;
	}

	return 0;
}

//----------------------------------------------------------------------------------------------------------------------
//
nlohmann::json networking_client::get_hashtable(){
	if(_is_json){
		return _hashtable;
	}

	return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
//
void networking_client::clear_message(){
	_hashtable.clear();
	_stored_message = "";
	_msg = "";
}

} //namespace utils
