
#include "pch.h"
#include "EntityFoo.h"

using namespace DOGEngine;
using namespace UnitTests;
using namespace std;

RTTI_DEFINITIONS(EntityFoo);

EntityFoo::EntityFoo(const string& name) :
	Entity(name)
{
	if(!areSignaturesInitialized())
	{
		initSignatures();
	}
	populate();
}

//-----------------------------------------------------------------

EntityFoo::EntityFoo(const EntityFoo& other) :
	Entity(other)
{
	updateExternalStorage();
}

//-----------------------------------------------------------------

EntityFoo& EntityFoo::operator=(const EntityFoo& other)
{
	if(this != &other)
	{
		Entity::operator=(other);
		updateExternalStorage();
	}

	return *this;
}

//-----------------------------------------------------------------

Scope* EntityFoo::copy()
{
	return new EntityFoo(*this);
}

//-----------------------------------------------------------------

EntityFoo::EntityFoo(EntityFoo&& other)
{
	operator=(std::move(other));
}

//-----------------------------------------------------------------

EntityFoo& EntityFoo::operator=(EntityFoo&& other)
{
	if(this != &other)
	{
		Entity::operator=(std::move(other));
	}

	return *this;
}

//-----------------------------------------------------------------

EntityFoo::~EntityFoo()
{
}

//-----------------------------------------------------------------

void EntityFoo::update(WorldState& worldState)
{
	Entity::update(worldState);

	(*this)["entity_foo_int"].get<int32_t>() += 1;
}

//-----------------------------------------------------------------

void EntityFoo::initSignatures()
{
	Entity::initSignatures();
	addSignature("entity_foo_int");
}

//-----------------------------------------------------------------

void EntityFoo::populate()
{
	Entity::populate();
	addInternalAttribute("entity_foo_int", 0);
}

//-----------------------------------------------------------------

void EntityFoo::updateExternalStorage()
{
	Entity::updateExternalStorage();
}

