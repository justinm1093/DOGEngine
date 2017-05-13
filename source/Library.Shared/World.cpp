
#include "pch.h"
#include "World.h"

using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(World)

const string World::sNameAttribute = "name";
const string World::sSectorsAttribute = "sectors";
const string World::sActionsAttribute = "actions";
const string World::sReactionsAttribute = "reactions";

World::World(const string& name) :
	Attributed(),
	mPendingDelete(),
	mEventQueue(),
	mState()
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

World::World(const World& other) :
	Attributed(other),
	mEventQueue(other.mEventQueue),
	mName(other.mName)
{
	updateExternalStorage();
}

//-----------------------------------------------------------------

World& World::operator=(const World& other)
{
	if(this != &other)
	{
		// we lose the objects this points to, so we just clear it
		//		don't copy it, because we don't manage what 'other' has
		mPendingDelete.clear();

		Attributed::operator=(other);
		mEventQueue = other.mEventQueue;
		mName = other.mName;

		updateExternalStorage();
	}

	return *this;
}

//-----------------------------------------------------------------

Scope* World::copy()
{
	return new World(*this);
}

//-----------------------------------------------------------------

World::World(World&& other)
{
	operator=(std::move(other));
}

//-----------------------------------------------------------------

World& World::operator=(World&& other)
{
	if(this != &other)
	{
		mName = other.mName;
		mEventQueue = std::move(other.mEventQueue);
		mPendingDelete = std::move(other.mPendingDelete);

		Attributed::operator=(std::move(other));
	}

	return *this;
}

//-----------------------------------------------------------------

World::~World()
{
}

//-----------------------------------------------------------------

void World::update()
{
	mState.world = this;

	// call update on each child action in this world
	Datum& actions = getActions();
	for(uint32_t i = 0; i < actions.size(); ++i)
	{
		static_cast<Action*>(&actions[i])->update(mState);
	}

	// call update on each child sector in this world
	Datum& sectors = getSectors();
	for(uint32_t i = 0; i < sectors.size(); ++i)
	{
		static_cast<Sector*>(&sectors[i])->update(mState);
	}

	// update queues
	mEventQueue.update(mState.gameTime);
	mPendingDelete.empty();
}

//-----------------------------------------------------------------

Sector* World::createSector(const string& name)
{
	Sector* sector = new Sector(name);
	adopt(sSectorsAttribute, *sector);

	return sector;
}

//-----------------------------------------------------------------

Action* World::createAction(const std::string& className, const std::string& objectName)
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

Reaction* World::createReaction(const std::string& className, const std::string& objectName)
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

void World::setName(const string& name)
{
	mName = name;
}

//-----------------------------------------------------------------

PendingDelete& World::getPendingDelete()
{
	return mPendingDelete;
}

//-----------------------------------------------------------------

EventQueue& World::getEventQueue()
{
	return mEventQueue;
}

//-----------------------------------------------------------------

WorldState& World::getWorldState()
{
	return mState;
}

//-----------------------------------------------------------------

const WorldState& World::getWorldState() const
{
	return mState;
}

//-----------------------------------------------------------------

const string& World::getName() const
{
	return mName;
}

//-----------------------------------------------------------------

Datum& World::getSectors() const
{
	return *find(sSectorsAttribute);
}

//-----------------------------------------------------------------

Datum& World::getActions() const
{
	return *find(sActionsAttribute);
}

//-----------------------------------------------------------------

Datum& World::getReactions() const
{
	return *find(sReactionsAttribute);
}

//-----------------------------------------------------------------

void World::initSignatures()
{
	Attributed::initSignatures();

	addSignature(sNameAttribute);
	addSignature(sSectorsAttribute);
	addSignature(sActionsAttribute);
	addSignature(sReactionsAttribute);
}

//-----------------------------------------------------------------

void World::populate()
{
	Attributed::populate();

	// add 'name' as external, then append an empty Datum for our sectors
	addExternalAttribute(sNameAttribute, &mName, "World");
	addTableEntry(sSectorsAttribute);
	addTableEntry(sActionsAttribute);
	addTableEntry(sReactionsAttribute);
}

//-----------------------------------------------------------------

void World::updateExternalStorage()
{
	Attributed::updateExternalStorage();
	(*this)[sNameAttribute].setStorage(&mName, 1);
}
