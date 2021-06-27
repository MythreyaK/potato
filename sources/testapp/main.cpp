#include <iostream>
#include <testapp/src/test.hpp>
#include <utils/utils.hpp>

int main() {

    using namespace testapp;
    namespace pu = potato::utils;

    try {
        app helloapp { 800,
                       600,
                       std::string("Hello, World!"),
                       { { 125, 125, pu::read_file("icon.bin") } } };
        helloapp.run();
    }
    catch ( std::exception e ) {
        std::cerr << "Exception: " << e.what() << '\n';
        std::flush(std::cout);
        return EXIT_FAILURE;
    }

    std::flush(std::cout);

    return EXIT_SUCCESS;
}
