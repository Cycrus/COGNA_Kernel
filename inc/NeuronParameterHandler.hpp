#ifndef INCLUDE_NEURONPARAMETERHANDLER_HPP
#define INCLUDE_NEURONPARAMETERHANDLER_HPP

#include "ConnectionParameterHandler.hpp"

const int EXCITATORY = 1;
const int INHIBITORY = -1;
const int NONDIRECTIONAL = 0;

const int FUNCTION_SIGMOID = 1;
const int FUNCTION_LINEAR = 2;
const int FUNCTION_RELU = 3;

const int LEARNING_NONE = 1;
const int LEARNING_HABITUATION = 2;
const int LEARNING_SENSITIZATION = 3;
const int LEARNING_HABISENS = 4;

const int POSITIVE_INFLUENCE = 1;
const int NEGATIVE_INFLUENCE = -1;

const int NO_TRANSMITTER = -1;
const int STD_TRANSMITTER = 0;

class NeuronParameterHandler: public ConnectionParameterHandler{
    public:
        float activation_threshold;

        float max_activation;
        float min_activation;

        float activation_backfall_curvature;
        float activation_backfall_steepness;

        float transmitter_change_curvature;
        float transmitter_change_steepness;

        int influenced_transmitter;            /* Transmitter this neuron should influence if activated */
        int transmitter_influence_direction;   /* Defines if transmitter is influenced positively or negatively */

        NeuronParameterHandler();
};

#endif /* INCLUDE_NEURONPARAMETERHANDLER_HPP */
