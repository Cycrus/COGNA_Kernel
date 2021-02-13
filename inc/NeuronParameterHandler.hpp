#ifndef INCLUDE_NEURONPARAMETERHANDLER_HPP
#define INCLUDE_NEURONPARAMETERHANDLER_HPP

#include "ParameterHandler.hpp"

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

const float DEFAULT_TRANSMITTER_WEIGHT = 1.0f;

const float DEFAULT_PRESYNAPTIC_POTENTIAL = 1.0f;

class NeuronParameterHandler: public ParameterHandler{
    public:
        int influenced_transmitter;            /* Transmitter this neuron should influence if activated */
        int transmitter_influence_direction;   /* Defines if transmitter is influenced positively or negatively */

        NeuronParameterHandler();
};

#endif /* INCLUDE_NEURONPARAMETERHANDLER_HPP */
