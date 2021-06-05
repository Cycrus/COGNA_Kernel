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
