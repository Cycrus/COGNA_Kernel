/**
 * @file CognaBuilder.hpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief A class reading a COGNA project structure and creating all requirements to run the network cluster.
 *
 * The class which compiles a cogna network based on the json configuration exported from
 * the COGNA editor.
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
    /**
     * @brief Initializes the builder and loads a certain COGNA projet.
     *
     * @param project_name      The name of the project to load.
     */
    CognaBuilder(std::string project_name);

    /**
     * @brief Destructor. Currently empty.
     */
    ~CognaBuilder();

    /**
     * @brief Calls all functions sequentially required to compile a COGNA
     *        project.
     *
     * @return  Error code.
     */
    int build_cogna_cluster();

    /**
     * @brief Tests certain aspects of the compiled network.
     *
     * Mainly for debug purposes.
     */
    void tester();

    /**
     * @brief Getters for certain private member parameters.
     */
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
    std::vector<nlohmann::json> _presynaptic_connections;
    std::vector<std::string> _transmitter_types;
    std::string _project_name;
    std::string _project_path;
    std::string _main_network;
    int _curr_network_neuron_number;

    /**
     * @brief Loads the global configuration file into the network cluster.
     *
     * @return  Error code.
     */
    int load_globals_file();

    /**
     * @brief Loads the file describing the neuron types into the network cluster.
     *
     * @return  Error code.
     */
    int load_neuron_types();

    /**
     * @brief Loads the file describing the neurotransmitters into the network cluster.
     *
     * @return  Error code.
     */
    int load_transmitters();

    /**
     * @brief Loads all neurons of a certain network.
     *
     * @param nn            A pointer to the network the neurons should be added to.
     * @param network_json  The json where the neurons to add are described.
     *
     * @return              Error code.
     */
    int load_neurons(NeuralNetwork *nn, nlohmann::json network_json);

    /**
     * @brief Loads all input/output nodes of a certain network.
     *
     * @param nn            A pointer to the network the nodes should be added to.
     * @param network_json  The json where the nodes to add are described.
     *
     * @return              Error code.
     */
    int load_nodes(NeuralNetwork *nn, nlohmann::json network_json);

    /**
     * @brief Loads all parameters which can be defined after a connection is built.
     *
     * @param connection_object     A pointer to the connection the parameters should be stored in.
     * @param connection_json       The json where the connection parameters are stored in.
     *
     * @return                      Error code.
     */
    int load_all_connection_parameter(Connection *connection_object, nlohmann::json connection_json);

    /**
     * @brief Loads a connection to another neurons.
     *
     * @param nn            A pointer to the network the connection should be added to.
     * @param network_json  The json where the connection is contained in.
     *
     * @return              Error code.
     */
    int load_neuron_connection(NeuralNetwork *nn, nlohmann::json connection_json);

    /**
     * @brief Loads a connection to an interface nodes.
     *
     * @param nn            A pointer to the network the connection should be added to.
     * @param network_json  The json where the connection is contained in.
     *
     * @return              Error code.
     */
    int load_node_connection(NeuralNetwork *nn, nlohmann::json connection_json);

    /**
     * @brief Loads all connections to subnetwork related input nodes.
     *
     * Stores a connection in a temporary list in the network.
     * Is used in find_starting_points() and find_end_points().
     *
     * @param nn            A pointer to the network the connection should be added to.
     * @param network_json  The json where the connection is contained in.
     *
     * @return              Error code.
     */
    int load_subnet_input_connection(NeuralNetwork *nn, nlohmann::json connection_json,
                                     nlohmann::json network_json);

    /**
     * @brief Loads a connection to subnetwork related output nodes.
     *
     * Stores the connections in a temporary list in the network.
     * Is used in find_starting_points() and find_end_points().
     *
     * @param nn            A pointer to the network the connection should be added to.
     * @param network_json  The json where the connection is contained in.
     *
     * @return              Error code.
     */
    int load_subnet_output_connection(NeuralNetwork *nn, nlohmann::json connection_json,
                                      nlohmann::json network_json);

    /**
     * @brief Loads a connection to other connections.
     *
     * @param nn            A pointer to the network the connection should be added to.
     * @param network_json  The json where the connection is contained in.
     *
     * @return              Error code.
     */
    int load_presynaptic_connection(NeuralNetwork *nn, nlohmann::json connection_json);

    /**
     * @brief Loads all connections from a network json into a network.
     *
     * @param nn            A pointer to the network the connections should be added to.
     * @param network_json  The json where the connections are stored in.
     *
     * @return              Error code.
     */
    int load_connections(NeuralNetwork *nn, nlohmann::json network_json);

    /**
     * @brief Loads the network specific parameters into a compiled network.
     *
     * @param nn            A pointer to the network the parameters should be stored in.
     * @param network_json  The json where the network parameters are stored in.
     *
     * @return              Error code.
     */
    int load_network_parameter(NeuralNetwork *nn, nlohmann::json network_json);

    /**
     * @brief Calls all functions required to load a single network.
     *
     * @param network_name  The name of the network to load as a String.
     *
     * @return              Error code.
     */
    int load_network(std::string network_name);

    /**
     * @brief Based on the temporary subnet connection lists, finds the first valid neuron to connect
     *        to another neuron.
     *
     * Used for connecting different subnetworks and overlying networks.
     * The temporary list is defined in load_subnet_input_connection() and load_subnet_output_connection().
     *
     * @param source_network_id     The ID of the network where the search starts.
     * @param source_connection     The json describing the connection the search starts at.
     *
     * @return                      A list of all found valid starting neurons.
     */
    std::vector<nlohmann::json> find_starting_points(int source_network_id, nlohmann::json source_connection);

    /**
     * @brief Based on the temporary subnet connection lists, finds the last valid neuron to connect
     *        to another neuron.
     *
     * Used for connecting different subnetworks and overlying networks.
     * The temporary list is defined in load_subnet_input_connection() and load_subnet_output_connection().
     *
     * @param source_network_id     The ID of the network where the search starts.
     * @param source_connection     The json describing the connection the search starts at.
     *
     * @return                      A list of all found valid end neurons.
     */
    std::vector<nlohmann::json> find_end_points(int source_network_id, nlohmann::json source_connection);

    /**
     * @brief Compiles the connections based on the starting neurons and end neurons lists.
     *
     * @param starting_points   The list of all starting neurons.
     * @param end_points        The list of all end neurons.
     */
    void create_subnet_neuron_connections(std::vector<nlohmann::json> starting_points, std::vector<nlohmann::json> end_points);

    /**
     * @brief Compiles the presynaptic connections of the whole network cluster.
     *
     * @return  Error code.
     */
    int create_presynaptic_connections();

    /**
     * @brief Finds all starting neurons and end neurons of all subnetwork connection in a certain network
     *        and connects them afterwards.
     *
     * @param curr_network_id   The ID of the network where the search should happen.
     */
    void connect_subnet_endpoints_neurons(unsigned int curr_network_id);

    /**
     * @brief Finds all starting neurons and end connections of all subnetwork connection in a certain network
     *        and appends them to the list @_presynaptic_connections.
     *
     * @param curr_network_id   The ID of the network where the search should happen.
     */
    void find_synaptic_connection_endpoints(unsigned int curr_network_id);

    /**
     * @brief Calls the connect_subnet_endpoints() for all networks in the cluster.
     *
     * @return  Error code.
     */
    int connect_subnetworks();

    /**
     * @brief Loads a certain parameter used by neurons from a json.
     *
     * @param neuron        The json of the neuron which should be loaded.
     * @param parameter     The name of the parameter asked.
     * @param neuron_type   The type of the neuron to ask the parameter.
     *
     * @return              The parameter loaded from the json data.
     */
    float load_neuron_parameter(nlohmann::json neuron, std::string parameter, std::string neuron_type);

    /**
     * @brief Loads a parameter used by connections. Only used for parameters which can be loaded after connection creation.
     *
     * @param init_value    The standard value. Is returned if the parameter is not found in the json data.
     * @param parameter     The name of the parameter in question.
     * @param neuron_type   The neuron type of the neuron the connection stems from.
     *
     * @return              The parameter loaded from the json data.
     */
    float load_connection_parameter(float init_value, nlohmann::json connection, std::string parameter);
    /**
     * @brief Loads a certain parameter used by connections. Only used for parameters required to create a connection.
     *
     * @param nn                A pointer to the network where the connection shoud be placed in.
     * @param connection        The json which describes the connection.
     * @param parameter         The name of the parameter to load.
     * @param source_neuron_id  The ID of the neuron the connection to load stems from.
     *
     * @return                  The parameter loaded from the json data.
     */
    float load_connection_init_parameter(NeuralNetwork *nn, nlohmann::json connection, std::string parameter, int source_neuron_id);
};

} //namespace COGNA

#endif //INCLUDE_COGNABUILDER_HPP
