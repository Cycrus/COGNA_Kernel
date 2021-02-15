#include <cstdlib>
#include "Connection.hpp"

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
