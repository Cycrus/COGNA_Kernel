#include <vector>
#include <iostream>

class TestClass{
public:
    TestClass(int var){
        _var = var;
    }

    int _var;
};

int main(){
    std::vector<TestClass*> test_vec;
    TestClass *test1 = new TestClass(5);
    test_vec.push_back(test1);
    std::cout << "Address of var " << test1->_var << " is " << test1 << std::endl;
    std::cout << "Address of vec " << test_vec[0]->_var << " is " << test_vec[0] << std::endl;

    return 0;
}
