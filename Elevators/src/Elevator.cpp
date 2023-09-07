#include "Elevator.h"
#include <chrono>
#include <algorithm>

Elevator::Elevator() : isMoving(false), isUpDirection(true), isOpen(false), currentFloor(1)
{
    delayThread = std::thread(&Elevator::DelayThread, this);
}

Elevator::Elevator(int floor) : isMoving(false), isUpDirection(true), isOpen(false), currentFloor(floor)
{
    delayThread = std::thread(&Elevator::DelayThread, this);
}

Elevator::~Elevator()
{
    delayThread.join();
}

bool Elevator::IsMoving() const
{
	return isMoving;
}

bool Elevator::IsUpDirection() const
{
	return isUpDirection;
}

bool Elevator::IsOpen() const
{
    return isOpen;
}

int Elevator::GetCurrentFloor() const
{
	return currentFloor;
}

void Elevator::AddDestination(int floor)
{
    std::unique_lock<std::mutex> lock(destinationsMutex);

    if (!isMoving)
    {
	    destinations.push_back(floor);
    }
    else
    {
        if (isUpDirection)
        {
            if (floor >= currentFloor)
            {
                // adds floor before the bigger one
                auto it = std::upper_bound(destinations.begin(), destinations.end(), floor);
                destinations.insert(it, floor);
            }
            else
            {
                // adds floor after bigger one from the end
                auto it = std::upper_bound(destinations.rbegin(), destinations.rend(), floor); 
                destinations.insert(it.base(), floor);
            }
        }
        else
        {
            if(floor <= currentFloor)
            {
                // adds floor before first number smaller or equal one
                auto it = std::find_if(destinations.begin(), destinations.end(), [floor](int x){ return x < floor; });
                destinations.insert(it, floor);
            }
            else
            {
                // adds floor before the bigger one from the end
                auto it = std::lower_bound(destinations.rbegin(), destinations.rend(), floor, std::greater<int>());
                destinations.insert(it.base(), floor);
            }
        }
    }

    lock.unlock();
}

void Elevator::AddRequest(int floor)
{
    std::unique_lock<std::mutex> lock(destinationsMutex);
    requests.emplace(floor);
    lock.unlock();

    AddDestination(floor);
}

void Elevator::RemoveRequest(int floor)
{
    std::unique_lock<std::mutex> lock(destinationsMutex);
    requests.erase(floor);
    if (currentFloor != floor)
    {
        auto it = std::find(destinations.begin(), destinations.end(), floor);
        if(it != destinations.end())
            destinations.erase(it);
    }

    lock.unlock();
}

void Elevator::Stop()
{
    std::unique_lock<std::mutex> lock(destinationsMutex);
    destinations.clear();
    isOpen = true;
    stopFlag = true;
    for (auto request : requests)
        destinations.push_back(request);

    lock.unlock();
}

void Elevator::Start()
{
	SortDestinations();
    stopFlag = false;

    std::unique_lock<std::mutex> lock(destinationsMutex, std::defer_lock);
    while (true)
    {
        lock.lock();
        if (stopFlag || destinations.empty()) {
            lock.unlock();
            isMoving = false;
            break;
        }

        isMoving = true;
        while (!destinations.empty() && !stopFlag && currentFloor != destinations.front()) 
        {
            if (currentFloor < destinations.front())
                MoveUp();
            else
                MoveDown();

            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            lock.lock();
        }

        if (!destinations.empty()) 
        {
            lock.unlock();

            isOpen = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            isOpen = false;

            lock.lock();
            while (!destinations.empty() && destinations.front() == currentFloor)
                destinations.erase(destinations.begin());

            requests.erase(currentFloor);
        }

        lock.unlock();
    }
}

void Elevator::SortDestinations()
{
    std::unique_lock<std::mutex> lock(destinationsMutex);

	destinations.push_back(currentFloor);
	std::sort(destinations.begin(), destinations.end());
	auto currentFloorIt = std::find(destinations.begin(), destinations.end(), currentFloor);

	int distToMinFloor = abs(currentFloor - destinations.front());
	int distToMaxFloor = abs(currentFloor - destinations.back());

	int startId = distToMinFloor < distToMaxFloor ? 0 : destinations.size() - 1;

	if (destinations[startId] <= currentFloor)
		std::reverse(destinations.begin(), currentFloorIt + 1);
	else
	{
		auto it = std::rotate(destinations.begin(), currentFloorIt, destinations.end());
		std::reverse(it, destinations.end());
	}

    destinations.erase(destinations.begin());

    lock.unlock();
}

void Elevator::MoveUp()
{
    isUpDirection = true;
	currentFloor += 1;
}

void Elevator::MoveDown()
{
    isUpDirection = false;
	currentFloor -= 1;
}

void Elevator::DelayThread()
{
    std::unique_lock<std::mutex> lock(destinationsMutex, std::defer_lock);
    while (true)
    {
        lock.lock();
        if (!requests.empty())
        {
            lock.unlock();
            isOpen = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            Start();
        }
        else if(!destinations.empty())
        {
            lock.unlock();
            isOpen = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            isOpen = false;
            Start();
        }
        else
        {
            lock.unlock();
            isOpen = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}
