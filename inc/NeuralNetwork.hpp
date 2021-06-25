/**
 * @file NeuralNetwork.hpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief A class containing every neuron and the functions for enabling communication between those.
 *
 * The NeuralNetwork class contains various functions to build the network by adding
 * neurons, connections, neurotransmitters, etc.
 *
 * @date 2021-02-16
 *
 */

#ifndef INCLUDE_NETWORK_HPP
#define INCLUDE_NETWORK_HPP

#include "Neuron.hpp"
#include "NeuralNetworkParameterHandler.hpp"
#include "NetworkingNode.hpp"
#include "networking_client.hpp"
#include "networking_sender.hpp"
#include "json.hpp"
#include <string>
#include <condition_variable>

namespace COGNA{

/**
 * @brief The class containing the whole network.
 *
 * Contains every neuron in the network and enables communication between those.
 *
 */
class NeuralNetwork{
public:
    std::vector<COGNA::Neuron*> _neurons;                   // All neurons contained in the network
    std::vector<COGNA::Connection*> _curr_connections;      // All connections which will be activated in this step
    COGNA::NeuralNetworkParameterHandler *_parameter;
    std::vector<COGNA::NetworkingNode*> _extern_input_nodes;
    std::vector<COGNA::NetworkingNode*> _extern_output_nodes;
    nlohmann::json _subnet_input_connection_list;
    nlohmann::json _subnet_output_connection_list;
    static int m_cluster_state;

    /**
     * @brief Initializes the neural network by setting some parameters and adding the Null-Neuron.
     *
     */
    NeuralNetwork();

    /**
     * @brief Frees all memory reserved by the whole network.
     *
     */
    ~NeuralNetwork();

    /**
     * @brief Adds a neuron to the network.
     *
     * The neuron inherits all parameters from the network.
     *
     * @param threshold    The activation threshold when the new neuron can fire.
     *
     * @return             Error code. SUCCESS_CODE if everything went right, ERROR_CODE if something went wrong.
     *
     */
    int add_neuron(float threshold);

    int add_extern_input_node(utils::networking_client *client, std::string channel);
    int add_extern_output_node(utils::networking_sender *sender, std::string channel);

    /**
     * @brief Sets a neuron to have an influence on a certain neurotransmitter if it fires.
     *
     * @param neuron_id              The ID of the neuron which should be set.
     * @param transmitter_id         The ID of the transmitter to be influenced by the neuron.
     * @param influence_direction    Decides if the transmitter is influenced positively or negatively.
     *
     * @return                       Error code. SUCCESS_CODE if everything went right, ERROR_CODE if something went wrong.
     *
     */
    int set_neural_transmitter_influence(int neuron_id,
                                         int transmitter_id,
                                         int influence_direction=POSITIVE_INFLUENCE);

    /**
     * @brief Defines the number of neurotransmitters existing in the network.
     *
     * @param number    The number of neurotransmitters used by the network.
     *
     * @return          Error code. SUCCESS_CODE if everything went right, ERROR_CODE if something went wrong.
     *
     */
    int define_transmitters(int number);

    /**
     * @brief Sets a neuron to autonomously fire at random intervals.
     *
     * @param neuron_id           The ID of the neuron which should fire randomly
     * @param chance              The chance of firing in each step. 1 = 1/100000000.
     * @param activation_value    The activation with which the neuron randomly fires
     *
     * @return                    Error code. SUCCESS_CODE if everything went right, ERROR_CODE if something went wrong.
     *
     */
    int set_random_neuron_activation(int neuron_id,
                                     int chance,
                                     float activation_value);

