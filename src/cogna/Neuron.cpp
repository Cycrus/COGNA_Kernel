#include "Neuron.hpp"

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>

int Neuron::s_max_id = 0;

/************************************************************
 * Getter & Setter
 *
 * Description: Functions for getting and setting various values in the neuron.
 */
long Neuron::get_step(){
    return _last_activated_step;
}
void Neuron::set_step(long new_step){
    _last_activated_step = new_step;
}

/************************************************************
 * Neuron::Neuron()
 *
 * description: initializes general neuron object.
 */
Neuron::Neuron(float activation_threshold){
    _id = s_max_id;
    s_max_id++;
    this->_threshold = activation_threshold;
    this->_random_activation = false;
    _random_chance = 0;
    _random_activation_value = 0.0f;
    _activation = 0.0f;
    _was_activated = true;
    _last_activated_step = 0;
    _last_fired_step = 0;

    _parameter = new NeuronParameterHandler();

    _parameter->activation_backfall_curvature = 0.0f;
    _parameter->activation_backfall_steepness = 0.0f;

    _parameter->short_habituation_curvature = 0.0f;
    _parameter->short_habituation_steepness = 0.0f;
    _parameter->short_sensitization_curvature = 0.0f;
    _parameter->short_sensitization_steepness = 0.0f;
    _parameter->short_dehabituation_curvature = 0.0f;
    _parameter->short_dehabituation_steepness = 0.0f;
    _parameter->short_desensitization_curvature = 0.0f;
    _parameter->short_desensitization_steepness = 0.0f;

    _parameter->long_habituation_curvature = 0.0f;
    _parameter->long_habituation_steepness = 0.0f;
    _parameter->long_sensitization_curvature = 0.0f;
    _parameter->long_sensitization_steepness = 0.0f;
    _parameter->long_dehabituation_curvature = 0.0f;
    _parameter->long_dehabituation_steepness = 0.0f;
    _parameter->long_desensitization_curvature = 0.0f;
    _parameter->long_desensitization_steepness = 0.0f;

    _parameter->presynaptic_potential_curvature = 0.0f;
    _parameter->presynaptic_potential_steepness = 0.0f;
    _parameter->presynaptic_backfall_curvature = 0.0f;
    _parameter->presynaptic_backfall_steepness = 0.0f;

    _parameter->habituation_threshold = 0.0f;
    _parameter->sensitization_threshold = 0.0f;

    _parameter->influenced_transmitter = NO_TRANSMITTER;
    _parameter->transmitter_influence_direction = POSITIVE_INFLUENCE;
}

/************************************************************
 * Neuron::~Neuron()
 *
 * Description: Deletes all reserved memory for this single
 *              neuron.
 */
Neuron::~Neuron(){
    _previous.clear();
    for(unsigned int i=0; i<_connections.size(); i++){
        delete _connections[i];
        _connections[i] = NULL;
    }
    _connections.clear();
}

/************************************************************
 * Neuron::check_next_connection(Neuron *n)
 *
 * Description: Checks if this neuron is connected with
 *              another neuron.
 *
 * Parameters: Neuron*  n       The neuron to be checked
 *
 * Return:     int      0       if false, 1 if true
 */
int Neuron::check_neuron_connection(Neuron *n){
    for(unsigned int i=0; i<_connections.size(); i++){
        if(n->_id == _connections[i]->next_neuron->_id){
            return true;
        }
    }
    return false;
}

/************************************************************
 * Neuron::check_synaptic_connection(Neuron *n)
 *
 * Description: Checks if this neuron is connected with
 *              a certain connection.
 *
 * Parameters: Connection*  con       The connection to be checked
 *
 * Return:     int          0         if false, 1 if true
 */
int Neuron::check_synaptic_connection(Connection *con){
    for(unsigned int i=0; i<_connections.size(); i++){
        if(_connections[i]->next_connection){
            if(con->prev_neuron == _connections[i]->next_connection->prev_neuron){
                if(con->next_neuron == _connections[i]->next_connection->next_neuron){
                       return true;
                }
                else if(con->next_connection == _connections[i]->next_connection->next_connection){
                       return true;
                }
            }
        }
    }
    return false;
}

/************************************************************
 * Neuron::add_neuron_connection()
 *
 * Description: Adds a new feed forward connection to another
 *              neuron in the network.
 *
 * Parameters:  Neuron* n   The neuron to which the connection shall happen
 *              float   w   The weight of the connection
 *              int     a   The type of connection (EXCITATORY/INHIBITORY)
 *              int     f   Activation function used to activate next neuron
 *
 * Return:      Connection* pointer to the new connection
 */
Connection* Neuron::add_neuron_connection(Neuron *n,
                                          float weight,
                                          int con_type,
                                          int fun_type,
                                          int learn_type,
                                          int transmitter_type){
    if(check_neuron_connection(n) == true){
        printf("[WARNING] N-%d is already connected with N-%d.\n",
               this->_id, n->_id);
    }
    else{
        Connection *temp_con = new Connection();
        temp_con->next_neuron = n;
        temp_con->next_connection = NULL;
        temp_con->prev_neuron = this;
        temp_con->base_weight = weight;
        temp_con->short_weight = weight;
        temp_con->long_weight = weight;
        temp_con->long_learning_weight = 1.0f;
        temp_con->presynaptic_potential = DEFAULT_PRESYNAPTIC_POTENTIAL;
        temp_con->activation_type = con_type;
        temp_con->activation_function = fun_type;
        temp_con->learning_type = learn_type;
        temp_con->transmitter_type = transmitter_type;
        temp_con->last_presynaptic_activated_step = 0;
        temp_con->last_activated_step = 0;
        _connections.push_back(temp_con);
        n->_previous.push_back(this);
        return temp_con;
    }
    return NULL;
}

