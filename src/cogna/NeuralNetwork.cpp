#include "NeuralNetwork.hpp"

#include <cstdio>
#include <ctime>
#include <cmath>
#include <sys/time.h>

bool DATA_ANALYTIC_OUTPUT = true;

bool DEBUG_MODE = false;
bool DEB_BASE = true;
bool DEB_HABITUATION = false;
bool DEB_SENSITIZATION = false;
bool DEB_TRANSMITTER = false;
bool DEB_NEURON_BACKFALL = false;
bool DEB_LONG_LEARNING_WEIGHT = false;
bool DEB_PRESYNAPTIC = false;

long get_time_microsec(struct timeval time){
    gettimeofday(&time, NULL);
    return ((time.tv_sec * MICROSECOND_FACTOR + time.tv_usec));
}

/***********************************************************
 * NeuralNetwork::NeuralNetwork
 *
 * Description: Initializes Neural Network by adding the NULL-neuron, which is
 *              needed for tangling neurons (without progressive connections) to
 *              get recognized by the algorithm. The setup_network() function
 *              connects all tangling neurons with this NULL-neuron. It is
 *              forbidden to access it.
 */
NeuralNetwork::NeuralNetwork(){
    add_neuron(99999.0);
    _network_step_counter = 0;

    _parameter = new NeuralNetworkParameterHandler();

    _transmitter_weights.push_back(1.0f);
}

/***********************************************************
 * NeuralNetwork::~NeuralNetwork()
 *
 * Description: Destructor. Clears every single vector
 */
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
}

/***********************************************************
 * NeuralNetwork::add_neuron()
 *
 * Description: Adds a new neuron to the network
 *
 * Parameters:  float    threshold          threshold when neuron can fire
 *              int      random_activation  determines if neuron sometimes fires randomly
 *
 * Return:      int      error code
 */
int NeuralNetwork::add_neuron(float threshold){
    Neuron *temp_neuron = new Neuron(threshold);

    temp_neuron->_parameter->activation_backfall_curvature = _parameter->activation_backfall_curvature;
    temp_neuron->_parameter->activation_backfall_steepness = _parameter->activation_backfall_steepness;

    temp_neuron->_parameter->short_habituation_curvature = _parameter->short_habituation_curvature;
    temp_neuron->_parameter->short_habituation_steepness = _parameter->short_habituation_steepness;
    temp_neuron->_parameter->short_sensitization_curvature = _parameter->short_sensitization_curvature;
    temp_neuron->_parameter->short_sensitization_steepness = _parameter->short_sensitization_steepness;

    temp_neuron->_parameter->short_dehabituation_curvature = _parameter->short_dehabituation_curvature;
    temp_neuron->_parameter->short_dehabituation_steepness = _parameter->short_dehabituation_steepness;
    temp_neuron->_parameter->short_desensitization_curvature = _parameter->short_desensitization_curvature;
    temp_neuron->_parameter->short_desensitization_steepness = _parameter->short_desensitization_steepness;

    temp_neuron->_parameter->long_habituation_steepness = _parameter->long_habituation_steepness;
    temp_neuron->_parameter->long_habituation_curvature = _parameter->long_habituation_curvature;
    temp_neuron->_parameter->long_sensitization_steepness = _parameter->long_sensitization_steepness;
    temp_neuron->_parameter->long_sensitization_curvature = _parameter->long_sensitization_curvature;

    temp_neuron->_parameter->long_dehabituation_steepness = _parameter->long_dehabituation_steepness;
    temp_neuron->_parameter->long_dehabituation_curvature = _parameter->long_dehabituation_curvature;
    temp_neuron->_parameter->long_desensitization_steepness = _parameter->long_desensitization_steepness;
    temp_neuron->_parameter->long_desensitization_curvature = _parameter->long_desensitization_curvature;

    temp_neuron->_parameter->presynaptic_potential_curvature = _parameter->presynaptic_potential_curvature;
    temp_neuron->_parameter->presynaptic_potential_steepness = _parameter->presynaptic_potential_steepness;
    temp_neuron->_parameter->presynaptic_backfall_curvature = _parameter->presynaptic_backfall_curvature;
    temp_neuron->_parameter->presynaptic_backfall_steepness = _parameter->presynaptic_backfall_steepness;
    temp_neuron->_parameter->habituation_threshold = _parameter->habituation_threshold;
    temp_neuron->_parameter->sensitization_threshold = _parameter->sensitization_threshold;
    temp_neuron->_parameter->long_learning_weight_reduction_curvature = _parameter->long_learning_weight_reduction_curvature;
    temp_neuron->_parameter->long_learning_weight_reduction_steepness = _parameter->long_learning_weight_reduction_steepness;
    temp_neuron->_parameter->long_learning_weight_backfall_curvature = _parameter->long_learning_weight_backfall_curvature;
    temp_neuron->_parameter->long_learning_weight_backfall_steepness = _parameter->long_learning_weight_backfall_steepness;

    _neurons.push_back(temp_neuron);
    return SUCCESS_CODE;
}

