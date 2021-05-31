#include "CognaBuilder.hpp"

int main(){
    COGNA::CognaBuilder *test_builder = new COGNA::CognaBuilder("Interface_Test");

    test_builder->build_cogna_cluster();
    test_builder->tester();

    delete test_builder;
    return 0;
}
