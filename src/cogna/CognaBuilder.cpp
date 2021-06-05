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

#include <iostream>
#include <fstream>

namespace COGNA{

CognaBuilder::CognaBuilder(std::string project_name){
    _project_name = project_name;
    _project_path = "Projects/" + project_name + "/";
    _frequency = 0;
}

//----------------------------------------------------------------------------------------------------------------------
//
void CognaBuilder::tester(){
    std::cout << _neuron_types["Default"]["base_weight"] << std::endl;
    std::cout << _neuron_types["TestNeuronType_1"]["base_weight"] << std::endl;
    for(unsigned int i=0; i<_transmitter_types.size(); i++){
        std::cout << _transmitter_types[i] << std::endl;
    }
    std::cout << _neuron_types["Default"]["transmitter_type"] << std::endl;

    for(unsigned int i=0; i < _network_list[0]->_neurons.size(); i++){
        std::cout << "N_" << _network_list[0]->_neurons[i]->_id << " -> "
                  << _network_list[0]->_neurons[i]->_parameter->activation_threshold << std::endl;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
CognaBuilder::~CognaBuilder(){
    for(unsigned int i=0; i < _network_list.size(); i++){
        delete _network_list[i];
        _network_list[i] = nullptr;
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::build_cogna_cluster(){
    if(load_globals_file() == ERROR_CODE) return ERROR_CODE;
    if(load_transmitters() == ERROR_CODE) return ERROR_CODE;
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
    load_network_parameter(nn, network_json);
    nn->define_transmitters(_transmitter_types.size());
    load_neurons(nn, network_json);


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
    float return_value = 0.0;
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
    else if(parameter == "influenced_parameter"){
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
            return_value = neuron[parameter];
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

} //namespace COGNA
