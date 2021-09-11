/**
 * @file CognaLauncher.hpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief A class loading a compiled COGNA cluster, starting it and supervising it during runtime.
 *
 * @date 2021-05-31
 *
 */

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
    /**
     * Constructor. Initializes the compiled COGNA cluster.
     */
    CognaLauncher(std::vector<NeuralNetwork*> network_list,
                  std::vector<utils::networking_client*> client_list,
                  std::vector<utils::networking_sender*> sender_list,
                  int frequency);

    /**
     * Destructor. Frees all memory used by the networks in the cluster.
     */
    ~CognaLauncher();

    /**
     * @brief Tests certain aspects of the compiled network.
     *
     * Mainly for debug purposes.
     */
    void tester();

    /**
     * @brief Calls every function for running the COGNA cluster.
     *
     * @return  Error code.
     */
    int run_cogna();

private:
    std::vector<NeuralNetwork*> _network_list;
    std::vector<utils::networking_client*> _client_list;
    std::vector<utils::networking_sender*> _sender_list;
    std::vector<std::thread*> _client_worker_list;
    std::vector<std::thread*> _cogna_worker_list;
    int _frequency;
    unsigned long long *_curr_cluster_step;

    /**
     * @brief Creates all threads working on UDP networking.
     *
     * @return  Error code.
     */
    int create_networking_workers();

    /**
     * @brief Creates all threads working on different COGNA networks.
     *
     * @param thread_lock   A mutex condition_variable for locking a thread until it is allowed to run.
     *                      Used for synchronizing all threads.
     *
     * @return              Error code.
     */
    int create_cogna_workers(std::condition_variable *thread_lock);
};

} //namespace COGNA

#endif //INCLUDE_COGNALAUNCHER_HPP
