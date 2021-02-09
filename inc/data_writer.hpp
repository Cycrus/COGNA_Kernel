#ifndef INCLUDE_DATA_WRITER_HPP
#define INCLUDE_DATA_WRITER_HPP

#include <fstream>
#include <string>

#include "network.hpp"

class DataWriter{
public:
    std::ofstream _output;
    std::string filename;

    DataWriter();
    ~DataWriter();

    int open_file();
    void write_head();
    void write_data(NeuralNetwork *nn);
};

#endif /* INCLUDE_DATAWRITER_HPP */
