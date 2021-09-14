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

    for(unsigned int n=0; n < _network_list.size(); n++){
        for(unsigned int i=0; i < _network_list[n]->_neurons.size(); i++){
            for(unsigned int j=0; j < _network_list[n]->_neurons[i]->_connections.size(); j++){
                if(_network_list[n]->_neurons[i]->_connections[j]->next_neuron){
                    std::cout << "N_" << i << " in NN_" << _network_list[n]->_neurons[i]->_network_id
                              << " is connected to N_" << _network_list[n]->_neurons[i]->_connections[j]->next_neuron->_id
                              << " in NN_<" << _network_list[n]->_neurons[i]->_connections[j]->next_neuron->_network_id
                              << "> with connection <" << _network_list[n]->_neurons[i]->_connections[j]->_id << ">." << std::endl;
                }
                else{
                    std::cout << "N_" << i << " in NN_" << _network_list[n]->_neurons[i]->_network_id
                              << " is connected to C_" << _network_list[n]->_neurons[i]->_connections[j]->next_connection->_id
                              << " in NN_" << _network_list[n]->_neurons[i]->_connections[j]->next_connection->prev_neuron->_network_id
                              << " with connection <" << _network_list[n]->_neurons[i]->_connections[j]->_id << ">." << std::endl;
                }
            }
        }
        std::cout << std::endl;
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
    std::cout << std::endl;
    std::cout << "####################### Loading COGNA #######################"
              << std::endl << std::endl;

    std::cout << "[INFO] Loading global parameters." << std::endl;
    if(load_globals_file() == ERROR_CODE) return ERROR_CODE;

    std::cout << "[INFO] Loading transmitter file." << std::endl;
    if(load_transmitters() == ERROR_CODE) return ERROR_CODE;

    std::cout << "[INFO] Loading neuron types." << std::endl;
    if(load_neuron_types() == ERROR_CODE) return ERROR_CODE;

    if(load_network(_main_network) == ERROR_CODE) return ERROR_CODE;

    std::cout << "[INFO] Connecting individual subnetworks." << std::endl;
    if(connect_subnetworks() == ERROR_CODE) return ERROR_CODE;

    // TODO Here when everything is already loaded, make another round for synaptic connections.
    for(unsigned int i = 0; i < _final_synaptic_connections.size(); i++){
        std::cout << "FINAL SYNAPTIC <" << i << "> = " << _final_synaptic_connections[i] << std::endl;
        int con_network_id = _final_synaptic_connections[i]["network_id"];
        load_presynaptic_connection(_network_list[con_network_id], _final_synaptic_connections[i]);
    }

    for(unsigned int i=0; i < _network_list.size(); i++){
        if(_network_list[i]->setup_network() == ERROR_CODE) return ERROR_CODE;
    }

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
        if(network_json["nodes"][i]["function"] == "interface_input" ||
                network_json["nodes"][i]["function"] == "interface_output"){
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

                int node_id = network_json["nodes"][i]["id"];
                nn->add_extern_input_node(node_id, _client_list[networking_id], channel);
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

                int node_id = network_json["nodes"][i]["id"];
                nn->add_extern_output_node(node_id, _sender_list[networking_id], channel);
            }
        }
    }

    return error_code;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_all_connection_parameter(Connection *connection_object, nlohmann::json connection_json){
    connection_object->_parameter->max_weight = load_connection_parameter(connection_object->_parameter->short_habituation_curvature,
                                                                                                  connection_json, "max_weight");
    connection_object->_parameter->min_weight = load_connection_parameter(connection_object->_parameter->short_habituation_curvature,
                                                                                                  connection_json, "min_weight");

    connection_object->_parameter->short_habituation_curvature = load_connection_parameter(connection_object->_parameter->short_habituation_curvature,
                                                                                                  connection_json, "short_habituation_curvature");
    connection_object->_parameter->short_habituation_steepness = load_connection_parameter(connection_object->_parameter->short_habituation_steepness,
                                                                                                  connection_json, "short_habituation_steepness");
    connection_object->_parameter-> short_sensitization_curvature = load_connection_parameter(connection_object->_parameter->short_sensitization_curvature,
                                                                                                  connection_json, "short_sensitization_curvature");
    connection_object->_parameter->short_sensitization_steepness = load_connection_parameter(connection_object->_parameter->short_sensitization_steepness,
                                                                                                  connection_json, "short_sensitization_steepness");
    connection_object->_parameter->short_dehabituation_curvature = load_connection_parameter(connection_object->_parameter->short_dehabituation_curvature,
                                                                                                  connection_json, "short_dehabituation_curvature");
    connection_object->_parameter->short_dehabituation_steepness = load_connection_parameter(connection_object->_parameter->short_dehabituation_steepness,
                                                                                                  connection_json, "short_dehabituation_steepness");
    connection_object->_parameter->short_desensitization_curvature = load_connection_parameter(connection_object->_parameter->short_desensitization_curvature,
                                                                                                  connection_json, "short_desensitization_curvature");
    connection_object->_parameter->short_desensitization_steepness = load_connection_parameter(connection_object->_parameter->short_desensitization_steepness,
                                                                                                  connection_json, "short_desensitization_steepness");

    connection_object->_parameter->long_habituation_curvature = load_connection_parameter(connection_object->_parameter->long_habituation_curvature,
                                                                                                  connection_json, "long_habituation_curvature");
    connection_object->_parameter->long_habituation_steepness = load_connection_parameter(connection_object->_parameter->long_habituation_steepness,
                                                                                                  connection_json, "long_habituation_steepness");
    connection_object->_parameter->long_sensitization_curvature = load_connection_parameter(connection_object->_parameter->long_sensitization_curvature,
                                                                                                  connection_json, "long_sensitization_curvature");
    connection_object->_parameter->long_sensitization_steepness = load_connection_parameter(connection_object->_parameter->long_sensitization_steepness,
                                                                                                  connection_json, "long_sensitization_steepness");
    connection_object->_parameter->long_dehabituation_curvature = load_connection_parameter(connection_object->_parameter->long_dehabituation_curvature,
                                                                                                  connection_json, "long_dehabituation_curvature");
    connection_object->_parameter->long_dehabituation_steepness = load_connection_parameter(connection_object->_parameter->long_dehabituation_steepness,
                                                                                                  connection_json, "long_dehabituation_steepness");
    connection_object->_parameter->long_desensitization_curvature = load_connection_parameter(connection_object->_parameter->long_desensitization_curvature,
                                                                                                  connection_json, "long_desensitization_curvature");
    connection_object->_parameter->long_desensitization_steepness = load_connection_parameter(connection_object->_parameter->long_desensitization_steepness,
                                                                                                  connection_json, "long_desensitization_steepness");

    connection_object->_parameter->presynaptic_potential_curvature = load_connection_parameter(connection_object->_parameter->presynaptic_potential_curvature,
                                                                                                  connection_json, "presynaptic_potential_curvature");
    connection_object->_parameter->presynaptic_potential_steepness = load_connection_parameter(connection_object->_parameter->presynaptic_potential_steepness,
                                                                                                  connection_json, "presynaptic_potential_steepness");
    connection_object->_parameter->presynaptic_backfall_curvature = load_connection_parameter(connection_object->_parameter->presynaptic_backfall_curvature,
                                                                                                  connection_json, "presynaptic_backfall_curvature");
    connection_object->_parameter->presynaptic_backfall_steepness = load_connection_parameter(connection_object->_parameter->presynaptic_backfall_steepness,
                                                                                                  connection_json, "presynaptic_backfall_steepness");

    connection_object->_parameter->long_learning_weight_reduction_curvature = load_connection_parameter(connection_object->_parameter->long_learning_weight_reduction_curvature,
                                                                                                  connection_json, "long_learning_weight_reduction_curvature");
    connection_object->_parameter->long_learning_weight_reduction_steepness = load_connection_parameter(connection_object->_parameter->long_learning_weight_reduction_steepness,
                                                                                                  connection_json, "long_learning_weight_reduction_steepness");
    connection_object->_parameter->long_learning_weight_backfall_curvature = load_connection_parameter(connection_object->_parameter->long_learning_weight_backfall_curvature,
                                                                                                  connection_json, "long_learning_weight_backfall_curvature");
    connection_object->_parameter->long_learning_weight_backfall_steepness = load_connection_parameter(connection_object->_parameter->long_learning_weight_backfall_steepness,
                                                                                                  connection_json, "long_learning_weight_backfall_steepness");

    connection_object->_parameter->habituation_threshold = load_connection_parameter(connection_object->_parameter->habituation_threshold,
                                                                                                  connection_json, "habituation_threshold");
    connection_object->_parameter->sensitization_threshold = load_connection_parameter(connection_object->_parameter->sensitization_threshold,
                                                                                                  connection_json, "sensitization_threshold");

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_neuron_connection(NeuralNetwork *nn, nlohmann::json connection_json){
    int source_neuron = connection_json["prev_neuron"];
    int target_neuron = connection_json["next_neuron"];
    float base_weight = load_connection_init_parameter(nn, connection_json, "base_weight", source_neuron);
    int connection_type = (int)load_connection_init_parameter(nn, connection_json, "activation_type", source_neuron);
    int function_type = (int)load_connection_init_parameter(nn, connection_json, "activation_function", source_neuron);
    int learning_type = (int)load_connection_init_parameter(nn, connection_json, "learning_type", source_neuron);
    int transmitter_type = (int)load_connection_init_parameter(nn, connection_json, "transmitter_type", source_neuron);

    Connection *temp_con = nn->add_neuron_connection(source_neuron, target_neuron, base_weight, connection_type,
                                                     function_type, learning_type, transmitter_type);
    temp_con->_json_id = (int)connection_json["id"];

    load_all_connection_parameter(temp_con, connection_json);

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_node_connection(NeuralNetwork *nn, nlohmann::json connection_json){
    int prev_id = connection_json["prev_neuron"];
    int next_id = connection_json["next_neuron"];
    NetworkingNode *target_node;

    if(connection_json["prev_neuron_function"] == "interface_input"){
        for(unsigned int j=0; j < nn->_extern_input_nodes.size(); j++){
            if(prev_id == nn->_extern_input_nodes[j]->id()){
                target_node = nn->_extern_input_nodes[j];
            }
        }

        if(connection_json["next_neuron_function"] == "neuron"){
            target_node->add_target(nn->_neurons[next_id]);
        }
        else if(connection_json["next_neuron_function"] == "interface_output"){
            int output_node_id = (int)connection_json["next_neuron"] - 1;
            std::string sender_ip = connection_json["nodes"][output_node_id]["ip_address"];
            int sender_port = std::stoi((std::string)connection_json["nodes"][output_node_id]["port"]);
            std::string sender_channel = connection_json["nodes"][output_node_id]["channel"];

            for(unsigned int ex_out=0; ex_out < nn->_extern_output_nodes.size(); ex_out++){
                if(nn->_extern_output_nodes[ex_out]->_sender->get_ip() == sender_ip &&
                        nn->_extern_output_nodes[ex_out]->_sender->get_port() == sender_port &&
                        nn->_extern_output_nodes[ex_out]->channel() == sender_channel){
                    target_node->add_target(nn->_extern_output_nodes[ex_out]);
                }
            }
        }
    }

    else if(connection_json["next_neuron_function"] == "interface_output"){
        for(unsigned int j=0; j < nn->_extern_output_nodes.size(); j++){
            if(next_id == nn->_extern_output_nodes[j]->id()){
                target_node = nn->_extern_output_nodes[j];
            }
        }

        if(connection_json["prev_neuron_function"] == "neuron"){
            target_node->add_target(nn->_neurons[prev_id]);
        }
    }

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_subnet_input_connection(NeuralNetwork *nn, nlohmann::json connection_json,
                                               nlohmann::json network_json){
    if((int)connection_json["prev_subnet_node_id"] == -1){
        std::cout << "[ERROR] Input-Node has invalid node_id." << std::endl;
        return ERROR_CODE;
    }

    nn->_subnet_input_connection_list.push_back(connection_json);

    if(connection_json["next_neuron_function"] == "connection"){
        for(unsigned int net_con = 0; net_con < network_json["connections"].size(); net_con++){
            if((int)connection_json["next_connection"] == (int)network_json["connections"][net_con]["id"]){
                if(network_json["connections"][net_con]["next_neuron_function"] != "subnet_output"){
                    network_json["connections"][net_con]["network_id"] = _network_list.size();
                    nlohmann::json synaptic_information = {{"starting_point", network_json["connections"][net_con]},
                                                           {"connection_id", (int)network_json["connections"][net_con]["id"]}};
                    _synaptic_connection_indicator.push_back(synaptic_information);
                }
            }
        }
    }

    if(connection_json["prev_neuron_function"] == "input"){
        int subnet_id = (int)connection_json["prev_subnetwork"];
        std::string prev_network_name = network_json["subnetworks"][subnet_id]["network_name"];
        nn->_subnet_input_connection_list[nn->_subnet_input_connection_list.size()-1]["prev_subnet_name"] = prev_network_name;
    }

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_subnet_output_connection(NeuralNetwork *nn, nlohmann::json connection_json,
                                                nlohmann::json network_json){
    if((int)connection_json["next_subnet_node_id"] == -1){
        std::cout << "[ERROR] Output-Node has invalid node_id." << std::endl;
        return ERROR_CODE;
    }

    nn->_subnet_output_connection_list.push_back(connection_json);

    if(network_json["next_neuron_function"] == "output"){
        int subnet_id = (int)connection_json["next_subnetwork"];
        std::string next_network_name = network_json["subnetworks"][subnet_id]["network_name"];
        nn->_subnet_output_connection_list[nn->_subnet_output_connection_list.size()-1]["next_subnet_name"] = next_network_name;
    }

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_presynaptic_connection(NeuralNetwork *nn, nlohmann::json connection_json){
    if((std::string)connection_json["prev_neuron_function"] == "neuron"){
        int source_neuron = connection_json["prev_neuron"];
        int target_connection = connection_json["next_connection"];

        float base_weight = (float)load_connection_init_parameter(nn, connection_json, "base_weight", source_neuron);
        int connection_type = (int)load_connection_init_parameter(nn, connection_json, "activation_type", source_neuron);
        int function_type = (int)load_connection_init_parameter(nn, connection_json, "activation_function", source_neuron);
        int learning_type = (int)load_connection_init_parameter(nn, connection_json, "learning_type", source_neuron);
        int transmitter_type = (int)load_connection_init_parameter(nn, connection_json, "transmitter_type", source_neuron);

        Connection *target_connection_pointer = nullptr;

        for(unsigned int n=0; n < nn->_neurons.size(); n++){
            for(unsigned int c=0; c < nn->_neurons[n]->_connections.size(); c++){
                if(target_connection == nn->_neurons[n]->_connections[c]->_json_id){
                    target_connection_pointer = nn->_neurons[n]->_connections[c];
                    goto break_loops;
                }
            }
        }
        break_loops:

        if(target_connection_pointer != nullptr){
            Connection::s_max_id = nn->_connections.size();
            Connection *temp_con = nn->add_synaptic_connection(source_neuron, target_connection_pointer, base_weight, connection_type,
                                                               function_type, learning_type, transmitter_type);
            temp_con->_json_id = (int)connection_json["id"];

            load_all_connection_parameter(temp_con, connection_json);
        }
        else{
            nlohmann::json temp_connection_json = connection_json;
            temp_connection_json["network_id"] = nn->_id;
            _final_synaptic_connections.push_back(temp_connection_json);
        }
    }

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_connections(NeuralNetwork *nn, nlohmann::json network_json){
    for(unsigned int i=0; i < network_json["connections"].size(); i++){
        if(network_json["connections"][i].find("next_neuron") != network_json["connections"][i].end()){
            if(network_json["connections"][i]["prev_neuron_function"] == "neuron" &&
               network_json["connections"][i]["next_neuron_function"] == "neuron"){
                   if(load_neuron_connection(nn, network_json["connections"][i]) == ERROR_CODE) return ERROR_CODE;
            }

            if(network_json["connections"][i]["prev_neuron_function"] == "interface_input" ||
                network_json["connections"][i]["next_neuron_function"] == "interface_output"){
                    if(load_node_connection(nn, network_json["connections"][i]) == ERROR_CODE) return ERROR_CODE;
            }
        }

        else if(network_json["connections"][i].find("next_connection") != network_json["connections"][i].end()){
            if(load_presynaptic_connection(nn, network_json["connections"][i]) == ERROR_CODE) return ERROR_CODE;
        }

        if(network_json["connections"][i]["prev_neuron_function"] == "input" ||
           network_json["connections"][i]["prev_neuron_function"] == "subnet_input"){
                if(load_subnet_input_connection(nn, network_json["connections"][i], network_json) == ERROR_CODE) return ERROR_CODE;
        }

        if(network_json["connections"][i]["next_neuron_function"] == "output" ||
           network_json["connections"][i]["next_neuron_function"] == "subnet_output"){
                if(load_subnet_output_connection(nn, network_json["connections"][i], network_json) == ERROR_CODE) return ERROR_CODE;
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
    std::cout << std::endl;
    std::cout << "[INFO] Loading network <" << network_name << ">." << std::endl;
    std::cout << "+++++++++++++++++++++++++++++++++++++++" << std::endl;

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

    int error_code = SUCCESS_CODE;

    NeuralNetwork *nn = new NeuralNetwork();
    nn->_network_name = network_name;
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
    std::cout << std::endl;

    if(error_code == ERROR_CODE){
        delete nn;
        return ERROR_CODE;
    }

    _curr_network_neuron_number = 0;

    _network_list.push_back(nn);

    for(unsigned int i=0; i < network_json["subnetworks"].size(); i++){
        Neuron::s_max_id = 0;
        Connection::s_max_id = 0;

        if(load_network(network_json["subnetworks"][i]["network_name"]) == ERROR_CODE) return ERROR_CODE;
    }

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
std::vector<nlohmann::json> CognaBuilder::find_starting_points(int source_network_id, nlohmann::json source_connection){
    std::vector<nlohmann::json> starting_points;

    source_connection["network_id"] = source_network_id;

    if(source_connection["prev_neuron_function"] == "neuron" || source_connection["prev_neuron_function"] == "interface_input"){
        starting_points.push_back(source_connection);
    }

    else if(source_connection["prev_neuron_function"] == "input"){
        int next_network_id = source_network_id + (int)source_connection["prev_subnetwork"] + 1;
        NeuralNetwork *next_network = _network_list[next_network_id];
        for(unsigned int next_con = 0; next_con < next_network->_subnet_output_connection_list.size(); next_con++){
            nlohmann::json next_connection = next_network->_subnet_output_connection_list[next_con];
            if(source_connection["prev_subnet_node_id"] == next_connection["next_subnet_node_id"] &&
                    next_connection["next_neuron_function"] == "subnet_output"){
                std::vector<nlohmann::json> temp_starting_points = find_starting_points(next_network_id, next_connection);
                starting_points.insert(std::end(starting_points), std::begin(temp_starting_points), std::end(temp_starting_points));
            }
        }
    }

    return starting_points;
}

//----------------------------------------------------------------------------------------------------------------------
//
std::vector<nlohmann::json> CognaBuilder::find_end_points(int source_network_id, nlohmann::json source_connection){
    std::vector<nlohmann::json> end_points;

    source_connection["network_id"] = source_network_id;

    if(source_connection["next_neuron_function"] == "neuron" ||
            source_connection["next_neuron_function"] == "interface_output" ||
            source_connection.find("next_connection") != source_connection.end()){
        end_points.push_back(source_connection);
    }

    else if(source_connection["next_neuron_function"] == "output"){
        int next_network_id = source_network_id + (int)source_connection["next_subnetwork"] + 1;
        NeuralNetwork *next_network = _network_list[next_network_id];
        for(unsigned int next_con = 0; next_con < next_network->_subnet_input_connection_list.size(); next_con++){
            nlohmann::json next_connection = next_network->_subnet_input_connection_list[next_con];
            if(source_connection["next_subnet_node_id"] == next_connection["prev_subnet_node_id"] &&
                    next_connection["prev_neuron_function"] == "subnet_input"){
                std::vector<nlohmann::json> temp_end_points = find_end_points(next_network_id, next_connection);
                end_points.insert(std::end(end_points), std::begin(temp_end_points), std::end(temp_end_points));
            }
        }
    }

    return end_points;
}

//----------------------------------------------------------------------------------------------------------------------
//
void CognaBuilder::create_subnet_neuron_connections(std::vector<nlohmann::json> starting_points, std::vector<nlohmann::json> end_points){
    for(unsigned int start_id = 0; start_id < starting_points.size(); start_id++){
        int source_network_id = (int)starting_points[start_id]["network_id"];
        NeuralNetwork *source_network = _network_list[source_network_id];
        int source_neuron_id = (int)starting_points[start_id]["prev_neuron"];
        float base_weight = load_connection_init_parameter(source_network, starting_points[start_id],
                                                           "base_weight", source_neuron_id);
        int connection_type = (int)load_connection_init_parameter(source_network, starting_points[start_id],
                                                                  "activation_type", source_neuron_id);
        int function_type = (int)load_connection_init_parameter(source_network, starting_points[start_id],
                                                                "activation_function", source_neuron_id);
        int learning_type = (int)load_connection_init_parameter(source_network, starting_points[start_id],
                                                               "learning_type", source_neuron_id);
        int transmitter_type = (int)load_connection_init_parameter(source_network, starting_points[start_id],
                                                                   "transmitter_type", source_neuron_id);

        for(unsigned int end_id = 0; end_id < end_points.size(); end_id++){
            int target_network_id = (int)end_points[end_id]["network_id"];

            Connection *temp_connection = nullptr;
            int target_neuron_id = 0;
            Neuron *target_neuron = nullptr;

            if(end_points[end_id].find("next_neuron") != end_points[end_id].end()){
                target_neuron_id = (int)end_points[end_id]["next_neuron"];
                target_neuron = _network_list[target_network_id]->_neurons[target_neuron_id];
                Connection::s_max_id = source_network->_connections.size();
                temp_connection = source_network->add_neuron_connection(source_neuron_id, target_neuron, base_weight, connection_type,
                                                                        function_type, learning_type, transmitter_type);
                if(temp_connection != nullptr){
                    temp_connection->_json_id = (int)starting_points[start_id]["id"];
                    nlohmann::json synaptic_information = {{"starting_point", starting_points[start_id]},
                                                           {"connection_id", (int)temp_connection->_id}};
                    _synaptic_connection_indicator.push_back(synaptic_information);
                    load_all_connection_parameter(temp_connection, starting_points[start_id]);
                }
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
void CognaBuilder::create_subnet_synaptic_connections(std::vector<nlohmann::json> starting_points, std::vector<nlohmann::json> end_points){
    for(unsigned int start_id = 0; start_id < starting_points.size(); start_id++){
        int source_network_id = (int)starting_points[start_id]["network_id"];
        NeuralNetwork *source_network = _network_list[source_network_id];
        int source_neuron_id = (int)starting_points[start_id]["prev_neuron"];
        float base_weight = load_connection_init_parameter(source_network, starting_points[start_id],
                                                         "base_weight", source_neuron_id);
        int connection_type = (int)load_connection_init_parameter(source_network, starting_points[start_id],
                                                                "activation_type", source_neuron_id);
        int function_type = (int)load_connection_init_parameter(source_network, starting_points[start_id],
                                                              "activation_function", source_neuron_id);
        int learning_type = (int)load_connection_init_parameter(source_network, starting_points[start_id],
                                                             "learning_type", source_neuron_id);
        int transmitter_type = (int)load_connection_init_parameter(source_network, starting_points[start_id],
                                                                 "transmitter_type", source_neuron_id);

        for(unsigned int end_id = 0; end_id < end_points.size(); end_id++){
            Connection *temp_connection = nullptr;
            Connection *target_connection = nullptr;

            if(end_points[end_id].find("next_connection") != end_points[end_id].end()){
                unsigned int synaptic_info_size = _synaptic_connection_indicator.size();
                std::vector<int> delete_synaptic_id;

                for(unsigned int synaptic_id = 0; synaptic_id < synaptic_info_size; synaptic_id++){
                    nlohmann::json temp_synaptic_information = _synaptic_connection_indicator[synaptic_id]["starting_point"];
                    int target_connection_json_id = (int)temp_synaptic_information["id"];
                    int target_connection_id = _synaptic_connection_indicator[synaptic_id]["connection_id"];

                    if((int)end_points[end_id]["next_connection"] == target_connection_json_id){
                        int target_network_id = (int)temp_synaptic_information["network_id"];
                        target_connection = _network_list[target_network_id]->_connections[target_connection_id];
                        Connection::s_max_id = source_network->_connections.size();
                        temp_connection = source_network->add_synaptic_connection(source_neuron_id, target_connection, base_weight,
                                                                                  connection_type, function_type, learning_type,
                                                                                  transmitter_type);
                        if(temp_connection != nullptr){
                            temp_connection->_json_id = (int)starting_points[start_id]["id"];
                            load_all_connection_parameter(temp_connection, starting_points[start_id]);
                        }
                    }
                }
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
void CognaBuilder::connect_subnet_endpoints_neurons(unsigned int curr_network_id){
    NeuralNetwork *source_network = _network_list[curr_network_id];

    //Walking over all connections(!) leading to an output or subnet output.
    std::vector<nlohmann::json> combined_connection_list = source_network->_subnet_output_connection_list;
    combined_connection_list.insert(std::end(combined_connection_list),
                                    std::begin(source_network->_subnet_input_connection_list),
                                    std::end(source_network->_subnet_input_connection_list));

    for(unsigned int con_id = 0; con_id < combined_connection_list.size(); con_id++){
        if(combined_connection_list[con_id]["next_neuron_function"] == "output" ||
                combined_connection_list[con_id]["prev_neuron_function"] == "input"){
            std::vector<nlohmann::json> starting_points;
            std::vector<nlohmann::json> end_points;

            nlohmann::json source_connection = combined_connection_list[con_id];

            starting_points = find_starting_points(curr_network_id, source_connection);
            if(starting_points.size() > 0){
                end_points = find_end_points(curr_network_id, source_connection);
            }

            create_subnet_neuron_connections(starting_points, end_points);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
void CognaBuilder::connect_subnet_endpoints_synaptic(unsigned int curr_network_id){
    NeuralNetwork *source_network = _network_list[curr_network_id];

    //Walking over all connections(!) leading to an output or subnet output.
    std::vector<nlohmann::json> combined_connection_list = source_network->_subnet_output_connection_list;
    combined_connection_list.insert(std::end(combined_connection_list),
                                    std::begin(source_network->_subnet_input_connection_list),
                                    std::end(source_network->_subnet_input_connection_list));

    for(unsigned int con_id = 0; con_id < combined_connection_list.size(); con_id++){
        if(combined_connection_list[con_id]["next_neuron_function"] == "output" ||
                combined_connection_list[con_id]["prev_neuron_function"] == "input"){
            std::vector<nlohmann::json> starting_points;
            std::vector<nlohmann::json> end_points;

            nlohmann::json source_connection = combined_connection_list[con_id];

            starting_points = find_starting_points(curr_network_id, source_connection);
            if(starting_points.size() > 0){
                end_points = find_end_points(curr_network_id, source_connection);
            }
            if(combined_connection_list[con_id]["next_neuron_function"] == "connection"){
            }

            create_subnet_synaptic_connections(starting_points, end_points);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::connect_subnetworks(){
    for(unsigned int nn=0; nn < _network_list.size(); nn++){
        connect_subnet_endpoints_neurons(nn);
    }

    for(unsigned int nn=0; nn < _network_list.size(); nn++){
        connect_subnet_endpoints_synaptic(nn);
    }

    return SUCCESS_CODE;
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
