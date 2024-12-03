#include <iostream>
using namespace std;

int main(){
    #ifdef KATHERINE
    std::cout << "Katherine" << endl;
    #else
    std::cout << "USER" << std::endl;
    #endif

    return 0;

}