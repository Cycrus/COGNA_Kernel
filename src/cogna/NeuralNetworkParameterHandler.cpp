/**
 * @file NeuralNetworkParameterHandler.cpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief Implementation of NeuralNetworkParameterHandler class
 *
 * @date 2021-02-18
 *
 */

#include "NeuralNetworkParameterHandler.hpp"

#include "Constants.hpp"

using namespace COGNA;

namespace COGNA{
    NeuralNetworkParameterHandler::NeuralNetworkParameterHandler(){
        transmitter_amount = 1;

        transmitter_backfall_curvature = 0.0f;
        transmitter_backfall_steepness = 0.0f;

        max_transmitter_weight = 3.0f;
        min_transmitter_weight = 0.0f;
    }
}