/***********************************************************
 * NeuralNetwork::set_neural_transmitter_influence()
 *
 * Description: Assigns a neurotransmitter to a neuron for it to influence at every activation.
 *
 * Parameters:  int     neuron_id           id of the neuron to assign function to
 *              int     transmitter_id      id of the neurotransmitter to influence
 *              int     influence_direction indicates if neuron should increase or decrease the transmitter
 *
 * Return:      int     error code
 */
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
            printf("[ERROR] Invalid transmitter ID <%d> when assigning transmitter influence to N-%d\n",
                   transmitter_id, neuron_id);
        }
    }
    else{
        printf("[ERROR] Invalid neuron ID when assigning transmitter influence to N-%d.\n", neuron_id);
    }
    return ERROR_CODE;
}

/***********************************************************
 * NeuralNetwork::define_transmitters(int number)
 *
 * Description: Defines how many transmitter types exist
 *
 * Parameters:  int     number      number of transmitter types in network
 */
 int NeuralNetwork::define_transmitters(int number){
    if(number > 0){
        for(int i=1; i<number; i++){
            _transmitter_weights.push_back(1.0f);
            return SUCCESS_CODE;
        }
    }
    else{
        printf("[ERROR] Invalid transmitter number of transmitters <%d> entered.\n", number);
    }
    return ERROR_CODE;
}

 /***********************************************************
  * NeuralNetwork::set_random_neuron_activation()
  *
  * Description: Defines how many transmitter types exist
  *
  * Parameters:  int     neuron_id      neuron ID where random activation shall be enabled
  *              int     chance         chance of activating each tick (0-100000)
  */
 int NeuralNetwork::set_random_neuron_activation(int neuron_id,
                                                 int chance,
                                                 float activation_value){
     if(neuron_id >= MIN_NEURON_ID && (unsigned)neuron_id < _neurons.size()){
        if(chance >= 0 && chance < MAX_CHANCE){
            _neurons[neuron_id]->set_random_activation(chance, activation_value);
            return SUCCESS_CODE;
        }
        else{
            printf("[ERROR] Invalid chance when setting random activation for N-%d.\n", neuron_id);
        }
     }
     else{
         printf("[ERROR] Invalid neuron ID when setting random activation for N-%d.\n", neuron_id);
     }
     return ERROR_CODE;
 }

/***********************************************************
 * NeuralNetwork::add_neuron_connection()
 *
 * Description: Adds a new connection between two neurons
 *
 * Parameters:  int         source_neuron        id of presynaptic neuron
 *              int         target_neuron  id of postsynaptic neuron
 *              float       weight              weight of the connection
 *              int         connection_type     if connection is inhibitory or excitatory
 *              int         learning_type       defines how the neuron learns
 *              int         transmitter_type    defines what neurotransmitter the connection uses
 */
Connection* NeuralNetwork::add_neuron_connection(int source_neuron, int target_neuron, float weight,
                                                 int connection_type,
                                                 int function_type,
                                                 int learning_type,
                                                 int transmitter_type){
    if(connection_type != EXCITATORY && connection_type != INHIBITORY){
        printf("[WARNING] Invalid connection type for C-%d~%d.\n", source_neuron, target_neuron);
        printf("[INFO] Connection type changed to <EXCITATORY>.\n");
        connection_type = EXCITATORY;
    }
    if(transmitter_type == NO_TRANSMITTER){
        transmitter_type = STD_TRANSMITTER;
    }
    else if(transmitter_type < NO_TRANSMITTER || (unsigned)transmitter_type >= _transmitter_weights.size()){
        printf("[WARNING] Invalid transmitter type for C-%d~%d.\n", source_neuron, target_neuron);
        printf("[INFO] Transmitter changed to std transmitter <0>.\n");
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
            printf("[ERROR] Addressed neuron IDs are not existing for C-%d~%d.\n", source_neuron, target_neuron);
        }
    }
    else{
        printf("[ERROR] Neuron IDs start at 1. Please check your given neurons for C-%d~%d.\n", source_neuron, target_neuron);
    }
    printf("[ERROR] Connecting N-%d to N-%d was unsuccessful.\n", source_neuron, target_neuron);
    return NULL;
}

