/**
 * @file Neuron.hpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief A class representing the neurons in the network.
 *
 * Every neuron contains all connections originating from it.
 * Neurons can activate other neurons via connections.
 * They can be set to fire autonomously at random intervals and influence
 * neurotransmitters when they fire.
 *
 * @date 2021-02-16
 *
 */

#ifndef INCLUDE_NEURON_HPP
#define INCLUDE_NEURON_HPP

#include <vector>
#include <cstdint>
#include "NeuronParameterHandler.hpp"
#include "NeuralNetworkParameterHandler.hpp"
#include "Connection.hpp"

namespace COGNA{
	class Neuron{
		public:
	        int _id;
	        static int s_max_id;
	        bool _random_activation;               /**< Determines if neuron sometimes fires randomly */
	        int _random_chance;                    /**< Chance of random activation per tick */
	        float _random_activation_value;        /**< How strong is the neuron activated when randomly activated */
	        float _activation;                     /**< Current activation potential level */
	        float _temp_activation;                /**< A temporary variable to calculate activation of next neurons */
	        int64_t _last_activated_step;          /**< Network step count, when neuron was last activated */
	        bool _was_activated;                   /**< Indicates if neuron was activated last time or this time */
	        int _last_fired_step;                  /**< Step when neuron last fired */

	        COGNA::NeuronParameterHandler *_parameter;

	        std::vector<COGNA::Connection*> _connections;
			std::vector<COGNA::Neuron*> _previous;

			/**
			 * @brief Initializes a neuron by setting its parameters to the default parameters coming from the network.
			 *
			 */
	        Neuron(NeuralNetworkParameterHandler *default_parameter);

			/**
			 * @brief Frees all memory allocated by the neuron.
			 *
			 */
	        ~Neuron();

			/**
			 * @brief Sets the neurons last_activated_step to the current network step number.
			 *
			 */
	        void set_step(long new_step);

			/**
			 * @brief Checks if this neuron is connected with another neuron.
			 *
			 * @param n    The neuron with which the connection should be checked.
			 *
			 * @return     true if neurons are connected, false if not.
			 *
			 */
	        bool check_neuron_connection(COGNA::Neuron *n);

			/**
			 * @brief Checks if this neuron is connected with another connection.
			 *
			 * @param con    The connection with which the connection should be checked.
			 *
			 * Can currently only check for synaptic connection, if the connection con
			 * is a connection between two neurons.
			 *
			 * @return     true if neuron is connected to the connection, false if not.
			 *
			 */
	        bool check_synaptic_connection(COGNA::Connection *con);

			/**
			 * @brief Adds a connection to another neuron in the network to this neuron.
			 *
			 * A neuron connection transmits activation potential to activate other neurons.
			 *
			 * @param n                   The neuron with which this neuron should be connected.
			 * @param weight              The default base weight of the new connection.
			 * @param con_type            The type of connection. Can be EXCITATORY or INHIBITORY.
			 * @param fun_type            The activation function of the connection.
			 * @param learn_type          Decides if connection can learn via habituation, sensitization, or both.
			 * @param transmitter_type    The transmitter the new connection uses for signal transmission.
			 *
			 */
	        Connection* add_neuron_connection(COGNA::Neuron *n,
											  float weight,
	                                          int con_type=EXCITATORY,
	                                          int fun_type=FUNCTION_RELU,
	                                          int learn_type=LEARNING_NONE,
	                                          int transmitter_type=STD_TRANSMITTER);

		     /**
			 * @brief Adds a connection to another connection in the network to this neuron.
			 *
			 * A synaptic connection transmits presynaptic potential to modulate the learning
			 * behavior of another connection which just fired a short time before.
			 *
			 * @param con                 The connection with which this neuron should be connected.
			 * @param weight              The default base weight of the new connection.
			 * @param con_type            The type of connection. Can be EXCITATORY or INHIBITORY.
			 * @param fun_type            The activation function of the connection.
			 * @param learn_type          Decides if connection can learn via habituation, sensitization, or both.
			 * @param transmitter_type    The transmitter the new connection uses for signal transmission.
			 *
			 */
	        Connection* add_synaptic_connection(COGNA::Connection *con,
												float weight,
	                                            int con_type=EXCITATORY,
	                                            int fun_type=FUNCTION_RELU,
	                                            int learn_type=LEARNING_NONE,
	                                            int transmitter_type=STD_TRANSMITTER);

			/**
			 * @brief Deletes a connection from this neuron.
			 *
			 * Not finished yet. Does not remove connections pointing to this connection.
			 * It is also not possible to delete presynaptic connections.
			 *
			 * @param n    The target neuron of the connection to delete. Acts as identification of the connection to delete.
			 *
			 */
	        void del_connection(COGNA::Neuron *n);

			/**
			 * @brief Checks if this neuron fired in the current step or not.
			 *
			 * @return    true if the neuron fired, false if it did not
			 *
			 */
	        bool is_active();

			/**
			 * @brief Sets the neuron to autonomously fire at random intervals.
			 *
			 * @param chance              The chance of firing in each step. 1 = 1/100000000.
			 * @param activation_value    The activation with which the neuron randomly fires
			 *
			 */
	        void set_random_activation(int chance, float activation_value);

			/**
			 * @brief Calculates the backfall of neuron activation when neuron does not fire.
			 *
			 * Only calculates a backfall, if the activation level of the neuron
			 * is lower than its threshold, but larger than 0.
			 *
			 * @param network_step    The current step/tick count of the network
			 *
			 */
			void calculate_neuron_backfall(int64_t network_step);

			/**
			 * @param Sets the activation level of the neuron to 0, if it did fire
			 *
			 * @param network_step    The current step/tick count of the network
			 *
			 */
            void clear_neuron_activation(int64_t network_step);
	};
}

#endif /* INCLUDE_NEURON_HPP */
