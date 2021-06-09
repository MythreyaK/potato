#include <iostream>
#include <testapp/src/test.hpp>

int main() {

    using namespace testpotato;

    try {
        testapp app { 800,
                      600,
                      std::string("Hello, World!"),
                      { { 125, 125, read_icon("icon.bin") } } };
        app.run();
    }
    catch ( std::exception e ) {
        std::cerr << "Exception: " << e.what() << '\n';
        std::flush(std::cout);
        return EXIT_FAILURE;
    }

    std::flush(std::cout);

    return EXIT_SUCCESS;
}
