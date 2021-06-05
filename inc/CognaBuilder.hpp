/**
 * @file CognaBuilder.hpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief A class reading a COGNA project structure and creating all requirements to run the network cluster.
 *
 *
 *
 * @date 2021-05-31
 *
 */

#ifndef INCLUDE_COGNABUILDER_HPP
#define INCLUDE_COGNABUILDER_HPP

#include <string>
#include <vector>
#include "NeuralNetwork.hpp"
#include "NeuronParameterHandler.hpp"
#include "json.hpp"

namespace COGNA{

class CognaBuilder{
public:
    CognaBuilder(std::string project_name);
    ~CognaBuilder();

    int build_cogna_cluster();

    void tester();

private:
    std::vector<NeuralNetwork*> _network_list;
    nlohmann::json _neuron_types;
    std::vector<std::string> _transmitter_types;
    std::string _project_name;
    std::string _project_path;
    std::string _main_network;
    int _frequency;

    int load_globals_file();
    int load_neuron_types();
    int load_transmitters();

    int load_neurons(NeuralNetwork *nn, nlohmann::json network_json);
    int load_network_parameter(NeuralNetwork *nn, nlohmann::json network_json);
    int load_network(std::string network_name);

    float load_neuron_parameter(nlohmann::json neuron, std::string parameter, std::string neuron_type);
};

} //namespace COGNA

#endif //INCLUDE_COGNABUILDER_HPP
