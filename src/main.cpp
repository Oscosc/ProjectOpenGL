#include "Application.hpp"
#include "Parser.hpp"

int main() {
    SceneParser::parseScene("resources/default_scene.json");

    Application app;
    app.run();

    return 0;
}