#pragma once
#include <memory>

class Scene;

class Application
{
public:

    Application(std::unique_ptr<Scene> scene);

    void run();

private:
    std::unique_ptr<Scene> scene;
};
