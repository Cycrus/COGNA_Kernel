#ifndef INCLUDE_NEURON_HPP
#define INCLUDE_NEURON_HPP

#include <vector>
#include <cstdint>
#include "NeuronParameterHandler.hpp"
#include "Connection.hpp"

class Neuron{
	public:
        int _id;
        static int s_max_id;                   /* Last neuron id which has been created. Used for new neuron ids */
        float _threshold;                      /* Threshold when neuron fires */
        bool _random_activation;                /* Determines if neuron sometimes fires randomly */
        int _random_chance;                    /* Chance of random activation per tick */
        float _random_activation_value;        /* How strong is the neuron activated when randomly activated */
        float _activation;                     /* Current activation potential level */
        float _temp_activation;                /* A temporary variable to calculate activation of next neurons */
        int64_t _last_activated_step;             /* Network step count, when neuron was last activated */
        bool _was_activated;                    /* Indicates if neuron was activated last time or this time */
        int _last_fired_step;                  /* Step when neuron last fired */

        NeuronParameterHandler *_parameter;

        std::vector<Connection*> _connections;
		std::vector<Neuron*> _previous;

        long get_step();
        void set_step(long new_step);

        Neuron(float threshold);
        ~Neuron();

        int check_neuron_connection(Neuron *n);
        int check_synaptic_connection(Connection *n);
        Connection* add_neuron_connection(Neuron *n, float weight,
                                          int con_type=EXCITATORY,
                                          int fun_type=FUNCTION_RELU,
                                          int learn_type=LEARNING_NONE,
                                          int transmitter_type=STD_TRANSMITTER);
        Connection* add_synaptic_connection(Connection *con, float weight,
                                            int con_type=EXCITATORY,
                                            int fun_type=FUNCTION_RELU,
                                            int learn_type=LEARNING_NONE,
                                            int transmitter_type=STD_TRANSMITTER);
        void del_connection(Neuron *n);
        float get_weight(Neuron *n, int type);
        int is_active();
        void set_random_activation(int chance, float activation_value);
};

float choose_activation_function(Connection *con, float input);

#endif /* INCLUDE_NEURON_HPP */
