
#include "pch.h"
#include "Scope.h"

using namespace std;
using namespace DOGEngine;

RTTI_DEFINITIONS(Scope)

uint32_t Scope::sNumTabs = 0;

Scope::Scope(uint32_t size) :
	mMap(),
	mVector(size),
	mParent(nullptr)
{
}

//-----------------------------------------------------------------

Scope::Scope(const Scope& other) :
	Scope()
{
	performDeepCopy(other);
}

//-----------------------------------------------------------------

Scope& Scope::operator=(const Scope& other)
{
	if(this != &other)
	{
		// blow away all data that's already here, then copy all data
		clear();
		performDeepCopy(other);
	}

	return *this;
}

//-----------------------------------------------------------------

Scope* Scope::copy()
{
	return new Scope(*this);
}

//-----------------------------------------------------------------

Scope::Scope(Scope&& other) :
	Scope()
{
	operator=(std::move(other));
}

//-----------------------------------------------------------------

Scope& Scope::operator=(Scope&& other)
{
	if(this != &other)
	{
		// detach from parent and clear all data
		orphan();
		clear();

		// other's parent adopts this object using other's name, then detach other from parent
		if(other.mParent != nullptr)
		{
			other.mParent->adopt(other.mParent->findName(&other), *this);
			other.orphan();
		}

		// move chains and inserted pair pointers from other into this
		mVector = std::move(other.mVector);
		mMap = std::move(other.mMap);

		// all of other's former child tables have to be reparented to this
		for(auto& kvPair : mVector)
		{
			Datum& datum = kvPair->second;
			if(datum.type() == Datum::DatumType::Table)
			{
				for(uint32_t i = 0; i < datum.size(); ++i)
				{
					datum[i].mParent = this;
				}
			}
		}
	}

	return *this;
}

//-----------------------------------------------------------------

Scope::~Scope()
{
	// have to detach from parent in case it's not being destroyed too
	orphan();
	clear();
}

//-----------------------------------------------------------------

bool Scope::Equals(const RTTI* rhs) const
{
	if(this == rhs)
	{
		return true;
	}

	// have to get other as a Scope if we can
	Scope* other = rhs->As<Scope>();
	if(other == nullptr)
	{
		return false;
	}

	return operator==(*other);
}

//-----------------------------------------------------------------

bool Scope::operator==(const Scope& other) const
{
	bool result = true;
	if(this != &other)
	{
		if(size() != other.size())
		{
			// different sizes, can't be equal
			result = false;
		}
		else
		{
			// same sizes -- check map equality
			result = mMap == other.mMap;
		}
	}

	return result;
}

//-----------------------------------------------------------------

bool Scope::operator!=(const Scope& other) const
{
	return !operator==(other);
}

//-----------------------------------------------------------------

Datum& Scope::operator[](const string& key)
{
	return append(key);
}

//-----------------------------------------------------------------

Datum& Scope::operator[](const uint32_t index)
{
	return mVector[index]->second;
}

//-----------------------------------------------------------------

Datum& Scope::at(const uint32_t index)
{
	return operator[](index);
}

//-----------------------------------------------------------------

Datum* Scope::find(const string& key) const
{
	Datum* datumPtr = nullptr;

	// try to find the string key in the map
	MapIter iter = mMap.find(key);
	if(iter != mMap.end())
	{
		// the key was found, return its Datum's address
		datumPtr = &(iter->second);
	}

	return datumPtr;
}

//-----------------------------------------------------------------

Datum* Scope::search(const string& key, const Scope** outScope) const
{
	const Scope* currentScope = this;
	Datum* findResult = nullptr;

	// walk the ancestry chain until we find the key or run out of ancestors
	while(currentScope != nullptr)
	{
		findResult = currentScope->find(key);
		if(findResult != nullptr)
		{
			break;
		}

		currentScope = currentScope->mParent;
	}

	// set Scope that owns the key if we can
	if(outScope != nullptr)
	{
		*outScope = currentScope;
	}

	return findResult;
}

//-----------------------------------------------------------------

string Scope::findName(Scope* const other) const
{
	string returnStr = "";
	for(auto& kvPair : mVector)
	{
		string& name = kvPair->first;
		Datum& datum = kvPair->second;

		if(datum.type() != Datum::DatumType::Table)
		{
			continue;
		}

		// for each field with type for Scope...
		// check each Table datum's entries for the address we want
		for(uint32_t i = 0; i < datum.size(); ++i)
		{
			if(&datum[i] == other)
			{
				returnStr = name;
				break;
			}
		}
	}

	return returnStr;
}

//-----------------------------------------------------------------

Datum& Scope::append(const string& key)
{
	if(key == "")
	{
		throw exception("Error -- cannot use an empty key name to append a data field!");
	}

	bool didInsert;
	MapIter iter = mMap.insert(PairType(key, Datum()), &didInsert);
	if(didInsert)
	{
		// push back new entries (address of the pair type) to the vector
		mVector.pushBack(&*iter);
	}

	return iter->second;
}

//-----------------------------------------------------------------

