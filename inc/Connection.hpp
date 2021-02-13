#ifndef INCLUDE_CONNECTION_HPP
#define INCLUDE_CONNECTION_HPP

#include <cstdint>

const float DEFAULT_TRANSMITTER_WEIGHT = 1.0f;
const float DEFAULT_PRESYNAPTIC_POTENTIAL = 1.0f;

class Neuron;

// Connection structure
class Connection{
    public:
        Neuron* next_neuron;      /* The neuron this connection is connected with */
        Connection* next_connection; /* The connection this connection is connected with */
        Neuron* prev_neuron;      /* The origin neuron of this connection */
        int activation_type;      /* Defines if connection is inhibitory or excitatory */
        int activation_function;  /* Defines what activation function the connection uses */
        int learning_type;        /* Defines the kind of learning the connection is capable of */
        int transmitter_type;     /* Type of neurotransmitter used for transmitting action potential */

        float base_weight;        /* Base weight where learning processes always slowly return to */
        float short_weight;       /* Weight of this connection changing for short term learning. This one is directly used */
        float long_weight;        /* Weight of this connection changing for long term learning */
        float long_learning_weight; /* Factor that controls the learning processes of long_weight */

        float presynaptic_potential;   /* Defines the additional activation by presynaptic connection */
        int64_t last_presynaptic_activated_step;  /* Stores the global network step when this connection was last presynaptic activated */

        int64_t last_activated_step;  /* Stores the global network step when this connection was last activated */

        Connection();
};

#endif /* INCLUDE_CONNECTION_HPP */
