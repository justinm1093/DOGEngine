
#include "pch.h"
#include "ActionListIf.h"

using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(ActionListIf)

const string ActionListIf::sConditionAttribute = "condition";
const string ActionListIf::sThenAttribute = "then";
const string ActionListIf::sElseAttribute = "else";

ActionListIf::ActionListIf(const string& name) :
	ActionList(name)
{
	if(!areSignaturesInitialized())
	{
		initSignatures();
	}
	populate();
}

//-----------------------------------------------------------------

ActionListIf::ActionListIf(const ActionListIf& other) :
	ActionList(other),
	mCondition(other.mCondition)
{
	updateExternalStorage();
}

//-----------------------------------------------------------------

ActionListIf& ActionListIf::operator=(const ActionListIf& other)
{
	if(this != &other)
	{
		ActionList::operator=(other);

		mCondition = other.mCondition;
		updateExternalStorage();
	}

	return *this;
}

//-----------------------------------------------------------------

Scope* ActionListIf::copy()
{
	return new ActionListIf(*this);
}

//-----------------------------------------------------------------

ActionListIf::ActionListIf(ActionListIf&& other)
{
	operator=(std::move(other));
}

//-----------------------------------------------------------------

ActionListIf& ActionListIf::operator=(ActionListIf&& other)
{
	if(this != &other)
	{
		mCondition = other.mCondition;
		ActionList::operator=(std::move(other));
	}

	return *this;
}

//-----------------------------------------------------------------

ActionListIf::~ActionListIf()
{
}

//-----------------------------------------------------------------

void ActionListIf::update(WorldState& worldState)
{
	worldState.action = this;

	if(mCondition != 0)
	{
		if(mThenBlock != nullptr)
		{
			assert(mThenBlock->Is(Action::TypeIdClass()));
			static_cast<Action*>(mThenBlock)->update(worldState);
		}
	}
	else
	{
		if(mElseBlock != nullptr)
		{
			assert(mElseBlock->Is(Action::TypeIdClass()));
			static_cast<Action*>(mElseBlock)->update(worldState);
		}
	}
}

//-----------------------------------------------------------------

Action* ActionListIf::createThenBlock(const string& className, const string& objName)
{
	Action* action = Factory<Action>::create(className);
	if(action != nullptr)
	{
		// delete previous then block
		delete mThenBlock;

		// adopt new then block
		adopt(sThenAttribute, *action);
		action->setName(objName);
		mThenBlock = action;
	}

	return action;
}

//-----------------------------------------------------------------

Action* ActionListIf::createElseBlock(const string& className, const string& objName)
{
	Action* action = Factory<Action>::create(className);
	if(action != nullptr)
	{
		// delete previous else block
		delete mElseBlock;

		// adopt new else block
		adopt(sElseAttribute, *action);
		action->setName(objName);
		mElseBlock = action;
	}

	return action;
}

//-----------------------------------------------------------------

Action* ActionListIf::getThenBlock() const
{
	assert(mThenBlock == nullptr || mThenBlock->Is(Action::TypeIdClass()));
	return static_cast<Action*>(mThenBlock);
}

//-----------------------------------------------------------------

Action* ActionListIf::getElseBlock() const
{
	assert(mElseBlock == nullptr || mElseBlock->Is(Action::TypeIdClass()));
	return static_cast<Action*>(mElseBlock);
}

//-----------------------------------------------------------------

void ActionListIf::initSignatures()
{
	ActionList::initSignatures();

	addSignature(sConditionAttribute);
	addSignature(sThenAttribute);
	addSignature(sElseAttribute);
}

//-----------------------------------------------------------------

void ActionListIf::populate()
{
	ActionList::populate();

	addExternalAttribute(sConditionAttribute, &mCondition, 0);
	addTableEntry(sThenAttribute);
	addTableEntry(sElseAttribute);

	// initializing values, then and else do not point to anything
	mThenBlock = nullptr;
	mElseBlock = nullptr;
}

//-----------------------------------------------------------------

void ActionListIf::updateExternalStorage()
{
	ActionList::updateExternalStorage();

	(*this)[sConditionAttribute].setStorage(&mCondition, 1);

	// update the then and else blocks
	mThenBlock = (*this)[sThenAttribute].size() == 0 ? nullptr : &(*this)[sThenAttribute][0];
	mElseBlock = (*this)[sElseAttribute].size() == 0 ? nullptr : &(*this)[sElseAttribute][0];
}