Scope& Scope::appendScope(const string& key)
{
	if(key == "")
	{
		throw exception("Error -- cannot use an empty key name to append a new child Scope!");
	}

	// attempt to append key (returns new Datum or one already there)
	Datum& datum = append(key);

	if(datum.type() != Datum::DatumType::Unknown && datum.type() != Datum::DatumType::Table)
	{
		// can only append scopes to unknown or table types
		throw exception("Error -- cannot append a scope to a field that is not of type Table!");
	}

	if(datum.isExternal())
	{
		// can only append scopes to internal datums
		throw exception("Error -- cannot append a scope to a field storing external data!");
	}

	// push back new scope to the datum that append returns
	Scope* scope = new Scope();
	scope->mParent = this;
	datum.pushBack(*scope);

	return *scope;
}

//-----------------------------------------------------------------

void Scope::clear()
{
	// destroy all child scopes
	for(auto& kvPair : mVector)
	{
		Datum& datum = kvPair->second;
		if(datum.type() != Datum::DatumType::Table)
		{
			continue;
		}

		for(; !datum.isEmpty(); )
		{
			// nulling the child scope's parent eliminates the orphan during deletion
			// we then remove the entry from the datum while where here
			Scope& childScope = datum[0];
			childScope.mParent = nullptr;
			datum.removeAt(0);

			// recursive delete the child scope
			//		detached from 'this', so no orphan performed (and no double-search)
			delete &childScope;
		}
	}

	// clear the map and vector of all allocated elements
	mMap.clear();
	mVector.clear();
	mVector.shrinkToFit();
}

//-----------------------------------------------------------------

void Scope::adopt(const string& key, Scope& child)
{
	if(key == "")
	{
		throw exception("Error -- cannot use an empty key name to adopt a child Scope!");
	}

	// can't adopt ourselves or one of our children
	if(&child != this && child.getParent() != this)
	{
		// get or create a new Datum for the key
		Datum& datum = append(key);

		if(datum.type() != Datum::DatumType::Unknown && datum.type() != Datum::DatumType::Table)
		{
			// can only adopt scopes to unknown or table types
			throw exception("Error -- cannot adopt a scope to a field that is not of type Table!");
		}

		if(datum.isExternal())
		{
			// can only adopt scopes to internal datums
			throw exception("Error -- cannot adopt a scope to a field storing external data!");
		}

		child.orphan();
		child.mParent = this;
		datum.pushBack(child);
	}
}

//-----------------------------------------------------------------

void Scope::orphan()
{
	if(mParent != nullptr)
	{
		// step thru inserted elements in parent
		for(auto& parentPair : mParent->mVector)
		{
			Datum& datum = parentPair->second;

			// attempt to remove the current Scope from the current Datum if it's of type Table
			//		removing an object not in the Datum is a noop, so we're safe in trying to remove it from wherever
			if(datum.type() == Datum::DatumType::Table)
			{
				if(datum.remove(*this))
				{
					break;
				}
			}
		}

		mParent = nullptr;
	}
}

//-----------------------------------------------------------------

Scope::TableVector::Iterator Scope::begin()
{
	return mVector.begin();
}

//-----------------------------------------------------------------

Scope::TableVector::Iterator Scope::end()
{
	return mVector.end();
}

//-----------------------------------------------------------------

bool Scope::isAncestor(const Scope& scope) const
{
	bool result = false;

	const Scope* currentScope = this;
	while(currentScope != nullptr)
	{
		if(currentScope == &scope)
		{
			result = true;
			break;
		}

		currentScope = currentScope->mParent;
	}

	return result;
}

//-----------------------------------------------------------------

uint32_t Scope::size() const
{
	return mVector.size();
}

//-----------------------------------------------------------------

Scope* Scope::getParent() const
{
	return mParent;
}

//-----------------------------------------------------------------

bool Scope::isEmpty() const
{
	return mVector.isEmpty();
}

//-----------------------------------------------------------------

string Scope::ToString() const
{
	// printing scope, add tag and increase indentation of contained fields
	string returnStr = "Scope:\n";
	++Scope::sNumTabs;

	for(auto& kvPair : mVector)
	{
		string& name = kvPair->first;
		Datum& datum = kvPair->second;

		// add field name in quotes, tab for value
		returnStr.append(addTabs() + "\"" + name + "\"\t");

		for(uint32_t i = 0; i < datum.size(); ++i)
		{
			// append each element in the Datum at the current key
			returnStr.append(datum.toString(i));
			if(i < datum.size() - 1)
			{
				returnStr.append(", ");
			}
		}
		returnStr.append("\n");
	}

	// decrease indentation 
	--Scope::sNumTabs;
	returnStr.append(addTabs() + "/Scope");

	return returnStr;
}

//-----------------------------------------------------------------

void Scope::performDeepCopy(const Scope& other)
{
	mVector.reserve(other.mVector.capacity());

	// step through all inserted fields of other
	for(auto& kvPair : other.mVector)
	{
		string& name = kvPair->first;
		Datum& datum = kvPair->second;

		if(datum.isEmpty())
		{
			append(name) = datum;
		}
		else
		{
			if(datum.type() != Datum::DatumType::Table)
			{
				append(name) = datum;
			}
			else
			{
				for(uint32_t i = 0; i < datum.size(); ++i)
				{
					// adopting a virtual copy-constructed pointer preserves the true type of the child
					adopt(name, *datum[i].copy());
				}
			}
		}	
	}
}

//-----------------------------------------------------------------

string Scope::addTabs() const
{
	string tabs = "";
	for(uint32_t i = 0; i < Scope::sNumTabs; ++i)
	{
		tabs.append("\t");
	}

	return tabs;
}
