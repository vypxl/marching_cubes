#include <iostream>

#include "mcubes.hpp"

int main() {
    MCubes app;
    if (app.init() != 0) {
        std::cerr << "Failed to initialize" << std::endl;
        return -1;
    };
    app.loop();
    return 0;
}
