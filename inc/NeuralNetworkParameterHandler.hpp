#ifndef INCLUDE_NEURALNETWORKPARAMETERHANDLER_HPP
#define INCLUDE_NEURALNETWORKPARAMETERHANDLER_HPP

#include "NeuronParameterHandler.hpp"

const int SUCCESS_CODE = 0;
const int ERROR_CODE = -1;

const int MICROSECOND_FACTOR = 1000000;
const int MIN_NEURON_ID = 1;

const int MIN_TRANSMITTER_WEIGHT = 0;
const int MAX_TRANSMITTER_WEIGHT = 3;

const float MIN_LONG_LEARNING_WEIGHT = 0.0f;
const float MAX_LONG_LEARNING_WEIGHT = 1.0f;

const long MAX_CHANCE = 100000000;

class NeuralNetworkParameterHandler: public NeuronParameterHandler{
    public:
        int transmitter_amount;

        float max_transmitter_weight;
        float min_transmitter_weight;

        float transmitter_backfall_curvature;
        float transmitter_backfall_steepness;

        NeuralNetworkParameterHandler();
};

#endif /*INCLUDE_NEURALNETWORKPARAMETERHANDLER_HPP */