/************************************************************
 * Neuron::add_synaptic_connection()
 *
 * Description: Adds a new feed forward connection to another
 *              connection in the network.
 *
 * Parameters:  Connection* con The connection to which the connection shall happen
 *              float       w   The weight of the connection
 *              int         a   The type of connection (EXCITATORY/INHIBITORY)
 *              int         f   Activation function used to activate next neuron
 *
 * Return:      Connection* pointer to the new connection
 */
Connection* Neuron::add_synaptic_connection(Connection *con,
                                            float weight,
                                            int con_type,
                                            int fun_type,
                                            int learn_type,
                                            int transmitter_type){
    if(check_synaptic_connection(con) == true){
        if(con->next_neuron){
            printf("[WARNING] N-%d is already connected with C-%d~%d.\n",
                   this->_id, con->prev_neuron->_id, con->next_neuron->_id);
        }
        else if(con->next_connection){
            printf("[WARNING] N-%d is already connected with the connection between N-%d and another connection coming from N-%d\n",
                   this->_id, con->prev_neuron->_id, con->next_connection->prev_neuron->_id);
        }
    }
    else{
        Connection *temp_con = new Connection();
        temp_con->next_connection = con;
        temp_con->next_neuron = NULL;
        temp_con->prev_neuron = this;
        temp_con->base_weight = weight;
        temp_con->short_weight = weight;
        temp_con->long_weight = weight;
        temp_con->long_learning_weight = 1.0f;
        temp_con->presynaptic_potential = 1.0f;
        temp_con->activation_type = con_type;
        temp_con->activation_function = fun_type;
        temp_con->learning_type = learn_type;
        temp_con->transmitter_type = transmitter_type;
        temp_con->last_presynaptic_activated_step = 0;
        temp_con->last_activated_step = 0;
        _connections.push_back(temp_con);
        return temp_con;
    }
    return NULL;
}

/************************************************************
 * Neuron::get_weight(Neuron *n)
 *
 * Description: Returns the weight to the given neuron.
 *
 * Params:  Neuron* n   defines what connection is meant
 *          int type    defines what type of connection is meant
 */
float Neuron::get_weight(Neuron *n, int type){
    for(unsigned int i=0; i<_connections.size(); i++){
        if(_connections[i]->next_neuron->_id == n->_id){
            if(type == 0)
                return _connections[i]->base_weight;
            else if(type == 1)
                return _connections[i]->long_weight;
            else if(type == 2)
                return _connections[i]->short_weight;
        }
    }
    printf("[ERROR] There is no connection between N-%d and N-%d.",
           this->_id, n->_id);
    return 0;
}

/************************************************************
 * Neuron::del_connection(Neuron *n)
 *
 * Description: Deletes a feed forward connection to another
 *              neuron in the network.
 *
 */
void Neuron::del_connection(Neuron *n){
    if(check_neuron_connection(n) == false){
        printf("[WARNING] N-%d is not connected with N-%d.\n", this->_id, n->_id);
    }
    else{
        for(unsigned int i=0; i<n->_previous.size(); i++){
            if(n->_previous[i]->_id == this->_id){
                n->_previous.erase(n->_previous.begin()+i);
                break;
            }
        }
        for(unsigned int i=0; i<_connections.size(); i++){
            if(_connections[i]->next_neuron->_id == n->_id){
                delete _connections[i];
                _connections[i] = NULL;
                _connections.erase(_connections.begin()+i);
                break;
            }
        }
    }
}

/************************************************************
 * Neuron::is_active()
 *
 * Description: Returns true if activation is greater than
 *              neural threshold.
 *
 * Return:      int   Boolean value if neuron is active
 */
int Neuron::is_active(){
    if(_activation >= _threshold){
        return true;
    }
    else{
        return false;
    }
}

/************************************************************
 * Neuron::set_random_activation()
 *
 * Description: Sets random activation potential to neuron
 *
 * Param:       int     chance              Sets chance for random activation per tick (0-10000)
 *              float   activation_value    Sets the random activation strenght
 */
void Neuron::set_random_activation(int chance, float activation_value){
    _random_activation = true;
    _random_chance = chance;
    _random_activation_value = activation_value;
}

float sigmoid(float input){
    return (float)(1.0 / (1.0 + (float)exp(-input)));
}

float linear(float input){
    return (float)input;
}

float relu(float input){
    if(input > 0) return (float)input;
    else return 0;
}

float choose_activation_function(Connection *con, float input){
    switch(con->activation_function){
      case FUNCTION_SIGMOID:
          return sigmoid(input);
          break;

      case FUNCTION_LINEAR:
          return linear(input);
          break;

      case FUNCTION_RELU:
          return relu(input);
          break;

      default:
          printf("[WARNING] Invalid activation function for connection of N-%d\n",
                 con->prev_neuron->_id);
          return 0.0f;
          break;
    }
}
