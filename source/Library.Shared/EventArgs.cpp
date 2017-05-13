
#include "pch.h"
#include "EventArgs.h"

using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(EventArgs)

EventArgs::EventArgs(const string& subtype) :
	Scope(),
	mSubtype(subtype),
	mWorldState(nullptr)
{
}

//-----------------------------------------------------------------

EventArgs::EventArgs(const EventArgs& other) :
	Scope(other),
	mSubtype(other.mSubtype),
	mWorldState(other.mWorldState)
{
}

//-----------------------------------------------------------------

EventArgs& EventArgs::operator=(const EventArgs& other)
{
	if(this != &other)
	{
		Scope::operator=(other);
		mSubtype = other.mSubtype;
		mWorldState = other.mWorldState;
	}

	return *this;
}

//-----------------------------------------------------------------

Scope* EventArgs::copy()
{
	return new EventArgs(*this);
}

//-----------------------------------------------------------------

EventArgs::EventArgs(EventArgs&& other)
{
	operator=(std::move(other));
}

//-----------------------------------------------------------------

EventArgs& EventArgs::operator=(EventArgs&& other)
{
	if(this != &other)
	{
		mSubtype = other.mSubtype;
		mWorldState = other.mWorldState;

		other.mSubtype = "";
		other.mWorldState = nullptr;

		Scope::operator=(std::move(other));
	}

	return *this;
}

//-----------------------------------------------------------------

EventArgs::~EventArgs()
{
}

//-----------------------------------------------------------------

void EventArgs::setSubtype(const string& subtype)
{
	mSubtype = subtype;
}

//-----------------------------------------------------------------

void EventArgs::setWorldState(WorldState& world)
{
	mWorldState = &world;
}

//-----------------------------------------------------------------

const string& EventArgs::getSubtype() const
{
	return mSubtype;
}

//-----------------------------------------------------------------

WorldState* EventArgs::getWorldState() const
{
	return mWorldState;
}
