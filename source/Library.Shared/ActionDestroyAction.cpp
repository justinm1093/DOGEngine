
#include "pch.h"
#include "ActionDestroyAction.h"

#include "World.h"
#include "Entity.h"
#include "ActionList.h"

using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(ActionDestroyAction)

const string ActionDestroyAction::sDeleteTargetAttribute = "delete_target";

ActionDestroyAction::ActionDestroyAction(const string& name) :
	Action(name)
{
	if(!areSignaturesInitialized())
	{
		initSignatures();
	}
	populate();
}

//-----------------------------------------------------------------

ActionDestroyAction::ActionDestroyAction(const ActionDestroyAction& other) :
	Action(other),
	mDeleteTarget(other.mDeleteTarget)
{
	updateExternalStorage();
}

//-----------------------------------------------------------------

ActionDestroyAction& ActionDestroyAction::operator=(const ActionDestroyAction& other)
{
	if(this != &other)
	{
		Action::operator=(other);

		mDeleteTarget = other.mDeleteTarget;
		updateExternalStorage();
	}

	return *this;
}

//-----------------------------------------------------------------

Scope* ActionDestroyAction::copy()
{
	return new ActionDestroyAction(*this);
}

//-----------------------------------------------------------------

ActionDestroyAction::ActionDestroyAction(ActionDestroyAction&& other)
{
	operator=(std::move(other));
}

//-----------------------------------------------------------------

ActionDestroyAction& ActionDestroyAction::operator=(ActionDestroyAction&& other)
{
	if(this != &other)
	{
		mDeleteTarget = other.mDeleteTarget;
		Action::operator=(std::move(other));
	}

	return *this;
}

//-----------------------------------------------------------------

ActionDestroyAction::~ActionDestroyAction()
{
}

//-----------------------------------------------------------------

void ActionDestroyAction::update(WorldState& worldState)
{
	assert(worldState.world != nullptr);
	worldState.action = this;

	assert(mParent != nullptr);
	assert(
		mParent->Is(World::TypeIdClass()) || mParent->Is(Sector::TypeIdClass()) || 
		mParent->Is(Entity::TypeIdClass()) || mParent->Is(ActionList::TypeIdClass()));

	Datum& actions = (*mParent)[Entity::sActionsAttribute];
	for(uint32_t i = 0; i < actions.size(); ++i)
	{
		assert(actions[i].Is(Action::TypeIdClass()));
		if(static_cast<Action*>(&actions[i])->getName() == mDeleteTarget)
		{
			worldState.world->getPendingDelete().enqueue(actions[i]);
			break;
		}
	}

	worldState.world->getPendingDelete().enqueue(*this);
}

//-----------------------------------------------------------------

void ActionDestroyAction::setDeleteTarget(const string& target)
{
	mDeleteTarget = target;
}

//-----------------------------------------------------------------

string ActionDestroyAction::getDeleteTarget() const
{
	return mDeleteTarget;
}

//-----------------------------------------------------------------

void ActionDestroyAction::initSignatures()
{
	Action::initSignatures();
	addSignature(sDeleteTargetAttribute);
}

//-----------------------------------------------------------------

void ActionDestroyAction::populate()
{
	Action::populate();
	addExternalAttribute(sDeleteTargetAttribute, &mDeleteTarget, "");
}

//-----------------------------------------------------------------

void ActionDestroyAction::updateExternalStorage()
{
	Action::updateExternalStorage();
	(*this)[sDeleteTargetAttribute].setStorage(&mDeleteTarget, 1);
}
