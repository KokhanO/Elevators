#include "Application.h"

#include "Scene.h"
#include "curses.h"

Application::Application(std::unique_ptr<Scene> scene) : scene(std::move(scene))
{
}

void Application::run()
{
    scene->Initialize();
    scene->Run();
    endwin();
}
