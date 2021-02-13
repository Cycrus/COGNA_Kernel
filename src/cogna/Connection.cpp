#include <cstdlib>
#include "Connection.hpp"

Connection::Connection(){
    _parameter = new ConnectionParameterHandler();

    next_neuron = NULL;
    next_connection = NULL;
    prev_neuron = NULL;

    base_weight = 0;
    short_weight = 0;
    long_weight = 1.0f;
    long_learning_weight = 1.0f;
    presynaptic_potential = DEFAULT_PRESYNAPTIC_POTENTIAL;

    last_presynaptic_activated_step = 0;
    last_activated_step = 0;
}
