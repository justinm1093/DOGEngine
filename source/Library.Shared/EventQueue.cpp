
#include "pch.h"
#include "EventQueue.h"

using namespace std::chrono;
using namespace DOGEngine;
using namespace std;

EventQueue::EventQueue() :
	mEvents()
{
}

//-----------------------------------------------------------------

EventQueue::EventQueue(const EventQueue& other) :
	mEvents(other.mEvents)
{
}

//-----------------------------------------------------------------

EventQueue& EventQueue::operator=(const EventQueue& other)
{
	if(this != &other)
	{
		mEvents = other.mEvents;
	}

	return *this;
}

//-----------------------------------------------------------------

EventQueue::EventQueue(EventQueue&& other) :
	mEvents(std::move(other.mEvents))
{
}

//-----------------------------------------------------------------

EventQueue& EventQueue::operator=(EventQueue&& other)
{
	if(this != &other)
	{
		mEvents = std::move(other.mEvents);
	}

	return *this;
}

//-----------------------------------------------------------------

EventQueue::~EventQueue()
{
}

//-----------------------------------------------------------------

void EventQueue::update(const GameTime& gameTime)
{
	// move all expired events to a temporary queue
	Events tempEvents;

	{
		// temporarily lock the queue while we partition it
		lock_guard<mutex> lock(mMutex);

		uint32_t last = mEvents.size();
		for(uint32_t start = 0; start < last; ++start)
		{
			while(start != last && mEvents[start]->isExpired(gameTime))
			{
				mEvents[start].swap(mEvents[--last]);
				tempEvents.pushBack(mEvents[last]);
				mEvents.popBack();
			}
		}
	}

	// spin up async calls for each expired event in our list
	vector<future<void>> futures;
	for(auto& publisher : tempEvents)
	{
		futures.emplace_back(async(&EventPublisher::deliver, publisher));
	}

	// wait for all threads to return
	for(future<void>& future : futures)
	{
		future.wait();
	}
}

//-----------------------------------------------------------------

void EventQueue::enqueue(EventPublisher& publisher, const GameTime& gameTime, milliseconds delay)
{
	lock_guard<mutex> lock(mMutex);
	publisher.setTime(gameTime.CurrentTime(), delay);
	mEvents.pushBack(shared_ptr<EventPublisher>(&publisher));
}

//-----------------------------------------------------------------

void EventQueue::enqueue(const shared_ptr<EventPublisher>& publisher, const GameTime& gameTime, milliseconds delay)
{
	assert(publisher.get() != nullptr);

	lock_guard<mutex> lock(mMutex);

	publisher->setTime(gameTime.CurrentTime(), delay);
	mEvents.pushBack(publisher);
}

//-----------------------------------------------------------------

void EventQueue::send(EventPublisher& publisher)
{
	shared_ptr<EventPublisher>(&publisher)->deliver();
}

//-----------------------------------------------------------------

void EventQueue::send(const shared_ptr<EventPublisher>& publisher)
{
	assert(publisher.get() != nullptr);
	publisher->deliver();
}

//-----------------------------------------------------------------

void EventQueue::clear()
{
	lock_guard<mutex> lock(mMutex);
	mEvents.clear();
}

//-----------------------------------------------------------------

uint32_t EventQueue::size() const
{
	lock_guard<mutex> lock(mMutex);
	return mEvents.size();
}

//-----------------------------------------------------------------

bool EventQueue::isEmpty() const
{
	lock_guard<mutex> lock(mMutex);
	return mEvents.isEmpty();
}
