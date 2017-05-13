
#include "pch.h"
#include "EventSubscriberFoo.h"

#include "Event.h"
#include "Foo.h"

using namespace DOGEngine;
using namespace UnitTests;
using namespace std;

EventSubscriberFoo::EventSubscriberFoo(SubscriberAction action) :
	mInt(0),
	mNumNotifies(0),
	mAction(action),
	mChildFoo(nullptr)
{
	Event<Foo>::subscribe(*this);
}

//-----------------------------------------------------------------

EventSubscriberFoo::~EventSubscriberFoo()
{
	Event<Foo>::unsubscribe(*this);
	if(mChildFoo != nullptr)
	{
		delete mChildFoo;
	}
}

//-----------------------------------------------------------------

void EventSubscriberFoo::notify(const EventPublisher& e)
{
	lock_guard<mutex> lock(mMutex);

	// we set our value to match that in the event message
	if(const Event<Foo>* eFoo = e.As<Event<Foo>>())
	{
		const Foo& foo = eFoo->message();
		mInt = foo.getValue();

		++mNumNotifies;

		switch(mAction)
		{
			case SubscriberAction::SUBSCRIBE:
				if(mChildFoo == nullptr)
				{
					mChildFoo = new EventSubscriberFoo();
				}
				break;

			case SubscriberAction::UNSUBSCRIBE:
				Event<Foo>::unsubscribe(*this);
				break;

			case SubscriberAction::CLEAR_SUBSCRIBERS:
				Event<Foo>::unsubscribeAll();
				break;

			default:
				break;
		}
	}
}

//-----------------------------------------------------------------

void EventSubscriberFoo::resetNumNotifies()
{
	mNumNotifies = 0;
}

//-----------------------------------------------------------------

int32_t EventSubscriberFoo::getInt() const
{
	return mInt;
}

//-----------------------------------------------------------------

uint32_t EventSubscriberFoo::getNumNotifies() const
{
	return mNumNotifies;
}

//-----------------------------------------------------------------

uint32_t EventSubscriberFoo::getChildNumNotifies() const
{
	return mChildFoo == nullptr ? 0 : mChildFoo->getNumNotifies();
}
