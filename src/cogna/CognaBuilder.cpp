/**
 * @file CognaBuilder.cpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief Implementation of CognaBuilder.
 *
 * @date 2021-05-31
 *
 */

#include "CognaBuilder.hpp"
#include "Constants.hpp"
#include "NeuralNetwork.hpp"
#include "Connection.hpp"
#include "Neuron.hpp"

#include <iostream>
#include <fstream>

namespace COGNA{

CognaBuilder::CognaBuilder(std::string project_name){
    _project_name = project_name;
    _project_path = "Projects/" + project_name + "/";
    _frequency = 0;
    _curr_network_neuron_number = 0;
}

//----------------------------------------------------------------------------------------------------------------------
//
CognaBuilder::~CognaBuilder(){

}

//----------------------------------------------------------------------------------------------------------------------
//
void CognaBuilder::tester(){
    std::cout << std::endl;
    std::cout << "Number of Neurons = " << _network_list[0]->_neurons.size() - 1 << std::endl << std::endl;

    for(unsigned int i=0; i < _network_list[0]->_neurons.size(); i++){
        for(unsigned int j=0; j < _network_list[0]->_neurons[i]->_connections.size(); j++){
            if(_network_list[0]->_neurons[i]->_connections[j]->next_neuron){
                std::cout << "N_" << i << " is connected to N_"
                          << _network_list[0]->_neurons[i]->_connections[j]->next_neuron->_id << std::endl;
            }
            else{
                std::cout << "N_" << i << " is connected to C_"
                          << _network_list[0]->_neurons[i]->_connections[j]->next_connection->_id << std::endl;
            }
        }
    }
    std::cout << std::endl;

    for(unsigned int i=0; i<_sender_list.size(); i++){
        std::cout << "Sender -> " << _sender_list[i]->get_ip() << ":" << _sender_list[i]->get_port() << std::endl;
    }
    for(unsigned int i=0; i<_client_list.size(); i++){
        std::cout << "Client -> " << _client_list[i]->get_ip() << ":" << _client_list[i]->get_port() << std::endl;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
std::vector<NeuralNetwork*> CognaBuilder::get_network_list(){
    return _network_list;
}

//----------------------------------------------------------------------------------------------------------------------
//
std::vector<utils::networking_client*> CognaBuilder::get_client_list(){
    return _client_list;
}

//----------------------------------------------------------------------------------------------------------------------
//
std::vector<utils::networking_sender*> CognaBuilder::get_sender_list(){
    return _sender_list;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::get_frequency(){
    return _frequency;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::build_cogna_cluster(){
    std::cout << "[INFO] Loading global parameters." << std::endl;
    if(load_globals_file() == ERROR_CODE) return ERROR_CODE;

    std::cout << "[INFO] Loading transmitter file." << std::endl;
    if(load_transmitters() == ERROR_CODE) return ERROR_CODE;

    std::cout << "[INFO] Loading neuron types." << std::endl;
    if(load_neuron_types() == ERROR_CODE) return ERROR_CODE;

    if(load_network(_main_network) == ERROR_CODE) return ERROR_CODE;

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_globals_file(){
    std::ifstream global_file;
    global_file.open(_project_path + "global.config");
    if(!global_file){
        std::cout << "[ERROR] Could not open global.config file of project "
                  << _project_name << std::endl;
        return ERROR_CODE;
    }

    nlohmann::json global_json;
    global_file >> global_json;
    global_file.close();

    _frequency = std::stoi((std::string)global_json["frequency"]);
    _main_network = global_json["main_network"];

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
// TODO Return error_code, if not all required parameters are set.y
int CognaBuilder::load_neuron_types(){
    std::ifstream neuron_file;
    neuron_file.open(_project_path + "neuron_type.config");
    if(!neuron_file){
        std::cout << "[ERROR] Could not open neuron_type.config file of project "
                  << _project_name << std::endl;
        return ERROR_CODE;
    }

    nlohmann::json neuron_json;
    neuron_file >> neuron_json;
    neuron_file.close();

    for(auto it = neuron_json.begin(); it != neuron_json.end(); ++it){
        _neuron_types[it.key()] = it.value();
    }

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_transmitters(){
    std::ifstream transmitter_file;
    transmitter_file.open(_project_path + "transmitters.config");
    if(!transmitter_file){
        std::cout << "[ERROR] Could not open transmitters.config file of project "
                  << _project_name << std::endl;
        return ERROR_CODE;
    }

    nlohmann::json transmitter_json;
    transmitter_file >> transmitter_json;
    transmitter_file.close();

    for(unsigned int i=0; i < transmitter_json["transmitters"].size(); i++){
        _transmitter_types.push_back(transmitter_json["transmitters"][i]);
    }

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_neurons(NeuralNetwork *nn, nlohmann::json network_json){
    for(unsigned int i=0; i < network_json["neurons"].size(); i++){
        std::string neuron_type = "";
        try{
            neuron_type = network_json["neurons"][i]["neuron_type"];
        }
        catch(...){
            neuron_type = "Default";
        }

        float temp_threshold = load_neuron_parameter(network_json["neurons"][i], "activation_threshold", neuron_type);
        int random_chance = (int)load_neuron_parameter(network_json["neurons"][i], "random_chance", neuron_type);
        int does_influence_transmitter = (int)load_neuron_parameter(network_json["neurons"][i], "influences_transmitter", neuron_type);
        nn->add_neuron(temp_threshold);
        int n_id = nn->_neurons.size()-1;

        if(does_influence_transmitter){
            int influenced_transmitter = (int)load_neuron_parameter(network_json["neurons"][i], "influenced_transmitter", neuron_type);
            int direction = (int)load_neuron_parameter(network_json["neurons"][i], "transmitter_influence_direction", neuron_type);
            nn->set_neural_transmitter_influence(n_id, influenced_transmitter, direction);
        }

        if(random_chance > 0){
            int random_value = load_neuron_parameter(network_json["neurons"][i], "random_activation_value", neuron_type);
            nn->set_random_neuron_activation(n_id, random_chance, random_value);
        }

        nn->_neurons[n_id]->_parameter->initial_base_weight = load_neuron_parameter(network_json["neurons"][i], "base_weight", neuron_type);
        nn->_neurons[n_id]->_parameter->max_weight = load_neuron_parameter(network_json["neurons"][i], "max_weight", neuron_type);
        nn->_neurons[n_id]->_parameter->min_weight = load_neuron_parameter(network_json["neurons"][i], "min_weight", neuron_type);
        nn->_neurons[n_id]->_parameter->activation_type = load_neuron_parameter(network_json["neurons"][i], "activation_type", neuron_type);
        nn->_neurons[n_id]->_parameter->activation_function = load_neuron_parameter(network_json["neurons"][i], "activation_function", neuron_type);
        nn->_neurons[n_id]->_parameter->learning_type = load_neuron_parameter(network_json["neurons"][i], "learning_type", neuron_type);
        nn->_neurons[n_id]->_parameter->transmitter_type = load_neuron_parameter(network_json["neurons"][i], "transmitter_type", neuron_type);

        nn->_neurons[n_id]->_parameter->short_habituation_curvature = load_neuron_parameter(network_json["neurons"][i], "short_habituation_curvature", neuron_type);
        nn->_neurons[n_id]->_parameter->short_habituation_steepness = load_neuron_parameter(network_json["neurons"][i], "short_habituation_steepness", neuron_type);
        nn->_neurons[n_id]->_parameter->short_sensitization_curvature = load_neuron_parameter(network_json["neurons"][i], "short_sensitization_curvature", neuron_type);
        nn->_neurons[n_id]->_parameter->short_sensitization_steepness = load_neuron_parameter(network_json["neurons"][i], "short_sensitization_steepness", neuron_type);
        nn->_neurons[n_id]->_parameter->short_dehabituation_curvature = load_neuron_parameter(network_json["neurons"][i], "short_dehabituation_curvature", neuron_type);
        nn->_neurons[n_id]->_parameter->short_dehabituation_steepness = load_neuron_parameter(network_json["neurons"][i], "short_dehabituation_steepness", neuron_type);
        nn->_neurons[n_id]->_parameter->short_desensitization_curvature = load_neuron_parameter(network_json["neurons"][i], "short_desensitization_curvature", neuron_type);
        nn->_neurons[n_id]->_parameter->short_desensitization_steepness = load_neuron_parameter(network_json["neurons"][i], "short_desensitization_steepness", neuron_type);

        nn->_neurons[n_id]->_parameter->long_habituation_curvature = load_neuron_parameter(network_json["neurons"][i], "long_habituation_curvature", neuron_type);
        nn->_neurons[n_id]->_parameter->long_habituation_steepness = load_neuron_parameter(network_json["neurons"][i], "long_habituation_steepness", neuron_type);
        nn->_neurons[n_id]->_parameter->long_sensitization_curvature = load_neuron_parameter(network_json["neurons"][i], "long_sensitization_curvature", neuron_type);
        nn->_neurons[n_id]->_parameter->long_sensitization_steepness = load_neuron_parameter(network_json["neurons"][i], "long_sensitization_steepness", neuron_type);
        nn->_neurons[n_id]->_parameter->long_dehabituation_curvature = load_neuron_parameter(network_json["neurons"][i], "long_dehabituation_curvature", neuron_type);
        nn->_neurons[n_id]->_parameter->long_dehabituation_steepness = load_neuron_parameter(network_json["neurons"][i], "long_dehabituation_steepness", neuron_type);
        nn->_neurons[n_id]->_parameter->long_desensitization_curvature = load_neuron_parameter(network_json["neurons"][i], "long_desensitization_curvature", neuron_type);
        nn->_neurons[n_id]->_parameter->long_desensitization_steepness = load_neuron_parameter(network_json["neurons"][i], "long_desensitization_steepness", neuron_type);

        nn->_neurons[n_id]->_parameter->presynaptic_potential_curvature = load_neuron_parameter(network_json["neurons"][i], "presynaptic_potential_curvature", neuron_type);
        nn->_neurons[n_id]->_parameter->presynaptic_potential_steepness = load_neuron_parameter(network_json["neurons"][i], "presynaptic_potential_steepness", neuron_type);
        nn->_neurons[n_id]->_parameter->presynaptic_backfall_curvature = load_neuron_parameter(network_json["neurons"][i], "presynaptic_backfall_curvature", neuron_type);
        nn->_neurons[n_id]->_parameter->presynaptic_backfall_steepness = load_neuron_parameter(network_json["neurons"][i], "presynaptic_backfall_steepness", neuron_type);

        nn->_neurons[n_id]->_parameter->long_learning_weight_reduction_curvature = load_neuron_parameter(network_json["neurons"][i], "long_learning_weight_reduction_curvature", neuron_type);
        nn->_neurons[n_id]->_parameter->long_learning_weight_reduction_steepness = load_neuron_parameter(network_json["neurons"][i], "long_learning_weight_reduction_steepness", neuron_type);
        nn->_neurons[n_id]->_parameter->long_learning_weight_backfall_curvature = load_neuron_parameter(network_json["neurons"][i], "long_learning_weight_backfall_curvature", neuron_type);
        nn->_neurons[n_id]->_parameter->long_learning_weight_backfall_steepness = load_neuron_parameter(network_json["neurons"][i], "long_learning_weight_backfall_steepness", neuron_type);

        nn->_neurons[n_id]->_parameter->habituation_threshold = load_neuron_parameter(network_json["neurons"][i], "habituation_threshold", neuron_type);
        nn->_neurons[n_id]->_parameter->sensitization_threshold = load_neuron_parameter(network_json["neurons"][i], "sensitization_threshold", neuron_type);

        nn->_neurons[n_id]->_parameter->max_activation = load_neuron_parameter(network_json["neurons"][i], "max_activation", neuron_type);
        nn->_neurons[n_id]->_parameter->min_activation = load_neuron_parameter(network_json["neurons"][i], "min_activation", neuron_type);
        nn->_neurons[n_id]->_parameter->activation_backfall_curvature = load_neuron_parameter(network_json["neurons"][i], "activation_backfall_curvature", neuron_type);
        nn->_neurons[n_id]->_parameter->activation_backfall_steepness = load_neuron_parameter(network_json["neurons"][i], "activation_backfall_steepness", neuron_type);
        nn->_neurons[n_id]->_parameter->transmitter_change_curvature = load_neuron_parameter(network_json["neurons"][i], "transmitter_change_curvature", neuron_type);
        nn->_neurons[n_id]->_parameter->transmitter_change_steepness = load_neuron_parameter(network_json["neurons"][i], "transmitter_change_steepness", neuron_type);
    }

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_nodes(NeuralNetwork *nn, nlohmann::json network_json){
    int error_code = SUCCESS_CODE;

    for(unsigned int i=0; i<network_json["nodes"].size(); i++){
        std::string ip = "0.0.0.0";
        int port = 0;
        std::string channel;
        try{
            port = std::stoi((std::string)network_json["nodes"][i]["port"]);
        }
        catch(...){
            std::cout << "[ERROR] Cannot parse port number of node." << std::endl;
            return ERROR_CODE;
        }
        try{
            channel = network_json["nodes"][i]["channel"];
        }
        catch(...){
            std::cout << "[ERROR] Cannot parse channel of node." << std::endl;
            return ERROR_CODE;
        }
        int networking_id = 0;

        if(network_json["nodes"][i]["function"] == "interface_input"){
            bool client_does_exist = false;
            for(unsigned int j=0; j < _client_list.size(); j++){
                if(_client_list[j]->get_port() == port){
                    client_does_exist = true;
                    networking_id = j;
                }
            }
            if(!client_does_exist){
                networking_id = _client_list.size();
                utils::networking_client *temp_client = new utils::networking_client(ip, port, true);
                _client_list.push_back(temp_client);
            }

            nn->add_extern_input_node(_client_list[networking_id], channel);
        }

        else if(network_json["nodes"][i]["function"] == "interface_output"){
            try{
                ip = network_json["nodes"][i]["ip_address"];
            }
            catch(...){
                std::cout << "[ERROR] Cannot parse ip address of node." << std::endl;
                return ERROR_CODE;
            }

            bool sender_does_exist = false;
            for(unsigned int j=0; j < _sender_list.size(); j++){
                if(_sender_list[j]->get_ip() == ip && _sender_list[j]->get_port() == port){
                    sender_does_exist = true;
                    networking_id = j;
                }
            }
            if(!sender_does_exist){
                networking_id = _sender_list.size();
                utils::networking_sender *temp_sender = new utils::networking_sender(ip, port);
                _sender_list.push_back(temp_sender);
            }

            nn->add_extern_output_node(_sender_list[networking_id], channel);
        }
    }

    return error_code;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_all_connection_parameter(Connection *temp_con, nlohmann::json network_json, int i){
    temp_con->_parameter->max_weight = load_connection_parameter(temp_con->_parameter->short_habituation_curvature,
                                                                                                  network_json["connections"][i], "max_weight");
    temp_con->_parameter->min_weight = load_connection_parameter(temp_con->_parameter->short_habituation_curvature,
                                                                                                  network_json["connections"][i], "min_weight");

    temp_con->_parameter->short_habituation_curvature = load_connection_parameter(temp_con->_parameter->short_habituation_curvature,
                                                                                                  network_json["connections"][i], "short_habituation_curvature");
    temp_con->_parameter->short_habituation_steepness = load_connection_parameter(temp_con->_parameter->short_habituation_steepness,
                                                                                                  network_json["connections"][i], "short_habituation_steepness");
    temp_con->_parameter-> short_sensitization_curvature = load_connection_parameter(temp_con->_parameter->short_sensitization_curvature,
                                                                                                  network_json["connections"][i], "short_sensitization_curvature");
    temp_con->_parameter->short_sensitization_steepness = load_connection_parameter(temp_con->_parameter->short_sensitization_steepness,
                                                                                                  network_json["connections"][i], "short_sensitization_steepness");
    temp_con->_parameter->short_dehabituation_curvature = load_connection_parameter(temp_con->_parameter->short_dehabituation_curvature,
                                                                                                  network_json["connections"][i], "short_dehabituation_curvature");
    temp_con->_parameter->short_dehabituation_steepness = load_connection_parameter(temp_con->_parameter->short_dehabituation_steepness,
                                                                                                  network_json["connections"][i], "short_dehabituation_steepness");
    temp_con->_parameter->short_desensitization_curvature = load_connection_parameter(temp_con->_parameter->short_desensitization_curvature,
                                                                                                  network_json["connections"][i], "short_desensitization_curvature");
    temp_con->_parameter->short_desensitization_steepness = load_connection_parameter(temp_con->_parameter->short_desensitization_steepness,
                                                                                                  network_json["connections"][i], "short_desensitization_steepness");

    temp_con->_parameter->long_habituation_curvature = load_connection_parameter(temp_con->_parameter->long_habituation_curvature,
                                                                                                  network_json["connections"][i], "long_habituation_curvature");
    temp_con->_parameter->long_habituation_steepness = load_connection_parameter(temp_con->_parameter->long_habituation_steepness,
                                                                                                  network_json["connections"][i], "long_habituation_steepness");
    temp_con->_parameter->long_sensitization_curvature = load_connection_parameter(temp_con->_parameter->long_sensitization_curvature,
                                                                                                  network_json["connections"][i], "long_sensitization_curvature");
    temp_con->_parameter->long_sensitization_steepness = load_connection_parameter(temp_con->_parameter->long_sensitization_steepness,
                                                                                                  network_json["connections"][i], "long_sensitization_steepness");
    temp_con->_parameter->long_dehabituation_curvature = load_connection_parameter(temp_con->_parameter->long_dehabituation_curvature,
                                                                                                  network_json["connections"][i], "long_dehabituation_curvature");
    temp_con->_parameter->long_dehabituation_steepness = load_connection_parameter(temp_con->_parameter->long_dehabituation_steepness,
                                                                                                  network_json["connections"][i], "long_dehabituation_steepness");
    temp_con->_parameter->long_desensitization_curvature = load_connection_parameter(temp_con->_parameter->long_desensitization_curvature,
                                                                                                  network_json["connections"][i], "long_desensitization_curvature");
    temp_con->_parameter->long_desensitization_steepness = load_connection_parameter(temp_con->_parameter->long_desensitization_steepness,
                                                                                                  network_json["connections"][i], "long_desensitization_steepness");

    temp_con->_parameter->presynaptic_potential_curvature = load_connection_parameter(temp_con->_parameter->presynaptic_potential_curvature,
                                                                                                  network_json["connections"][i], "presynaptic_potential_curvature");
    temp_con->_parameter->presynaptic_potential_steepness = load_connection_parameter(temp_con->_parameter->presynaptic_potential_steepness,
                                                                                                  network_json["connections"][i], "presynaptic_potential_steepness");
    temp_con->_parameter->presynaptic_backfall_curvature = load_connection_parameter(temp_con->_parameter->presynaptic_backfall_curvature,
                                                                                                  network_json["connections"][i], "presynaptic_backfall_curvature");
    temp_con->_parameter->presynaptic_backfall_steepness = load_connection_parameter(temp_con->_parameter->presynaptic_backfall_steepness,
                                                                                                  network_json["connections"][i], "presynaptic_backfall_steepness");

    temp_con->_parameter->long_learning_weight_reduction_curvature = load_connection_parameter(temp_con->_parameter->long_learning_weight_reduction_curvature,
                                                                                                  network_json["connections"][i], "long_learning_weight_reduction_curvature");
    temp_con->_parameter->long_learning_weight_reduction_steepness = load_connection_parameter(temp_con->_parameter->long_learning_weight_reduction_steepness,
                                                                                                  network_json["connections"][i], "long_learning_weight_reduction_steepness");
    temp_con->_parameter->long_learning_weight_backfall_curvature = load_connection_parameter(temp_con->_parameter->long_learning_weight_backfall_curvature,
                                                                                                  network_json["connections"][i], "long_learning_weight_backfall_curvature");
    temp_con->_parameter->long_learning_weight_backfall_steepness = load_connection_parameter(temp_con->_parameter->long_learning_weight_backfall_steepness,
                                                                                                  network_json["connections"][i], "long_learning_weight_backfall_steepness");

    temp_con->_parameter->habituation_threshold = load_connection_parameter(temp_con->_parameter->habituation_threshold,
                                                                                                  network_json["connections"][i], "habituation_threshold");
    temp_con->_parameter->sensitization_threshold = load_connection_parameter(temp_con->_parameter->sensitization_threshold,
                                                                                                  network_json["connections"][i], "sensitization_threshold");

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_neuron_connection(NeuralNetwork *nn, nlohmann::json network_json, unsigned int i){
    int source_neuron = network_json["connections"][i]["prev_neuron"];
    int target_neuron = network_json["connections"][i]["next_neuron"];
    float base_weight = load_connection_init_parameter(nn, network_json["connections"][i], "base_weight", source_neuron);
    int connection_type = (int)load_connection_init_parameter(nn, network_json["connections"][i], "activation_type", source_neuron);
    int function_type = (int)load_connection_init_parameter(nn, network_json["connections"][i], "activation_function", source_neuron);
    int learning_type = (int)load_connection_init_parameter(nn, network_json["connections"][i], "learning_type", source_neuron);
    int transmitter_type = (int)load_connection_init_parameter(nn, network_json["connections"][i], "transmitter_type", source_neuron);

    Connection *temp_con = nn->add_neuron_connection(source_neuron, target_neuron, base_weight, connection_type,
                                                     function_type, learning_type, transmitter_type);

    load_all_connection_parameter(temp_con, network_json, i);

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_node_connection(NeuralNetwork *nn, nlohmann::json network_json, unsigned int i){
    int prev_id = network_json["connections"][i]["prev_neuron"];
    int next_id = network_json["connections"][i]["next_neuron"];
    NetworkingNode *target_node;

    if(network_json["connections"][i]["prev_neuron_function"] == "interface_input"){
        for(unsigned int j=0; j < nn->_extern_input_nodes.size(); j++){
            if(prev_id == nn->_extern_input_nodes[j]->id()){
                target_node = nn->_extern_input_nodes[j];
            }
        }
        target_node->add_target(nn->_neurons[next_id]);
    }

    else if(network_json["connections"][i]["next_neuron_function"] == "interface_output"){
        for(unsigned int j=0; j < nn->_extern_output_nodes.size(); j++){
            if(next_id == nn->_extern_output_nodes[j]->id()){
                target_node = nn->_extern_output_nodes[j];
            }
        }
        target_node->add_target(nn->_neurons[prev_id]);
    }

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_presynaptic_connection(NeuralNetwork *nn, nlohmann::json network_json, unsigned int i){
    int source_neuron = network_json["connections"][i]["prev_neuron"];
    int target_connection = network_json["connections"][i]["next_connection"];

    float base_weight = (float)load_connection_init_parameter(nn, network_json["connections"][i], "base_weight", source_neuron);
    int connection_type = (int)load_connection_init_parameter(nn, network_json["connections"][i], "activation_type", source_neuron);
    int function_type = (int)load_connection_init_parameter(nn, network_json["connections"][i], "activation_function", source_neuron);
    int learning_type = (int)load_connection_init_parameter(nn, network_json["connections"][i], "learning_type", source_neuron);
    int transmitter_type = (int)load_connection_init_parameter(nn, network_json["connections"][i], "transmitter_type", source_neuron);

    Connection *target_connection_pointer;

    for(unsigned int n=0; n < nn->_neurons.size(); n++){
        for(unsigned int c=0; c < nn->_neurons[n]->_connections.size(); c++){
            if(target_connection == nn->_neurons[n]->_connections[c]->_id){
                target_connection_pointer = nn->_neurons[n]->_connections[c];
                goto break_loops;
            }
        }
    }
    break_loops:

    Connection *temp_con = nn->add_synaptic_connection(source_neuron, target_connection_pointer, base_weight, connection_type,
                                                       function_type, learning_type, transmitter_type);

    load_all_connection_parameter(temp_con, network_json, i);

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_connections(NeuralNetwork *nn, nlohmann::json network_json){
    for(unsigned int i=0; i < network_json["connections"].size(); i++){
        if(network_json["connections"][i].find("next_neuron") != network_json["connections"][i].end()){
            if(network_json["connections"][i]["prev_neuron_function"] == "neuron" &&
               network_json["connections"][i]["next_neuron_function"] == "neuron"){
                   load_neuron_connection(nn, network_json, i);
            }
            else if(network_json["connections"][i]["prev_neuron_function"] == "interface_input" ||
                network_json["connections"][i]["next_neuron_function"] == "interface_output"){
                    load_node_connection(nn, network_json, i);
            }
        }
        else if(network_json["connections"][i].find("next_connection") != network_json["connections"][i].end()){
            load_presynaptic_connection(nn, network_json, i);
        }
        else if(network_json["connections"][i]["prev_neuron_function"] == "subnet_input" ||
            network_json["connections"][i]["next_neuron_function"] == "subnet_output"){
                std::cout << "Found subnetwork exchanging connections" << std::endl;
            }
        else{
            std::cout << "[ERROR] Missing target type for connection." << std::endl;
            return ERROR_CODE;
        }
    }

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_network_parameter(NeuralNetwork *nn, nlohmann::json network_json){
    nn->_parameter->transmitter_backfall_curvature = std::stof((std::string)network_json["network"]["transmitter_backfall_curvature"]);
    nn->_parameter->transmitter_backfall_steepness = std::stof((std::string)network_json["network"]["transmitter_backfall_steepness"]);
    nn->_parameter->max_transmitter_weight = std::stof((std::string)network_json["network"]["max_transmitter_weight"]);
    nn->_parameter->min_transmitter_weight = std::stof((std::string)network_json["network"]["min_transmitter_weight"]);

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_network(std::string network_name){
    std::cout << "[INFO] Loading network <" << network_name << ">." << std::endl;
    std::ifstream network_file;
    network_file.open(_project_path + "networks/" + network_name);
    if(!network_file){
        std::cout << "[ERROR] Could not open " << network_name << " file of project "
                  << _project_name << std::endl;
        return ERROR_CODE;
    }

    nlohmann::json network_json;
    network_file >> network_json;
    network_file.close();

    // DONE neurons = network_json["neurons"]
    // connections = network_json["connections"]
    // nodes = network_json["nodes"]
    // network = network_json["network"]

    int error_code = SUCCESS_CODE;

    NeuralNetwork *nn = new NeuralNetwork();
    std::cout << "[INFO] Loading network parameters." << std::endl;
    if(load_network_parameter(nn, network_json) == ERROR_CODE) error_code = ERROR_CODE;
    if(error_code == SUCCESS_CODE){
        std::cout << "[INFO] Defining neurotransmitters." << std::endl;
        if(nn->define_transmitters(_transmitter_types.size()) == ERROR_CODE) error_code = ERROR_CODE;
    }
    if(error_code == SUCCESS_CODE){
        std::cout << "[INFO] Loading neurons." << std::endl;
        if(load_neurons(nn, network_json) == ERROR_CODE) error_code = ERROR_CODE;
    }
    if(error_code == SUCCESS_CODE){
        std::cout << "[INFO] Loading exchange nodes." << std::endl;
        if(load_nodes(nn, network_json) == ERROR_CODE) error_code = ERROR_CODE;
    }
    if(error_code == SUCCESS_CODE){
        std::cout << "[INFO] Loading connections." << std::endl;
        if(load_connections(nn, network_json) == ERROR_CODE) error_code = ERROR_CODE;
    }

    if(error_code == SUCCESS_CODE){
        if(nn->setup_network() == ERROR_CODE) error_code = ERROR_CODE;
    }
    std::cout << std::endl;

    _curr_network_neuron_number = 0;

    if(error_code == SUCCESS_CODE){
        _network_list.push_back(nn);
        return SUCCESS_CODE;
    }
    else{
        delete nn;
        return ERROR_CODE;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
float CognaBuilder::load_neuron_parameter(nlohmann::json neuron, std::string parameter, std::string neuron_type){
    float return_value = 0.0f;
    std::string parameter_value = "";
    try{
        parameter_value = neuron[parameter];
    }
    catch(...){
        try{
            parameter_value = _neuron_types[neuron_type][parameter];
        }
        catch(...){
            std::cout << "[ERROR] Unable to load parameter <" << parameter << "> from neuron." << std::endl;
            return ERROR_CODE;
        }
    }

    if(parameter == "activation_type"){
        if(parameter_value == "Excitatory")
            return_value = EXCITATORY;
        else if(parameter_value == "Inhibitory")
            return_value = INHIBITORY;
        else if(parameter_value == "Nondirectional")
            return_value = NONDIRECTIONAL;
    }
    else if(parameter == "activation_function"){
        if(parameter_value == "Linear")
            return_value = FUNCTION_LINEAR;
        else if(parameter_value == "Relu")
            return_value = FUNCTION_RELU;
        else if(parameter_value == "Sigmoid")
            return_value = FUNCTION_SIGMOID;
    }
    else if(parameter == "learning_type"){
        if(parameter_value == "None")
            return_value = LEARNING_NONE;
        else if(parameter_value == "Habituation")
            return_value = LEARNING_HABITUATION;
        else if(parameter_value == "Sensitization")
            return_value = LEARNING_SENSITIZATION;
        else if(parameter_value == "HabiSens")
            return_value = LEARNING_HABISENS;
    }
    else if(parameter == "transmitter_type"){
        for(unsigned int i=0; i<_transmitter_types.size(); i++){
            if(_transmitter_types[i] == parameter_value){
                return_value = i;
            }
            if(_transmitter_types[i] == parameter_value){
                return_value = i;
            }
        }
    }
    else if(parameter == "influenced_transmitter"){
        for(unsigned int i=0; i<_transmitter_types.size(); i++){
            if(_transmitter_types[i] == parameter_value){
                return_value = i;
            }
            if(_transmitter_types[i] == parameter_value){
                return_value = i;
            }
        }
    }
    else if(parameter == "influences_transmitter"){
        if(parameter_value == "No"){
            return_value = 0;
        }
        else if(parameter_value == "Yes"){
            return_value = 1;
        }
    }
    else if(parameter == "transmitter_influence_direction"){
        if(parameter_value == "Positive Influence"){
            return_value = POSITIVE_INFLUENCE;
        }
        else if(parameter_value == "Negative Influence"){
            return_value = NEGATIVE_INFLUENCE;
        }
    }

    else{
        try{
            return_value = std::stof((std::string)neuron[parameter]);
        }
        catch(...){
            try{
                return_value = std::stof((std::string)_neuron_types[neuron_type][parameter]);
            }
            catch(...){
                std::cout << "[ERROR] Unable to load parameter <" << parameter << "> from neuron." << std::endl;
                return ERROR_CODE;
            }
        }
    }

    if(parameter == "random_chance") std::cout << parameter << " = " << return_value << std::endl;
    //std::cout << parameter << " = " << return_value << std::endl;

    return return_value;
}

//----------------------------------------------------------------------------------------------------------------------
//
float CognaBuilder::load_connection_parameter(float init_value, nlohmann::json connection, std::string parameter){
    float return_value = init_value;
    if(connection.find(parameter) != connection.end()){
        return_value = std::stof((std::string)connection[parameter]);
    }
    return return_value;
}

//----------------------------------------------------------------------------------------------------------------------
//
float CognaBuilder::load_connection_init_parameter(NeuralNetwork * nn, nlohmann::json connection, std::string parameter, int source_neuron_id){
    float return_value = 0.0f;

    if(parameter == "base_weight" || parameter == "activation_type" || parameter == "activation_function" ||
       parameter == "learning_type" || parameter == "transmitter_type"){
           std::string parameter_value = "";

           if(parameter == "transmitter_type"){
               try{
                   parameter_value = connection[parameter];
                   for(unsigned int i=0; i<_transmitter_types.size(); i++){
                       if(_transmitter_types[i] == parameter_value){
                           return_value = i;
                       }
                       if(_transmitter_types[i] == parameter_value){
                           return_value = i;
                       }
                   }
               }
               catch(...){
                   return_value = nn->_neurons[source_neuron_id]->_parameter->transmitter_type;
               }
           }

           else{
                try{
                    parameter_value = connection[parameter];

                    if(parameter == "activation_type"){
                        if(parameter_value == "Excitatory")
                            return_value = EXCITATORY;
                        else if(parameter_value == "Inhibitory")
                            return_value = INHIBITORY;
                        else if(parameter_value == "Nondirectional")
                            return_value = NONDIRECTIONAL;
                    }
                    else if(parameter == "activation_function"){
                        if(parameter_value == "Linear")
                            return_value = FUNCTION_LINEAR;
                        else if(parameter_value == "Relu")
                            return_value = FUNCTION_RELU;
                        else if(parameter_value == "Sigmoid")
                            return_value = FUNCTION_SIGMOID;
                    }
                    else if(parameter == "learning_type"){
                        if(parameter_value == "None")
                            return_value = LEARNING_NONE;
                        else if(parameter_value == "Habituation")
                            return_value = LEARNING_HABITUATION;
                        else if(parameter_value == "Sensitization")
                            return_value = LEARNING_SENSITIZATION;
                        else if(parameter_value == "HabiSens")
                            return_value = LEARNING_HABISENS;
                    }
                    else{
                        return_value = std::stof((std::string)connection[parameter]);
                    }
                }
                catch(...){
                    if(parameter == "base_weight")
                        return_value = nn->_neurons[source_neuron_id]->_parameter->initial_base_weight;
                    else if(parameter == "activation_type")
                        return_value = nn->_neurons[source_neuron_id]->_parameter->activation_type;
                    else if(parameter == "activation_function")
                        return_value = nn->_neurons[source_neuron_id]->_parameter->activation_function;
                    else if(parameter == "learning_type")
                        return_value = nn->_neurons[source_neuron_id]->_parameter->learning_type;
                }
            }
    }
    else{
        std::cout << "[ERROR] Invalid initialization parameter <" << parameter << ">." << std::endl;
    }

    return return_value;
}

} //namespace COGNA
