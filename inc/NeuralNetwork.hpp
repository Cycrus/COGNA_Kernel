#ifndef INCLUDE_NETWORK_HPP
#define INCLUDE_NETWORK_HPP

#include "Neuron.hpp"
#include "NeuralNetworkParameterHandler.hpp"

namespace COGNA{
    class NeuralNetwork{
        private:
            void change_transmitter_weight(int transmitter_id,
                                           float new_weight);

            void calculate_neuron_backfall(COGNA::Neuron *n);
            void clear_neuron_activation(COGNA::Neuron *n);

            void influence_transmitter(COGNA::Neuron *n);
            void transmitter_backfall();

            void presynaptic_potential_backfall(COGNA::Connection *con);

            void activate_random_neurons();
            void activate_next_entities();
            void activate_next_neuron(COGNA::Connection *con);
            void activate_next_connection(COGNA::Connection *con);
            void save_next_neurons();
            void switch_vectors();

        public:
            std::vector<COGNA::Neuron*> _neurons;                   // All neurons contained in the network
            std::vector<COGNA::Neuron*> _random_neurons;            // All neurons in the network which can activate randomly
            std::vector<float> _transmitter_weights;
            std::vector<COGNA::Connection*> _curr_connections;      // All connections which will be activated in this step
            std::vector<COGNA::Connection*> _next_connections;      // All connections which will be activated in the next step
            int64_t _network_step_counter;

            COGNA::NeuralNetworkParameterHandler *_parameter;

            NeuralNetwork();
            ~NeuralNetwork();

            int add_neuron(float threshold);
            int set_neural_transmitter_influence(int neuron_id,
                                                 int transmitter_id,
                                                 int influence_direction=POSITIVE_INFLUENCE);
            int define_transmitters(int number);
            int set_random_neuron_activation(int neuron_id,
                                             int chance,
                                             float activation_value);
            Connection* add_neuron_connection(int source_neuron,
                                              int target_neuron,
                                              float weight,
                                              int connection_type=EXCITATORY,
                                              int function_type=FUNCTION_RELU,
                                              int learning_type=LEARNING_NONE,
                                              int transmitter_type=STD_TRANSMITTER);
            Connection* add_synaptic_connection(int source_neuron,
                                                int connected_neuron_1,
                                                int connected_neuron_2,
                                                float weight,
                                                int connection_type=EXCITATORY,
                                                int function_type=FUNCTION_RELU,
                                                int learning_type=LEARNING_NONE,
                                                int transmitter_type=STD_TRANSMITTER);
            Connection* add_synaptic_connection(int source_neuron,
                                                Connection *con,
                                                float weight,
                                                int connection_type=EXCITATORY,
                                                int function_type=FUNCTION_RELU,
                                                int learning_type=LEARNING_NONE,
                                                int transmitter_type=STD_TRANSMITTER);
            int init_activation(int target_neuron,
                                float activation);
            int setup_network();
            void feed_forward();
            void print_activation();

            int64_t get_step_count();
            Neuron *get_neuron(int neuron_id);
            int neuron_is_active(int neuron_id);
            float get_neuron_activation(int neuron_id);
            float get_transmitter_weight(int transmitter_id);
    };

    long get_time_microsec(struct timeval time);
}

#endif /* INCLUDE_NETWORK_HPP */
