/**
 * @file NeuralNetworkParameterHandler.hpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief A class containing all behavior parameters of the neural network.
 *
 * @date 2021-02-18
 *
 */

#ifndef INCLUDE_NEURALNETWORKPARAMETERHANDLER_HPP
#define INCLUDE_NEURALNETWORKPARAMETERHANDLER_HPP

#include "NeuronParameterHandler.hpp"

namespace COGNA{
    /**
     * @brief A class containing all behavior parameters used by the NeuralNetwork class.
     *
     * Also contains all behavior parameters of Neuron and Connection classes,
     * to be used as default parameters for all neurons inside of this network.
     */
    class NeuralNetworkParameterHandler: public COGNA::NeuronParameterHandler{
        public:
            int transmitter_amount;

            float max_transmitter_weight;
            float min_transmitter_weight;

            float transmitter_backfall_curvature;
            float transmitter_backfall_steepness;

            /**
             * @brief Initializes network parameters.
             *
             */
            NeuralNetworkParameterHandler();
    };
}

#endif /*INCLUDE_NEURALNETWORKPARAMETERHANDLER_HPP */
