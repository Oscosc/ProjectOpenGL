#include "Application.hpp"
#include "SceneParser.hpp"

int main() {

    Application app;
    app.run("resources/default_scene.json");

    return 0;
}