    /**
     * @brief Adds a new connection between two neurons to the network.
     *
     * @param source_neuron       The neuron which should send a signal.
     * @param target_neuron       The neuron which should receive a signal.
     * @param weight              The default base weight of the new connection.
     * @param con_type            The type of connection. Can be EXCITATORY or INHIBITORY.
     * @param fun_type            The activation function of the connection.
     * @param learn_type          Decides if connection can learn via habituation, sensitization, or both.
     * @param transmitter_type    The transmitter the new connection uses for signal transmission.
     *
     * @return                    The newly created connection. Used for synaptic connections.
     *
     */
    Connection* add_neuron_connection(int source_neuron,
                                      int target_neuron,
                                      float weight,
                                      int connection_type=EXCITATORY,
                                      int function_type=FUNCTION_RELU,
                                      int learning_type=LEARNING_NONE,
                                      int transmitter_type=STD_TRANSMITTER);

    /**
    * @brief Adds a new connection between two neurons to the network.
    *
    * @param source_neuron       The neuron which should send a signal.
    * @param target_neuron       The object of the neuron which should receive a signal.
    * @param weight              The default base weight of the new connection.
    * @param con_type            The type of connection. Can be EXCITATORY or INHIBITORY.
    * @param fun_type            The activation function of the connection.
    * @param learn_type          Decides if connection can learn via habituation, sensitization, or both.
    * @param transmitter_type    The transmitter the new connection uses for signal transmission.
    *
    * @return                    The newly created connection. Used for synaptic connections.
    *
    */
    Connection* add_neuron_connection(int source_neuron,
                                      Neuron *target_neuron,
                                      float weight,
                                      int connection_type=EXCITATORY,
                                      int function_type=FUNCTION_RELU,
                                      int learning_type=LEARNING_NONE,
                                      int transmitter_type=STD_TRANSMITTER);

    /**
     * @brief Adds a new connection between a neuron and a connection.
     *
     * Uses the ID of two neurons the receiving connection connects.
     *
     * @param source_neuron         The neuron which should send a signal.
     * @param connected_neuron_1    The source neuron of the receiving connection
     * @param connected_neuron_2    The target neuron of the receiving connection
     * @param weight                The default base weight of the new connection.
     * @param con_type              The type of connection. Can be EXCITATORY or INHIBITORY.
     * @param fun_type              The activation function of the connection.
     * @param learn_type            Decides if connection can learn via habituation, sensitization, or both.
     * @param transmitter_type      The transmitter the new connection uses for signal transmission.
     *
     * @return                      The newly created connection. Used for synaptic connections.
     *
     */
    Connection* add_synaptic_connection(int source_neuron,
                                        int connected_neuron_1,
                                        int connected_neuron_2,
                                        float weight,
                                        int connection_type=EXCITATORY,
                                        int function_type=FUNCTION_RELU,
                                        int learning_type=LEARNING_NONE,
                                        int transmitter_type=STD_TRANSMITTER);

    /**
     * @brief Adds a new connection between a neuron and a connection.
     *
     * Uses a pointer to the receiving connection. Therefore is more
     * versatile than the alternative with two neuron IDs.
     *
     * @param source_neuron         The neuron which should send a signal.
     * @param con                   A pointer to the connection which should receive a signal.
     * @param weight                The default base weight of the new connection.
     * @param con_type              The type of connection. Can be EXCITATORY or INHIBITORY.
     * @param fun_type              The activation function of the connection.
     * @param learn_type            Decides if connection can learn via habituation, sensitization, or both.
     * @param transmitter_type      The transmitter the new connection uses for signal transmission.
     *
     * @return                      The newly created connection. Used for synaptic connections.
     *
     */
    Connection* add_synaptic_connection(int source_neuron,
                                        Connection *con,
                                        float weight,
                                        int connection_type=EXCITATORY,
                                        int function_type=FUNCTION_RELU,
                                        int learning_type=LEARNING_NONE,
                                        int transmitter_type=STD_TRANSMITTER);

    /**
     * @brief Initializes a certain activation niveau into a neuron.
     *
     * Can be used to input signals into the network.
     *
     * @param target_neuron    The ID of neuron it initialize activation in.
     * @param activation       The value of activation to initialize into the neuron.
     *
     * @return                 Error code. SUCCESS_CODE if everything went right, ERROR_CODE if something went wrong.
     *
     */
    int init_activation(int target_neuron,
                        float activation);

