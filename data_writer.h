#ifndef DATA_WRITER
#define DATA_WRITER

#include <fstream>
#include <string>
#include <ctime>
#include <stdint.h>
#include <iostream>

#include "network.h"

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

#endif
