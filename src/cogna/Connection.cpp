#include <cstdlib>
#include "Connection.hpp"

Connection::Connection(){
    next_neuron = NULL;
    next_connection = NULL;
    prev_neuron = NULL;
    activation_type = 0;
    activation_function = 0;
    learning_type = 0;
    transmitter_type = 0;

    base_weight = 0;
    short_weight = 0;
    long_weight = 1.0f;
    long_learning_weight = 1.0f;
    presynaptic_potential = DEFAULT_PRESYNAPTIC_POTENTIAL;

    last_presynaptic_activated_step = 0;
    last_activated_step = 0;
}
