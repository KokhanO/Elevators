#pragma once
#include <vector>
#include <unordered_set>
#include <thread>
#include <mutex>

class Elevator
{
public:
	Elevator();
	Elevator(int floor);
	Elevator(const Elevator&) = delete;

	~Elevator();

	bool IsMoving() const;
	bool IsUpDirection() const;
	bool IsOpen() const;
	int GetCurrentFloor() const;

	void AddDestination(int floor);
	void AddRequest(int floor);
	void RemoveRequest(int floor);
	void Stop();

private:
	void Start();
	void SortDestinations();
	void MoveUp();
	void MoveDown();
	void DelayThread();

	bool isMoving;
	bool isUpDirection;
	bool isOpen;
	bool stopFlag;
	int currentFloor;

	std::vector<int> destinations;
	std::unordered_set<int> requests;
	std::mutex destinationsMutex;
	std::thread delayThread;
};
