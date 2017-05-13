
#include "pch.h"
#include "ActionList.h"

using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(ActionList)

const string ActionList::sActionsAttribute = "actions";

ActionList::ActionList(const string& name) :
	Action(name)
{
	if(!areSignaturesInitialized())
	{
		initSignatures();
	}
	populate();
}

//-----------------------------------------------------------------

ActionList::ActionList(const ActionList& other) :
	Action(other)
{
	updateExternalStorage();
}

//-----------------------------------------------------------------

ActionList& ActionList::operator=(const ActionList& other)
{
	if(this != &other)
	{
		Action::operator=(other);
		updateExternalStorage();
	}

	return *this;
}

//-----------------------------------------------------------------

Scope* ActionList::copy()
{
	return new ActionList(*this);
}

//-----------------------------------------------------------------

ActionList::ActionList(ActionList&& other)
{
	operator=(std::move(other));
}

//-----------------------------------------------------------------

ActionList& ActionList::operator=(ActionList&& other)
{
	if(this != &other)
	{
		Action::operator=(std::move(other));
	}

	return *this;
}

//-----------------------------------------------------------------

ActionList::~ActionList()
{
}

//-----------------------------------------------------------------

void ActionList::update(WorldState& worldState)
{
	worldState.action = this;

	// call update on each child action in this action list
	Datum& actions = getActions();
	for(uint32_t i = 0; i < actions.size(); ++i)
	{
		static_cast<Action*>(&actions[i])->update(worldState);
	}
}

//-----------------------------------------------------------------

Action* ActionList::createAction(const string& className, const string& objectName)
{
	Action* action = Factory<Action>::create(className);

	if(action != nullptr)
	{
		adopt(sActionsAttribute, *action);
		action->setName(objectName);
	}

	return action;
}

//-----------------------------------------------------------------

Datum& ActionList::getActions() const
{
	return *find(sActionsAttribute);
}

//-----------------------------------------------------------------

void ActionList::initSignatures()
{
	Action::initSignatures();
	addSignature(sActionsAttribute);
}

//-----------------------------------------------------------------

void ActionList::populate()
{
	Action::populate();
	addTableEntry(sActionsAttribute);
}

//-----------------------------------------------------------------

void ActionList::updateExternalStorage()
{
	Action::updateExternalStorage();
}
