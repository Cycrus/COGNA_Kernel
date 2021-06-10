#include "CognaBuilder.hpp"
#include "Constants.hpp"
#include "CognaLauncher.hpp"
#include <iostream>

int main(int argc, char** argv){
    if(argc != 2){
        std::cout << "[ERROR] Invalid amount of program parameters." << std::endl
                  << "Please run program as ./program <project_name>." << std::endl;
        return 1;
    }

    std::string network_name = argv[1];
    COGNA::CognaBuilder *cluster_builder = new COGNA::CognaBuilder(network_name);

    int error_code = cluster_builder->build_cogna_cluster();

    if(error_code == COGNA::ERROR_CODE){
        std::cout << "[ERROR] Building failed..." << std::endl;
        delete cluster_builder;
        return 1;
    }

    cluster_builder->tester();

    COGNA::CognaLauncher *cluster_launcher = new COGNA::CognaLauncher(cluster_builder->get_network_list(),
                                                                      cluster_builder->get_client_list(),
                                                                      cluster_builder->get_sender_list(),
                                                                      cluster_builder->get_frequency());

    delete cluster_builder;
    cluster_builder = nullptr;

    cluster_launcher->run_cogna();

    delete cluster_launcher;
    cluster_launcher = nullptr;
    return 0;
}
