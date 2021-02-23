/**
 * @file NeuronParameterHandler.hpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief A class containing all behavior parameters of neurons.
 *
 * @date 2021-02-18
 *
 */

#ifndef INCLUDE_NEURONPARAMETERHANDLER_HPP
#define INCLUDE_NEURONPARAMETERHANDLER_HPP

#include "Constants.hpp"
#include "ConnectionParameterHandler.hpp"

namespace COGNA{
    /**
     * @brief A class containing all behavior parameters used by the Neuron class.
     *
     * Also contains all behavior parameters of Connection class, to be used
     * as default parameters for those connections with their origin in this
     * neuron.
     */
    class NeuronParameterHandler: public COGNA::ConnectionParameterHandler{
        public:
            float activation_threshold;

            float max_activation;
            float min_activation;

            float activation_backfall_curvature;
            float activation_backfall_steepness;

            float transmitter_change_curvature;
            float transmitter_change_steepness;

            int influenced_transmitter;            /**< Transmitter this neuron should influence if activated */
            int transmitter_influence_direction;   /**< Defines if transmitter is influenced positively or negatively */

            bool random_activation;               /**< Determines if neuron sometimes fires randomly */
	        int random_chance;                    /**< Chance of random activation per tick */
	        float random_activation_value;        /**< How strong is the neuron activated when randomly activated */

            /**
             * @brief Initializes neuron parameters by copying the relevant
             *        parameter values from the neural network.
             *
             */
            NeuronParameterHandler();
    };
}

#endif /* INCLUDE_NEURONPARAMETERHANDLER_HPP */
