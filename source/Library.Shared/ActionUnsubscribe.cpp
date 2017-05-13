
#include "pch.h"
#include "ActionUnsubscribe.h"
#include "ReactionAttributed.h"

#include "Event.h"
#include "EventArgs.h"

using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(ActionUnsubscribe)

ActionUnsubscribe::ActionUnsubscribe(const string& name) :
	Action(name)
{
	if(!areSignaturesInitialized())
	{
		initSignatures();
	}
	populate();
}

//-----------------------------------------------------------------

ActionUnsubscribe::ActionUnsubscribe(const ActionUnsubscribe& other) :
	Action(other)
{
	updateExternalStorage();
}

//-----------------------------------------------------------------

ActionUnsubscribe& ActionUnsubscribe::operator=(const ActionUnsubscribe& other)
{
	if(this != &other)
	{
		Action::operator=(other);
		updateExternalStorage();
	}

	return *this;
}

//-----------------------------------------------------------------

Scope* ActionUnsubscribe::copy()
{
	return new ActionUnsubscribe(*this);
}

//-----------------------------------------------------------------

ActionUnsubscribe::ActionUnsubscribe(ActionUnsubscribe&& other)
{
	operator=(std::move(other));
}

//-----------------------------------------------------------------

ActionUnsubscribe& ActionUnsubscribe::operator=(ActionUnsubscribe&& other)
{
	if(this != &other)
	{
		Action::operator=(std::move(other));
	}

	return *this;
}

//-----------------------------------------------------------------

ActionUnsubscribe::~ActionUnsubscribe()
{
}

//-----------------------------------------------------------------

void ActionUnsubscribe::update(WorldState& worldState)
{
	worldState.action = this;

	// unsubscribes parent from Event<EventArgs> if it is ReactionAttributed
	assert(mParent != nullptr);
	if(ReactionAttributed* parentReaction = mParent->As<ReactionAttributed>())
	{
		Event<EventArgs>::unsubscribe(*parentReaction);
	}
}
