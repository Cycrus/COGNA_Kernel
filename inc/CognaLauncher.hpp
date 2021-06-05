#ifndef INCLUDE_COGNALAUNCHER_HPP
#define INCLUDE_COGNALAUNCHER_HPP

#include "NeuralNetwork.hpp"
#include "networking_client.hpp"
#include "networking_sender.hpp"
#include <vector>
#include <thread>

namespace COGNA{

class CognaLauncher{
public:
    CognaLauncher(std::vector<NeuralNetwork*> network_list,
                  std::vector<utils::networking_client*> client_list,
                  std::vector<utils::networking_sender*> sender_list,
                  int frequency);
    ~CognaLauncher();

    void tester();

private:
    std::vector<NeuralNetwork*> _network_list;
    std::vector<utils::networking_client*> _client_list;
    std::vector<utils::networking_sender*> _sender_list;
    int _frequency;
};

} //namespace COGNA

#endif //INCLUDE_COGNALAUNCHER_HPP
