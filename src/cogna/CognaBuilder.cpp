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
#include "json.hpp"
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
    std::cout << _neuron_types["Default"].initial_base_weight << std::endl;
    std::cout << _neuron_types["TestNeuronType_1"].initial_base_weight << std::endl;
    for(unsigned int i=0; i<_transmitter_types.size(); i++){
        std::cout << _transmitter_types[i] << std::endl;
    }
    std::cout << _neuron_types["Default"].transmitter_type << std::endl;

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
        NeuronParameterHandler temp_neuron;

        temp_neuron.initial_base_weight = std::stof((std::string)it.value()["base_weight"]);

        if(it.value()["activation_type"] == "Excitatory")
            temp_neuron.activation_type = EXCITATORY;
        else if(it.value()["activation_type"] == "Inhibitory")
            temp_neuron.activation_type = INHIBITORY;
        else if(it.value()["activation_type"] == "Nondirectional")
            temp_neuron.activation_type = NONDIRECTIONAL;

        if(it.value()["activation_function"] == "Linear")
            temp_neuron.activation_function = FUNCTION_LINEAR;
        else if(it.value()["activation_function"] == "Relu")
            temp_neuron.activation_function = FUNCTION_RELU;
        else if(it.value()["activation_function"] == "Sigmoid")
            temp_neuron.activation_function = FUNCTION_SIGMOID;

        if(it.value()["learning_type"] == "None")
            temp_neuron.learning_type = LEARNING_NONE;
        else if(it.value()["learning_type"] == "Habituation")
            temp_neuron.learning_type = LEARNING_HABITUATION;
        else if(it.value()["learning_type"] == "Sensitization")
            temp_neuron.learning_type = LEARNING_SENSITIZATION;
        else if(it.value()["learning_type"] == "HabiSens")
            temp_neuron.learning_type = LEARNING_HABISENS;

        for(unsigned int i=0; i<_transmitter_types.size(); i++){
            if(_transmitter_types[i] == it.value()["transmitter_type"]){
                temp_neuron.transmitter_type = i;
            }
            if(_transmitter_types[i] == it.value()["influenced_transmitter"]){
                temp_neuron.influenced_transmitter = i;
            }
        }

        temp_neuron.activation_threshold = std::stof((std::string)it.value()["activation_threshold"]);

        temp_neuron.max_activation = std::stof((std::string)it.value()["max_activation"]);
        temp_neuron.min_activation = std::stof((std::string)it.value()["min_activation"]);

        temp_neuron.activation_backfall_curvature = std::stof((std::string)it.value()["activation_backfall_curvature"]);
        temp_neuron.activation_backfall_steepness = std::stof((std::string)it.value()["activation_backfall_steepness"]);

        temp_neuron.transmitter_change_curvature = std::stof((std::string)it.value()["transmitter_change_curvature"]);
        temp_neuron.transmitter_change_steepness = std::stof((std::string)it.value()["transmitter_change_steepness"]);

        if(it.value()["transmitter_influence_direction"] == "Positive Influence")
            temp_neuron.transmitter_influence_direction = POSITIVE_INFLUENCE;
        else if(it.value()["transmitter_influence_direction"] == "Negative Influence")
            temp_neuron.transmitter_influence_direction = NEGATIVE_INFLUENCE;

        temp_neuron.random_chance = std::stoi((std::string)it.value()["random_chance"]);
        if(temp_neuron.random_chance > 0){
            temp_neuron.random_activation = true;
        }
        else{
            temp_neuron.random_activation = false;
        }
        temp_neuron.random_activation_value = std::stof((std::string)it.value()["random_activation_value"]);

        temp_neuron.max_weight = std::stof((std::string)it.value()["max_weight"]);
        temp_neuron.min_weight = std::stof((std::string)it.value()["min_weight"]);

        temp_neuron.short_habituation_curvature = std::stof((std::string)it.value()["short_habituation_curvature"]);
        temp_neuron.short_habituation_steepness = std::stof((std::string)it.value()["short_habituation_steepness"]);
        temp_neuron.short_sensitization_curvature = std::stof((std::string)it.value()["short_sensitization_curvature"]);
        temp_neuron.short_sensitization_steepness = std::stof((std::string)it.value()["short_sensitization_steepness"]);
        temp_neuron.short_dehabituation_curvature = std::stof((std::string)it.value()["short_dehabituation_curvature"]);
        temp_neuron.short_dehabituation_steepness = std::stof((std::string)it.value()["short_dehabituation_steepness"]);
        temp_neuron.short_desensitization_curvature = std::stof((std::string)it.value()["short_desensitization_curvature"]);
        temp_neuron.short_desensitization_steepness = std::stof((std::string)it.value()["short_desensitization_steepness"]);

        temp_neuron.long_habituation_curvature = std::stof((std::string)it.value()["long_habituation_curvature"]);
        temp_neuron.long_habituation_steepness = std::stof((std::string)it.value()["long_habituation_steepness"]);
        temp_neuron.long_sensitization_curvature = std::stof((std::string)it.value()["long_sensitization_curvature"]);
        temp_neuron.long_sensitization_steepness = std::stof((std::string)it.value()["long_sensitization_steepness"]);
        temp_neuron.long_dehabituation_curvature = std::stof((std::string)it.value()["long_dehabituation_curvature"]);
        temp_neuron.long_dehabituation_steepness = std::stof((std::string)it.value()["long_dehabituation_steepness"]);
        temp_neuron.long_desensitization_curvature = std::stof((std::string)it.value()["long_desensitization_curvature"]);
        temp_neuron.long_desensitization_steepness = std::stof((std::string)it.value()["long_desensitization_steepness"]);

        temp_neuron.presynaptic_potential_curvature = std::stof((std::string)it.value()["presynaptic_potential_curvature"]);
        temp_neuron.presynaptic_potential_steepness = std::stof((std::string)it.value()["presynaptic_potential_steepness"]);
        temp_neuron.presynaptic_backfall_curvature = std::stof((std::string)it.value()["presynaptic_backfall_curvature"]);
        temp_neuron.presynaptic_backfall_steepness = std::stof((std::string)it.value()["presynaptic_backfall_steepness"]);

        temp_neuron.long_learning_weight_reduction_curvature = std::stof((std::string)it.value()["long_learning_weight_reduction_curvature"]);
        temp_neuron.long_learning_weight_reduction_steepness = std::stof((std::string)it.value()["long_learning_weight_reduction_steepness"]);
        temp_neuron.long_learning_weight_backfall_curvature = std::stof((std::string)it.value()["long_learning_weight_backfall_curvature"]);
        temp_neuron.long_learning_weight_backfall_steepness = std::stof((std::string)it.value()["long_learning_weight_backfall_steepness"]);

        temp_neuron.habituation_threshold = std::stof((std::string)it.value()["habituation_threshold"]);
        temp_neuron.sensitization_threshold = std::stof((std::string)it.value()["sensitization_threshold"]);

        _neuron_types[it.key()] = temp_neuron;
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

    // neurons = network_json["neurons"]
    // connections = network_json["connections"]
    // nodes = network_json["nodes"]
    // network = network_json["network"]

    int error_code = SUCCESS_CODE;

    NeuralNetwork *nn = new NeuralNetwork();

    for(unsigned int i=0; i < network_json["neurons"].size(); i++){
        std::string neuron_type = "";
        try{
            neuron_type = network_json["neurons"][i]["neuron_type"];
        }
        catch(...){
            neuron_type = "Default";
        }

        float temp_threshold = 0.0;
        auto threshold_checker = network_json["neurons"][i].find("activation_threshold");
        if(threshold_checker != network_json["neurons"][i].end()){
            temp_threshold = std::stof((std::string)network_json["neurons"][i]["activation_threshold"]);
        }
        else{
            temp_threshold = _neuron_types[neuron_type].activation_threshold;
        }

        nn->add_neuron(temp_threshold);
    }

    if(error_code == SUCCESS_CODE){
        _network_list.push_back(nn);
        return SUCCESS_CODE;
    }
    else{
        delete nn;
        return ERROR_CODE;
    }
}

} //namespace COGNA
