
#include "pch.h"
#include "Action.h"

using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(Action)

const string Action::sNameAttribute = "name";
const string Action::sReactionsAttribute = "reactions";

Action::Action(const string& name) :
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

Action::Action(const Action& other) :
	Attributed(other),
	mName(other.mName)
{
	updateExternalStorage();
}

//-----------------------------------------------------------------

Action& Action::operator=(const Action& other)
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

Action::Action(Action&& other)
{
	operator=(std::move(other));
}

//-----------------------------------------------------------------

Action& Action::operator=(Action&& other)
{
	if(this != &other)
	{
		mName = other.mName;
		Attributed::operator=(std::move(other));
	}

	return *this;
}

//-----------------------------------------------------------------

Action::~Action()
{
}

//-----------------------------------------------------------------

void Action::setName(const string& name)
{
	mName = name;
}

//-----------------------------------------------------------------

const string& Action::getName() const
{
	return mName;
}

//-----------------------------------------------------------------

void Action::initSignatures()
{
	Attributed::initSignatures();
	addSignature(sNameAttribute);
	addSignature(sReactionsAttribute);
}

//-----------------------------------------------------------------

void Action::populate()
{
	Attributed::populate();
	addExternalAttribute(sNameAttribute, &mName, "Action");
	addTableEntry(sReactionsAttribute);
}

//-----------------------------------------------------------------

void Action::updateExternalStorage()
{
	Attributed::updateExternalStorage();
	(*this)[sNameAttribute].setStorage(&mName, 1);
}
