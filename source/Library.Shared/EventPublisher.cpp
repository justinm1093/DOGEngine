
#include "pch.h"
#include "EventPublisher.h"

using namespace std::chrono;
using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(EventPublisher)

EventPublisher::EventPublisher(Subscribers& subscribers, mutex& mutex) :
	mSubscribers(&subscribers),
	mTimeEnqueued(),
	mMutex(&mutex),
	mDelay()
{
}

//-----------------------------------------------------------------

EventPublisher::EventPublisher(const EventPublisher& other) :
	mSubscribers(other.mSubscribers),
	mTimeEnqueued(other.mTimeEnqueued),
	mDelay(other.mDelay)
{
}

//-----------------------------------------------------------------

EventPublisher& EventPublisher::operator=(const EventPublisher& other)
{
	if(this != &other)
	{
		mSubscribers = other.mSubscribers;
		mTimeEnqueued = other.mTimeEnqueued;
		mDelay = other.mDelay;
	}

	return *this;
}

//-----------------------------------------------------------------

EventPublisher::EventPublisher(EventPublisher&& other) :
	mSubscribers(other.mSubscribers),
	mTimeEnqueued(other.mTimeEnqueued),
	mDelay(other.mDelay)
{
	// we don't reset other's subscribers pointer, since it points to the same static memory
	other.mTimeEnqueued = high_resolution_clock::time_point();
	other.mDelay = milliseconds();
}

//-----------------------------------------------------------------

EventPublisher& EventPublisher::operator=(EventPublisher&& other)
{
	if(this != &other)
	{
		mSubscribers = other.mSubscribers;
		mTimeEnqueued = other.mTimeEnqueued;
		mDelay = other.mDelay;

		// we don't reset other's subscribers pointer, since it points to the same static memory
		other.mTimeEnqueued = high_resolution_clock::time_point();
		other.mDelay = milliseconds();
	}

	return *this;
}

//-----------------------------------------------------------------

EventPublisher::~EventPublisher()
{
}

//-----------------------------------------------------------------

void EventPublisher::deliver()
{
	assert(mSubscribers != nullptr);
	assert(mMutex != nullptr);

	// don't want to trash the list in the middle of processing, so iterating over a copy
	//		locking during copy
	Subscribers subscribers;
	{
		lock_guard<mutex> lock(*mMutex);
		subscribers = *mSubscribers;
	}

	// spin up async calls for each subscriber in our list
	vector<future<void>> futures;
	for(IEventSubscriber* subscriber : subscribers)
	{
		futures.emplace_back(async(&IEventSubscriber::notify, subscriber, cref(*this)));
	}

	// wait for all threads to return
	for(future<void>& future : futures)
	{
		future.wait();
	}
}

//-----------------------------------------------------------------

void EventPublisher::setTime(const high_resolution_clock::time_point& timeEnqueued, const milliseconds& delay)
{
	mTimeEnqueued = timeEnqueued;
	mDelay = delay;
}

//-----------------------------------------------------------------

const high_resolution_clock::time_point& EventPublisher::timeEnqueued() const
{
	return mTimeEnqueued;
}

//-----------------------------------------------------------------

const milliseconds& EventPublisher::delay() const
{
	return mDelay;
}

//-----------------------------------------------------------------

uint32_t EventPublisher::numSubscribers() const
{
	assert(mSubscribers != nullptr);
	assert(mMutex != nullptr);

	lock_guard<mutex> lock(*mMutex);
	return mSubscribers->size();
}

//-----------------------------------------------------------------

bool EventPublisher::isExpired(const GameTime& gameTime) const
{
	return mTimeEnqueued + mDelay < gameTime.CurrentTime();
}
