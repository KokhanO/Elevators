#include "Controller.h"
#include "Elevator.h"

Controller::Controller()
{
	trackRequestThread = std::thread(&Controller::TrackRequest, this);
}

Controller::~Controller()
{
	trackRequestThread.join();
}

void Controller::SetFloorCount(int floorCount)
{
	this->floorCount = floorCount;
}

int Controller::GetFloorCount() const
{
	return floorCount;
}

void Controller::AddElevators(std::size_t elevatorsCount)
{
	for (std::size_t i = 0; i < elevatorsCount; ++i)
		elevators.emplace_back(std::make_unique<Elevator>());
}

std::size_t Controller::GetElevatorsCount() const
{
	return elevators.size();
}

int Controller::GetElevatorPos(std::size_t elevatorId) const
{
	if(elevatorId < 0 || elevatorId >= GetElevatorsCount())
		return -1;

	return elevators[elevatorId]->GetCurrentFloor();
}

bool Controller::IsElevatorOpen(std::size_t elevatorId) const
{
	if (elevatorId < 0 || elevatorId >= GetElevatorsCount())
		return false;

	return elevators[elevatorId]->IsOpen();
}

void Controller::TrackRequest()
{
	std::unique_lock<std::mutex> lock(requestsMutex, std::defer_lock);
	while (true)
	{
		lock.lock();
		if (!requests.empty())
		{
			for (auto& elevator : elevators)
			{
				auto requestIt = requests.find(elevator->GetCurrentFloor());
				if (requestIt != requests.end())
				{
					for (auto& el : elevators)
						el->RemoveRequest(*requestIt);

					requests.erase(requestIt);
				}
			}
		}

		lock.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void Controller::RequestElevator(int floor)
{
	if (elevators.empty() || floor < 1 || floor > floorCount)
		return;

	requests.emplace(floor);

	for (auto& elevator : elevators)
		elevator->AddRequest(floor);
}

void Controller::AddDestination(int floor, std::size_t elevatorId)
{
	if (floor < 1 || floor > floorCount)
		return;

	if(elevatorId < elevators.size())
		elevators[elevatorId]->AddDestination(floor);
}

void Controller::StopElevator(std::size_t elevatorId)
{
	elevators[elevatorId]->Stop();
}
