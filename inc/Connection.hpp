/**
 * @file Connection.hpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief A class connecting the different neurons in the network. It also is responsible for learning behavior.
 *
 * The connection type is always nested inside a neuron and connects this neuron to
 * another neuron or another connection. When connected to a neuron, it sends
 * an impulse to activate the following neuron. When connected to another connection,
 * it sends an impulse that the following connection should get weaker or stronger.
 *
 * It can be configured to learn each time when sending an impulse. If it can
 * sensitize, it gets stronger each time a strong impulse is sent, if it can
 * habituate, it gets weaker each time a weak impulse is sent.
 *
 * @date 2021-02-16
 *
 */

#ifndef INCLUDE_CONNECTION_HPP
#define INCLUDE_CONNECTION_HPP

#include <cstdint>
#include <cstdlib>

namespace COGNA{
    class Neuron;
    class NeuronParameterHandler;
    class ConnectionParameterHandler;

    /**
     * @brief Class for connections between entities in the network.
     *
     */
    class Connection{
        public:
            COGNA::Neuron* next_neuron;
            COGNA::Connection* next_connection;
            COGNA::Neuron* prev_neuron;

            /**
             * @brief Contains the parameters specifiying connection behavior.
             *
             */
            COGNA::ConnectionParameterHandler *_parameter;

            float base_weight;        /* Base weight where learning processes always slowly return to */
            float short_weight;       /* Weight of this connection changing for short term learning. This one is directly used */
            float long_weight;        /* Weight of this connection changing for long term learning */
            float long_learning_weight; /* Factor that controls the learning processes of long_weight */

            float presynaptic_potential;   /* Defines the additional activation by presynaptic connection */
            int64_t last_presynaptic_activated_step;  /* Stores the global network step when this connection was last presynaptic activated */

            int64_t last_activated_step;  /* Stores the global network step when this connection was last activated */

            /**
             * @brief Initializes all behavior relevant parameter of Connection.
             *
             * Inherits the connection-specific parameters of the neuron, where
             * the connection is stored in.
             *
             * @param default_parameter    Parameters from source neuron.
             *
             */
            Connection(COGNA::NeuronParameterHandler *default_parameter);

            ~Connection();

            /**
             * @brief Calculates the backfall of the factor which reduces longterm learning after some time of nonactivation.
             *
             * Based on a static gradient, the backfall of the reducing factor for
             * longterm learning is calculated. The longer the connection did not
             * fire, the more long_learning_weight falls back to 1.0.
             *
             * @param network_step    The current step/tick count of the network.
             *
             */
            void long_learning_weight_backfall(int64_t network_step);

            /**
             * @brief Calculates the reduction of long term learning after repeated activation.
             *
             * Based on a dynamic gradient, the reduction of longterm learning
             * is calculated. The more often the connection is activated, the
             * more reduction happens.
             *
             * @param network_step    The current step/tick count of the network.
             *
             */
            void long_learning_weight_reduction(int64_t network_step);

            /**
             * @brief Calculates habituation of connection if weakly activated.
             *
             * Can happen directly by weak activation of this connection, or by
             * a presynaptic connection, which acts as a conditioning input.
             *
             * @param network_step        The current step/tick count of the network.
             * @param conditioning_con    Another connection which is connected to this one.
             *
             */
            void habituate(int64_t network_step, Connection *conditioning_con=NULL);

            /**
             * @brief Calculates sansitization of connection if strongly activated.
             *
             * Can happen directly by strong activation of this connection, or by
             * a presynaptic connection, which acts as a conditioning input.
             *
             * @param network_step        The current step/tick count of the network.
             * @param conditioning_con    Another connection which is connected to this one.
             *
             */
            void sensitize(int64_t network_step, Connection *conditioning_con=NULL);

            /**
             * @brief Calculates dehabituation of connection after time.
             *
             * The more steps/ticks pass since last activation, the more the
             * connection dehabituates.
             *
             * @param network_step    The current step/tick count of the network.
             *
             */
            void dehabituate(int64_t network_step);

            /**
             * @brief Calculates desensitization of connection after time.
             *
             * The more steps/ticks pass since last activation, the more the
             * connection desensitizes.
             *
             * @param network_step    The current step/tick count of the network.
             *
             */
            void desensitize(int64_t network_step);

            /**
             * @brief A wrapper including all learning functions of a connection
             *
             * Also contains logic controlling if specific learning behaviors
             * should activate at every given moment.
             *
             * @param network_step        The current step/tick count of the network.
             * @param conditioning_con    Another connection which is connected to this one.
             *
             */
            void basic_learning(int64_t network_step, Connection *conditioning_con=NULL);

            /**
             * @brief Calculates backfall of presynaptic potential, if the connection is not presynaptically activated.
             *
             * The more steps/ticks pass since last activation, the more the
             * presynaptic potential falls back to a neutral state.
             *
             * @param network_step        The current step/tick count of the network.
             *
             */
            void presynaptic_potential_backfall(int64_t network_step);

            /**
             * @brief Decides which activation function should be used for activation.
             *
             * @param input    The input value for the function.
             *
             * @return         The result of the function.
             *
             */
            float choose_activation_function(float input);
    };
}

#endif /* INCLUDE_CONNECTION_HPP */
