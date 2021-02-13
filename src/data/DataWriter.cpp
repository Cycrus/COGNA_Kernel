#include "DataWriter.hpp"

#include <ctime>
#include <cstdint>
#include <iostream>

DataWriter::DataWriter(){
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%Y-%m-%d_%H:%M:%S",timeinfo);
    std::string datetime(buffer);
    std::string short_filename("network_output_" + datetime);
    std::string fullname(short_filename + ".csv");
    std::string temp_filename(fullname);
    filename = temp_filename;
}

DataWriter::~DataWriter(){
    if(_output.is_open()){
        _output.close();
    }
}

int DataWriter::open_file(){
    _output.open(filename, std::ios::in | std::ios::app);

    if(_output.is_open()){
        printf("Saving output to <%s>\n\n", filename.c_str());
        write_head();
        return SUCCESS_CODE;
    }

    printf("[WARNING] Datafile could not be created_\n\n");
    return ERROR_CODE;
}

void DataWriter::write_head(){
    //_output << "time" << ",";
    _output << "step" << ",";
    _output << "type" << ",";

    _output << "N_id" << ",";
    _output << "N_activation" << ",";
    _output << "N_random_chance" << ",";
    _output << "N_activation_threshold" << ",";
    _output << "N_activation_backfall_curvature" << ",";
    _output << "N_activation_backfall_steepness" << ",";
    _output << "N_habituation_threshold" << ",";
    _output << "N_sensitization_threshold" << ",";
    _output << "N_short_habituation_curvature" << ",";
    _output << "N_short_habituation_steepness" << ",";
    _output << "N_short_sensitization_curvature" << ",";
    _output << "N_short_sensitization_steepness" << ",";
    _output << "N_short_dehabituation_curvature" << ",";
    _output << "N_short_dehabituation_steepness" << ",";
    _output << "N_short_desensitization_curvature" << ",";
    _output << "N_short_desensitization_steepness" << ",";
    _output << "N_long_habituation_curvature" << ",";
    _output << "N_long_habituation_steepness" << ",";
    _output << "N_long_sensitization_curvature" << ",";
    _output << "N_long_sensitization_steepness" << ",";
    _output << "N_long_dehabituation_curvature" << ",";
    _output << "N_long_dehabituation_steepness" << ",";
    _output << "N_long_desensitization_curvature" << ",";
    _output << "N_long_desensitization_steepness" << ",";
    _output << "N_presynaptic_potential_curvature" << ",";
    _output << "N_presynaptic_potential_steepness" << ",";
    _output << "N_presynaptic_backfall_curvature" << ",";
    _output << "N_presynaptic_backfall_steepness" << ",";
    _output << "N_long_learning_weight_reduction_curvature" << ",";
    _output << "N_long_learning_weight_reduction_steepness" << ",";
    _output << "N_long_learning_weight_backfall_curvature" << ",";
    _output << "N_long_learning_weight_backfall_steepness" << ",";

    _output << "C_source_neuron" << ",";
    _output << "C_target_neuron" << ",";
    _output << "C_target_connection_source_neuron" << ",";
    _output << "C_short_weight" << ",";
    _output << "C_long_weight" << ",";
    _output << "C_long_learner" << ",";
    _output << "C_presynaptic_potential" << ",";
    _output << "C_activation_type" << ",";
    _output << "C_activation_function" << ",";
    _output << "C_learning_type" << ",";
    _output << "C_transmitter_type" << ",";

    _output << "NET_min_activation" << ",";
    _output << "NET_max_activation" << ",";
    _output << "NET_min_weight" << ",";
    _output << "NET_max_weight" << ",";
    _output << "NET_min_transmitter_weight" << ",";
    _output << "NET_max_transmitter_weight" << ",";
    _output << "NET_transmitter_change_curvature" << ",";
    _output << "NET_transmitter_change_steepness" << ",";
    _output << "NET_transmitter_backfall_curvature" << ",";
    _output << "NET_transmitter_backfall_steepness" << "\n";
}

