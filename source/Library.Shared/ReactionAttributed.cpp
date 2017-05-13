
#include "pch.h"
#include "ReactionAttributed.h"

#include "Event.h"
#include "EventArgs.h"

using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(ReactionAttributed)

const string ReactionAttributed::sSubtypeAttribute = "subtype";

ReactionAttributed::ReactionAttributed(const string& name) :
	Reaction(name)
{
	if(!areSignaturesInitialized())
	{
		initSignatures();
	}
	populate();

	// subscribe this object to events with attributed event args
	Event<EventArgs>::subscribe(*this);
}

//-----------------------------------------------------------------

ReactionAttributed::ReactionAttributed(const ReactionAttributed& other) :
	Reaction(other)
{
	updateExternalStorage();
}


//-----------------------------------------------------------------

ReactionAttributed& ReactionAttributed::operator=(const ReactionAttributed& other)
{
	if(this != &other)
	{
		ActionList::operator=(other);
		updateExternalStorage();
	}

	return *this;
}


//-----------------------------------------------------------------

Scope* ReactionAttributed::copy()
{
	return new ReactionAttributed(*this);
}


//-----------------------------------------------------------------

ReactionAttributed::ReactionAttributed(ReactionAttributed&& other)
{
	operator=(std::move(other));
}


//-----------------------------------------------------------------

ReactionAttributed& ReactionAttributed::operator=(ReactionAttributed&& other)
{
	if(this != &other)
	{
		ActionList::operator=(std::move(other));
	}

	return *this;
}


//-----------------------------------------------------------------

ReactionAttributed::~ReactionAttributed()
{
	Event<EventArgs>::unsubscribe(*this);
}


//-----------------------------------------------------------------

void ReactionAttributed::notify(const EventPublisher& e)
{
	// lock this subscriber so we don't process multiple events at once
	lock_guard<mutex> lock(mMutex);

	// validate the type and subtype of the event
	if(const Event<EventArgs>* eCast = e.As<Event<EventArgs>>())
	{
		EventArgs args = eCast->message();
		if(isSubtype(args.getSubtype()))
		{
			// copy data from args to this object
			for(auto& iter : args)
			{
				append((*iter).first) = (*iter).second;
			}

			// call all actions as part of this event reaction
			assert(args.getWorldState() != nullptr);
			ActionList::update(*args.getWorldState());
		}
	}
}

//-----------------------------------------------------------------

void ReactionAttributed::addSubtype(const string& subtype)
{
	(*this)[sSubtypeAttribute].pushBack(subtype);
}

//-----------------------------------------------------------------

bool ReactionAttributed::removeSubtype(const string& subtype)
{
	return (*this)[sSubtypeAttribute].remove(subtype);
}

//-----------------------------------------------------------------

const Datum& ReactionAttributed::getSubtypes() const
{
	assert(find(sSubtypeAttribute) != nullptr);
	return *find(sSubtypeAttribute);
}

//-----------------------------------------------------------------

bool ReactionAttributed::isSubtype(const string& subtype) const
{
	bool result = false;

	// find string in subtype Datum
	const Datum& datum = getSubtypes();
	for(uint32_t i = 0; i < datum.size(); ++i)
	{
		if(datum.get<string>(i) == subtype)
		{
			result = true;
			break;
		}
	}

	return result;
}

//-----------------------------------------------------------------

void ReactionAttributed::initSignatures()
{
	ActionList::initSignatures();
	addSignature(sSubtypeAttribute);
}

//-----------------------------------------------------------------

void ReactionAttributed::populate()
{
	ActionList::populate();
	addInternalAttribute(sSubtypeAttribute, "");
}

//-----------------------------------------------------------------

void ReactionAttributed::updateExternalStorage()
{
	ActionList::updateExternalStorage();
}
