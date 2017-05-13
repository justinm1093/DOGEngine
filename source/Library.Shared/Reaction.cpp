
#include "pch.h"
#include "Reaction.h"

using namespace DOGEngine;
using namespace std;

RTTI_DEFINITIONS(Reaction)

Reaction::Reaction(const string& name) :
	ActionList(name)
{
}

//-----------------------------------------------------------------

Reaction::Reaction(const Reaction& other) :
	ActionList(other)
{
}
