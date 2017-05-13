
namespace DOGEngine
{

#pragma region SList

	template <typename T>
	SList<T>::SList() :
		mCount(0),
		mFront(nullptr),
		mBack(nullptr)
	{
	}

	//-----------------------------------------------------------------

	template <typename T>
	SList<T>::SList(const SList& other) :
		mCount(0),
		mFront(nullptr),
		mBack(nullptr)
	{
		performDeepCopy(other);
	}

	//-----------------------------------------------------------------

	template <typename T>
	SList<T>& SList<T>::operator=(const SList& other)
	{
		// if not a self-assignment, we can actually deepcopy
		if(this != &other)
		{
			// we have to clear whatever may be in this list before copying
			clear();
			performDeepCopy(other);
		}

		return *this;
	}

	//-----------------------------------------------------------------

	template <typename T>
	SList<T>::SList(SList&& other) :
		mCount(other.mCount),
		mFront(other.mFront),
		mBack(other.mBack)
	{
		other.mCount = 0;
		other.mFront = nullptr;
		other.mBack = nullptr;
	}

	//-----------------------------------------------------------------

	template <typename T>
	SList<T>& SList<T>::operator=(SList&& other)
	{
		if(&other != this)
		{
			clear();

			mCount = other.mCount;
			mFront = other.mFront;
			mBack = other.mBack;

			other.mCount = 0;
			other.mFront = nullptr;
			other.mBack = nullptr;
		}

		return *this;
	}

	//-----------------------------------------------------------------

	template <typename T>
	SList<T>::~SList()
	{
		// we need to deallocate all Nodes that this list may contain
		clear();
	}

	//-----------------------------------------------------------------

	template <typename T>
	typename SList<T>::Iterator SList<T>::pushFront(const T& data)
	{
		if(isEmpty())
		{
			mFront = new Node(data);
			mBack = mFront;
		}
		else
		{
			mFront = new Node(data, mFront);
		}

		mCount++;

		return Iterator(this, mFront);
	}

	//-----------------------------------------------------------------

	template <typename T>
	void SList<T>::popFront()
	{
		if(!isEmpty())
		{
			// list is not empty, can remove element
			Node* oldFront = mFront;
			mFront = oldFront->mNext;
			delete oldFront;

			mCount--;

			if(isEmpty())
			{
				// list is now empty, reassign back to null as sanity check
				mBack = nullptr;
			}
		}
	}

	//-----------------------------------------------------------------

	template <typename T>
	T& SList<T>::front()
	{
		if(isEmpty())
		{
			throw std::exception("Error -- cannot return the front of an empty list!");
		}

		return mFront->mData;
	}

	//-----------------------------------------------------------------

	template <typename T>
	const T& SList<T>::front() const
	{
		if(isEmpty())
		{
			throw std::exception("Error -- cannot return the front of an empty list!");
		}

		return mFront->mData;
	}

	//-----------------------------------------------------------------

	template <typename T>
	typename SList<T>::Iterator SList<T>::pushBack(const T& data)
	{
		if(isEmpty())
		{
			mBack = new Node(data);
			mFront = mBack;
		}
		else
		{
			mBack->mNext = new Node(data);
			mBack = mBack->mNext;
		}

		mCount++;

		return Iterator(this, mBack);
	}

	//-----------------------------------------------------------------

	template <typename T>
	T& SList<T>::back()
	{
		if(isEmpty())
		{
			throw std::exception("Error -- cannot return the back of an empty list!");
		}

		return mBack->mData;
	}

	//-----------------------------------------------------------------

	template <typename T>
	const T& SList<T>::back() const
	{
		if(isEmpty())
		{
			throw std::exception("Error -- cannot return the back of an empty list!");
		}

		return mBack->mData;
	}

	//-----------------------------------------------------------------

	template <typename T>
	void SList<T>::clear()
	{
		while(!isEmpty())
		{
			// remove elements until list is empty
			popFront();
		}
	}

	//-----------------------------------------------------------------

	template <typename T>
	std::uint32_t SList<T>::size() const
	{
		return mCount;
	}

	//-----------------------------------------------------------------

	template <typename T>
	bool SList<T>::isEmpty() const
	{
		return mCount == 0;
	}

	//-----------------------------------------------------------------

	template <typename T>
	typename SList<T>::Iterator SList<T>::begin()
	{
		// begin is an Iterator belonging to this list and pointing to mFront
		//		if the list is empty, mFront is null and begin == end
		return Iterator(this, mFront);
	}

	//-----------------------------------------------------------------

	template <typename T>
	typename const SList<T>::Iterator SList<T>::begin() const
	{
		// begin is an Iterator belonging to this list and pointing to mFront
		//		if the list is empty, mFront is null and begin == end
		return Iterator(this, mFront);
	}

	//-----------------------------------------------------------------

	template <typename T>
	typename SList<T>::Iterator SList<T>::end()
	{
		// end is an Iterator belonging to this list and pointing to no Node
		return Iterator(this, nullptr);
	}

	//-----------------------------------------------------------------

	template <typename T>
	typename const SList<T>::Iterator SList<T>::end() const
	{
		// end is an Iterator belonging to this list and pointing to no Node
		return Iterator(this, nullptr);
	}

	//-----------------------------------------------------------------

	template <typename T>
	typename SList<T>::Iterator SList<T>::insertAfter(typename const SList<T>::Iterator& iter, const T& data)
	{
		if(iter.mOwner != this)
		{
			throw std::exception("Error -- cannot insert data after an Iterator that does not belong to this list!");
		}

		Iterator returnIter;
		if(iter == end() || iter.mNode == mBack)
		{
			// Iterator is at or passed end of list, pushBack so back is reassigned
			returnIter = pushBack(data);
		}
		else
		{
			// otherwise, we create a new Node after the given Iterator
			iter.mNode->mNext = new Node(data, iter.mNode->mNext);
			mCount++;

			returnIter = Iterator(this, iter.mNode->mNext);
		}

		return returnIter;
	}

