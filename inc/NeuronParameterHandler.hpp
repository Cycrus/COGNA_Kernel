#ifndef INCLUDE_NEURONPARAMETERHANDLER_HPP
#define INCLUDE_NEURONPARAMETERHANDLER_HPP

#include "ConnectionParameterHandler.hpp"
#include "Constants.hpp"

namespace COGNA{
    class NeuronParameterHandler: public COGNA::ConnectionParameterHandler{
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
}

#endif /* INCLUDE_NEURONPARAMETERHANDLER_HPP */
