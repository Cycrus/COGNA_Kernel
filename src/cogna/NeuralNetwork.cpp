/**
 * @file NeuralNetwork.cpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief Implementation of NeuralNetwork class
 *
 * @date 2021-02-18
 *
 */

#include "NeuralNetwork.hpp"

#include <cstdio>
#include <ctime>
#include <cmath>
#include <sys/time.h>
#include "Constants.hpp"
#include "MathUtils.hpp"
#include "LoggerStd.hpp"

using namespace COGNA;

namespace COGNA{
    long get_time_microsec(struct timeval time){
        gettimeofday(&time, NULL);
        return ((time.tv_sec * MICROSECOND_FACTOR + time.tv_usec));
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    NeuralNetwork::NeuralNetwork(){
        Logger::init_Global(new LoggerStd());

        _parameter = new NeuralNetworkParameterHandler();
        add_neuron(99999.0);
        _network_step_counter = 0;
        _transmitter_weights.push_back(1.0f);
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    NeuralNetwork::~NeuralNetwork(){
        _curr_connections.clear();
        _next_connections.clear();

        for(unsigned int i=0; i<_neurons.size(); i++){
            delete _neurons[i];
            _neurons[i] = NULL;
        }
        _neurons.clear();
        _random_neurons.clear();

        _transmitter_weights.clear();

        delete _parameter;
        _parameter = NULL;

        Logger::destroy_Global();
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    int NeuralNetwork::add_neuron(float threshold){
        Neuron *temp_neuron = new Neuron(_parameter);

        temp_neuron->_parameter->activation_threshold = threshold;

        _neurons.push_back(temp_neuron);
        return SUCCESS_CODE;
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    int NeuralNetwork::set_neural_transmitter_influence(int neuron_id,
                                                        int transmitter_id,
                                                        int influence_direction){
        if(neuron_id >= MIN_NEURON_ID && (unsigned)neuron_id < _neurons.size()){
            if(transmitter_id >= 0 && (unsigned)transmitter_id < _transmitter_weights.size()){
                if(influence_direction == POSITIVE_INFLUENCE ||
                   influence_direction == NEGATIVE_INFLUENCE){
                       _neurons[neuron_id]->_parameter->influenced_transmitter = transmitter_id;
                       _neurons[neuron_id]->_parameter->transmitter_influence_direction = influence_direction;
                       return SUCCESS_CODE;
                }
            }
            else{
                LOG_ERROR("Invalid transmitter ID <%d> when assigning transmitter influence to N-%d\n",
                       transmitter_id, neuron_id);
            }
        }
        else{
            LOG_ERROR("Invalid neuron ID when assigning transmitter influence to N-%d.\n", neuron_id);
        }
        return ERROR_CODE;
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
     int NeuralNetwork::define_transmitters(int number){
        _parameter->transmitter_amount = number;

        if(number > 0){
            for(int i=1; i<number; i++){
                _transmitter_weights.push_back(1.0f);
                return SUCCESS_CODE;
            }
        }
        else{
            LOG_ERROR("Invalid transmitter number of transmitters <%d> entered.\n", number);
        }
        return ERROR_CODE;
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
     int NeuralNetwork::set_random_neuron_activation(int neuron_id,
                                                     int chance,
                                                     float activation_value){
         if(neuron_id >= MIN_NEURON_ID && (unsigned)neuron_id < _neurons.size()){
            if(chance >= 0 && chance < MAX_CHANCE){
                _neurons[neuron_id]->set_random_activation(chance, activation_value);
                return SUCCESS_CODE;
            }
            else{
                LOG_ERROR("Invalid chance when setting random activation for N-%d.\n", neuron_id);
            }
         }
         else{
             LOG_ERROR("Invalid neuron ID when setting random activation for N-%d.\n", neuron_id);
         }
         return ERROR_CODE;
     }

     //----------------------------------------------------------------------------------------------------------------------
     //
    Connection* NeuralNetwork::add_neuron_connection(int source_neuron, int target_neuron, float weight,
                                                     int connection_type,
                                                     int function_type,
                                                     int learning_type,
                                                     int transmitter_type){
        if(connection_type != EXCITATORY && connection_type != INHIBITORY){
            LOG_WARN("Invalid connection type for C-%d~%d.\n", source_neuron, target_neuron);
            LOG_INFO("Connection type changed to <EXCITATORY>.\n");
            connection_type = EXCITATORY;
        }
        if(transmitter_type == NO_TRANSMITTER){
            transmitter_type = STD_TRANSMITTER;
        }
        else if(transmitter_type < NO_TRANSMITTER || (unsigned)transmitter_type >= _transmitter_weights.size()){
            LOG_WARN("Invalid transmitter type for C-%d~%d.\n", source_neuron, target_neuron);
            LOG_INFO("Transmitter changed to std transmitter <0>.\n");
            transmitter_type = STD_TRANSMITTER;
        }

        if(source_neuron >= MIN_NEURON_ID || target_neuron >= MIN_NEURON_ID){
            if(source_neuron < Neuron::s_max_id && target_neuron < Neuron::s_max_id){
                Connection *temp_con = _neurons[source_neuron]->add_neuron_connection(_neurons[target_neuron], weight,
                                                                                      connection_type,
                                                                                      function_type,
                                                                                      learning_type,
                                                                                      transmitter_type);
                return temp_con;
            }
            else{
                LOG_ERROR("Addressed neuron IDs are not existing for C-%d~%d.\n", source_neuron, target_neuron);
            }
        }
        else{
            LOG_ERROR("Neuron IDs start at 1. Please check your given neurons for C-%d~%d.\n", source_neuron, target_neuron);
        }
        LOG_ERROR("Connecting N-%d to N-%d was unsuccessful.\n", source_neuron, target_neuron);
        return NULL;
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    Connection* NeuralNetwork::add_synaptic_connection(int source_neuron,
                                                       int connected_neuron_1,
                                                       int connected_neuron_2,
                                                       float weight,
                                                       int connection_type,
                                                       int function_type,
                                                       int learning_type,
                                                       int transmitter_type){
        // TODO Too many nested constructs
        if(connection_type != EXCITATORY && connection_type != INHIBITORY && connection_type != NONDIRECTIONAL){
            LOG_WARN("Invalid connection type for C-%d~C-%d.\n", source_neuron, connected_neuron_1);
            LOG_INFO("Connection type changed to <EXCITATORY>.\n");
            connection_type = EXCITATORY;
        }
        if(transmitter_type == NO_TRANSMITTER){
            transmitter_type = STD_TRANSMITTER;
        }
        else if(transmitter_type < NO_TRANSMITTER || (unsigned)transmitter_type >= _transmitter_weights.size()){
            LOG_WARN("Invalid transmitter type for C-%d~C-%d\n", source_neuron, connected_neuron_1);
            transmitter_type = STD_TRANSMITTER;
        }

        if(source_neuron >= MIN_NEURON_ID && connected_neuron_1 >= MIN_NEURON_ID && connected_neuron_2 >= MIN_NEURON_ID){
            if(source_neuron < Neuron::s_max_id && connected_neuron_1 < Neuron::s_max_id && connected_neuron_2 < Neuron::s_max_id){
                Connection *temp_connection = NULL;
                for(unsigned i=0; i<_neurons[connected_neuron_1]->_connections.size(); i++){
                    if(_neurons[connected_neuron_1]->_connections[i]->next_neuron){
                        if(_neurons[connected_neuron_1]->_connections[i]->next_neuron->_id == connected_neuron_2){
                            temp_connection = _neurons[connected_neuron_1]->_connections[i];
                        }
                    }
                }
                if(temp_connection){
                    temp_connection = _neurons[source_neuron]->add_synaptic_connection(temp_connection, weight, connection_type,
                                                                                      function_type, learning_type, transmitter_type);
                    return temp_connection;
                }
                else{
                    LOG_ERROR("Neuron N-%d and N-%d are not connected.\n", connected_neuron_1, connected_neuron_2);
                }
            }
            else{
                LOG_ERROR("Addressed neuron IDs are not existing for C-%d~C-%d.\n", source_neuron, connected_neuron_1);
            }
        }
        else{
            LOG_ERROR("Neuron IDs start at 1. Please check your given neurons for C-%d.\n", source_neuron);
        }
        LOG_ERROR("Connecting N-%d to C-%d was unsuccessful.\n", source_neuron, connected_neuron_1);
        return NULL;
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    Connection* NeuralNetwork::add_synaptic_connection(int source_neuron,
                                                       Connection *con,
                                                       float weight,
                                                       int connection_type,
                                                       int function_type,
                                                       int learning_type,
                                                       int transmitter_type){
        if(connection_type != EXCITATORY && connection_type != INHIBITORY && connection_type != NONDIRECTIONAL){
            LOG_WARN("Invalid connection type for C-%d~C-%d.\n", source_neuron, con->prev_neuron->_id);
            LOG_INFO("Connection type changed to <EXCITATORY>.\n");
            connection_type = EXCITATORY;
        }
        if(transmitter_type == NO_TRANSMITTER){
            transmitter_type = STD_TRANSMITTER;
        }
        else if(transmitter_type < NO_TRANSMITTER || (unsigned)transmitter_type >= _transmitter_weights.size()){
            LOG_WARN("Invalid transmitter type for C-%d~C-%d\n", source_neuron, con->prev_neuron->_id);
            transmitter_type = STD_TRANSMITTER;
        }
        if(source_neuron >= MIN_NEURON_ID){
            if(source_neuron < Neuron::s_max_id){
                Connection *temp_con = _neurons[source_neuron]->add_synaptic_connection(con, weight, connection_type,
                                                                                        function_type, learning_type, transmitter_type);
                return temp_con;
            }
            else{
                LOG_ERROR("Addressed neuron IDs are not existing for C-%d~C-%d.\n", source_neuron, con->prev_neuron->_id);
            }
        }
        else{
            LOG_ERROR("Neuron IDs start at 1. Please check your given neurons for C-%d~C%d.\n", source_neuron, con->prev_neuron->_id);
        }
        LOG_ERROR("Connecting N-%d to C-%d was unsuccessful.\n", source_neuron, con->prev_neuron->_id);
        return NULL;
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    int NeuralNetwork::init_activation(int target_neuron, float activation){
        if(target_neuron >= MIN_NEURON_ID && target_neuron < Neuron::s_max_id){
            _neurons[target_neuron]->_activation = activation;

            _curr_connections.insert(std::end(_curr_connections),
                                     std::begin(_neurons[target_neuron]->_connections),
                                     std::end(_neurons[target_neuron]->_connections));
        }
        else{
            LOG_ERROR("Initializing activation in N-%d was unsuccessful. Invalid ID.\n", target_neuron);
            return ERROR_CODE;
        }
        return SUCCESS_CODE;
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    int NeuralNetwork::setup_network(){
        for(unsigned int n=MIN_NEURON_ID; n<_neurons.size(); n++){
            if(_neurons[n]->_connections.size() == 0){
                _neurons[n]->add_neuron_connection(_neurons[0], 0.0);
            }

            if(_neurons[n]->_random_activation == true){
                _random_neurons.push_back(_neurons[n]);
            }
        }

        srandom(time(0));
        return SUCCESS_CODE;
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    int64_t NeuralNetwork::get_step_count(){
        return _network_step_counter;
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    Neuron* NeuralNetwork::get_neuron(int neuron_id){
        if(neuron_id >= MIN_NEURON_ID && (unsigned)neuron_id < _neurons.size()){
            return _neurons[neuron_id];
        }
        else{
            LOG_WARN("Neuron ID %d for getting neuron is invalid.\n", neuron_id);
        }
        return NULL;
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    bool NeuralNetwork::neuron_is_active(int neuron_id){
        if(neuron_id >= MIN_NEURON_ID && (unsigned)neuron_id < _neurons.size()){
            return _neurons[neuron_id]->is_active();
        }
        else{
            LOG_WARN("Neuron ID %d for getting neuron state is invalid.\n", neuron_id);
        }
        return 0.0f;
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    float NeuralNetwork::get_neuron_activation(int neuron_id){
        if(neuron_id >= MIN_NEURON_ID && (unsigned)neuron_id < _neurons.size()){
            return _neurons[neuron_id]->_activation;
        }
        else{
            LOG_WARN("Neuron ID %d for getting neuron activation is invalid.\n", neuron_id);
        }
        return 0.0f;
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    float NeuralNetwork::get_transmitter_weight(int transmitter_id){
        if(transmitter_id >= 0 && (unsigned)transmitter_id < _transmitter_weights.size()){
            return _transmitter_weights[transmitter_id];
        }
        else{
            LOG_WARN("Transmitter ID %d for getting transmitter weight is invalid.\n", transmitter_id);
        }
        return 0.0f;
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    void NeuralNetwork::change_transmitter_weight(int transmitter_id, float new_weight){
        if(DEBUG_MODE == true && DEB_TRANSMITTER == true)
            printf("<%ld> T-%d -> Weight before change = %.3f\n",
                   _network_step_counter, transmitter_id, _transmitter_weights[transmitter_id]);

        _transmitter_weights[transmitter_id] = new_weight;

        if(DEBUG_MODE == true && DEB_TRANSMITTER == true)
            printf("<%ld> T-%d -> Weight after change = %.3f\n\n",
                   _network_step_counter, transmitter_id, _transmitter_weights[transmitter_id]);
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    void NeuralNetwork::influence_transmitter(Neuron *n){
        if(n->_last_fired_step < _network_step_counter){
            if(n->_parameter->influenced_transmitter > NO_TRANSMITTER){
                if(n->_parameter->transmitter_influence_direction == POSITIVE_INFLUENCE){
                    change_transmitter_weight(n->_parameter->influenced_transmitter,
                                               MathUtils::calculate_dynamic_gradient(_transmitter_weights[n->_parameter->influenced_transmitter],
                                              n->_parameter->transmitter_change_steepness,
                                              n->_activation,
                                              n->_parameter->transmitter_change_curvature,
                                              ADD,
                                              _parameter->max_transmitter_weight,
                                              _parameter->min_transmitter_weight));
                }

                else if(n->_parameter->transmitter_influence_direction == NEGATIVE_INFLUENCE){
                    change_transmitter_weight(n->_parameter->influenced_transmitter,
                                               MathUtils::calculate_dynamic_gradient(_transmitter_weights[n->_parameter->influenced_transmitter],
                                              n->_parameter->transmitter_change_steepness,
                                              n->_activation,
                                              n->_parameter->transmitter_change_curvature,
                                              SUBTRACT,
                                              _parameter->max_transmitter_weight,
                                              _parameter->min_transmitter_weight));
                }
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    void NeuralNetwork::transmitter_backfall(){
        for(unsigned i=0; i<_transmitter_weights.size(); i++){
            if(_transmitter_weights[i] > 1.0f){
                _transmitter_weights[i] =  MathUtils::calculate_static_gradient(_transmitter_weights[i],
                                                                    _parameter->transmitter_backfall_steepness,
                                                                    1,
                                                                    _parameter->transmitter_backfall_curvature,
                                                                    SUBTRACT,
                                                                    _parameter->max_transmitter_weight,
                                                                    DEFAULT_TRANSMITTER_WEIGHT);
            }

            else if(_transmitter_weights[i] < 1.0f){
                _transmitter_weights[i] =  MathUtils::calculate_dynamic_gradient(_transmitter_weights[i],
                                                                     _parameter->transmitter_backfall_steepness,
                                                                     1,
                                                                     _parameter->transmitter_backfall_curvature,
                                                                     ADD,
                                                                     DEFAULT_TRANSMITTER_WEIGHT,
                                                                     _parameter->min_transmitter_weight);
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    void NeuralNetwork::activate_next_entities(){
        for(unsigned int con=0; con<_curr_connections.size(); con++){
            if(_curr_connections[con]->prev_neuron->_activation >= _curr_connections[con]->prev_neuron->_parameter->activation_threshold){
                _curr_connections[con]->basic_learning(_network_step_counter);
                _curr_connections[con]->presynaptic_potential = 2.0f;
                influence_transmitter(_curr_connections[con]->prev_neuron);

                if(_curr_connections[con]->next_neuron){
                    activate_next_neuron(_curr_connections[con]);
                }

                else if(_curr_connections[con]->next_connection){
                    activate_next_connection(_curr_connections[con]);
                }

                _curr_connections[con]->prev_neuron->_last_fired_step = _network_step_counter;
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    void NeuralNetwork::activate_next_neuron(Connection *con){
        con->next_neuron->calculate_neuron_backfall(_network_step_counter);

        con->prev_neuron->_temp_activation = con->short_weight *
                                             con->prev_neuron->_activation;

        con->next_neuron->_activation +=
              con->choose_activation_function(con->prev_neuron->_temp_activation) *
              con->_parameter->activation_type *
              _transmitter_weights[con->_parameter->transmitter_type];

        con->next_neuron->_was_activated = true;

        if(con->next_neuron->_id != 0){
            if(DEBUG_MODE && DEB_BASE){
                printf("<%ld> N-%d~N-%d -> force = %.2f\n",
                       _network_step_counter,
                       con->prev_neuron->_id,
                       con->next_neuron->_id,
                       con->prev_neuron->_activation);
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    void NeuralNetwork::activate_next_connection(Connection *con){
        if(DEBUG_MODE && DEB_PRESYNAPTIC){
            printf("<%ld> C-%d -> Presynaptic potential before influence : %.3f\n",
                   _network_step_counter,
                   con->prev_neuron->_id,
                   con->next_connection->presynaptic_potential);
        }

        con->presynaptic_potential_backfall(_network_step_counter);
        if(con->next_connection->presynaptic_potential > DEFAULT_PRESYNAPTIC_POTENTIAL){
            con->next_connection->basic_learning(_network_step_counter, con);
            con->next_connection->presynaptic_potential = DEFAULT_PRESYNAPTIC_POTENTIAL;
        }

        con->next_connection->last_presynaptic_activated_step = _network_step_counter;

        if(DEBUG_MODE && DEB_PRESYNAPTIC){
            printf("<%ld> C-%d -> Presynaptic potential after influence : %.3f\n",
                   _network_step_counter,
                   con->prev_neuron->_id,
                   con->next_connection->presynaptic_potential);
        }
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    void NeuralNetwork::save_next_neurons(){
        // TODO Too chaotic
        if(DEBUG_MODE && _curr_connections.size() > 0)
            printf("\n*******************NEXT STEP*******************\n\n");

        for(unsigned int con=0; con<_curr_connections.size(); con++){
            /* Only do if neuron fired in this round */
            _curr_connections[con]->prev_neuron->clear_neuron_activation(_network_step_counter);

            if(_curr_connections[con]->next_neuron){
                _curr_connections[con]->next_neuron->_was_activated = false;

                /* Only do if next neuron is really activated */
                if(_curr_connections[con]->next_neuron->_activation > 0){
                    _curr_connections[con]->next_neuron->set_step(_network_step_counter);

                    int is_contained = false;
                    for(unsigned int nex=0; nex<_next_connections.size(); nex++){
                        if(_curr_connections[con]->next_neuron == _next_connections[nex]->prev_neuron){
                            is_contained = true;
                        }
                    }
                    /* Only do if neuron is not already in the next_connections list */
                    if(is_contained == false){
                        _next_connections.insert(std::end(_next_connections),
                                                 std::begin(_curr_connections[con]->next_neuron->_connections),
                                                 std::end(_curr_connections[con]->next_neuron->_connections));
                    }
                }
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    void NeuralNetwork::activate_random_neurons(){
        for(unsigned int n = 0; n < _random_neurons.size(); n++){
            if(random()%MAX_CHANCE <= _random_neurons[n]->_random_chance){
                init_activation(_random_neurons[n]->_id, _random_neurons[n]->_random_activation_value);
                srandom(time(0));
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    void NeuralNetwork::switch_vectors(){
        _curr_connections = _next_connections;
        _next_connections.clear();
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    void NeuralNetwork::feed_forward(){
        _network_step_counter += 1;

        transmitter_backfall();
        activate_random_neurons();
        activate_next_entities();
        save_next_neurons();
        switch_vectors();
    }

    //----------------------------------------------------------------------------------------------------------------------
    //
    void NeuralNetwork::print_activation(){
        // TODO Too many nested constructs
        if(_curr_connections.size() > 0){
            printf("\n");
            for(unsigned int con=0; con<_curr_connections.size(); con++){
                if(_curr_connections[con]->next_neuron){
                    if(_curr_connections[con]->next_neuron->_id != 0){
                        printf("*** N-%d fires at N-%d ***\n", _curr_connections[con]->prev_neuron->_id,
                                                               _curr_connections[con]->next_neuron->_id);
                    }
                }
                else if(_curr_connections[con]->next_connection){
                    printf("*** N-%d fires at C-%d ***\n", _curr_connections[con]->prev_neuron->_id,
                                                           _curr_connections[con]->next_connection->prev_neuron->_id);
                }
            }
            printf("\n");
        }
    }
}
