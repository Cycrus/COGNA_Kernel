#include "Neuron.hpp"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "MathUtils.hpp"

using namespace COGNA;

namespace COGNA{
    int COGNA::Neuron::s_max_id = 0;

    /************************************************************
     * Getter & Setter
     *
     * Description: Functions for getting and setting various values in the neuron.
     */
    long COGNA::Neuron::get_step(){
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
    Neuron::Neuron(NeuralNetworkParameterHandler *default_parameter){
        _parameter = new NeuronParameterHandler();

        _parameter->activation_type = default_parameter->activation_type;
        _parameter->activation_function = default_parameter->activation_function;
        _parameter->max_activation = default_parameter->max_activation;
        _parameter->min_activation = default_parameter->min_activation;
        _parameter->activation_backfall_curvature = default_parameter->activation_backfall_curvature;
        _parameter->activation_backfall_steepness = default_parameter->activation_backfall_steepness;
        _parameter->influenced_transmitter = default_parameter->influenced_transmitter;
        _parameter->transmitter_influence_direction = default_parameter->transmitter_influence_direction;

        _parameter->max_weight = default_parameter->max_weight;
        _parameter->min_weight = default_parameter->min_weight;

        _parameter->learning_type = default_parameter->learning_type;
        _parameter->transmitter_type = default_parameter->transmitter_type;

        _parameter->activation_backfall_curvature = default_parameter->activation_backfall_curvature;
        _parameter->activation_backfall_steepness = default_parameter->activation_backfall_steepness;

        _parameter->short_habituation_curvature = default_parameter->short_habituation_curvature;
        _parameter->short_habituation_steepness = default_parameter->short_habituation_steepness;
        _parameter->short_sensitization_curvature = default_parameter->short_sensitization_curvature;
        _parameter->short_sensitization_steepness = default_parameter->short_sensitization_steepness;

        _parameter->short_dehabituation_curvature = default_parameter->short_dehabituation_curvature;
        _parameter->short_dehabituation_steepness = default_parameter->short_dehabituation_steepness;
        _parameter->short_desensitization_curvature = default_parameter->short_desensitization_curvature;
        _parameter->short_desensitization_steepness = default_parameter->short_desensitization_steepness;

        _parameter->long_habituation_steepness = default_parameter->long_habituation_steepness;
        _parameter->long_habituation_curvature = default_parameter->long_habituation_curvature;
        _parameter->long_sensitization_steepness = default_parameter->long_sensitization_steepness;
        _parameter->long_sensitization_curvature = default_parameter->long_sensitization_curvature;

        _parameter->long_dehabituation_steepness = default_parameter->long_dehabituation_steepness;
        _parameter->long_dehabituation_curvature = default_parameter->long_dehabituation_curvature;
        _parameter->long_desensitization_steepness = default_parameter->long_desensitization_steepness;
        _parameter->long_desensitization_curvature = default_parameter->long_desensitization_curvature;

        _parameter->presynaptic_potential_curvature = default_parameter->presynaptic_potential_curvature;
        _parameter->presynaptic_potential_steepness = default_parameter->presynaptic_potential_steepness;
        _parameter->presynaptic_backfall_curvature = default_parameter->presynaptic_backfall_curvature;
        _parameter->presynaptic_backfall_steepness = default_parameter->presynaptic_backfall_steepness;

        _parameter->habituation_threshold = default_parameter->habituation_threshold;
        _parameter->sensitization_threshold = default_parameter->sensitization_threshold;

        _parameter->long_learning_weight_reduction_curvature = default_parameter->long_learning_weight_reduction_curvature;
        _parameter->long_learning_weight_reduction_steepness = default_parameter->long_learning_weight_reduction_steepness;
        _parameter->long_learning_weight_backfall_curvature = default_parameter->long_learning_weight_backfall_curvature;
        _parameter->long_learning_weight_backfall_steepness = default_parameter->long_learning_weight_backfall_steepness;

        _id = s_max_id;
        s_max_id++;
        //this->_activation_threshold = activation_threshold;
        this->_random_activation = false;
        _random_chance = 0;
        _random_activation_value = 0.0f;
        _activation = 0.0f;
        _was_activated = true;
        _last_activated_step = 0;
        _last_fired_step = 0;
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

        delete _parameter;
        _parameter = NULL;
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
            Connection *temp_con = new Connection(_parameter);
            temp_con->next_neuron = n;
            temp_con->next_connection = NULL;
            temp_con->prev_neuron = this;
            temp_con->base_weight = weight;
            temp_con->short_weight = weight;
            temp_con->long_weight = weight;
            temp_con->long_learning_weight = 1.0f;
            temp_con->presynaptic_potential = 1.0f;
            temp_con->_parameter->activation_type = con_type;
            temp_con->_parameter->activation_function = fun_type;
            temp_con->_parameter->learning_type = learn_type;
            temp_con->_parameter->transmitter_type = transmitter_type;
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
            Connection *temp_con = new Connection(_parameter);
            temp_con->next_connection = con;
            temp_con->next_neuron = NULL;
            temp_con->prev_neuron = this;
            temp_con->base_weight = weight;
            temp_con->short_weight = weight;
            temp_con->long_weight = weight;
            temp_con->_parameter->activation_type = con_type;
            temp_con->_parameter->activation_function = fun_type;
            temp_con->_parameter->learning_type = learn_type;
            temp_con->_parameter->transmitter_type = transmitter_type;
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
        if(_activation >= _parameter->activation_threshold){
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

    float choose_activation_function(Connection *con, float input){
        switch(con->_parameter->activation_function){
          case FUNCTION_SIGMOID:
              return MathUtils::sigmoid(input);
              break;

          case FUNCTION_LINEAR:
              return MathUtils::linear(input);
              break;

          case FUNCTION_RELU:
              return MathUtils::relu(input);
              break;

          default:
              printf("[WARNING] Invalid activation function for connection of N-%d\n",
                     con->prev_neuron->_id);
              return 0.0f;
              break;
        }
    }
}
