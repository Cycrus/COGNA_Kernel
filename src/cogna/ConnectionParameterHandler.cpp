#include "ConnectionParameterHandler.hpp"

ConnectionParameterHandler::ConnectionParameterHandler(){
    activation_type = 0;
    activation_function = 0;
    learning_type = 0;
    transmitter_type = 0;

    max_weight = 5.0f;
    min_weight = 0.0f;

    short_habituation_curvature = 0.0f;
    short_habituation_steepness = 0.0f;
    short_sensitization_curvature = 0.0f;
    short_sensitization_steepness = 0.0f;
    short_dehabituation_curvature = 0.0f;
    short_dehabituation_steepness = 0.0f;
    short_desensitization_curvature = 0.0f;
    short_desensitization_steepness = 0.0f;

    long_habituation_curvature = 0.0f;
    long_habituation_steepness = 0.0f;
    long_sensitization_curvature = 0.0f;
    long_sensitization_steepness = 0.0f;
    long_dehabituation_curvature = 0.0f;
    long_dehabituation_steepness = 0.0f;
    long_desensitization_curvature = 0.0f;
    long_desensitization_steepness = 0.0f;

    presynaptic_potential_curvature = 0.0f;
    presynaptic_potential_steepness = 0.0f;
    presynaptic_backfall_curvature = 0.0f;
    presynaptic_backfall_steepness = 0.0f;

    habituation_threshold = 0.0f;
    sensitization_threshold = 0.0f;

    long_learning_weight_reduction_curvature = 0.0f;
    long_learning_weight_reduction_steepness = 0.0f;
    long_learning_weight_backfall_curvature = 0.0f;
    long_learning_weight_backfall_steepness = 0.0f;
}
