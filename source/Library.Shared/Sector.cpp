
#include "pch.h"
#include "Sector.h"
#include "Factory.h"

#include "World.h"

using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(Sector)

const string Sector::sNameAttribute = "name";
const string Sector::sEntitiesAttribute = "entities";
const string Sector::sActionsAttribute = "actions";
const string Sector::sReactionsAttribute = "reactions";

Sector::Sector(const std::string& name) :
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

Sector::Sector(const Sector& other) :
	Attributed(other),
	mName(other.mName)
{
	updateExternalStorage();
}

//-----------------------------------------------------------------

Sector& Sector::operator=(const Sector& other)
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

Scope* Sector::copy()
{
	return new Sector(*this);
}

//-----------------------------------------------------------------

Sector::Sector(Sector&& other)
{
	operator=(std::move(other));
}

//-----------------------------------------------------------------

Sector& Sector::operator=(Sector&& other)
{
	if(this != &other)
	{
		mName = other.mName;
		Attributed::operator=(std::move(other));
	}

	return *this;
}

//-----------------------------------------------------------------

Sector::~Sector()
{
}

//-----------------------------------------------------------------

void Sector::update(WorldState& worldState)
{
	worldState.sector = this;

	// call update on each child action in this world
	Datum& actions = getActions();
	for(uint32_t i = 0; i < actions.size(); ++i)
	{
		static_cast<Action*>(&actions[i])->update(worldState);
	}

	// call update on each child entity in this sector
	Datum& entities = getEntities();
	for(uint32_t i = 0; i < entities.size(); ++i)
	{
		static_cast<Entity*>(&entities[i])->update(worldState);
	}
}

//-----------------------------------------------------------------

Entity* Sector::createEntity(const string& className, const string& objectName)
{
	Entity* entity = Factory<Entity>::create(className);

	if(entity != nullptr)
	{
		adopt(sEntitiesAttribute, *entity);
		entity->setName(objectName);
	}

	return entity;
}

//-----------------------------------------------------------------

Action* Sector::createAction(const std::string& className, const std::string& objectName)
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

Reaction* Sector::createReaction(const std::string& className, const std::string& objectName)
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

void Sector::setName(const string& name)
{
	mName = name;
}

//-----------------------------------------------------------------

const string& Sector::getName() const
{
	return mName;
}

//-----------------------------------------------------------------

World* Sector::getWorld() const
{
	assert(mParent == nullptr || mParent->Is(World::TypeIdClass()));
	return static_cast<World*>(mParent);
}

//-----------------------------------------------------------------

Datum& Sector::getEntities() const
{
	return *find(sEntitiesAttribute);
}

//-----------------------------------------------------------------

Datum& Sector::getActions() const
{
	return *find(sActionsAttribute);
}

//-----------------------------------------------------------------

Datum& Sector::getReactions() const
{
	return *find(sReactionsAttribute);
}

//-----------------------------------------------------------------

void Sector::initSignatures()
{
	Attributed::initSignatures();

	addSignature(sNameAttribute);
	addSignature(sEntitiesAttribute);
	addSignature(sActionsAttribute);
	addSignature(sReactionsAttribute);
}

//-----------------------------------------------------------------

void Sector::populate()
{
	Attributed::populate();

	// add 'name' as external, then append an empty Datum for our entities
	addExternalAttribute(sNameAttribute, &mName, "Sector");
	addTableEntry(sEntitiesAttribute);
	addTableEntry(sActionsAttribute);
	addTableEntry(sReactionsAttribute);
}

//-----------------------------------------------------------------

void Sector::updateExternalStorage()
{
	Attributed::updateExternalStorage();
	(*this)[sNameAttribute].setStorage(&mName, 1);
}
