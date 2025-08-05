#include <iostream>

int main(int args, char* argv[]){

    std::cout << "Kolvo args: " << args << '\n';

    for(int i = 0; i < args; i++){
        std::cout << "i = " << i << ' ' << argv[i] << '\n';
    }

}