/***********************************************************
 * NeuralNetwork::add_synaptic_connection()
 *
 * Description: Adds a new connection between two neurons given two neurons
 *
 * Parameters:  int         source_neuron            id of presynaptic neuron
 *              int         connected_neuron_1      id of source neuron of target connection
 *              into        connected_neuron_2      id of target neuron of target connection
 *              float       weight                  weight of the connection
 *              int         connection_type         if connection is inhibitory or excitatory
 *              int         learning_type           defines how the neuron learns
 *              int         transmitter_type        defines what neurotransmitter the connection uses
 *
 * Return:      Connection* pointer to new connection
 */
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
        printf("[WARNING] Invalid connection type for C-%d~C-%d.\n", source_neuron, connected_neuron_1);
        printf("[INFO] Connection type changed to <EXCITATORY>.\n");
        connection_type = EXCITATORY;
    }
    if(transmitter_type == NO_TRANSMITTER){
        transmitter_type = STD_TRANSMITTER;
    }
    else if(transmitter_type < NO_TRANSMITTER || (unsigned)transmitter_type >= _transmitter_weights.size()){
        printf("[WARNING] Invalid transmitter type for C-%d~C-%d\n", source_neuron, connected_neuron_1);
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
                printf("[ERROR] Neuron N-%d and N-%d are not connected.\n", connected_neuron_1, connected_neuron_2);
            }
        }
        else{
            printf("[ERROR] Addressed neuron IDs are not existing for C-%d~C-%d.\n", source_neuron, connected_neuron_1);
        }
    }
    else{
        printf("[ERROR] Neuron IDs start at 1. Please check your given neurons for C-%d.\n", source_neuron);
    }
    printf("[ERROR] Connecting N-%d to C-%d was unsuccessful.\n", source_neuron, connected_neuron_1);
    return NULL;
}

/***********************************************************
 * NeuralNetwork::add_synaptic_connection()
 *
 * Description: Adds a new connection between two neurons given a connection
 *
 * Parameters:  int             source_neuron            id of presynaptic neuron
 *              Connection*     con                     connection to connect with
 *              float           weight                  weight of the connection
 *              int             connection_type         if connection is inhibitory or excitatory
 *              int             learning_type           defines how the neuron learns
 *              int             transmitter_type        defines what neurotransmitter the connection uses
 *
 * Return:      Connection*     pointer to new connection
 */
Connection* NeuralNetwork::add_synaptic_connection(int source_neuron,
                                                   Connection *con,
                                                   float weight,
                                                   int connection_type,
                                                   int function_type,
                                                   int learning_type,
                                                   int transmitter_type){
    if(connection_type != EXCITATORY && connection_type != INHIBITORY && connection_type != NONDIRECTIONAL){
        printf("[WARNING] Invalid connection type for C-%d~C-%d.\n", source_neuron, con->prev_neuron->_id);
        printf("[INFO] Connection type changed to <EXCITATORY>.\n");
        connection_type = EXCITATORY;
    }
    if(transmitter_type == NO_TRANSMITTER){
        transmitter_type = STD_TRANSMITTER;
    }
    else if(transmitter_type < NO_TRANSMITTER || (unsigned)transmitter_type >= _transmitter_weights.size()){
        printf("[WARNING] Invalid transmitter type for C-%d~C-%d\n", source_neuron, con->prev_neuron->_id);
        transmitter_type = STD_TRANSMITTER;
    }
    if(source_neuron >= MIN_NEURON_ID){
        if(source_neuron < Neuron::s_max_id){
            Connection *temp_con = _neurons[source_neuron]->add_synaptic_connection(con, weight, connection_type,
                                                                                    function_type, learning_type, transmitter_type);
            return temp_con;
        }
        else{
            printf("[ERROR] Addressed neuron IDs are not existing for C-%d~C-%d.\n", source_neuron, con->prev_neuron->_id);
        }
    }
    else{
        printf("[ERROR] Neuron IDs start at 1. Please check your given neurons for C-%d~C%d.\n", source_neuron, con->prev_neuron->_id);
    }
    printf("[ERROR] Connecting N-%d to C-%d was unsuccessful.\n", source_neuron, con->prev_neuron->_id);
    return NULL;
}

/***********************************************************
 * NeuralNetwork::init_activation(int target_neuron)
 *
 * Description: Initializes the first neuron in the network which should fire, can be called multiple times
 *
 * Parameters: int  target_neuron   neuron which shall be activated
 */
int NeuralNetwork::init_activation(int target_neuron, float activation){
    if(target_neuron >= MIN_NEURON_ID && target_neuron < Neuron::s_max_id){
        _neurons[target_neuron]->_activation = activation;

        _curr_connections.insert(std::end(_curr_connections),
                                 std::begin(_neurons[target_neuron]->_connections),
                                 std::end(_neurons[target_neuron]->_connections));
    }
    else{
        printf("[ERROR] Initializing activation in N-%d was unsuccessful. Invalid ID.\n", target_neuron);
        return ERROR_CODE;
    }
    return SUCCESS_CODE;
}

