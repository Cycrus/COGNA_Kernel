#include <cstdlib>
#include <cstdio>
#include "Connection.hpp"
#include "Neuron.hpp"
#include "MathUtils.hpp"

Connection::Connection(NeuronParameterHandler *default_parameter){
    _parameter = new ConnectionParameterHandler();

    _parameter->activation_type = default_parameter->activation_type;
    _parameter->activation_function = default_parameter->activation_function;

    _parameter->max_weight = default_parameter->max_weight;
    _parameter->min_weight = default_parameter->min_weight;

    _parameter->learning_type = default_parameter->learning_type;
    _parameter->transmitter_type = default_parameter->transmitter_type;

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

    next_neuron = NULL;
    next_connection = NULL;
    prev_neuron = NULL;

    base_weight = 0;
    short_weight = 0;
    long_weight = 1.0f;
    long_learning_weight = 1.0f;
    presynaptic_potential = DEFAULT_PRESYNAPTIC_POTENTIAL;

    last_presynaptic_activated_step = 0;
    last_activated_step = 0;
}

/***********************************************************
 * Connection::long_learning_weight_backfall(Connection *con)
 *
 * Description: Calculates the backfall of the "long learning weight" variable
 *              used to control the learning rate of long term weights
 *
 * Params:      Connection*     con     target neuron of learner weight backfall
 */
void Connection::long_learning_weight_backfall(int64_t network_step){
    if(DEBUG_MODE && DEB_LONG_LEARNING_WEIGHT)
        printf("<%ld> C-%d -> Long learning weight before backfall = %.3f\n",
               network_step, prev_neuron->_id, long_learning_weight);

    long_learning_weight =  MathUtils::calculate_static_gradient(long_learning_weight,
                                                          _parameter->long_learning_weight_backfall_steepness,
                                                          network_step - last_activated_step,
                                                          _parameter->long_learning_weight_backfall_curvature,
                                                          ADD,
                                                          MAX_LONG_LEARNING_WEIGHT,
                                                          MIN_LONG_LEARNING_WEIGHT);

    if(DEBUG_MODE && DEB_LONG_LEARNING_WEIGHT)
        printf("<%ld> C-%d -> Long learning weight after backfall = %.3f\n\n",
               network_step, prev_neuron->_id, long_learning_weight);
}

/***********************************************************
 * Connection::long_learning_weight_reduction(Connection *con)
 *
 * Description: Calculates the reduction of the "long learning weight" variable
 *              used to control the learning rate of long term weights
 *
 * Params:      Connection*     con     target neuron of learner weight reduction
 */
void Connection::long_learning_weight_reduction(int64_t network_step){
    if(DEBUG_MODE && DEB_LONG_LEARNING_WEIGHT)
        printf("<%ld> C-%d -> Long learning weight before reduction = %.3f\n",
               network_step, prev_neuron->_id, long_learning_weight);

    long_learning_weight =  MathUtils::calculate_static_gradient(long_learning_weight,
                                                          _parameter->long_learning_weight_reduction_steepness,
                                                          1,
                                                          _parameter->long_learning_weight_reduction_curvature,
                                                          SUBTRACT,
                                                          MAX_LONG_LEARNING_WEIGHT,
                                                          MIN_LONG_LEARNING_WEIGHT);
    if(DEBUG_MODE && DEB_LONG_LEARNING_WEIGHT)
        printf("<%ld> C-%d -> Long learning weight after reduction = %.3f\n\n",
               network_step, prev_neuron->_id, long_learning_weight);
}

/***********************************************************
 * Connection::habituate(Connection *con)
 *
 * Description: Calculates the basic learning process habituation for a connection
 *
 * Params:     Connection *     con                 the connection which should be transformed
 *             Connection *     coniditioning_con   the connection that points to con (if it exists)
 */
