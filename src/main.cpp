#include "Application.hpp"
#include <iostream>

int main() {
    try {
        Application app("Fluxborn Prototype", 1600, 900);
        app.Run();
    } catch (const std::exception& e) {
        std::cerr << "Critical Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