/***********************************************************
 * NeuralNetwork::setup_network()
 *
 * Description: Looks for tangling neurons without connection and adds a connection to the NULL-neuron.
 *              This is necessary, because the tangling neurons cannot be activated otherwise, but they
 *              could be important for motorneurons or yet unknown functions.
 */
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

/***********************************************************
 * Getter and Setter
 */
int64_t NeuralNetwork::get_step_count(){
    return _network_step_counter;
}

Neuron* NeuralNetwork::get_neuron(int neuron_id){
    if(neuron_id >= MIN_NEURON_ID && (unsigned)neuron_id < _neurons.size()){
        return _neurons[neuron_id];
    }
    else{
        printf("[WARNING] Neuron ID %d for getting neuron is invalid.\n", neuron_id);
    }
    return NULL;
}

int NeuralNetwork::neuron_is_active(int neuron_id){
    if(neuron_id >= MIN_NEURON_ID && (unsigned)neuron_id < _neurons.size()){
        return _neurons[neuron_id]->is_active();
    }
    else{
        printf("[WARNING] Neuron ID %d for getting neuron state is invalid.\n", neuron_id);
    }
    return 0.0f;
}
float NeuralNetwork::get_neuron_activation(int neuron_id){
    if(neuron_id >= MIN_NEURON_ID && (unsigned)neuron_id < _neurons.size()){
        return _neurons[neuron_id]->_activation;
    }
    else{
        printf("[WARNING] Neuron ID %d for getting neuron activation is invalid.\n", neuron_id);
    }
    return 0.0f;
}

float NeuralNetwork::get_transmitter_weight(int transmitter_id){
    if(transmitter_id >= 0 && (unsigned)transmitter_id < _transmitter_weights.size()){
        return _transmitter_weights[transmitter_id];
    }
    else{
        printf("[WARNING] Transmitter ID %d for getting transmitter weight is invalid.\n", transmitter_id);
    }
    return 0.0f;
}

/**********************************************************************************************************************/
/* ACTIVATION LOOP BEGIN */

/***********************************************************
 * NeuralNetwork::change_transmitter_weight()
 *
 * Description: Changes the weight of a single transmitter type
 *
 * Parameters:  int     transmitter_id      id of the current transmitter type
 *              float   new_weight          new weight the transmitter is receiving
 */
void NeuralNetwork::change_transmitter_weight(int transmitter_id, float new_weight){
    if(DEBUG_MODE == true && DEB_TRANSMITTER == true)
        printf("<%ld> T-%d -> Weight before change = %.3f\n",
               _network_step_counter, transmitter_id, _transmitter_weights[transmitter_id]);

    _transmitter_weights[transmitter_id] = new_weight;

    if(DEBUG_MODE == true && DEB_TRANSMITTER == true)
        printf("<%ld> T-%d -> Weight after change = %.3f\n\n",
               _network_step_counter, transmitter_id, _transmitter_weights[transmitter_id]);
}

/***********************************************************
 * NeuralNetwork::calculate_neuron_backfall(unsigned int con)
 *
 * Description: Calculates the backfall in activation of neurons that were activated but did not fire.
 *
 * Parameters:  NeuronNeuron*       n      the neuron which should be checked for backfall
 */
void NeuralNetwork::calculate_neuron_backfall(Neuron *n){
    if(DEBUG_MODE && DEB_NEURON_BACKFALL)
        printf("<%ld> N-%d -> Activation before backfall = %.3f\n",
               _network_step_counter, n->_id, n->_activation);

    if(n->_was_activated == false){
        n->_activation =  MathUtils::calculate_static_gradient(n->_activation,
                                                   n->_parameter->activation_backfall_steepness,
                                                   this->_network_step_counter-n->get_step(),
                                                   n->_parameter->activation_backfall_curvature,
                                                   SUBTRACT,
                                                   n->_parameter->max_activation,
                                                   n->_parameter->min_activation);
    }

    if(DEBUG_MODE && DEB_NEURON_BACKFALL)
        printf("<%ld> N-%d -> Activation after backfall = %.3f\n\n",
               _network_step_counter, n->_id, n->_activation);
}

/***********************************************************
 * NeuralNetwork::clear_neuron_activation(Neuron *n)
 *
 * Description: empties neural activation in one neuron, if activation reached threshold
 *
 * Params:      Neuron *    n       the neuron which should be cleared
 */
void NeuralNetwork::clear_neuron_activation(Neuron *n){
    if(DEBUG_MODE && DEB_NEURON_BACKFALL)
        printf("<%ld> N-%d -> Activation before clearing = %.3f\n",
               _network_step_counter, n->_id, n->_activation);

    if(n->_activation >= n->_threshold){
        n->_activation = n->_parameter->min_activation;
    }

    if(DEBUG_MODE && DEB_NEURON_BACKFALL)
        printf("<%ld> N-%d -> Activation after clearing = %.3f\n\n",
               _network_step_counter, n->_id, n->_activation);
}