void Connection::habituate(int64_t network_step, Connection *conditioning_con){
    // TODO first part not good
    float activation = prev_neuron->_activation;
    float conditioning_type = NONDIRECTIONAL;

    if(conditioning_con){
        activation = conditioning_con->prev_neuron->_activation;
        conditioning_type = conditioning_con->_parameter->activation_type;
    }

    if((conditioning_type == NONDIRECTIONAL && activation < _parameter->habituation_threshold) ||
       (conditioning_type == INHIBITORY)){
        if(DEBUG_MODE && DEB_HABITUATION){
            printf("<%ld> N-%d -> Short before habituation = %.5f\n",
                   network_step, prev_neuron->_id, short_weight);
            printf("<%ld> N-%d -> Long before habituation = %.5f\n",
                   network_step, prev_neuron->_id, long_weight);
        }
        if(DEBUG_MODE && DEB_LONG_LEARNING_WEIGHT)
            printf("<%ld> N-%d -> Learner before habituation = %.5f\n",
                   network_step, prev_neuron->_id, long_learning_weight);

        if(conditioning_type == NONDIRECTIONAL)
            activation = _parameter->habituation_threshold - activation;

        short_weight =  MathUtils::calculate_dynamic_gradient(short_weight,
                                               _parameter->short_habituation_steepness,
                                               activation,
                                               _parameter->short_habituation_curvature,
                                               SUBTRACT,
                                               _parameter->max_weight,
                                               _parameter->min_weight);

        long_weight =  MathUtils::calculate_dynamic_gradient(long_weight,
                                              _parameter->long_habituation_steepness,
                                              activation * long_learning_weight,
                                              _parameter->long_habituation_curvature,
                                              SUBTRACT,
                                              _parameter->max_weight,
                                              _parameter->min_weight);

        long_learning_weight_reduction(network_step);

        if(DEBUG_MODE && DEB_HABITUATION){
            printf("<%ld> N-%d -> Short after habituation = %.5f\n",
                   network_step, prev_neuron->_id, short_weight);
            printf("<%ld> N-%d -> Long after habituation = %.5f\n",
                   network_step, prev_neuron->_id, long_weight);
        }
        if(DEBUG_MODE && DEB_LONG_LEARNING_WEIGHT)
            printf("<%ld> N-%d -> Learner after habituation = %.5f\n\n",
                   network_step, prev_neuron->_id, long_learning_weight);
    }
}

/***********************************************************
 * Connection::sensitize()
 *
 * Description: Calculates the basic learning process sensitization for a connection
 *
 * Params:     Connection *     con                 the connection which should be transformed
 *             Connection *     coniditioning_con   the connection that points to con (if it exists)
 */
void Connection::sensitize(int64_t network_step, Connection *conditioning_con){
    // TODO first part not good
    float activation = prev_neuron->_activation;
    float conditioning_type = NONDIRECTIONAL;

    if(conditioning_con){
        activation = conditioning_con->prev_neuron->_activation;
        conditioning_type = conditioning_con->_parameter->activation_type;
    }

    if((conditioning_type == NONDIRECTIONAL && activation > _parameter->sensitization_threshold) ||
       (conditioning_type == EXCITATORY)){
        if(DEBUG_MODE && DEB_SENSITIZATION){
            printf("<%ld> N-%d -> Short before sensitization = %.5f\n",
                   network_step,  prev_neuron->_id, short_weight);
            printf("<%ld> N-%d -> Long before sensitization = %.5f\n",
                   network_step, prev_neuron->_id, long_weight);
        }
        if(DEBUG_MODE && DEB_LONG_LEARNING_WEIGHT)
            printf("<%ld> N-%d -> Learner before sensitization = %.5f\n",
                   network_step, prev_neuron->_id, long_learning_weight);

        if(conditioning_type == NONDIRECTIONAL)
            activation = activation - _parameter->sensitization_threshold;

        short_weight =  MathUtils::calculate_dynamic_gradient(short_weight,
                                               _parameter->short_sensitization_steepness,
                                               activation,
                                               _parameter->short_sensitization_curvature,
                                               ADD,
                                               _parameter->max_weight,
                                               _parameter->min_weight);

        long_weight =  MathUtils::calculate_dynamic_gradient(long_weight,
                                              _parameter->long_sensitization_steepness,
                                              activation * long_learning_weight,
                                              _parameter->long_sensitization_curvature,
                                              ADD,
                                              _parameter->max_weight,
                                              _parameter->min_weight);

        long_learning_weight_reduction(network_step);

        if(DEBUG_MODE && DEB_SENSITIZATION){
            printf("<%ld> N-%d -> Short after sensitization = %.5f\n",
                   network_step, prev_neuron->_id, short_weight);
            printf("<%ld> N-%d -> Long after sensitization = %.5f\n",
                   network_step, prev_neuron->_id, long_weight);
        }
        if(DEBUG_MODE && DEB_LONG_LEARNING_WEIGHT)
            printf("<%ld> N-%d -> Learner after sensitization = %.5f\n\n",
                   network_step, prev_neuron->_id, long_learning_weight);
    }
}