    /**
     * @brief Must be called before the network loop starts to clean some things up.
     *
     * Sets random number seed and connects al loose neurons to the Null-Neuron.
     * Otherwise those could never be called by the network.
     *
     * @return    Error code. SUCCESS_CODE if everything went right, ERROR_CODE if something went wrong.
     *
     */
    int setup_network();

    /**
     * @brief This function calls every necessary function to do one step of the network.
     *
     * Usually called inside of the network function.
     *
     */
    void feed_forward();

    /**
     * @brief This function listens to the cluster step counter and activates the network, if required.
     *
     */
    void listen_to_cluster(std::condition_variable *thread_halter, int *main_thread_lock);

    /**
     * @brief A debug function to print the activation of each firing neuron to std output.
     *
     */
    void print_activation();

    /**
     * @brief Returns the current step number of the network.
     *
     * @return    The current step of the network.
     *
     */
    int64_t get_step_count();

    /**
     * @brief Returns a neuron based on it's ID.
     *
     * @param neuron_id    The ID of the neuron to return.
     *
     */
    Neuron *get_neuron(int neuron_id);

    /**
     * @brief Checks if a certain neuron fired in this step.
     *
     * @param neuron_id    The ID of the neuron to check.
     *
     * @return             true if neuron fired, false if not.
     *
     */
    bool neuron_is_active(int neuron_id);

    /**
     * @brief Returns the activation of the neuron in this step.
     *
     * @param neuron_id    The ID of the neuron to get activation from.
     *
     * @return             Activation of given neuron.
     *
     */
    float get_neuron_activation(int neuron_id);

    /**
     * @brief Returns the current weight of a certain neurotransmitter.
     *
     * @param transmitter_id    The ID of the transmitter to get the weight from.
     *
     * @return                  The current transmitter weight.
     *
     */
    float get_transmitter_weight(int transmitter_id);

    void receive_data();

    private:
        std::vector<COGNA::Neuron*> _random_neurons;            // All neurons in the network which can activate randomly
        std::vector<float> _transmitter_weights;
        std::vector<COGNA::Connection*> _next_connections;      // All connections which will be activated in the next step
        int64_t _network_step_counter;
        int _id;
        static int m_max_id;

        /**
         * @brief Sets a new weight value to a certain neurotransmitter.
         *
         * @param transmitter_id    ID of the neurotransmitter to change
         * @param new_weight        The new weight to set the neurotransmitter
         *
         */
        void change_transmitter_weight(int transmitter_id,
                                       float new_weight);

        /**
         * @brief Calculates the change of a certain neurotransmitter if a certain neuron fires.
         *
         * Uses a dynamic gradient and the behavior parameters @c #transmitter_change_curvature
         * and @c #transmitter_change_steepness for calculations.
         *
         * @param n    The neuron which should influence the neurotransmitter
         *
         */
        void influence_transmitter(COGNA::Neuron *n);

        /**
         * @brief Calculates the slow backfall of neurotransmitter weight to 1 after it has been changed.
         *
         * Transmitter backfall uses the network parameters transmitter_backfall_curvature
         * und transmitter_backfall_steepness for backfall calculation.
         *
         */
        void transmitter_backfall();

        /**
         * @brief Decides for each neuron which can activate randomly if it should fire.
         *
         */
        void activate_random_neurons();

        /**
         * @brief Contains the basic learning of the connections and the logic if a neuron or a connection is activated.
         *
         */
        void activate_next_entities();

        /**
         * @brief Stores the connections of all activated neurons, if their activation is higher than their threshold in a vector.
         *
         */
        void save_next_neurons();

        /**
         * @brief Clears the vector containing the current connections and pushes the vector with the next connection to the current ones.
         *
         */
        void switch_vectors();

        void send_data();
};

} //namespace COGNA

#endif /* INCLUDE_NETWORK_HPP */
