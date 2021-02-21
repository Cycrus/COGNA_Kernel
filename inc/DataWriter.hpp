/**
 * @file DataWriter.hpp
 * @author Cyril Marx (https://github.com/cycrus)
 *
 * @brief A class writing data into a csv file after every step/tick.
 *
 * Written csv contains all behavior parameter of network, neurons and
 * connections; neuron activations; etc.
 *
 * @date 2021-02-16
 *
 */

#ifndef INCLUDE_DATA_WRITER_HPP
#define INCLUDE_DATA_WRITER_HPP

#include <fstream>
#include <string>

namespace COGNA{
    class NeuralNetwork;

    /**
     * @brief Class for writing data into csv file.
     *
     */
    class DataWriter{
    public:
        std::ofstream _output;
        std::string filename;

        /**
         * @brief Initializes information required for writing.
         *
         * Creates filename derived from current datetime.
         */
        DataWriter();

        /**
         * @brief Closes file connection.
         *
         */
        ~DataWriter();

        /**
         * @brief Creates file and establishes file connection if possible.
         *
         * @return    Error code: SUCCESS_CODE if successful; ERROR_CODE if not successful
         *
         */
        int open_file();

        /**
         * @brief Writes head of csv file (all variable names)
         *
         */
        void write_head();

        /**
         * @brief Writes data of network into the csv.
         *
         */
        void write_data(COGNA::NeuralNetwork *nn);
    };
}

#endif /* INCLUDE_DATAWRITER_HPP */
