#include <iostream>
#include "ConsoleMainwindow.h"

int main(int argc, char *argv[])
{
    try {
        // check the input now without loading everything
        if (! checkFirstInput(argc, argv) ) return -1;

        ConsoleMainwindow w(argc, argv);
        return !w.run();

    } catch(...) {
        std::cerr << "ConsoleMain(): exception caught" << std::endl;
        return -1;
    }
}
