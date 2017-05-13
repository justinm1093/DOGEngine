
#include "pch.h"
#include "ActionClearEvents.h"
#include "World.h"

using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(ActionClearEvents)

ActionClearEvents::ActionClearEvents(const string& name) :
	Action(name)
{
	if(!areSignaturesInitialized())
	{
		initSignatures();
	}
	populate();
}

//-----------------------------------------------------------------

ActionClearEvents::ActionClearEvents(const ActionClearEvents& other) :
	Action(other)
{
	updateExternalStorage();
}

//-----------------------------------------------------------------

ActionClearEvents& ActionClearEvents::operator=(const ActionClearEvents& other)
{
	if(this != &other)
	{
		Action::operator=(other);
		updateExternalStorage();
	}

	return *this;
}

//-----------------------------------------------------------------

Scope* ActionClearEvents::copy()
{
	return new ActionClearEvents(*this);
}

//-----------------------------------------------------------------

ActionClearEvents::ActionClearEvents(ActionClearEvents&& other)
{
	operator=(std::move(other));
}

//-----------------------------------------------------------------

ActionClearEvents& ActionClearEvents::operator=(ActionClearEvents&& other)
{
	if(this != &other)
	{
		Action::operator=(std::move(other));
	}

	return *this;
}

//-----------------------------------------------------------------

ActionClearEvents::~ActionClearEvents()
{
}

//-----------------------------------------------------------------

void ActionClearEvents::update(WorldState& worldState)
{
	assert(worldState.world != nullptr);

	// clears the current world's event queue
	worldState.action = this;
	worldState.world->getEventQueue().clear();
}
