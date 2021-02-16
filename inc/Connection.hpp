/**
 * @file Connection.hpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief A class connecting the different neurons in the network. It also is responsible for learning behavior.
 *
 * **Note:**
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
#include "ConnectionParameterHandler.hpp"
#include "NeuronParameterHandler.hpp"

namespace COGNA{
    class Neuron;

    // Connection structure
    class Connection{
        public:
            COGNA::Neuron* next_neuron;      /* The neuron this connection is connected with */
            COGNA::Connection* next_connection; /* The connection this connection is connected with */
            COGNA::Neuron* prev_neuron;      /* The origin neuron of this connection */

            COGNA::ConnectionParameterHandler *_parameter;

            float base_weight;        /* Base weight where learning processes always slowly return to */
            float short_weight;       /* Weight of this connection changing for short term learning. This one is directly used */
            float long_weight;        /* Weight of this connection changing for long term learning */
            float long_learning_weight; /* Factor that controls the learning processes of long_weight */

            float presynaptic_potential;   /* Defines the additional activation by presynaptic connection */
            int64_t last_presynaptic_activated_step;  /* Stores the global network step when this connection was last presynaptic activated */

            int64_t last_activated_step;  /* Stores the global network step when this connection was last activated */

            Connection(COGNA::NeuronParameterHandler *default_parameter);

            void long_learning_weight_backfall(int64_t network_step);
            void long_learning_weight_reduction(int64_t network_step);

            void habituate(int64_t network_step, Connection *conditioning_con=NULL);
            void sensitize(int64_t network_step, Connection *conditioning_con=NULL);

            void dehabituate(int64_t network_step);
            void desensitize(int64_t network_step);

            void basic_learning(int64_t network_step, Connection *conditioning_con=NULL);
    };
}

#endif /* INCLUDE_CONNECTION_HPP */
