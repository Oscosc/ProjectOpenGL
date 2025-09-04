#include <ProjectIGAI/core/Application.hpp>
#include <ProjectIGAI/core/SceneParser.hpp>
#include <ProjectIGAI/raytracing/RayTracing.hpp>
#include <ProjectIGAI/core/Logger.hpp>


int main(int argc, char* argv[]) {

    Logger::setColoredMode(true);
    Logger::setVerbosity(MAX_LOG_LEVEL);

    Application app(800, 600);
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
