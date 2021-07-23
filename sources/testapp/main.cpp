import std.core;
import testapp;
import potato.core;

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
        return -1;
    }

    std::flush(std::cout);

    return 0;
}
