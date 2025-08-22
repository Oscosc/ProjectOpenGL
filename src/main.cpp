#include "Application.hpp"
#include "SceneParser.hpp"
#include "RayTracing.hpp"
#include "Logger.hpp"

int main(int argc, char* argv[]) {

    Logger::setColoredMode(true);
    Logger::setVerbosity(1);

    Application app;
    if(argc < 2) {
        Logger::logWarning("No scene file specified for this application, loading default scene");
        app.run("resources/scenes/default_scene.json");
    } else if (argc == 2) {
        app.run(argv[1]);
    } else {
        Logger::logError("Two many arguments provided, expected one : name of the scene file to load");
    }

    return 0;
}