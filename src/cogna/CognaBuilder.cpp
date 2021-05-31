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

#include <iostream>
#include <fstream>

namespace COGNA{

CognaBuilder::CognaBuilder(std::string project_name){
    _project_name = project_name;
    _project_path = "Networks/" + project_name + "/";
    _frequency = 0;
}

void CognaBuilder::tester(){
    std::cout << _neuron_types["Default"].max_weight << std::endl;
    for(unsigned int i=0; i<_transmitter_types.size(); i++){
        std::cout << _transmitter_types[i] << std::endl;
    }
    std::cout << _neuron_types["Default"].transmitter_type << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
//
CognaBuilder::~CognaBuilder(){

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
        //TODO temp_neuron.base_weight

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

    int i = 0;
    while(true){
        try{
            _transmitter_types.push_back(transmitter_json["transmitters"][i]);
        }
        catch(...){
            break;
        }

        i++;
    }

    return SUCCESS_CODE;
}

//----------------------------------------------------------------------------------------------------------------------
//
int CognaBuilder::load_network(std::string network_name){
    return SUCCESS_CODE;
}

} //namespace COGNA
