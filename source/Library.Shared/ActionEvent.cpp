
#include "pch.h"
#include "ActionEvent.h"

#include "World.h"
#include "Event.h"
#include "EventArgs.h"

using namespace std::chrono;
using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(ActionEvent)

const string ActionEvent::sSubtypeAttribute = "subtype";
const string ActionEvent::sDelayAttribute = "delay";

ActionEvent::ActionEvent(const string& name) :
	Action(name)
{
	if(!areSignaturesInitialized())
	{
		initSignatures();
	}
	populate();
}

//-----------------------------------------------------------------

ActionEvent::ActionEvent(const ActionEvent& other) :
	Action(other),
	mSubtype(other.mSubtype),
	mDelay(other.mDelay)
{
	updateExternalStorage();
}

//-----------------------------------------------------------------

ActionEvent& ActionEvent::operator=(const ActionEvent& other)
{
	if(this != &other)
	{
		Action::operator=(other);

		mSubtype = other.mSubtype;
		mDelay = other.mDelay;

		updateExternalStorage();
	}

	return *this;
}

//-----------------------------------------------------------------

Scope* ActionEvent::copy()
{
	return new ActionEvent(*this);
}

//-----------------------------------------------------------------

ActionEvent::ActionEvent(ActionEvent&& other)
{
	operator=(std::move(other));
}

//-----------------------------------------------------------------

ActionEvent& ActionEvent::operator=(ActionEvent&& other)
{
	if(this != &other)
	{
		mSubtype = other.mSubtype;
		mDelay = other.mDelay;

		Action::operator=(std::move(other));
	}

	return *this;
}

//-----------------------------------------------------------------

ActionEvent::~ActionEvent()
{
}

//-----------------------------------------------------------------

void ActionEvent::update(WorldState& worldState)
{
	assert(worldState.world != nullptr);
	worldState.action = this;

	// auxiliary attributes are data for the event argument
	//		DOES NOT support child Scopes as arguments
	EventArgs args;
	for(uint32_t i = auxiliaryBeginIndex(); i < size(); ++i)
	{
		PairType* kvPair = mVector[i];
		args.append(kvPair->first) = kvPair->second;
	}

	// set event worldstate and subtype
	args.setWorldState(worldState);
	args.setSubtype(mSubtype);

	// enqueue event with our args and our delay
	worldState.world->getEventQueue().enqueue(
		make_shared<Event<EventArgs>>(args),
		worldState.gameTime,
		milliseconds(mDelay));
}

//-----------------------------------------------------------------

void ActionEvent::setSubtype(const string& subtype)
{
	mSubtype = subtype;
}

//-----------------------------------------------------------------

void ActionEvent::setDelay(const int32_t delay)
{
	mDelay = delay;
}

//-----------------------------------------------------------------

const string& ActionEvent::getSubtype() const
{
	return mSubtype;
}

//-----------------------------------------------------------------

int32_t ActionEvent::getDelay() const
{
	return mDelay;
}

//-----------------------------------------------------------------

void ActionEvent::initSignatures()
{
	Action::initSignatures();

	addSignature(sSubtypeAttribute);
	addSignature(sDelayAttribute);
}

//-----------------------------------------------------------------

void ActionEvent::populate()
{
	Action::populate();

	addExternalAttribute(sSubtypeAttribute, &mSubtype, "");
	addExternalAttribute(sDelayAttribute, &mDelay, 0);
}

//-----------------------------------------------------------------

void ActionEvent::updateExternalStorage()
{
	Action::updateExternalStorage();

	(*this)[sSubtypeAttribute].setStorage(&mSubtype, 1);
	(*this)[sDelayAttribute].setStorage(&mDelay, 1);
}