/***********************************************************
 * NeuralNetwork::long_learning_weight_backfall(Connection *con)
 *
 * Description: Calculates the backfall of the "long learning weight" variable
 *              used to control the learning rate of long term weights
 *
 * Params:      Connection*     con     target neuron of learner weight backfall
 */
void NeuralNetwork::long_learning_weight_backfall(Connection *con){
    if(DEBUG_MODE && DEB_LONG_LEARNING_WEIGHT)
        printf("<%ld> C-%d -> Long learning weight before backfall = %.3f\n",
               _network_step_counter, con->prev_neuron->_id, con->long_learning_weight);

    con->long_learning_weight =  MathUtils::calculate_static_gradient(con->long_learning_weight,
                                                          con->prev_neuron->_parameter->long_learning_weight_backfall_steepness,
                                                          this->_network_step_counter - con->last_activated_step,
                                                          con->prev_neuron->_parameter->long_learning_weight_backfall_curvature,
                                                          ADD,
                                                          MAX_LONG_LEARNING_WEIGHT,
                                                          MIN_LONG_LEARNING_WEIGHT);

    if(DEBUG_MODE && DEB_LONG_LEARNING_WEIGHT)
        printf("<%ld> C-%d -> Long learning weight after backfall = %.3f\n\n",
               _network_step_counter, con->prev_neuron->_id, con->long_learning_weight);
}

/***********************************************************
 * NeuralNetwork::long_learning_weight_reduction(Connection *con)
 *
 * Description: Calculates the reduction of the "long learning weight" variable
 *              used to control the learning rate of long term weights
 *
 * Params:      Connection*     con     target neuron of learner weight reduction
 */
void NeuralNetwork::long_learning_weight_reduction(Connection *con){
    if(DEBUG_MODE && DEB_LONG_LEARNING_WEIGHT)
        printf("<%ld> C-%d -> Long learning weight before reduction = %.3f\n",
               _network_step_counter, con->prev_neuron->_id, con->long_learning_weight);

    con->long_learning_weight =  MathUtils::calculate_static_gradient(con->long_learning_weight,
                                                          con->prev_neuron->_parameter->long_learning_weight_reduction_steepness,
                                                          1,
                                                          con->prev_neuron->_parameter->long_learning_weight_reduction_curvature,
                                                          SUBTRACT,
                                                          MAX_LONG_LEARNING_WEIGHT,
                                                          MIN_LONG_LEARNING_WEIGHT);
    if(DEBUG_MODE && DEB_LONG_LEARNING_WEIGHT)
        printf("<%ld> C-%d -> Long learning weight after reduction = %.3f\n\n",
               _network_step_counter, con->prev_neuron->_id, con->long_learning_weight);
}

/***********************************************************
 * NeuralNetwork::dehabituate(Connection *con)
 *
 * Description: Calculates the backfall of learning functions
 *
 * Params:     Connection *     con     the connection which should be transformed
 */
void NeuralNetwork::dehabituate(Connection *con){
    if(DEBUG_MODE && DEB_HABITUATION){
        printf("<%ld> N-%d -> Short before dehabituation = %.5f\n",
               _network_step_counter, con->prev_neuron->_id, con->short_weight);
        printf("<%ld> N-%d -> Long before dehabituation = %.5f\n",
               _network_step_counter, con->prev_neuron->_id, con->long_weight);
    }

    if(con->long_weight < con->base_weight){
       con->long_weight =  MathUtils::calculate_static_gradient(con->long_weight,
                                                    con->prev_neuron->_parameter->long_dehabituation_steepness,
                                                    this->_network_step_counter - con->last_activated_step,
                                                    con->prev_neuron->_parameter->long_dehabituation_curvature,
                                                    ADD,
                                                    con->base_weight,
                                                    con->prev_neuron->_parameter->min_weight);
    }

    if(con->short_weight < con->long_weight){
       con->short_weight =  MathUtils::calculate_static_gradient(con->short_weight,
                                                     con->prev_neuron->_parameter->short_dehabituation_steepness,
                                                     this->_network_step_counter - con->last_activated_step,
                                                     con->prev_neuron->_parameter->short_dehabituation_curvature,
                                                     ADD,
                                                     con->long_weight,
                                                     con->prev_neuron->_parameter->min_weight);
    }

    if(DEBUG_MODE && DEB_HABITUATION){
        printf("<%ld> N-%d -> Short after dehabituation = %.5f\n",
               _network_step_counter, con->prev_neuron->_id, con->short_weight);
        printf("<%ld> N-%d -> Long after dehabituation = %.5f\n\n",
               _network_step_counter, con->prev_neuron->_id, con->long_weight);
    }
}


