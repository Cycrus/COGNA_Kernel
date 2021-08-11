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
    for(unsigned int i=0; i<_network_list.size(); i++){
        //std::cout << "Network " << i << " network_list has " << _network_list[i]->_all_networks_list->size() << " elements." << std::endl;
    }
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

    std::condition_variable *thread_condition_lock = new std::condition_variable;

    create_networking_workers();
    create_cogna_workers(thread_condition_lock);

    usleep(100000); //wait 0.1 seconds to ensure networking sockets and networks to connect
    int iterator = 0;

    curr_time = utils::get_time_microsec(_cluster_time);
    prev_time = curr_time;
    while(NeuralNetwork::m_cluster_state != STATE_STOPPED){
        if(NeuralNetwork::m_cluster_state != STATE_PAUSE){
            std::cout << std::endl;
            prev_time = utils::get_time_microsec(_cluster_time);
            for(unsigned int i=0; i < _client_list.size(); i++){
                _client_list[i]->store_message();
            }
            std::cout << "After storing new UDP data..." << std::endl;

            for(unsigned int i=0; i < _network_list.size(); i++){
                _network_list[i]->receive_data();   // Maybe here happens seg fault
            }

            std::cout << "After receiving data..." << std::endl;
            thread_condition_lock->notify_all();

            std::cout << iterator << std::endl;
            // Waiting and checking if all threads have finished
            int finished_thread_number = 0;
            while(finished_thread_number < (int)_cogna_worker_list.size()){
                finished_thread_number = 0;
                for(unsigned int i=0; i < _network_list.size(); i++){
                    finished_thread_number += (int)_network_list[i]->_is_finished;
                }
            }
            for(unsigned int i=0; i < _network_list.size(); i++){
                _network_list[i]->_is_finished = false;
            }

            std::cout << "After waiting loop..." << std::endl;
            for(unsigned int i=0; i < _sender_list.size(); i++){
                _sender_list[i]->send_payload();
            }
            std::cout << "After sending payload..." << std::endl;

            for(unsigned int i=0; i < _client_list.size(); i++){
                _client_list[i]->clear_message();
            }
            std::cout << "After clearing messages..." << std::endl;

            curr_time = utils::get_time_microsec(_cluster_time);

            time_delta = curr_time - prev_time;
            sleep_time = (MICROSECOND_FACTOR / _frequency) - time_delta;

            iterator ++;

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
int CognaLauncher::create_cogna_workers(std::condition_variable *thread_lock){
    for(unsigned int i=0; i < _network_list.size(); i++){
        std::thread *cogna_worker1 = new std::thread(&NeuralNetwork::listen_to_cluster,
                                                    _network_list[i],
                                                    _network_list,
                                                    thread_lock);
        _cogna_worker_list.push_back(cogna_worker1);
    }

    return SUCCESS_CODE;
}

} //namespace COGNA
