#include "CognaBuilder.hpp"
#include "Constants.hpp"
#include <iostream>

int main(){
    COGNA::CognaBuilder *test_builder = new COGNA::CognaBuilder("Interface_Test");

    int error_code = test_builder->build_cogna_cluster();

    if(error_code == COGNA::ERROR_CODE){
        std::cout << "[ERROR] Building failed..." << std::endl;
        return 1;
    }

    test_builder->tester();

    delete test_builder;
    return 0;
}