void DataWriter::write_data(NeuralNetwork *nn){
    int connection_param_gap_size = 32;
    int network_param_gap_size = connection_param_gap_size + 11;

    if(nn->_curr_connections.size() > 0){
        for(unsigned int n=1; n<nn->_neurons.size(); n++){
            /* SAVING NEURONS */
            _output << nn->get_step_count() << ",";
            _output << "neuron" << ",";
            _output << nn->_neurons[n]->_id << ",";
            _output << nn->_neurons[n]->_activation << ",";
            _output << nn->_neurons[n]->_random_chance << ",";
            _output << nn->_neurons[n]->_threshold << ",";
            _output << nn->_neurons[n]->_parameter->activation_backfall_curvature << ",";
            _output << nn->_neurons[n]->_parameter->activation_backfall_steepness << ",";
            _output << nn->_neurons[n]->_parameter->habituation_threshold << ",";
            _output << nn->_neurons[n]->_parameter->sensitization_threshold << ",";
            _output << nn->_neurons[n]->_parameter->short_habituation_curvature << ",";
            _output << nn->_neurons[n]->_parameter->short_habituation_steepness << ",";
            _output << nn->_neurons[n]->_parameter->short_sensitization_curvature << ",";
            _output << nn->_neurons[n]->_parameter->short_sensitization_steepness << ",";
            _output << nn->_neurons[n]->_parameter->short_dehabituation_curvature << ",";
            _output << nn->_neurons[n]->_parameter->short_dehabituation_steepness << ",";
            _output << nn->_neurons[n]->_parameter->short_desensitization_curvature << ",";
            _output << nn->_neurons[n]->_parameter->short_desensitization_steepness << ",";
            _output << nn->_neurons[n]->_parameter->long_habituation_curvature << ",";
            _output << nn->_neurons[n]->_parameter->long_habituation_steepness << ",";
            _output << nn->_neurons[n]->_parameter->long_sensitization_curvature << ",";
            _output << nn->_neurons[n]->_parameter->long_sensitization_steepness << ",";
            _output << nn->_neurons[n]->_parameter->long_dehabituation_curvature << ",";
            _output << nn->_neurons[n]->_parameter->long_dehabituation_steepness << ",";
            _output << nn->_neurons[n]->_parameter->long_desensitization_curvature << ",";
            _output << nn->_neurons[n]->_parameter->long_desensitization_steepness << ",";
            _output << nn->_neurons[n]->_parameter->presynaptic_potential_curvature << ",";
            _output << nn->_neurons[n]->_parameter->presynaptic_potential_steepness << ",";
            _output << nn->_neurons[n]->_parameter->presynaptic_backfall_curvature << ",";
            _output << nn->_neurons[n]->_parameter->presynaptic_backfall_steepness << ",";
            _output << nn->_neurons[n]->_parameter->long_learning_weight_reduction_curvature << ",";
            _output << nn->_neurons[n]->_parameter->long_learning_weight_reduction_steepness << ",";
            _output << nn->_neurons[n]->_parameter->long_learning_weight_backfall_curvature << ",";
            _output << nn->_neurons[n]->_parameter->long_learning_weight_backfall_steepness;
            _output << "\n";

            /* SAVING CONNECTIONS */
            for(unsigned int con=0; con<nn->_neurons[n]->_connections.size(); con++){
                _output << nn->get_step_count() << ",";
                _output << "connection" << ",";
                for(int i=0; i<connection_param_gap_size; i++){
                    _output << ",";
                }
                _output << nn->_neurons[n]->_id << ",";
                if(nn->_neurons[n]->_connections[con]->next_neuron){
                    _output << nn->_neurons[n]->_connections[con]->next_neuron->_id << ",";
                    _output << ",";
                }
                else{
                    _output << ",";
                    _output << nn->_neurons[n]->_connections[con]->next_connection->prev_neuron->_id << ",";
                }

                _output << nn->_neurons[n]->_connections[con]->short_weight << ",";
                _output << nn->_neurons[n]->_connections[con]->long_weight << ",";
                _output << nn->_neurons[n]->_connections[con]->long_learning_weight << ",";
                _output << nn->_neurons[n]->_connections[con]->presynaptic_potential << ",";
                _output << nn->_neurons[n]->_connections[con]->_parameter->activation_type << ",";
                _output << nn->_neurons[n]->_connections[con]->_parameter->activation_function << ",";
                _output << nn->_neurons[n]->_connections[con]->_parameter->learning_type << ",";
                _output << nn->_neurons[n]->_connections[con]->_parameter->transmitter_type;
                _output << "\n";
            }
        }

        /* SAVING NETWORK PARAMETERS */
        _output << nn->get_step_count() << ",";
        _output << "global_parameter" << ",";
        for(int i=0; i<network_param_gap_size; i++){
            _output << ",";
        }
        _output << nn->_parameter->min_activation << ",";
        _output << nn->_parameter->max_activation << ",";
        _output << nn->_parameter->min_weight << ",";
        _output << nn->_parameter->max_weight << ",";
        _output << nn->_parameter->min_transmitter_weight << ",";
        _output << nn->_parameter->max_transmitter_weight << ",";
        _output << nn->_parameter->transmitter_change_curvature << ",";
        _output << nn->_parameter->transmitter_change_steepness << ",";
        _output << nn->_parameter->transmitter_backfall_curvature << ",";
        _output << nn->_parameter->transmitter_backfall_steepness;
        _output << "\n";
    }
}
