
#include "pch.h"
#include "PendingDelete.h"

#include "World.h"

using namespace DOGEngine;
using namespace std;

PendingDelete::PendingDelete() :
	mPending()
{
}

//-----------------------------------------------------------------

PendingDelete::PendingDelete(PendingDelete&& other) :
	mPending(std::move(other.mPending))
{
}

//-----------------------------------------------------------------

PendingDelete& PendingDelete::operator=(PendingDelete&& other)
{
	if(this != &other)
	{
		mPending = std::move(other.mPending);
	}

	return *this;
}

//-----------------------------------------------------------------

PendingDelete::~PendingDelete()
{
}

//-----------------------------------------------------------------

void PendingDelete::enqueue(Scope& scope)
{
	lock_guard<recursive_mutex> lock(mMutex);

	// can only insert non-World Scopes and those not already pending delete
	if(!scope.Is(World::TypeIdClass()) && !isPendingDelete(scope))
	{
		// partition off the inserted elements that are children of incoming scope
		uint32_t last = mPending.size();
		for(uint32_t start = 0; start < last; ++start)
		{
			while(mPending[start]->isAncestor(scope) && start != last)
			{
				std::swap(mPending[start], mPending[--last]);
			}
		}

		// remove partitioned children of incoming scope
		while(last < mPending.size())
		{
			mPending.popBack();
		}

		// insert scope
		mPending.pushBack(&scope);
	}
}

//-----------------------------------------------------------------

void PendingDelete::empty()
{
	lock_guard<recursive_mutex> lock(mMutex);

	// delete objects, then clear
	for(auto& scope : mPending)
	{
		delete scope;
	}
	clear();
}

//-----------------------------------------------------------------

void PendingDelete::clear()
{
	lock_guard<recursive_mutex> lock(mMutex);
	mPending.clear();
}

//-----------------------------------------------------------------

bool PendingDelete::isPendingDelete(Scope& scope)
{
	lock_guard<recursive_mutex> lock(mMutex);

	bool result = false;
	for(uint32_t i = 0; i < mPending.size(); ++i)
	{
		// scope has ancestor in queue (could include itself)
		if(scope.isAncestor(*mPending[i]))
		{
			result = true;
			break;
		}
	}

	return result;
}

//-----------------------------------------------------------------

uint32_t PendingDelete::size()
{
	lock_guard<recursive_mutex> lock(mMutex);
	return mPending.size();
}
