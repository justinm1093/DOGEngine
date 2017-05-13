
#include "pch.h"
#include "ActionCreateAction.h"

#include "World.h"
#include "Entity.h"
#include "ActionList.h"

#include "Factory.h"

#include "PendingDelete.h"

using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(ActionCreateAction)

const string ActionCreateAction::sClassNameAttribute = "class_name";
const string ActionCreateAction::sInstanceNameAttribute = "instance_name";

ActionCreateAction::ActionCreateAction(const string& name) :
	Action(name)
{
	if(!areSignaturesInitialized())
	{
		initSignatures();
	}
	populate();
}

//-----------------------------------------------------------------

ActionCreateAction::ActionCreateAction(const ActionCreateAction& other) :
	Action(other),
	mClassName(other.mClassName),
	mInstanceName(other.mInstanceName)
{
	updateExternalStorage();
}

//-----------------------------------------------------------------

ActionCreateAction& ActionCreateAction::operator=(const ActionCreateAction& other)
{
	if(this != &other)
	{
		Action::operator=(other);

		mClassName = other.mClassName;
		mInstanceName = other.mInstanceName;
		updateExternalStorage();
	}

	return *this;
}

//-----------------------------------------------------------------

Scope* ActionCreateAction::copy()
{
	return new ActionCreateAction(*this);
}

//-----------------------------------------------------------------

ActionCreateAction::ActionCreateAction(ActionCreateAction&& other)
{
	operator=(std::move(other));
}

//-----------------------------------------------------------------

ActionCreateAction& ActionCreateAction::operator=(ActionCreateAction&& other)
{
	if(this != &other)
	{
		mClassName = other.mClassName;
		mInstanceName = other.mInstanceName;
		Action::operator=(std::move(other));
	}

	return *this;
}

//-----------------------------------------------------------------

ActionCreateAction::~ActionCreateAction()
{
}

//-----------------------------------------------------------------

void ActionCreateAction::update(WorldState& worldState)
{
	assert(worldState.world != nullptr);
	worldState.action = this;
	
	assert(mParent != nullptr);
	assert(
		mParent->Is(World::TypeIdClass()) || mParent->Is(Sector::TypeIdClass()) ||
		mParent->Is(Entity::TypeIdClass()) || mParent->Is(ActionList::TypeIdClass()));

	// create a new action, adopt it into parent's "actions" attribute
	Action* action = Factory<Action>::create(mClassName);
	if(action != nullptr)
	{
		action->setName(mInstanceName);
		mParent->adopt(Entity::sActionsAttribute, *action);
	}

	worldState.world->getPendingDelete().enqueue(*this);
}

//-----------------------------------------------------------------

void ActionCreateAction::setClassName(const string& name)
{
	mClassName = name;
}

//-----------------------------------------------------------------

void ActionCreateAction::setInstanceName(const string& name)
{
	mInstanceName = name;
}

//-----------------------------------------------------------------

string ActionCreateAction::getClassName() const
{
	return mClassName;
}

//-----------------------------------------------------------------

string ActionCreateAction::getInstanceName() const
{
	return mInstanceName;
}

//-----------------------------------------------------------------

void ActionCreateAction::initSignatures()
{
	Action::initSignatures();

	addSignature(sClassNameAttribute);
	addSignature(sInstanceNameAttribute);
}

//-----------------------------------------------------------------

void ActionCreateAction::populate()
{
	Action::populate();

	addExternalAttribute(sClassNameAttribute, &mClassName, "");
	addExternalAttribute(sInstanceNameAttribute, &mInstanceName, "");
}

//-----------------------------------------------------------------

void ActionCreateAction::updateExternalStorage()
{
	Action::updateExternalStorage();
	(*this)[sClassNameAttribute].setStorage(&mClassName, 1);
	(*this)[sInstanceNameAttribute].setStorage(&mInstanceName, 1);
}
