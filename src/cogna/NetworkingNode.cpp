/**
 * @file NetworkingNode.cpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief Implementation of the NetworkingNode class.
 *
 * @date 2021-05-31
 *
 */

#include "NetworkingNode.hpp"
#include "Constants.hpp"
#include <iostream>

namespace COGNA{

NetworkingNode::NetworkingNode(int id, std::string channel){
    _id = id;
    _channel = channel;

    _client = nullptr;
    _sender = nullptr;
}

//----------------------------------------------------------------------------------------------------------------------
//
NetworkingNode::~NetworkingNode(){

}

//----------------------------------------------------------------------------------------------------------------------
//
int NetworkingNode::add_target(Neuron *target){
    bool can_add = true;
    for(unsigned int i=0; i < _target_list.size(); i++){
        if(target->_id == _target_list[i]->_id){
            can_add = false;
        }
    }

    if(can_add){
        _target_list.push_back(target);
        return SUCCESS_CODE;
    }
    else{
        std::cout << "[ERROR] Cannot add neuron N_" << target->_id
                  << " to Node " << _id << "." << std::endl;
        return ERROR_CODE;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
int NetworkingNode::add_target(NetworkingNode *target){
    bool can_add = true;
    for(unsigned int i=0; i < _output_target_list.size(); i++){
        if(target->_sender->get_ip() == _output_target_list[i]->_sender->get_ip() &&
                target->_sender->get_port() == _output_target_list[i]->_sender->get_port()){
            can_add = false;
        }
    }

    if(can_add){
        _output_target_list.push_back(target);
        return SUCCESS_CODE;
    }
    else{
        std::string client_address = _client->get_ip() + ":" + std::to_string(_client->get_port());
        std::string target_address = target->_sender->get_ip() + ":" + std::to_string(target->_sender->get_port());
        std::cout << "[ERROR] Cannot add connection between node with address " << client_address
                  << " and node with address " << target_address << "." << std::endl;
        return ERROR_CODE;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
void NetworkingNode::remote_activate_senders(float injected_activation){
    for(unsigned int i=0; i < _output_target_list.size(); i++){
        _output_target_list[i]->_sender->add_data(_output_target_list[i]->channel(), injected_activation);
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
int NetworkingNode::setup_client(utils::networking_client *client){
    if(_client == nullptr && _sender == nullptr){
        _client = client;
        _role = ROLE_EXTERN_INPUT;
        return SUCCESS_CODE;
    }
    else{
        std::cout << "[ERROR] Node <" << _id << "> is already set. Cannot setup as client." << std::endl;
        return ERROR_CODE;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
int NetworkingNode::setup_sender(utils::networking_sender *sender){
    if(_client == nullptr && _sender == nullptr){
        _sender = sender;
        _role = ROLE_EXTERN_OUTPUT;
        return SUCCESS_CODE;
    }
    else{
        std::cout << "[ERROR] Node <" << _id << "> is already set. Cannot setup as sender." << std::endl;
        return ERROR_CODE;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
int NetworkingNode::id(){
    return _id;
}

//----------------------------------------------------------------------------------------------------------------------
//
int NetworkingNode::role(){
    return _role;
}

//----------------------------------------------------------------------------------------------------------------------
//
std::string NetworkingNode::channel(){
    return _channel;
}

//----------------------------------------------------------------------------------------------------------------------
//
std::vector<Neuron*> NetworkingNode::targets(){
    return _target_list;
}

} //namespace COGNA
