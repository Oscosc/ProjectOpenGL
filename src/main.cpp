#include "Application.hpp"
#include "SceneParser.hpp"

int main(int argc, char* argv[]) {

    Application app;
    if(argc < 2) {
        std::cout << "[WARNING] No scene file specified for this application, loading default scene" << std::endl;
        app.run("resources/default_scene.json");
    } else if (argc == 2) {
        app.run(argv[1]);
    } else {
        std::cout << "[ERROR] Two many arguments provided, expected one : name of the scene file to load" << std::endl;
    }

    return 0;
}