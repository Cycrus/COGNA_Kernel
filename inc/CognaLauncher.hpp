#ifndef INCLUDE_COGNALAUNCHER_HPP
#define INCLUDE_COGNALAUNCHER_HPP

#include "NeuralNetwork.hpp"
#include "networking_client.hpp"
#include "networking_sender.hpp"
#include <vector>
#include <thread>
#include <condition_variable>

namespace COGNA{

class CognaLauncher{
public:
    CognaLauncher(std::vector<NeuralNetwork*> network_list,
                  std::vector<utils::networking_client*> client_list,
                  std::vector<utils::networking_sender*> sender_list,
                  int frequency);
    ~CognaLauncher();

    void tester();
    int run_cogna();

private:
    std::vector<NeuralNetwork*> _network_list;
    std::vector<utils::networking_client*> _client_list;
    std::vector<utils::networking_sender*> _sender_list;
    std::vector<std::thread*> _client_worker_list;
    std::vector<std::thread*> _cogna_worker_list;
    int _frequency;
    unsigned long long *_curr_cluster_step;

    int create_networking_workers();
    int create_cogna_workers(std::condition_variable *thread_lock, int *main_lock);
};

} //namespace COGNA

#endif //INCLUDE_COGNALAUNCHER_HPP
