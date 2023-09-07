#pragma once
#include <vector>
#include <unordered_set>
#include <thread>
#include <mutex>

class Elevator;

class Controller
{
public:
	Controller();
	Controller(const Controller&) = delete;

	~Controller();

	void SetFloorCount(int floorCount);
	int GetFloorCount() const;

	void AddElevators(std::size_t elevatorsCount);
	std::size_t GetElevatorsCount() const;

	int GetElevatorPos(std::size_t elevatorId) const;
	bool IsElevatorOpen(std::size_t elevatorId) const;

	void TrackRequest();
	void RequestElevator(int floor);
	void AddDestination(int floor, std::size_t elevatorId);
	void StopElevator(std::size_t elevatorId);

private:
	int floorCount = 0;
	std::vector<std::unique_ptr<Elevator>> elevators;
	std::mutex requestsMutex;
	std::thread trackRequestThread;
	std::unordered_set<int> requests;
};

