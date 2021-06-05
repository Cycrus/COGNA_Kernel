/**
 * @file networking_sender.hpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief A class responsible for sending messages to the behavioral system of an agent.
 *
 * It stores information in a json shaped hashtable and forwards everything at once when
 * called.
 *
 * The messages sent are always in json shape.
 *
 * @date 2021-05-27
 *
 */

#ifndef NETWORKING_SENDER_HPP
#define NETWORKING_SENDER_HPP

#include <string>
#include "json.hpp"
#include "client_server.hpp"

namespace utils{

class networking_sender{
public:
	/**
	 * @brief Creates the UDP socket.
	 *
	 * @param ip		The ip of the message server.
	 * @param port		The port where the information is sent on.
	 *
	 */
	networking_sender(std::string ip, int port);

	/**
	 * @brief Closes UDP socket.
	 *
	 */
	~networking_sender();

	/**
	 * @brief Returns the given ip address of the socket.
	 *
	 * @return The ip address as string.
	 */
	std::string get_ip();

	/**
	 * @brief Returns the given port of the socket.
	 *
	 * @return The port as integer.
	 */
	int get_port();

	/**
	 * @brief Adds data to the json payload.
	 *
	 * Does not yet send the data. It gets only stored until deleted or send_payload() is called.
	 *
	 * @param key	The key which should indicate the value in the json data.
	 * @param value	The value of the new data inserted in the json. Adapts automatically to the type of the value.
	 *
	 */
	void add_data(std::string key, float value);
	void add_data(std::string key, int value);
	void add_data(std::string key, std::string value);

	/**
	 * @brief Removes a single key-value pair from the json, if it exists.
	 *
	 * @param key	The key of the value to be deleted.
	 *
	 */
	void remove_data(std::string key);

	/**
	 * @brief Clears every data from the json payload.
	 *
	 */
	void clear_payload();

	/**
	 * @brief Returns the complete payload ad a string
	 *
	 * @param indent	The indent of the returned string. Important for formatting. -1 returns most compact version of string.
	 *
	 * @return string	The stringified json payload.
	 *
	 */
	std::string stringify_payload(int indent=-1);

	/**
	 * @brief Sends the whole payload at once to the designated ip and port.
	 *
	 * The json payload sent is cleared afterwards.
	 *
	 */
	void send_payload();

private:
	udp_client_server::udp_client *_sender;
	nlohmann::json _payload;
};

} //namespace utils

#endif //NETWORKING_SENDER_HPP