/***********************************************************
 * NeuralNetwork::desensitize(Connection *con)
 *
 * Description: Calculates the backfall of learning functions
 *
 * Params:     Connection *     con     the connection which should be transformed
 */
void NeuralNetwork::desensitize(Connection *con){
    if(DEBUG_MODE && DEB_SENSITIZATION){
        printf("<%ld> N-%d -> Short before desensitization = %.5f\n",
               _network_step_counter, con->prev_neuron->_id, con->short_weight);
        printf("<%ld> N-%d -> Long before desensitization = %.5f\n",
               _network_step_counter, con->prev_neuron->_id, con->long_weight);
    }

    if(con->long_weight > con->base_weight){
        con->long_weight =  MathUtils::calculate_static_gradient(con->long_weight,
                                                     con->prev_neuron->_parameter->long_desensitization_steepness,
                                                     this->_network_step_counter - con->last_activated_step,
                                                     con->prev_neuron->_parameter->long_desensitization_curvature,
                                                     SUBTRACT,
                                                     con->prev_neuron->_parameter->max_weight,
                                                     con->base_weight);
    }

    if(con->short_weight > con->long_weight){
        con->short_weight =  MathUtils::calculate_static_gradient(con->short_weight,
                                                      con->prev_neuron->_parameter->short_desensitization_steepness,
                                                      this->_network_step_counter - con->last_activated_step,
                                                      con->prev_neuron->_parameter->short_desensitization_curvature,
                                                      SUBTRACT,
                                                      con->prev_neuron->_parameter->max_weight,
                                                      con->long_weight);
    }

    if(DEBUG_MODE && DEB_SENSITIZATION){
        printf("<%ld> N-%d -> Short after desensitization = %.5f\n",
               _network_step_counter, con->prev_neuron->_id, con->short_weight);
        printf("<%ld> N-%d -> Long after desensitization = %.5f\n\n",
               _network_step_counter, con->prev_neuron->_id, con->long_weight);
    }
}

/***********************************************************
 * NeuralNetwork::habituate(Connection *con)
 *
 * Description: Calculates the basic learning process habituation for a connection
 *
 * Params:     Connection *     con                 the connection which should be transformed
 *             Connection *     coniditioning_con   the connection that points to con (if it exists)
 */
void NeuralNetwork::habituate(Connection *con, Connection *conditioning_con){
    // TODO first part not good
    float activation = con->prev_neuron->_activation;
    float conditioning_type = NONDIRECTIONAL;

    if(conditioning_con){
        activation = conditioning_con->prev_neuron->_activation;
        conditioning_type = conditioning_con->activation_type;
    }

    if((conditioning_type == NONDIRECTIONAL && activation < con->prev_neuron->_parameter->habituation_threshold) ||
       (conditioning_type == INHIBITORY)){
        if(DEBUG_MODE && DEB_HABITUATION){
            printf("<%ld> N-%d -> Short before habituation = %.5f\n",
                   _network_step_counter, con->prev_neuron->_id, con->short_weight);
            printf("<%ld> N-%d -> Long before habituation = %.5f\n",
                   _network_step_counter, con->prev_neuron->_id, con->long_weight);
        }
        if(DEBUG_MODE && DEB_LONG_LEARNING_WEIGHT)
            printf("<%ld> N-%d -> Learner before habituation = %.5f\n",
                   _network_step_counter, con->prev_neuron->_id, con->long_learning_weight);

        if(conditioning_type == NONDIRECTIONAL)
            activation = con->prev_neuron->_parameter->habituation_threshold - activation;

        con->short_weight =  MathUtils::calculate_dynamic_gradient(con->short_weight,
                                               con->prev_neuron->_parameter->short_habituation_steepness,
                                               activation,
                                               con->prev_neuron->_parameter->short_habituation_curvature,
                                               SUBTRACT,
                                               con->prev_neuron->_parameter->max_weight,
                                               con->prev_neuron->_parameter->min_weight);

        con->long_weight =  MathUtils::calculate_dynamic_gradient(con->long_weight,
                                              con->prev_neuron->_parameter->long_habituation_steepness,
                                              activation * con->long_learning_weight,
                                              con->prev_neuron->_parameter->long_habituation_curvature,
                                              SUBTRACT,
                                              con->prev_neuron->_parameter->max_weight,
                                              con->prev_neuron->_parameter->min_weight);

        long_learning_weight_reduction(con);

        if(DEBUG_MODE && DEB_HABITUATION){
            printf("<%ld> N-%d -> Short after habituation = %.5f\n",
                   _network_step_counter, con->prev_neuron->_id, con->short_weight);
            printf("<%ld> N-%d -> Long after habituation = %.5f\n",
                   _network_step_counter, con->prev_neuron->_id, con->long_weight);
        }
        if(DEBUG_MODE && DEB_LONG_LEARNING_WEIGHT)
            printf("<%ld> N-%d -> Learner after habituation = %.5f\n\n",
                   _network_step_counter, con->prev_neuron->_id, con->long_learning_weight);
    }
}