	//-----------------------------------------------------------------

	template <typename T>
	typename SList<T>::Iterator SList<T>::find(const T& data)
	{
		SList<T>::Iterator iter;

		SList<T>::Iterator endIter = end();
		for(iter = begin(); iter != endIter; ++iter)
		{
			// we can break and return the current Iterator when we find a Node with matching data
			//		iter will equal end() if no match is found, and we return that
			if(*iter == data)
			{
				break;
			}
		}

		return iter;
	}

	//-----------------------------------------------------------------

	template <typename T>
	typename const SList<T>::Iterator SList<T>::find(const T& data) const
	{
		SList<T>::Iterator iter;

		SList<T>::Iterator endIter = end();
		for(iter = begin(); iter != endIter; ++iter)
		{
			// we can break and return the current Iterator when we find a Node with matching data
			//		iter will equal end() if no match is found, and we return that
			if(*iter == data)
			{
				break;
			}
		}

		return iter;
	}

	//-----------------------------------------------------------------

	template <typename T>
	bool SList<T>::remove(const T& data)
	{	
		Iterator iter = find(data);
		return remove(iter);
	}

	//-----------------------------------------------------------------

	template <typename T>
	bool SList<T>::remove(typename SList<T>::Iterator& iter)
	{
		if(iter.mOwner != this)
		{
			throw exception("Error -- cannot remove an Iterator that does not belong to this list!");
		}

		bool result = false;
		if(iter != end())
		{
			if(mCount == 1)
			{
				// removing the only object from the list
				delete mFront;
				mFront = mBack = nullptr;
			}
			else
			{
				Node* currNode = iter.mNode;
				Node* nextNode = iter.mNode->mNext;
				
				// removing back, get the previous node and set it to back
				if(currNode == mBack)
				{
					for(nextNode = mFront; nextNode->mNext != currNode; nextNode = nextNode->mNext);
					nextNode->mNext = nullptr;
					mBack = nextNode;
					delete currNode;
				}

				// removing any other node
				else
				{
					// if next was back, we need to reassign back
					if(nextNode == mBack)
					{
						mBack = currNode;
					}

					// replace data in current, delete next
					currNode->mData = nextNode->mData;
					currNode->mNext = nextNode->mNext;
					delete nextNode;
				}
			}

			mCount--;
			result = true;
		}

		return result;
	}

	//-----------------------------------------------------------------

	template <typename T>
	void SList<T>::performDeepCopy(const SList& other)
	{
		for(T& data : other)
		{
			// stepping through 'other' list from front to back,
			// so we need to add each new element to the back of this list to preserve order
			pushBack(data);
		}
	}

#pragma endregion

	//=================================================================

#pragma region SList::Iterator

	template <typename T>
	SList<T>::Iterator::Iterator() :
		mOwner(nullptr),
		mNode(nullptr)
	{
	}

	//-----------------------------------------------------------------

	template <typename T>
	SList<T>::Iterator::Iterator(const SList* owner, Node* node) :
		mOwner(owner),
		mNode(node)
	{
	}

	//-----------------------------------------------------------------

	template <typename T>
	SList<T>::Iterator::Iterator(const Iterator& other) :
		mOwner(other.mOwner),
		mNode(other.mNode)
	{
	}

	//-----------------------------------------------------------------

	template <typename T>
	SList<T>::Iterator::~Iterator()
	{
	}

	//-----------------------------------------------------------------

	template <typename T>
	typename SList<T>::Iterator& SList<T>::Iterator::operator=(const Iterator& other)
	{
		if(this != &other)
		{
			// if this is not a self-assignment, change to what this Iterator points
			mOwner = other.mOwner;
			mNode = other.mNode;
		}

		return *this;
	}

	//-----------------------------------------------------------------

	template <typename T>
	bool SList<T>::Iterator::operator==(const Iterator& other) const
	{
		// the Iterators are the same if the owners AND the nodes are the same
		return mOwner == other.mOwner && mNode == other.mNode;
	}

	//-----------------------------------------------------------------

	template <typename T>
	bool SList<T>::Iterator::operator!=(const Iterator& other) const
	{
		// the Iterators are different if the owners are different OR the nodes are different
		return mOwner != other.mOwner || mNode != other.mNode;
	}

	//-----------------------------------------------------------------

	template <typename T>
	typename SList<T>::Iterator& SList<T>::Iterator::operator++()
	{
		if(mNode != nullptr)
		{
			mNode = mNode->mNext;
		}

		return *this;
	}

	//-----------------------------------------------------------------

	template <typename T>
	typename SList<T>::Iterator SList<T>::Iterator::operator++(int)
	{
		// postincrement operator saves a copy of the object before incrementing
		Iterator savedIterator = *this;

		if(mNode != nullptr)
		{
			mNode = mNode->mNext;
		}

		// we edit this Iterator's stuff, but return the saved copy
		return savedIterator;
	}

	//-----------------------------------------------------------------

	template <typename T>
	T& SList<T>::Iterator::operator*()
	{
		if(mNode == nullptr)
		{
			throw std::exception("Error -- cannot dereference an Iterator pointing to a null location in an SList!");
		}

		return mNode->mData;
	}

	//-----------------------------------------------------------------

	template <typename T>
	const T& SList<T>::Iterator::operator*() const
	{
		if(mNode == nullptr)
		{
			throw std::exception("Error -- cannot dereference an Iterator pointing to a null location in an SList!");
		}

		return mNode->mData;
	}

#pragma endregion

}
