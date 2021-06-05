#include "CognaLauncher.hpp"
#include <iostream>

namespace COGNA{

CognaLauncher::CognaLauncher(std::vector<NeuralNetwork*> network_list,
                             std::vector<utils::networking_client*> client_list,
                             std::vector<utils::networking_sender*> sender_list,
                             int frequency){
    _network_list = network_list;
    _client_list = client_list;
    _sender_list = sender_list;
    _frequency = frequency;
}

//----------------------------------------------------------------------------------------------------------------------
//
CognaLauncher::~CognaLauncher(){
    for(unsigned int i=0; i < _network_list.size(); i++){
        delete _network_list[i];
        _network_list[i] = nullptr;
    }
    for(unsigned int i=0; i < _sender_list.size(); i++){
        delete _sender_list[i];
        _sender_list[i] = nullptr;
    }
    for(unsigned int i=0; i < _client_list.size(); i++){
        delete _client_list[i];
        _client_list[i] = nullptr;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
void CognaLauncher::tester(){
    std::cout << "Hello World..." << _network_list[0]->_neurons[0]->_id << std::endl;
}

} //namespace COGNA