/***********************************************************
 * NeuralNetwork::sensitize()
 *
 * Description: Calculates the basic learning process sensitization for a connection
 *
 * Params:     Connection *     con                 the connection which should be transformed
 *             Connection *     coniditioning_con   the connection that points to con (if it exists)
 */
void NeuralNetwork::sensitize(Connection *con, Connection *conditioning_con){
    // TODO first part not good
    float activation = con->prev_neuron->_activation;
    float conditioning_type = NONDIRECTIONAL;

    if(conditioning_con){
        activation = conditioning_con->prev_neuron->_activation;
        conditioning_type = conditioning_con->activation_type;
    }

    if((conditioning_type == NONDIRECTIONAL && activation > con->prev_neuron->_parameter->sensitization_threshold) ||
       (conditioning_type == EXCITATORY)){
        if(DEBUG_MODE && DEB_SENSITIZATION){
            printf("<%ld> N-%d -> Short before sensitization = %.5f\n",
                   _network_step_counter,  con->prev_neuron->_id, con->short_weight);
            printf("<%ld> N-%d -> Long before sensitization = %.5f\n",
                   _network_step_counter, con->prev_neuron->_id, con->long_weight);
        }
        if(DEBUG_MODE && DEB_LONG_LEARNING_WEIGHT)
            printf("<%ld> N-%d -> Learner before sensitization = %.5f\n",
                   _network_step_counter, con->prev_neuron->_id, con->long_learning_weight);

        if(conditioning_type == NONDIRECTIONAL)
            activation = activation - con->prev_neuron->_parameter->sensitization_threshold;

        con->short_weight =  MathUtils::calculate_dynamic_gradient(con->short_weight,
                                               con->prev_neuron->_parameter->short_sensitization_steepness,
                                               activation,
                                               con->prev_neuron->_parameter->short_sensitization_curvature,
                                               ADD,
                                               con->prev_neuron->_parameter->max_weight,
                                               con->prev_neuron->_parameter->min_weight);

        con->long_weight =  MathUtils::calculate_dynamic_gradient(con->long_weight,
                                              con->prev_neuron->_parameter->long_sensitization_steepness,
                                              activation * con->long_learning_weight,
                                              con->prev_neuron->_parameter->long_sensitization_curvature,
                                              ADD,
                                              con->prev_neuron->_parameter->max_weight,
                                              con->prev_neuron->_parameter->min_weight);

        long_learning_weight_reduction(con);

        if(DEBUG_MODE && DEB_SENSITIZATION){
            printf("<%ld> N-%d -> Short after sensitization = %.5f\n",
                   _network_step_counter, con->prev_neuron->_id, con->short_weight);
            printf("<%ld> N-%d -> Long after sensitization = %.5f\n",
                   _network_step_counter, con->prev_neuron->_id, con->long_weight);
        }
        if(DEBUG_MODE && DEB_LONG_LEARNING_WEIGHT)
            printf("<%ld> N-%d -> Learner after sensitization = %.5f\n\n",
                   _network_step_counter, con->prev_neuron->_id, con->long_learning_weight);
    }
}

/***********************************************************
 * NeuralNetwork::basic_learning()
 *
 * Description: Calculates the basic learning process sensitization for a connection
 *
 * Params:     Connection *     con                 the connection which should be transformed
 *             Connection *     coniditioning_con   the connection that points to con (if it exists)
 */
void NeuralNetwork::basic_learning(Connection *con, Connection *conditioning_con){
    long_learning_weight_backfall(con);

    if(con->learning_type == LEARNING_HABITUATION ||
       con->learning_type == LEARNING_HABISENS){
        dehabituate(con);
        habituate(con, conditioning_con);
    }

    if(con->learning_type == LEARNING_SENSITIZATION ||
       con->learning_type == LEARNING_HABISENS){
         desensitize(con);
         sensitize(con, conditioning_con);
    }

    con->last_activated_step = _network_step_counter;
}

/***********************************************************
 * NeuralNetwork::activate_next_entities()
 *
 * Description: Allows predefined neurons to change the weight of a single Neurotransmitter
 *              during runtime.
 *
 * Params:      Neuron*   n   The current neuron
 */
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

/***********************************************************
 * NeuralNetwork::transmitter_backfall()
 *
 * Description: Calculates the backfall of neurotransmitters if changed.
 */
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

/***********************************************************
 * NeuralNetwork::presynaptic_potential_backfall()
 *
 * Description: Calculates the backfall of presynaptic activation towards 1.0.
 *
 * Param:       Connection*     con     Connection to calculate backfall for.
 */
