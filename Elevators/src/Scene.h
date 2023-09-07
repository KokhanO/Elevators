#pragma once
#include <memory>

class Controller;

class Scene
{
private:
	static constexpr const char rowStr = '-';
	static constexpr const char columnStr = '|';
	static constexpr const char* closedElevatorStr = "[====]";
	static constexpr const char* openedElevatorStr = "[    ]";
	static constexpr const char* stopStr = "STOP";
	static constexpr const char* chooseFloorsNumStr = "Enter the number of the floors (more than zero and less than 15): ";
	static constexpr const char* chooseElevatorsNumStr = "Enter the number of the elevators (more than zero and less than 6): ";

public:
	Scene(std::unique_ptr<Controller> controller);
	void Initialize();
	void Run();

	~Scene();

private:

	void ChooseFloorsCount();
	void ChooseElevatorsCount();

	void DrawBackground();
	void DrawElevators();
	void DrawMenu();
	void DrawPanel(int panelId);

	void HandleInput();

	int floors;
	int elevators;
	int selectedFloor;
	int panelsCount;
	int selectedPanel;
	std::unique_ptr<Controller> controller;
};
