
#include "pch.h"
#include "XmlParseFoo.h"

using namespace DOGEngine;
using namespace UnitTests;
using namespace std;

RTTI_DEFINITIONS(XmlParseFoo)
RTTI_DEFINITIONS(SharedDataFoo)

XmlParseFoo::StartHandlers XmlParseFoo::sStartHandlers;
XmlParseFoo::EndHandlers XmlParseFoo::sEndHandlers;

#pragma region XmlParseFoo Public Interface

XmlParseFoo::XmlParseFoo()
{
	// register handler methods for tags
	if(sStartHandlers.isEmpty())
	{
		sStartHandlers["Person"] = &XmlParseFoo::startElementHandlerPerson;
		sStartHandlers["String"] = &XmlParseFoo::startElementHandlerString;
		sStartHandlers["Int"] = &XmlParseFoo::startElementHandlerInt;
	}
	if(sEndHandlers.isEmpty())
	{
		sEndHandlers["Person"] = &XmlParseFoo::endElementHandlerPerson;
		sEndHandlers["String"] = &XmlParseFoo::endElementHandlerString;
		sEndHandlers["Int"] = &XmlParseFoo::endElementHandlerInt;
	}
}

//-----------------------------------------------------------------

XmlParseFoo::~XmlParseFoo()
{
}

//-----------------------------------------------------------------

XmlParseFoo* XmlParseFoo::clone()
{
	return new XmlParseFoo();
}

//-----------------------------------------------------------------

bool  XmlParseFoo::startElementHandler(SharedData& sharedData, const string& name, const AttributeMap& attributes)
{
	UNREFERENCED_PARAMETER(attributes);

	bool result = false;

	SharedDataFoo* sharedFoo = sharedData.As<SharedDataFoo>();
	if(sharedFoo != nullptr)
	{
		// if we have a registered handler for this tag, call it
		if(sStartHandlers.containsKey(name))
		{
			sharedFoo->incrementNumElementsParsed();
			sStartHandlers[name](*this, *sharedFoo, attributes);
			result = true;
		}
	}

	return result;
}

//-----------------------------------------------------------------

bool  XmlParseFoo::endElementHandler(SharedData& sharedData, const string& name)
{
	bool result = false;

	SharedDataFoo* sharedFoo = sharedData.As<SharedDataFoo>();
	if(sharedFoo != nullptr)
	{
		// if we have a registered handler for this tag, call it
		if(sEndHandlers.containsKey(name))
		{
			sEndHandlers[name](*this, *sharedFoo);
			result = true;
		}
	}

	return result;
}

//-----------------------------------------------------------------

void XmlParseFoo::clearHandlerCaches()
{
	sStartHandlers.clear();
	sEndHandlers.clear();
}

#pragma endregion

//-----------------------------------------------------------------

#pragma region XmlParseFoo Private Parse Handlers

void XmlParseFoo::startElementHandlerPerson(SharedDataFoo& sharedData, const AttributeMap& attributes)
{
	UNREFERENCED_PARAMETER(attributes);

	sharedData.mCurrentTable = &sharedData.mTable.appendScope("People");
}

//-----------------------------------------------------------------

void XmlParseFoo::startElementHandlerString(SharedDataFoo& sharedData, const AttributeMap& attributes)
{
	for(auto& kvPair : attributes)
	{
		string& key = kvPair.first;
		string& value = kvPair.second;

		// append new property key, and push value to back
		sharedData.mCurrentTable->append(key).pushBack(value);
	}
}

//-----------------------------------------------------------------

void XmlParseFoo::startElementHandlerInt(SharedDataFoo& sharedData, const AttributeMap& attributes)
{
	for(auto& kvPair : attributes)
	{
		string& key = kvPair.first;
		int32_t value = stoi(kvPair.second);

		// append new property key, and push value to back
		sharedData.mCurrentTable->append(key).pushBack(value);
	}
}

//-----------------------------------------------------------------

void XmlParseFoo::endElementHandlerPerson(SharedDataFoo& sharedData)
{
	sharedData.mCurrentTable = sharedData.mCurrentTable->getParent();
}

//-----------------------------------------------------------------

void XmlParseFoo::endElementHandlerString(SharedDataFoo& sharedData)
{
	UNREFERENCED_PARAMETER(sharedData);
}

//-----------------------------------------------------------------
void XmlParseFoo::endElementHandlerInt(SharedDataFoo& sharedData)
{
	UNREFERENCED_PARAMETER(sharedData);
}

#pragma endregion

//=================================================================

#pragma region SharedDataFoo

SharedDataFoo::SharedDataFoo() : 
	SharedData(),
	mNumElementsParsed(0)
{
	mCurrentTable = &mTable;
}

//-----------------------------------------------------------------

SharedDataFoo::~SharedDataFoo()
{
}

//-----------------------------------------------------------------

SharedDataFoo* SharedDataFoo::clone()
{
	return new SharedDataFoo();
}

//-----------------------------------------------------------------

void SharedDataFoo::initialize()
{
	// call parent initialize
	SharedData::initialize();

	// then reset our specific members
	mCurrentTable = &mTable;
	mNumElementsParsed = 0;
	mTable.clear();
}

//-----------------------------------------------------------------

void SharedDataFoo::incrementNumElementsParsed()
{
	++mNumElementsParsed;
}

//-----------------------------------------------------------------

Scope& SharedDataFoo::getTable()
{
	return mTable;
}

//-----------------------------------------------------------------

const Scope& SharedDataFoo::getTable() const
{
	return mTable;
}

//-----------------------------------------------------------------

uint32_t SharedDataFoo::numElementsParsed() const
{
	return mNumElementsParsed;
}

#pragma endregion