void NeuralNetwork::presynaptic_potential_backfall(Connection *con){
    if(DEBUG_MODE && DEB_PRESYNAPTIC)
        printf("<%ld> C-%d -> Presynaptic potential before backfall = %.3f\n",
               _network_step_counter, con->prev_neuron->_id, con->presynaptic_potential);

    con->presynaptic_potential =  MathUtils::calculate_static_gradient(con->presynaptic_potential,
                                                     con->prev_neuron->_parameter->presynaptic_backfall_steepness,
                                                     _network_step_counter - con->last_presynaptic_activated_step,
                                                     con->prev_neuron->_parameter->presynaptic_backfall_curvature,
                                                     SUBTRACT,
                                                     con->prev_neuron->_parameter->max_weight,
                                                     DEFAULT_PRESYNAPTIC_POTENTIAL);

    if(DEBUG_MODE && DEB_PRESYNAPTIC)
        printf("<%ld> C-%d -> Presynaptic potential after backfall = %.3f\n\n",
               _network_step_counter, con->prev_neuron->_id, con->presynaptic_potential);
}

/***********************************************************
 * NeuralNetwork::activate_next_entities()
 *
 * Description: A wrapper containing the basic logic for activation of next neurons and
 *              connections
 */
void NeuralNetwork::activate_next_entities(){
    for(unsigned int con=0; con<_curr_connections.size(); con++){
        if(_curr_connections[con]->prev_neuron->_activation >= _curr_connections[con]->prev_neuron->_threshold){
            basic_learning(_curr_connections[con]);
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

/***********************************************************
 * NeuralNetwork::activate_next_neuron()
 *
 * Description: Calculates the activation function for each saved current connection &
 *              passes it to the next neurons.
 */
void NeuralNetwork::activate_next_neuron(Connection *con){
    calculate_neuron_backfall(con->next_neuron);

    con->prev_neuron->_temp_activation = con->short_weight *
                                         con->prev_neuron->_activation;

    con->next_neuron->_activation +=
          choose_activation_function(con, con->prev_neuron->_temp_activation) *
          con->activation_type *
          _transmitter_weights[con->transmitter_type];

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

/***********************************************************
 * NeuralNetwork::activate_next_connection()
 *
 * Description: Calculates the learning effects due to presynaptic connections.
 */
void NeuralNetwork::activate_next_connection(Connection *con){
    if(DEBUG_MODE && DEB_PRESYNAPTIC){
        printf("<%ld> C-%d -> Presynaptic potential before influence : %.3f\n",
               _network_step_counter,
               con->prev_neuron->_id,
               con->next_connection->presynaptic_potential);
    }

    presynaptic_potential_backfall(con->next_connection);
    if(con->next_connection->presynaptic_potential > DEFAULT_PRESYNAPTIC_POTENTIAL){
        basic_learning(con->next_connection, con);
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

/***********************************************************
 * NeuralNetwork::save_next_neurons()
 *
 * Description: Stores all next neurons in a list.
 *              At the beginning, it also resets every neuron.
 */
void NeuralNetwork::save_next_neurons(){
    // TODO Too chaotic
    if(DEBUG_MODE && _curr_connections.size() > 0)
        printf("\n*******************NEXT STEP*******************\n\n");

    for(unsigned int con=0; con<_curr_connections.size(); con++){
        /* Only do if neuron fired in this round */
        clear_neuron_activation(_curr_connections[con]->prev_neuron);

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

/***********************************************************
 * NeuralNetwork::activate_random_neurons()
 *
 * Description: Walks through random_neuron vector and checks if random
 *              activations happen
 */
void NeuralNetwork::activate_random_neurons(){
    for(unsigned int n = 0; n < _random_neurons.size(); n++){
        if(random()%MAX_CHANCE <= _random_neurons[n]->_random_chance){
            init_activation(_random_neurons[n]->_id, _random_neurons[n]->_random_activation_value);
            srandom(time(0));
        }
    }
}

/***********************************************************
 * NeuralNetwork::switch_vectors()
 *
 * Description: Loads next connections into current connections for the next run
 *              Must be called after feed_forward()
 */
void NeuralNetwork::switch_vectors(){
    _curr_connections = _next_connections;
    _next_connections.clear();
}

/***********************************************************
 * NeuralNetwork::feed_forward()
 *
 * Description: Feeds one step through the network
 */
void NeuralNetwork::feed_forward(){
    _network_step_counter += 1;

    transmitter_backfall();
    activate_random_neurons();
    activate_next_entities();
    save_next_neurons();
    switch_vectors();
}

/* ACTIVATION LOOP END */
/**********************************************************************************************************************/

/***********************************************************
 * NeuralNetwork::print_activation()
 *
 * Description: Prints all currently firing neurons
 */
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
