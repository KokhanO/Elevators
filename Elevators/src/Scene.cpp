#include "Scene.h"
#include <iostream>
#include "curses.h"
#include "Controller.h"

Scene::Scene(std::unique_ptr<Controller> controller) 
    : controller(std::move(controller)), 
        floors(0), elevators(0), selectedFloor(1),
        panelsCount(1), selectedPanel(1) 
{

}

void Scene::Initialize()
{
    ChooseFloorsCount();
    ChooseElevatorsCount();

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
}

void Scene::ChooseFloorsCount()
{
    
    while (floors < 1 || floors > 14) 
    {
        std::cout << chooseFloorsNumStr; 
        std::cin >> floors;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    controller->SetFloorCount(floors);
}

void Scene::ChooseElevatorsCount()
{
    while (elevators < 1 || elevators > 5)
    {
        std::cout << chooseElevatorsNumStr;
        std::cin >> elevators;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    controller->AddElevators((std::size_t)elevators);
    panelsCount = controller->GetElevatorsCount() + 1;
}

void Scene::DrawBackground()
{
    // Drawing walls
    for (int i = 0; i < LINES - 1; i++) 
    {
        mvaddch(i, 0, columnStr);
        mvaddch(i, COLS / 2 - 1, columnStr);
    }
    for (int i = 0; i < COLS / 2; i++) 
    {
        mvaddch(0, i, rowStr);
        mvaddch(LINES - 2, i, rowStr);
    }

    // Drawing floors separator
    for (int i = 0; i < controller->GetFloorCount() - 1; i++) 
    {
        for (int j = 1; j < COLS / 2 - 1; j++) 
        {
            mvaddch(LINES - (i + 2) * 2, j, rowStr);
        }
    }

    // Numerating floors
    for (int i = 1; i <= floors; ++i)
    {
        mvprintw(LINES - 2*i -1, COLS / 2, "%d", i);
    }
}

void Scene::DrawElevators()
{
    for (std::size_t i = 0; i < controller->GetElevatorsCount(); ++i) 
    {
        int elevatorLocationY = LINES - controller->GetElevatorPos(i) * 2 - 1;
        int elevatorLocationX = 10 + i * 10;
        if(controller->IsElevatorOpen(i))
            mvaddstr(elevatorLocationY, elevatorLocationX, openedElevatorStr);
        else
            mvaddstr(elevatorLocationY, elevatorLocationX, closedElevatorStr);
    }
}

void Scene::DrawMenu()
{
    for (int i = 1; i <= panelsCount; ++i)
        DrawPanel(i);
}

void Scene::Run()
{
    while (true) {
        clear();
        DrawBackground();

        DrawMenu();

        DrawElevators();

        HandleInput();
        refresh();
    }
}

Scene::~Scene()
{
}

void Scene::DrawPanel(int panelId)
{
    int panelLocationX = COLS - (panelsCount - (panelId - 1)) * 5;
    
    for (int i = 1; i <= floors; ++i)
    {
        if (panelId == selectedPanel && i == selectedFloor)
            attron(A_STANDOUT);

        int panelLocationY = floors - i + 1;
        mvprintw(panelLocationY, panelLocationX, "%d", i);

        if (panelId == selectedPanel && i == selectedFloor)
            attroff(A_STANDOUT);
    }

    if (panelId != 1)
    {
        if (panelId == selectedPanel && selectedFloor == 0)
            attron(A_STANDOUT);

        int panelLocationY = floors + 2;
        mvprintw(panelLocationY, panelLocationX, stopStr);

        if (panelId == selectedPanel && selectedFloor == 0)
            attroff(A_STANDOUT);
    }
}

void Scene::HandleInput()
{
    nodelay(stdscr, true);
    halfdelay(1);
    int ch = getch();
    if (ch == KEY_LEFT && selectedPanel > 1) {
        selectedPanel--;
        selectedFloor = 1;
    }
    else if (ch == KEY_RIGHT && selectedPanel < panelsCount) {
        selectedPanel++;
        selectedFloor = 1;
    }
    else if (ch == KEY_UP && selectedFloor < floors) {
        selectedFloor++;
    }
    else if (ch == KEY_DOWN) {
        if (selectedFloor > 1)
            selectedFloor--;
        else if (selectedPanel > 1 && selectedFloor > 0)
            selectedFloor--;
    }
    else if (ch == 10) {
        if (selectedPanel == 1)
            controller->RequestElevator(selectedFloor);
        else if (selectedFloor == 0)
            controller->StopElevator(selectedPanel - 2);
        else
            controller->AddDestination(selectedFloor, selectedPanel - 2);
    }
}

