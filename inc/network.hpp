#ifndef INCLUDE_NETWORK_HPP
#define INCLUDE_NETWORK_HPP

#include "neuron.hpp"

extern int DATA_ANALYTIC_OUTPUT;

extern int DEBUG_MODE;
extern int DEB_BASE;
extern int DEB_HABITUATION;
extern int DEB_SENSITIZATION;
extern int DEB_TRANSMITTER;
extern int DEB_NEURON_BACKFALL;
extern int DEB_LONG_LEARNING_WEIGHT;
extern int DEB_PRESYNAPTIC;

#define SUCCESS_CODE 0
#define ERROR_CODE -1

#define MICROSECOND_FACTOR 1000000
#define MIN_NEURON_ID 1

#define ADD 1
#define SUBTRACT -1

#define MIN_TRANSMITTER_WEIGHT 0
#define MAX_TRANSMITTER_WEIGHT 3

#define MIN_LONG_LEARNING_WEIGHT 0.0f
#define MAX_LONG_LEARNING_WEIGHT 1.0f

#define MAX_CHANCE 100000000


class NeuralNetwork{
    // TODO Too powerful
    private:
        float calculate_dynamic_gradient(float source_value,
                                         float curve_factor,
                                         double x,
                                         float power_factor,
                                         int method=ADD,
                                         float max=100.0f,
                                         float min=0.0f);
        float calculate_static_gradient(float source_value,
                                        float curve_factor,
                                        double x,
                                        float power_factor,
                                        int method=ADD,
                                        float max=100.0f,
                                        float min=0.0f);

        void change_transmitter_weight(int transmitter_id,
                                       float new_weight);

        void calculate_neuron_backfall(Neuron *n);
        void clear_neuron_activation(Neuron *n);

        void long_learning_weight_backfall(Connection *con);
        void long_learning_weight_reduction(Connection *con);

        void dehabituate(Connection *con);
        void desensitize(Connection *con);
        void habituate(Connection *con, Connection *conditioning_con=NULL);
        void sensitize(Connection *con, Connection *conditioning_con=NULL);
        void basic_learning(Connection *con, Connection *conditioning_con=NULL);

        void influence_transmitter(Neuron *n);
        void transmitter_backfall();

        void presynaptic_potential_backfall(Connection *con);

        void activate_random_neurons();
        void activate_next_entities();
        void activate_next_neuron(Connection *con);
        void activate_next_connection(Connection *con);
        void save_next_neurons();
        void switch_vectors();

    public:
        std::vector<Neuron*> _neurons;                   // All neurons contained in the network
        std::vector<Neuron*> _random_neurons;            // All neurons in the network which can activate randomly
        std::vector<float> _transmitter_weights;
        std::vector<Connection*> _curr_connections;      // All connections which will be activated in this step
        std::vector<Connection*> _next_connections;      // All connections which will be activated in the next step
        int64_t _network_step_counter;

        float _global_max_activation;
        float _global_min_activation;

        float _global_max_weight;
        float _global_min_weight;

        float _global_backfall_curvature;
        float _global_backfall_steepness;

        float _global_max_transmitter_weight;
        float _global_min_transmitter_weight;

        float _global_short_habituation_curvature;
        float _global_short_habituation_steepness;
        float _global_short_sensitization_curvature;
        float _global_short_sensitization_steepness;
        float _global_short_dehabituation_curvature;
        float _global_short_dehabituation_steepness;
        float _global_short_desensitization_curvature;
        float _global_short_desensitization_steepness;

        float _global_long_habituation_curvature;
        float _global_long_habituation_steepness;
        float _global_long_sensitization_curvature;
        float _global_long_sensitization_steepness;
        float _global_long_dehabituation_curvature;
        float _global_long_dehabituation_steepness;
        float _global_long_desensitization_curvature;
        float _global_long_desensitization_steepness;

        float _global_presynaptic_potential_curvature;
        float _global_presynaptic_potential_steepness;
        float _global_presynaptic_backfall_curvature;
        float _global_presynaptic_backfall_steepness;

        float _global_long_learning_weight_reduction_curvature;
        float _global_long_learning_weight_reduction_steepness;
        float _global_long_learning_weight_backfall_curvature;
        float _global_long_learning_weight_backfall_steepness;

        float _global_habituation_threshold;
        float _global_sensitization_threshold;

        float _transmitter_change_curvature;
        float _transmitter_change_steepness;
        float _transmitter_backfall_curvature;
        float _transmitter_backfall_steepness;

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

        void set_max_activation(float value);
        void set_min_activation(float value);

        void set_max_weight(float value);
        void set_min_weight(float value);

        void set_max_transmitter_weight(float value);
        void set_min_transmitter_weight(float value);

        void set_activation_backfall_curvature(float value);
        void set_activation_backfall_steepness(float value);

        void set_short_habituation_curvature(float value);
        void set_short_habituation_steepness(float value);
        void set_short_sensitization_curvature(float value);
        void set_short_sensitization_steepness(float value);
        void set_short_dehabituation_curvature(float value);
        void set_short_dehabituation_steepness(float value);
        void set_short_desensitization_curvature(float value);
        void set_short_desensitization_steepness(float value);

        void set_long_habituation_curvature(float value);
        void set_long_habituation_steepness(float value);
        void set_long_sensitization_curvature(float value);
        void set_long_sensitization_steepness(float value);
        void set_long_dehabituation_curvature(float value);
        void set_long_dehabituation_steepness(float value);
        void set_long_desensitization_curvature(float value);
        void set_long_desensitization_steepness(float value);

        void set_presynaptic_potential_curvature(float value);
        void set_presynaptic_potential_steepness(float value);
        void set_presynaptic_backfall_curvature(float value);
        void set_presynaptic_backfall_steepness(float value);

        void set_long_learning_weight_reduction_curvature(float value);
        void set_long_learning_weight_reduction_steepness(float value);
        void set_long_learning_weight_backfall_curvature(float value);
        void set_long_learning_weight_backfall_steepness(float value);

        void set_habituation_threshold(float value);
        void set_sensitization_threshold(float value);

        void set_transmitter_change_curvature(float value);
        void set_transmitter_change_steepness(float value);
        void set_transmitter_backfall_curvature(float value);
        void set_transmitter_backfall_steepness(float value);
};

long get_time_microsec(struct timeval time);

#endif /* INCLUDE_NETWORK_HPP */
