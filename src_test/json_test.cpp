#include "json.hpp"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>

int main(const int argc, const char* const argv[]){
  std::cout << "Start of json test..." << "\n\n";

  std::cout << argv[0] << std::endl;

  std::ifstream i;
  i.open("Networks/Testfiles/test.json");
  nlohmann::json j;
  i >> j;
  i.close();

  for(auto it = j.begin(); it != j.end(); ++it){
    std::cout << "Key: " << it.key() << " " << it.value() << std::endl;
  }

  std::cout << "\n";

  nlohmann::json obj_test = j.at("object");
  for(auto it = obj_test.begin(); it != obj_test.end(); it++){
    std::cout << "Key: " << it.key() << " " << it.value() << std::endl;
  }

  std::cout << "\n";

  std::vector<int> list_test = j.at("list");
  for(uint i=0; i<list_test.size(); i++){
	std::cout << "List value is " << list_test[i] << std::endl;
  }

  std::cout << "\n";

  // C standard
  float pi = j.at("pi");
  printf("Pi is %f\n", pi);
  std::cout << "Pi is " << j.at("pi") << std::endl;

  std::string name_v = j.at("name");
  if(name_v == "Niels"){
    printf("The name is Niels\n");
  }
  std:: cout << "Name is " << j.at("name") << std::endl;

  std::cout << "\nEnd of json test...\n" << std::endl;
  return 0;
}
