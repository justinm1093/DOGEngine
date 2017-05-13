
#include "pch.h"
#include "Entity.h"

#include "Sector.h"

using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(Entity)

const string Entity::sNameAttribute = "name";
const string Entity::sActionsAttribute = "actions";
const string Entity::sReactionsAttribute = "reactions";

Entity::Entity(const string& name) :
	Attributed()
{
	if(!areSignaturesInitialized())
	{
		initSignatures();
	}
	populate();

	// set name to the argument if we were given one (populate sets name to some default value)
	if(name != "")
	{
		mName = name;
	}
}

//-----------------------------------------------------------------

Entity::Entity(const Entity& other) :
	Attributed(other),
	mName(other.mName)
{
	updateExternalStorage();
}

//-----------------------------------------------------------------

Entity& Entity::operator=(const Entity& other)
{
	if(this != &other)
	{
		Attributed::operator=(other);
		mName = other.mName;

		updateExternalStorage();
	}

	return *this;
}

//-----------------------------------------------------------------

Scope* Entity::copy()
{
	return new Entity(*this);
}

//-----------------------------------------------------------------

Entity::Entity(Entity&& other)
{
	operator=(std::move(other));
}

//-----------------------------------------------------------------

Entity& Entity::operator=(Entity&& other)
{
	if(this != &other)
	{
		mName = other.mName;
		Attributed::operator=(std::move(other));
	}

	return *this;
}

//-----------------------------------------------------------------

Entity::~Entity()
{
}

//-----------------------------------------------------------------

void Entity::update(WorldState& worldState)
{
	worldState.entity = this;

	// call update on each child action in this entity
	Datum& actions = getActions();
	for(uint32_t i = 0; i < actions.size(); ++i)
	{
		static_cast<Action*>(&actions[i])->update(worldState);
	}
}

//-----------------------------------------------------------------

Action* Entity::createAction(const string& className, const string& objectName)
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

Reaction* Entity::createReaction(const std::string& className, const std::string& objectName)
{
	Reaction* reaction = Factory<Reaction>::create(className);

	if(reaction != nullptr)
	{
		adopt(sReactionsAttribute, *reaction);
		reaction->setName(objectName);
	}

	return reaction;
}

//-----------------------------------------------------------------

void Entity::setName(const string& name)
{
	mName = name;
}

//-----------------------------------------------------------------

const string& Entity::getName() const
{
	return mName;
}

//-----------------------------------------------------------------

Sector* Entity::getSector() const
{
	assert(mParent == nullptr || mParent->Is(Sector::TypeIdClass()));
	return static_cast<Sector*>(mParent);
}

//-----------------------------------------------------------------

Datum& Entity::getActions() const
{
	return *find(sActionsAttribute);
}

//-----------------------------------------------------------------

Datum& Entity::getReactions() const
{
	return *find(sReactionsAttribute);
}

//-----------------------------------------------------------------

void Entity::initSignatures()
{
	Attributed::initSignatures();

	addSignature(sNameAttribute);
	addSignature(sActionsAttribute);
	addSignature(sReactionsAttribute);
}

//-----------------------------------------------------------------

void Entity::populate()
{
	Attributed::populate();

	// add 'name' as external, then add empty Datum for our actions
	addExternalAttribute(sNameAttribute, &mName, "Entity");
	addTableEntry(sActionsAttribute);
	addTableEntry(sReactionsAttribute);
}

//-----------------------------------------------------------------

void Entity::updateExternalStorage()
{
	Attributed::updateExternalStorage();
	(*this)[sNameAttribute].setStorage(&mName, 1);
}
