
namespace DOGEngine
{

#pragma region HashMap
	
	template <typename TKey, typename TValue, typename THash, typename TComp>
	HashMap<TKey, TValue, THash, TComp>::HashMap(std::uint32_t numBuckets) :
		mBuckets(numBuckets),
		mSize(0)
	{
		if(numBuckets == 0)
		{
			throw std::exception("Error -- cannot create a HashMap with 0 buckets!");
		}

		// start a new empty chain for each bucket in the hashmap
		for(std::uint32_t i = 0; i < numBuckets; ++i)
		{
			mBuckets.pushBack(ChainType());
		}
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	HashMap<TKey, TValue, THash, TComp>::HashMap(const HashMap& other) :
		mBuckets(other.mBuckets),
		mSize(other.mSize)
	{
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	HashMap<TKey, TValue, THash, TComp>& HashMap<TKey, TValue, THash, TComp>::operator=(const HashMap& other)
	{
		if(this != &other)
		{
			// assignment operator for Vector handles deep copy of data
			mBuckets = other.mBuckets;
			mSize = other.mSize;
		}

		return *this;
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	HashMap<TKey, TValue, THash, TComp>::HashMap(HashMap&& other)
	{
		operator=(std::move(other));
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	HashMap<TKey, TValue, THash, TComp>& HashMap<TKey, TValue, THash, TComp>::operator=(HashMap&& other)
	{
		if(&other != this)
		{
			// blow away all data in this object, then reserve space in this vector using other's size
			mBuckets.clear();
			mBuckets.shrinkToFit();
			mBuckets.reserve(other.mBuckets.size());

			// create new chains and move other's chains to them
			mSize = other.mSize;
			for(std::uint32_t i = 0; i < mBuckets.capacity(); ++i)
			{
				mBuckets.pushBack(ChainType());
				mBuckets[i] = std::move(other.mBuckets[i]);
			}

			other.mSize = 0;
		}

		return *this;
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	HashMap<TKey, TValue, THash, TComp>::~HashMap()
	{
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	bool HashMap<TKey, TValue, THash, TComp>::operator==(const HashMap& other) const
	{
		// we assume the hashmaps are equal, and may be proven false
		bool result = true;
		if(this != &other)
		{
			if(mSize == other.mSize)
			{
				Iterator otherFindIter;
				Iterator otherEndIter = other.end();

				// size is the same, so we check each element in the hashmaps
				// order should not matter, so we just check that all the data here is also there
				for(PairType& kvPair : *this)
				{
					otherFindIter = other.find(kvPair.first);
					if(otherFindIter == otherEndIter)
					{
						// key is not in other, no match
						result = false;
						break;
					}
					if(kvPair.second != otherFindIter->second)
					{
						// value at key not same, no match
						result = false;
						break;
					}
				}
			}
			else
			{
				// different sizes, can't be equal
				result = false;
			}
		}

		return result;
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	bool HashMap<TKey, TValue, THash, TComp>::operator!=(const HashMap& other) const
	{
		return !operator==(other);
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	TValue& HashMap<TKey, TValue, THash, TComp>::operator[](const TKey& key)
	{
		// attempt to insert a default PairType to the list
		//		if key is already in the list, insert() returns an Iterator to the data that's there and the Hashmap is unchanged
		//		if key is not in the list, then the default PairType is inserted
		Iterator dataIter = insert(PairType(key, TValue()));

		// either way, we return a reference to the TData from insert's returned Iterator
		return dataIter->second;
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	const TValue& HashMap<TKey, TValue, THash, TComp>::operator[](const TKey& key) const
	{
		return at(key).second;
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	typename HashMap<TKey, TValue, THash, TComp>::PairType& HashMap<TKey, TValue, THash, TComp>::at(const TKey& key)
	{
		Iterator dataIter = find(key);
		if(dataIter == end())
		{
			throw std::exception("Error -- the TKey argument is not in the list!");
		}

		return *dataIter;
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	typename const HashMap<TKey, TValue, THash, TComp>::PairType& HashMap<TKey, TValue, THash, TComp>::at(const TKey& key) const
	{
		Iterator dataIter = find(key);
		if(dataIter == end())
		{
			throw std::exception("Error -- the TKey argument is not in the list!");
		}

		return *dataIter;
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	typename HashMap<TKey, TValue, THash, TComp>::Iterator HashMap<TKey, TValue, THash, TComp>::find(const TKey& key) const
	{
		// init our return value to the Hashmap end() (overwritten if the data is found in the expected chain)
		Iterator returnIter = end();

		// we need to get the bucket index in which we expect to find the requested data
		std::uint32_t index = mHashFunc(key) % mBuckets.size();

		ChainIter iter;
		ChainIter endIter = mBuckets[index].end();
		for(iter = mBuckets[index].begin(); iter != endIter; ++iter)
		{
			// go through the expected chain, and reset our return value if we find the data
			if(mCompFunc(key, (*iter).first))
			{
				returnIter = Iterator(this, index, iter);
				break;
			}
		}

		return returnIter;
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	typename HashMap<TKey, TValue, THash, TComp>::Iterator HashMap<TKey, TValue, THash, TComp>::insert(const PairType& data, bool* outDidInsert)
	{
		if(outDidInsert != nullptr)
		{
			*outDidInsert = false;
		}

		// insert the data to the Hashmap only if the key is not found
		//		if the data is found, we return the Iterator that find() returns
		Iterator dataIter = find(data.first);
		if(dataIter == end())
		{
			// we need to get the bucket to which we are inserting
			std::uint32_t index = mHashFunc(data.first) % mBuckets.size();

			// we return an Iterator pointing to the inserted element
			dataIter = Iterator(this, index, mBuckets[index].pushBack(data));
			++mSize;

			if(outDidInsert != nullptr)
			{
				*outDidInsert = true;
			}
		}

		return dataIter;
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	void HashMap<TKey, TValue, THash, TComp>::remove(const TKey& key)
	{
		Iterator removeIter = find(key);
		if(removeIter != end())
		{
			mBuckets[removeIter.mIndex].remove(removeIter.mChainIter);
			--mSize;
		}
	}
	
	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	void HashMap<TKey, TValue, THash, TComp>::clear()
	{
		// clear each bucket
		mSize = 0;
		for(ChainType& chain : mBuckets)
		{
			chain.clear();
		}
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	bool HashMap<TKey, TValue, THash, TComp>::containsKey(const TKey& key) const
	{
		// Hashmap contains key if find result is not end()
		return find(key) != end();
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	std::uint32_t HashMap<TKey, TValue, THash, TComp>::size() const
	{
		return mSize;
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	bool HashMap<TKey, TValue, THash, TComp>::isEmpty() const
	{
		return mSize == 0;
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	typename HashMap<TKey, TValue, THash, TComp>::Iterator HashMap<TKey, TValue, THash, TComp>::begin()
	{
		std::uint32_t startIndex = 0;
		ChainIter chainBegin = getNextChainBegin(startIndex);

		return Iterator(this, startIndex, chainBegin);
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	typename const HashMap<TKey, TValue, THash, TComp>::Iterator HashMap<TKey, TValue, THash, TComp>::begin() const
	{
		std::uint32_t startIndex = 0;
		ChainIter chainBegin = getNextChainBegin(startIndex);

		return Iterator(this, startIndex, chainBegin);
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	typename HashMap<TKey, TValue, THash, TComp>::Iterator HashMap<TKey, TValue, THash, TComp>::end()
	{
		// the end Iterator points to the end of the last chain
		std::uint32_t numBuckets = mBuckets.size();
		return Iterator(this, numBuckets - 1, mBuckets[numBuckets - 1].end());
	}
	
	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	typename const HashMap<TKey, TValue, THash, TComp>::Iterator HashMap<TKey, TValue, THash, TComp>::end() const
	{
		// the end Iterator points to the end of the last chain
		std::uint32_t numBuckets = mBuckets.size();
		return Iterator(this, numBuckets - 1, mBuckets[numBuckets - 1].end());
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	typename HashMap<TKey, TValue, THash, TComp>::ChainIter HashMap<TKey, TValue, THash, TComp>::getNextChainBegin(std::uint32_t& index) const
	{
		std::uint32_t numBuckets = mBuckets.size();
		for(; index < numBuckets; ++index)
		{
			// check chains' isEmpty while the index is within range of the buckets Vector
			//		'index' is passed byref, so it value is changed
			if(!mBuckets[index].isEmpty())
			{
				break;
			}
		}

		if(index >= numBuckets)
		{
			// no non-empty bucekts were found, so we have to bring index back in range
			//		in this case, begin() and end() for the chain are the same
			index = numBuckets - 1;
		}

		return mBuckets[index].begin();
	}

#pragma endregion

	//=================================================================

#pragma region HashMapIterator

	template <typename TKey, typename TValue, typename THash, typename TComp>
	HashMap<TKey, TValue, THash, TComp>::Iterator::Iterator() :
		mOwner(nullptr),
		mIndex(0),
		mChainIter(ChainIter())
	{
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	HashMap<TKey, TValue, THash, TComp>::Iterator::Iterator(const HashMap* owner, std::uint32_t index, ChainIter chainIter) :
		mOwner(owner),
		mIndex(index),
		mChainIter(chainIter)
	{
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	HashMap<TKey, TValue, THash, TComp>::Iterator::Iterator(const Iterator& other) :
		mOwner(other.mOwner),
		mIndex(other.mIndex),
		mChainIter(other.mChainIter)
	{
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	HashMap<TKey, TValue, THash, TComp>::Iterator::~Iterator()
	{
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	typename HashMap<TKey, TValue, THash, TComp>::Iterator& HashMap<TKey, TValue, THash, TComp>::Iterator::operator=(const Iterator& other)
	{
		if(this != &other)
		{
			mOwner = other.mOwner;
			mIndex = other.mIndex;
			mChainIter = other.mChainIter;
		}

		return *this;
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	bool HashMap<TKey, TValue, THash, TComp>::Iterator::operator==(const Iterator& other) const
	{
		// mChainIter equivalency depends on which chain each Iterator points to (or place in the Buckets)
		//		so checking the mIndex equivalency is irrelevant
		return mOwner == other.mOwner && mChainIter == other.mChainIter;
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	bool HashMap<TKey, TValue, THash, TComp>::Iterator::operator!=(const Iterator& other) const
	{
		return !operator==(other);
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	typename HashMap<TKey, TValue, THash, TComp>::Iterator& HashMap<TKey, TValue, THash, TComp>::Iterator::operator++()
	{
		if(mOwner == nullptr)
		{
			throw std::exception("Error -- cannot increment an unowned Iterator!");
		}

		// increment the current chain Iterator
		if(++mChainIter == mOwner->mBuckets[mIndex].end())
		{
			// if we're at the current chain's end(), we have to check if the current chain is the last one in the map
			if(mIndex != mOwner->mBuckets.size() - 1)
			{
				// if the current chain is not the last one in the map, we get the begin() of the next non-empty chain
				// this way, the Iterator only points to a chain's end() if it is also pointing to the HashMap's end()
				mChainIter = mOwner->getNextChainBegin(++mIndex);
			}
		}

		return *this;
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	typename HashMap<TKey, TValue, THash, TComp>::Iterator HashMap<TKey, TValue, THash, TComp>::Iterator::operator++(int)
	{
		Iterator savedIter = *this;
		operator++();

		return savedIter;
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	typename HashMap<TKey, TValue, THash, TComp>::PairType& HashMap<TKey, TValue, THash, TComp>::Iterator::operator*()
	{
		if(mOwner == nullptr)
		{
			throw std::exception("Error -- cannot dereference an Iterator that does not belong to a HashMap!");
		}

		if(*this == mOwner->end())
		{
			throw std::exception("Error -- cannot dereference an Iterator that does not point to data!");
		}

		return *mChainIter;
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	typename const HashMap<TKey, TValue, THash, TComp>::PairType& HashMap<TKey, TValue, THash, TComp>::Iterator::operator*() const
	{
		if(mOwner == nullptr)
		{
			throw std::exception("Error -- cannot dereference an Iterator that does not belong to a HashMap!");
		}

		if(*this == mOwner->end())
		{
			throw std::exception("Error -- cannot dereference an Iterator that does not point to data!");
		}

		return *mChainIter;
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	typename HashMap<TKey, TValue, THash, TComp>::PairType* HashMap<TKey, TValue, THash, TComp>::Iterator::operator->()
	{
		return &(operator*());
	}

	//-----------------------------------------------------------------

	template <typename TKey, typename TValue, typename THash, typename TComp>
	typename const HashMap<TKey, TValue, THash, TComp>::PairType* HashMap<TKey, TValue, THash, TComp>::Iterator::operator->() const
	{
		return &(operator*());
	}

#pragma endregion

	//=================================================================

#pragma region Hash Functors

	template <typename TKey>
	inline std::uint32_t HashFunc<TKey>::operator()(const TKey& key) const
	{
		std::uint32_t hashValue = 0;

		const char* byteArray = reinterpret_cast<const char*>(&key);
		for(std::uint32_t i = 0; i < sizeof(key); ++i)
		{
			hashValue += 13 + byteArray[i];
		}

		return hashValue;
	}

	//-----------------------------------------------------------------

	inline std::uint32_t HashFunc<char*>::operator()(char* const& key) const
	{
		std::uint32_t hashValue = 0;

		size_t keyLength = strlen(key);
		for(std::uint32_t i = 0; i < keyLength; ++i)
		{
			hashValue += 13 + key[i];
		}

		return hashValue;
	}

	//-----------------------------------------------------------------

	inline std::uint32_t HashFunc<std::string>::operator()(const std::string& key) const
	{
		std::uint32_t hashValue = 0;
		for(char character : key)
		{
			hashValue += 13 + character;
		}

		return hashValue;
	}

#pragma endregion

	//=================================================================

#pragma region Compare Functors

	template <typename TKey>
	inline bool CompareFunc<TKey>::operator()(const TKey& key, const TKey& otherKey) const
	{
		return key == otherKey;
	}

	//-----------------------------------------------------------------

	inline bool CompareFunc<char*>::operator()(char* const& key, char* const& otherKey) const
	{
		return strcmp(key, otherKey) == 0;
	}

#pragma endregion

}
