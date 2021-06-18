#include "CognaLauncher.hpp"
#include "Constants.hpp"
#include "HelperFunctions.hpp"
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <mutex>

namespace COGNA{

CognaLauncher::CognaLauncher(std::vector<NeuralNetwork*> network_list,
                             std::vector<utils::networking_client*> client_list,
                             std::vector<utils::networking_sender*> sender_list,
                             int frequency){
    _network_list = network_list;
    _client_list = client_list;
    _sender_list = sender_list;
    _frequency = frequency;
    _curr_cluster_step = new unsigned long long(0);
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

    delete _curr_cluster_step;
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
    // TODO Split in multiple functions
    std::cout << std::endl;
    std::cout << "####################### Starting COGNA #######################"
              << std::endl << std::endl;

    struct timeval _cluster_time;
    long curr_time, prev_time, time_delta, sleep_time = 0;

    int *main_thread_lock = new int(0);
    std::condition_variable *thread_condition_lock = new std::condition_variable;

    create_networking_workers();
    create_cogna_workers(thread_condition_lock, main_thread_lock);

    usleep(100000); //wait 0.1 seconds to ensure networking sockets and networks to connect

    curr_time = utils::get_time_microsec(_cluster_time);
    prev_time = curr_time;
    while(NeuralNetwork::m_cluster_state != STATE_STOPPED){
        if(NeuralNetwork::m_cluster_state != STATE_PAUSE){
            prev_time = utils::get_time_microsec(_cluster_time);
            for(unsigned int i=0; i < _client_list.size(); i++){
                _client_list[i]->store_message();
            }

            for(unsigned int i=0; i < _network_list.size(); i++){
                _network_list[i]->receive_data();
            }

            // Unlocking threads comes here.
            thread_condition_lock->notify_all();

            while(*main_thread_lock < (int)_cogna_worker_list.size()){

            }
            *main_thread_lock = 0;

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
    }

    for(unsigned int i=0; i < _cogna_worker_list.size(); i++){
        delete _cogna_worker_list[i];
        _cogna_worker_list[i] = nullptr;
    }
    _cogna_worker_list.clear();

    delete main_thread_lock;
    main_thread_lock = nullptr;

    delete thread_condition_lock;
    thread_condition_lock = nullptr;

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

//----------------------------------------------------------------------------------------------------------------------
//
int CognaLauncher::create_cogna_workers(std::condition_variable *thread_lock, int *main_lock){
    for(unsigned int i=0; i < _network_list.size(); i++){
        std::thread *cogna_worker1 = new std::thread(&NeuralNetwork::listen_to_cluster,
                                                    _network_list[i],
                                                    thread_lock,
                                                    main_lock);
        _cogna_worker_list.push_back(cogna_worker1);
    }

    return SUCCESS_CODE;
}

} //namespace COGNA
