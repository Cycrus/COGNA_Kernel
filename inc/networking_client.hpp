/**
 * @file networking_client.hpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief A class responsible for receiving messages from an environment.
 *
 * It receives messages via UDP/IP and stores them in a hashtable.
 * Can return the full message as a string, a hashtable, or can return
 * only certain values of the message.
 *
 * The messages should be in json shape for full functionality of the class.
 *
 * @date 2021-05-27
 *
 */

#ifndef NETWORKING_CLIENT_HPP
#define NETWORKING_CLIENT_HPP

#include <string>
#include "json.hpp"
#include "client_server.hpp"

namespace utils{

class networking_client{
public:
	/**
	 * @brief Creates the UDP socket.
	 *
	 * @param ip		The ip of the message server.
	 * @param port		The port where the information is sent on.
	 * @param is_json	Determines if the received information is supposedly in json format.
	 *
	 */
	networking_client(std::string ip, int port, bool is_json);

	/**
	 * @brief Closes UDP socket.
	 *
	 */
	~networking_client();

	/**
	 * @brief Receives a message via UDP and stores it as a string.
	 *
	 * Should be called in its own worker thread, so that it can continuously receive messages.
	 */
	void receive_message();

	/**
	 * @brief Stores the message in a returnable variable.
	 *
	 * This function makes a snapshot of the incoming message stream. This snapshot can later be accessed to
	 * via different functions. To receive the latest message, this function must be called.
	 */
	void store_message();

	/**
	 * @brief Returns the last received message as string.
	 *
	 * @param indent	The indent of the returned string. Important for formatting.
	 *					-1 returns most compact version of string. Only works if message is in json format.
	 *
	 * @return The complete last message received.
	 */
	std::string get_message(int indent=-1);

	/**
	 * @brief Returns a certain value of the message, if it is coded as json.
	 *
	 * @param key	The key of the value in the json.
	 *
	 * @return		The supposed value as a json. 0 if no value could be extracted for some reason.
	 */
	nlohmann::json get_json_value(std::string key);

	/**
	 * @brief Returns the complete json hashtable.
	 *
	 * @return	The complete stored json hashtable, if it exists.
	 */
	nlohmann::json get_hashtable();

private:
	std::string _msg;
	std::string _stored_message;
	udp_client_server::udp_server *_receiver;
	nlohmann::json _hashtable;
	bool _is_json;
};

} //namespace utils

#endif //NETWORKING_CLIENT_HPP
