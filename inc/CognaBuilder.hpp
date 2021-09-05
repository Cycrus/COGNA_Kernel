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
#include "networking_client.hpp"
#include "networking_sender.hpp"

namespace COGNA{

class CognaBuilder{
public:
    CognaBuilder(std::string project_name);
    ~CognaBuilder();

    int build_cogna_cluster();

    void tester();

    std::vector<NeuralNetwork*> get_network_list();
    std::vector<utils::networking_client*> get_client_list();
    std::vector<utils::networking_sender*> get_sender_list();
    int get_frequency();

private:
    std::vector<NeuralNetwork*> _network_list;
    std::vector<utils::networking_client*> _client_list;
    std::vector<utils::networking_sender*> _sender_list;
    int _frequency;

    nlohmann::json _neuron_types;
    std::vector<std::string> _transmitter_types;
    std::string _project_name;
    std::string _project_path;
    std::string _main_network;
    int _curr_network_neuron_number;

    int load_globals_file();
    int load_neuron_types();
    int load_transmitters();

    int load_neurons(NeuralNetwork *nn, nlohmann::json network_json);
    int load_nodes(NeuralNetwork *nn, nlohmann::json network_json);
    int load_all_connection_parameter(Connection *connection_object, nlohmann::json connection_json);
    int load_neuron_connection(NeuralNetwork *nn, nlohmann::json network_json, unsigned int i);
    int load_node_connection(NeuralNetwork *nn, nlohmann::json network_json, unsigned int i);
    int load_subnet_input_connection(NeuralNetwork *nn, nlohmann::json network_json, unsigned int i);
    int load_subnet_output_connection(NeuralNetwork *nn, nlohmann::json network_json, unsigned int i);
    int load_presynaptic_connection(NeuralNetwork *nn, nlohmann::json network_json, unsigned int i);
    int load_connections(NeuralNetwork *nn, nlohmann::json network_json);
    int load_network_parameter(NeuralNetwork *nn, nlohmann::json network_json);
    int load_network(std::string network_name);
    std::vector<nlohmann::json> find_end_points(int source_network_id, int curr_network_id, std::vector<nlohmann::json> source_connections);
    void connect_subnet_endpoints(unsigned int curr_network_id);
    int connect_subnet_outputs(unsigned int curr_network_id, NeuralNetwork *source_network);
    int connect_subnet_inputs(unsigned int curr_network_id, NeuralNetwork *source_network);
    int connect_subnetworks();

    float load_neuron_parameter(nlohmann::json neuron, std::string parameter, std::string neuron_type);
    float load_connection_parameter(float init_value, nlohmann::json connection, std::string parameter);
    float load_connection_init_parameter(NeuralNetwork * nn, nlohmann::json connection, std::string parameter, int source_neuron_id);
};

} //namespace COGNA

#endif //INCLUDE_COGNABUILDER_HPP
