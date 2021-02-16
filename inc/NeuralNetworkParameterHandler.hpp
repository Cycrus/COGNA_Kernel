#ifndef INCLUDE_NEURALNETWORKPARAMETERHANDLER_HPP
#define INCLUDE_NEURALNETWORKPARAMETERHANDLER_HPP

#include "NeuronParameterHandler.hpp"

namespace COGNA{
    class NeuralNetworkParameterHandler: public COGNA::NeuronParameterHandler{
        public:
            int transmitter_amount;

            float max_transmitter_weight;
            float min_transmitter_weight;

            float transmitter_backfall_curvature;
            float transmitter_backfall_steepness;

            NeuralNetworkParameterHandler();
    };
}

#endif /*INCLUDE_NEURALNETWORKPARAMETERHANDLER_HPP */
