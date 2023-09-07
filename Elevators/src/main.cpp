#include "Controller.h"
#include "Scene.h"
#include "Application.h"

int main() {

    auto controller = std::make_unique<Controller>();
    auto scene = std::make_unique<Scene>(std::move(controller));
    Application app(std::move(scene));
    app.run();

    return 0;
}
