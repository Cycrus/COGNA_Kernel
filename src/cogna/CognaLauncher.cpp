#include "CognaLauncher.hpp"
#include "Constants.hpp"
#include "HelperFunctions.hpp"
#include <iostream>
#include <unistd.h>
#include <ctime>

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
    for(unsigned int i=0; i < _client_worker_list.size(); i++){
        delete _client_worker_list[i];
        _client_worker_list[i] = nullptr;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
void CognaLauncher::tester(){
    std::cout << "Frequency = " << _frequency << std::endl;
    std::cout << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaLauncher::run_cogna(){
    std::cout << std::endl;
    std::cout << "####################### Starting COGNA #######################"
              << std::endl << std::endl;

    struct timeval _cluster_time;
    long curr_time, prev_time, time_delta, sleep_time = 0;

    create_networking_workers();

    usleep(200000); //sleep 2 second to ensure networking sockets to connect

    curr_time = utils::get_time_microsec(_cluster_time);
    prev_time = curr_time;
    while(true){
        prev_time = utils::get_time_microsec(_cluster_time);
        for(unsigned int i=0; i < _client_list.size(); i++){
            _client_list[i]->store_message();
        }

        _network_list[0]->feed_forward();

        for(unsigned int i=0; i < _sender_list.size(); i++){
            _sender_list[i]->send_payload();
        }

        for(unsigned int i=0; i < _client_list.size(); i++){
            _client_list[i]->clear_message();
        }

        curr_time = utils::get_time_microsec(_cluster_time);

        time_delta = curr_time - prev_time;
        sleep_time = (MICROSECOND_FACTOR / _frequency) - time_delta;
        if(sleep_time > 0){
            usleep(sleep_time);
        }
    }

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaLauncher::create_networking_workers(){
    for(unsigned int i=0; i < _client_list.size(); i++){
        std::thread *client_worker = new std::thread(&utils::networking_client::receive_message, _client_list[i]);
        _client_worker_list.push_back(client_worker);
    }

    return SUCCESS_CODE;
}

} //namespace COGNA
