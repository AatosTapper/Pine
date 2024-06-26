#include "SceneParser.h"

int main() {
    auto scene = read_scene("../app/scenes/scene.xml");
    scene->print_tree();

    return 0;
}