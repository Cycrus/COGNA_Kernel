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

    NetworkingNode(int id, std::string channel);
    ~NetworkingNode();

    int add_target(Neuron *target);
    int add_target(NetworkingNode *target);
    int setup_client(utils::networking_client *client);
    int setup_sender(utils::networking_sender *sender);

    void remote_activate_senders(float injected_activation);

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
