/**
 * @file NetworkingNode.hpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief The class representing the COGNA networking input/output node.
 *
 * A node can either be listening on a UDP port or send to a UDP ip/port.
 * What it does is determined by one of two function calls.
 *
 * @date 2021-05-31
 *
 */

#ifndef INCLUDE_NETWORKINGNODE_HPP
#define INCLUDE_NETWORKINGNODE_HPP

#include "Neuron.hpp"
#include <vector>
#include <string>
#include "networking_client.hpp"
#include "networking_sender.hpp"

namespace COGNA{

class NetworkingNode{
public:
    utils::networking_client *_client;
    utils::networking_sender *_sender;

    /**
     * @brief Constructor. Initializes a networking input/output node.
     *
     * @param id        The ID of the networking node.
     * @param channel   The channel the node listens to or sends on.
     */
    NetworkingNode(int id, std::string channel);

    /**
     * @brief Destructor. Empty.
     */
    ~NetworkingNode();

    /**
     * @brief Adds a target neuron the node either listens to or sends to.
     *
     * @param target    The target neuron of the node.
     *
     * @return          Error code.
     */
    int add_target(Neuron *target);

    /**
     * @brief Adds a target node the node either listens to or sends to.
     *
     * @param target    The target node of the node.
     *
     * @return          Error code.
     */
    int add_target(NetworkingNode *target);

    /**
     * @brief Sets up a client for the node. Turns the node into an input node listens to a port.
     *
     * @param client    A pointer to the UDP client of the node.
     *
     * @return          Error code.
     */
    int setup_client(utils::networking_client *client);

    /**
     * @brief Sets up a sender for the node. Turns the node into an output node sendint to an ip/port.
     *
     * @param sender    A pointer to the UDP sender of the node.
     *
     * @return          Error code.
     */
    int setup_sender(utils::networking_sender *sender);

    /**
     * @brief Directly forwards the information of this (input) node to another output node if connected.
     *
     * @param injected_activation   The activation which should be forwarded.
     */
    void remote_activate_senders(float injected_activation);

    /**
     * @brief Getters for certain private member variables.
     */
    int id();
    int role();
    std::string channel();
    std::vector<Neuron*> targets();

private:
    int _id;
    int _role;
    std::string _channel;
    std::vector<Neuron*> _target_list;
    std::vector<NetworkingNode*> _output_target_list;
};

} //namespace COGNA

#endif //INCLUDE_NETWORKINGNODE_HPP
