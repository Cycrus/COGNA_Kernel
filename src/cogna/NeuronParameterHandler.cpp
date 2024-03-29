/**
 * @file NeuronParameterHandler.cpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief Implementation of NeuronParameterHandler class
 *
 * @date 2021-02-18
 *
 */

#include "NeuronParameterHandler.hpp"

using namespace COGNA;

namespace COGNA{
    NeuronParameterHandler::NeuronParameterHandler(){
        activation_threshold = 0.0f;

        max_activation = 50.0f;
        min_activation = 0.0f;

        activation_backfall_curvature = 0.0f;
        activation_backfall_steepness = 0.0f;

        transmitter_change_curvature = 0.0f;
        transmitter_change_steepness = 0.0f;

        influenced_transmitter = NO_TRANSMITTER;
        transmitter_influence_direction = POSITIVE_INFLUENCE;

        random_activation = false;
        random_chance = 0;
        random_activation_value = 0.0f;
    }
}
