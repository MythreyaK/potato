#include "test.hpp"

#include <chrono>
#include <graphics/render.hpp>

int main(int argc, char* argv[]) {

    try {
        testapp app { testapp(800, 600, std::string("Hello, World!")) };

        std::cout << app.required_extns().size() << '\n';

        app.window_loop();
    }
    catch ( std::exception e ) {
        std::cerr << "Exception " << e.what() << " was raised.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
