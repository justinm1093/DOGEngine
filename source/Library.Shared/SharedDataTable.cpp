
#include "pch.h"
#include "SharedDataTable.h"

using namespace DOGEngine;
using namespace std;
using namespace glm;

RTTI_DEFINITIONS(SharedDataTable)

SharedDataTable::SharedDataTable() :
	SharedData(),
	mScope(nullptr)
{
}

//-----------------------------------------------------------------

SharedDataTable::~SharedDataTable()
{
	delete mScope;
}

//-----------------------------------------------------------------

SharedDataTable* SharedDataTable::clone()
{
	return new SharedDataTable();
}

//-----------------------------------------------------------------

void SharedDataTable::initialize()
{
	SharedData::initialize();

	delete mScope;
	mScope = nullptr;
}

//-----------------------------------------------------------------

void SharedDataTable::setScope(const std::string& name, Scope& child)
{
	if(mScope != nullptr)
	{
		mScope->adopt(name, child);
	}
	mScope = &child;
}

//-----------------------------------------------------------------

void SharedDataTable::setScopeToParent()
{
	// scope becomes its parent if there is one
	if(mScope != nullptr)
	{
		if(mScope->getParent() != nullptr)
		{
			mScope = mScope->getParent();
		}
	}
}

//-----------------------------------------------------------------

Scope* SharedDataTable::extractScope()
{
	Scope* scope = mScope;
	mScope = nullptr;

	return scope;
}

//-----------------------------------------------------------------

Scope* SharedDataTable::getScope() const
{
	return mScope;
}