/***********************************************************
 * Connection::dehabituate(Connection *con)
 *
 * Description: Calculates the backfall of learning functions
 *
 * Params:     Connection *     con     the connection which should be transformed
 */
void Connection::dehabituate(int64_t network_step){
    if(DEBUG_MODE && DEB_HABITUATION){
        printf("<%ld> N-%d -> Short before dehabituation = %.5f\n",
               network_step, prev_neuron->_id, short_weight);
        printf("<%ld> N-%d -> Long before dehabituation = %.5f\n",
               network_step, prev_neuron->_id, long_weight);
    }

    if(long_weight < base_weight){
       long_weight =  MathUtils::calculate_static_gradient(long_weight,
                                                    _parameter->long_dehabituation_steepness,
                                                    network_step - last_activated_step,
                                                    _parameter->long_dehabituation_curvature,
                                                    ADD,
                                                    base_weight,
                                                    _parameter->min_weight);
    }

    if(short_weight < long_weight){
       short_weight =  MathUtils::calculate_static_gradient(short_weight,
                                                     _parameter->short_dehabituation_steepness,
                                                     network_step - last_activated_step,
                                                     _parameter->short_dehabituation_curvature,
                                                     ADD,
                                                     long_weight,
                                                     _parameter->min_weight);
    }

    if(DEBUG_MODE && DEB_HABITUATION){
        printf("<%ld> N-%d -> Short after dehabituation = %.5f\n",
               network_step, prev_neuron->_id, short_weight);
        printf("<%ld> N-%d -> Long after dehabituation = %.5f\n\n",
               network_step, prev_neuron->_id, long_weight);
    }
}


/***********************************************************
 * Connection::desensitize(Connection *con)
 *
 * Description: Calculates the backfall of learning functions
 *
 * Params:     Connection *     con     the connection which should be transformed
 */
void Connection::desensitize(int64_t network_step){
    if(DEBUG_MODE && DEB_SENSITIZATION){
        printf("<%ld> N-%d -> Short before desensitization = %.5f\n",
               network_step, prev_neuron->_id, short_weight);
        printf("<%ld> N-%d -> Long before desensitization = %.5f\n",
               network_step, prev_neuron->_id, long_weight);
    }

    if(long_weight > base_weight){
        long_weight =  MathUtils::calculate_static_gradient(long_weight,
                                                     _parameter->long_desensitization_steepness,
                                                     network_step - last_activated_step,
                                                     _parameter->long_desensitization_curvature,
                                                     SUBTRACT,
                                                     _parameter->max_weight,
                                                     base_weight);
    }

    if(short_weight > long_weight){
        short_weight =  MathUtils::calculate_static_gradient(short_weight,
                                                      _parameter->short_desensitization_steepness,
                                                      network_step - last_activated_step,
                                                      _parameter->short_desensitization_curvature,
                                                      SUBTRACT,
                                                      _parameter->max_weight,
                                                      long_weight);
    }

    if(DEBUG_MODE && DEB_SENSITIZATION){
        printf("<%ld> N-%d -> Short after desensitization = %.5f\n",
               network_step, prev_neuron->_id, short_weight);
        printf("<%ld> N-%d -> Long after desensitization = %.5f\n\n",
               network_step, prev_neuron->_id, long_weight);
    }
}

/***********************************************************
 * Connection::basic_learning()
 *
 * Description: Calculates the basic learning process sensitization for a connection
 *
 * Params:     Connection *     con                 the connection which should be transformed
 *             Connection *     coniditioning_con   the connection that points to con (if it exists)
 */
void Connection::basic_learning(int64_t network_step, Connection *conditioning_con){
    long_learning_weight_backfall(network_step);

    if(_parameter->learning_type == LEARNING_HABITUATION ||
       _parameter->learning_type == LEARNING_HABISENS){
        dehabituate(network_step);
        habituate(network_step, conditioning_con);
    }

    if(_parameter->learning_type == LEARNING_SENSITIZATION ||
       _parameter->learning_type == LEARNING_HABISENS){
         desensitize(network_step);
         sensitize(network_step, conditioning_con);
    }

    last_activated_step = network_step;
}
