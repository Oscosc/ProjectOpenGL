#include "Application.hpp"
#include "SceneParser.hpp"

int main() {
    SceneParser myParser;
    myParser.parseScene("resources/default.scene");

    Application app;
    app.run();

    return 0;
}