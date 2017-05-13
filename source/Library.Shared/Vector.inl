
namespace DOGEngine
{

#pragma region Vector

	template <typename T>
	Vector<T>::Vector(const std::uint32_t capacity) :
		mArray(nullptr),
		mSize(0),
		mCapacity(0)
	{
		// reserve a default capacity
		reserve(capacity);
	}

	//-----------------------------------------------------------------

	template <typename T>
	Vector<T>::Vector(const Vector& other) :
		mArray(nullptr),
		mSize(0),
		mCapacity(0)
	{
		performDeepCopy(other);
	}

	//-----------------------------------------------------------------

	template <typename T>
	Vector<T>& Vector<T>::operator=(const Vector& other)
	{
		if(this != &other)
		{
			// clear all existing data
			clear();
			mCapacity = 0;

			// this reserves space using other's capacity (we're guaranteed to reserve new space with capacity set to 0)
			// this also pushes data from other, which will set the size to the right thing
			performDeepCopy(other);
		}

		return *this;
	}

	//-----------------------------------------------------------------

	template <typename T>
	Vector<T>::Vector(Vector&& other) :
		mArray(other.mArray),
		mSize(other.mSize),
		mCapacity(other.mCapacity)
	{
		other.mArray = nullptr;
		other.mSize = 0;
		other.mCapacity = 0;
	}

	//-----------------------------------------------------------------

	template <typename T>
	Vector<T>& Vector<T>::operator=(Vector&& other)
	{
		if(&other != this)
		{
			// clear data that we may be storing
			clear();
			free(mArray);

			mArray = other.mArray;
			mSize = other.mSize;
			mCapacity = other.mCapacity;

			other.mArray = nullptr;
			other.mSize = 0;
			other.mCapacity = 0;
		}

		return *this;
	}

	//-----------------------------------------------------------------

	template <typename T>
	Vector<T>::~Vector()
	{
		// we only need to free() mArray after clear() because clearing the vector
		// has already destroyed all the elements
		//		there is no object left for 'delete' to invoke a destructor
		clear();
		free(mArray);
	}

	//-----------------------------------------------------------------

	template <typename T>
	T& Vector<T>::operator[](const std::uint32_t index)
	{
		if(index >= mSize)
		{
			throw std::exception("Error -- the provided index was out of bounds for the Vector!");
		}

		return *(mArray + index);
	}

	//-----------------------------------------------------------------

	template <typename T>
	const T& Vector<T>::operator[](const std::uint32_t index) const
	{
		if(index >= mSize)
		{
			throw std::exception("Error -- the provided index was out of bounds for the Vector!");
		}

		return *(mArray + index);
	}

	//-----------------------------------------------------------------

	template <typename T>
	void Vector<T>::reserve(const std::uint32_t reserveSize)
	{
		// only need to reserve space if we're requesting more space than we already have
		if(reserveSize > mCapacity)
		{
			// create new allocation and shallow-copy data from old to new buffers
			T* newArray = reinterpret_cast<T*>(malloc(reserveSize * sizeof(T)));
			memcpy(newArray, mArray, mCapacity * sizeof(T));

			// we're done using the old allocation, so we free it
			//		we don't call the destructors on the old copies of the stored objects because
			//		this would invalidate pointers to heap-allocated data that the new copies may have
			free(mArray);

			mArray = newArray;
			mCapacity = reserveSize;
		}
	}

	//-----------------------------------------------------------------

	template <typename T>
	void Vector<T>::shrinkToFit()
	{
		T* newArray = reinterpret_cast<T*>(malloc(mSize * sizeof(T)));
		memcpy(newArray, mArray, mSize * sizeof(T));

		free(mArray);

		mArray = newArray;
		mCapacity = mSize;
	}

	//-----------------------------------------------------------------

	template <typename T>
	typename Vector<T>::Iterator Vector<T>::find(const T& data) const
	{
		Iterator iter;
		Iterator endIter = end();
		for(iter = begin(); iter != endIter; ++iter)
		{
			if(*iter == data)
			{
				break;
			}
		}

		return iter;
	}

	//-----------------------------------------------------------------

	template <typename T>
	T& Vector<T>::at(const std::uint32_t index)
	{
		if(index >= mSize)
		{
			throw std::exception("Error -- the index is out-of-bounds for the Vector!");
		}

		return mArray[index];
	}
	
	//-----------------------------------------------------------------

	template <typename T>
	const T& Vector<T>::at(const std::uint32_t index) const
	{
		if(index >= mSize)
		{
			throw std::exception("Error -- the index is out-of-bounds for the Vector!");
		}

		return mArray[index];
	}

	//-----------------------------------------------------------------

	template <typename T>
	T& Vector<T>::front()
	{
		if(isEmpty())
		{
			throw std::exception("Error -- can't return the front of an empty Vector!");
		}

		return mArray[0];
	}

	//-----------------------------------------------------------------

	template <typename T>
	const T& Vector<T>::front() const
	{
		if(isEmpty())
		{
			throw std::exception("Error -- can't return the front of an empty Vector!");
		}

		return mArray[0];
	}
	
	//-----------------------------------------------------------------
	
	template <typename T>
	T& Vector<T>::back()
	{
		if(isEmpty())
		{
			throw std::exception("Error -- can't return the back of an empty Vector!");
		}

		return mArray[mSize - 1];
	}

	//-----------------------------------------------------------------

	template <typename T>
	const T& Vector<T>::back() const
	{
		if(isEmpty())
		{
			throw std::exception("Error -- can't return the back of an empty Vector!");
		}

		return mArray[mSize - 1];
	}

	//-----------------------------------------------------------------

	template <typename T>
	void Vector<T>::pushBack(const T& data)
	{
		// the array is as big as our capacity, we need to allocate more space for the array
		//		if capacity is 0, then we create an array of size 1
		if(mSize == mCapacity)
		{
			std::uint32_t reserveSize = std::max<std::uint32_t>(1, mCapacity * 2);
			reserve(reserveSize);
		}

		// create a new element using mSize as an offset from mArray's address
		// this element is destroyed when popped off the Vector with popBack()
		new(mArray + mSize)T(data);
		++mSize;
	}

	//-----------------------------------------------------------------

	template <typename T>
	void Vector<T>::popBack()
	{
		if(!isEmpty())
		{
			// if there is anything in the list, destroy the element at the last index
			mArray[--mSize].~T();
		}
	}

	//-----------------------------------------------------------------

	template <typename T>
	bool Vector<T>::remove(const T& data)
	{
		bool result = false;

		Iterator foundIter = find(data);
		if(foundIter != end())
		{
			Iterator nextIter = foundIter;
			++nextIter;

			// removing a found element is the same as removing a range of 1 starting at the found Iterator
			remove(foundIter, nextIter);
			result = true;
		}

		return result;
	}

	//-----------------------------------------------------------------

	template <typename T>
	void Vector<T>::remove(Iterator start, Iterator finish)
	{
		// problem if the Iterator owners are bad
		if(start.mOwner != this || finish.mOwner != this)
		{
			throw std::exception("Error -- one or both of the Iterators passed to this method does not belong to the Vector!");
		}

		// also a problem if the range is bad
		//		if start and finish are the same, then the below loops do element self-assignment and leave the Vector unchanged
		if(start.mIndex > finish.mIndex)
		{
			throw std::exception("Error -- invalid range, the start Iterator points to a spot in the Vector after the finish Iterator!");
		}

		while(finish.mIndex < mSize)
		{
			*start = *finish;
			++start;
			++finish;
		}

		while(start.mIndex < mSize)
		{
			popBack();
		}
	}

	//-----------------------------------------------------------------

	template <typename T>
	void Vector<T>::clear()
	{
		while(!isEmpty())
		{
			popBack();
		}
	}

	//-----------------------------------------------------------------

	template <typename T>
	std::uint32_t Vector<T>::size() const
	{
		return mSize;
	}

	//-----------------------------------------------------------------

	template <typename T>
	std::uint32_t Vector<T>::capacity() const
	{
		return mCapacity;
	}

	//-----------------------------------------------------------------

	template <typename T>
	bool Vector<T>::isEmpty() const
	{
		return mSize == 0;
	}

	//-----------------------------------------------------------------

	template <typename T>
	typename Vector<T>::Iterator Vector<T>::begin()
	{
		return Iterator(this, 0);
	}
	
	//-----------------------------------------------------------------

	template <typename T>
	typename const Vector<T>::Iterator Vector<T>::begin() const
	{
		return Iterator(this, 0);
	}
	
	//-----------------------------------------------------------------

	template <typename T>
	typename Vector<T>::Iterator Vector<T>::end()
	{
		return Iterator(this, mSize);
	}
	
	//-----------------------------------------------------------------

	template <typename T>
	typename const Vector<T>::Iterator Vector<T>::end() const
	{
		return Iterator(this, mSize);
	}

	//-----------------------------------------------------------------

	template <typename T>
	void Vector<T>::performDeepCopy(const Vector& other)
	{
		// reserve space equal to other's capacity, then copy other's data
		reserve(other.mCapacity);
		for(T& data : other)
		{
			pushBack(data);
		}
	}

#pragma endregion

	//=================================================================

#pragma region Vector::Iterator

	template <typename T>
	Vector<T>::Iterator::Iterator() :
		mOwner(nullptr),
		mIndex(0)
	{
	}

	//-----------------------------------------------------------------

	template <typename T>
	Vector<T>::Iterator::Iterator(const Vector* owner, std::uint32_t index) :
		mOwner(owner),
		mIndex(index)
	{
	}

	//-----------------------------------------------------------------

	template <typename T>
	Vector<T>::Iterator::Iterator(const Iterator& other) :
		mOwner(other.mOwner),
		mIndex(other.mIndex)
	{
	}
	
	//-----------------------------------------------------------------
	
	template <typename T>
	Vector<T>::Iterator::~Iterator()
	{
	}

	//-----------------------------------------------------------------

	template <typename T>
	typename Vector<T>::Iterator& Vector<T>::Iterator::operator=(const Iterator& other)
	{
		if(this != &other)
		{
			mOwner = other.mOwner;
			mIndex = other.mIndex;
		}

		return *this;
	}

	//-----------------------------------------------------------------

	template <typename T>
	bool Vector<T>::Iterator::operator==(const Iterator& other) const
	{
		return mOwner == other.mOwner && mIndex == other.mIndex;
	}
	
	//-----------------------------------------------------------------

	template <typename T>
	bool Vector<T>::Iterator::operator!=(const Iterator& other) const
	{
		return !operator==(other);
	}

	//-----------------------------------------------------------------

	template <typename T>
	typename Vector<T>::Iterator& Vector<T>::Iterator::operator++()
	{
		if(mOwner == nullptr)
		{
			throw std::exception("Error -- cannot increment an Iterator with no owner!");
		}

		if(mIndex < mOwner->mSize)
		{
			// can only increment if we are within the range of the Vector's size
			++mIndex;
		}

		return *this;
	}
	
	//-----------------------------------------------------------------

	template <typename T>
	typename Vector<T>::Iterator Vector<T>::Iterator::operator++(int)
	{
		Iterator savedIterator = *this;
		operator++();

		return savedIterator;
	}

	//-----------------------------------------------------------------

	template <typename T>
	T& Vector<T>::Iterator::operator*()
	{
		// can't dereference an Iterator with no owner
		if(mOwner == nullptr)
		{
			throw std::exception("Error -- can't dereference an Iterator that has no owner!");
		}

		// can't dereference on an out-of-bounds
		if(mIndex >= mOwner->mSize)
		{
			throw std::exception("Error -- can't dereference an Iterator that is out-of-bounds of its Vector!");
		}

		return mOwner->mArray[mIndex];
	}
	
	//-----------------------------------------------------------------

	template <typename T>
	const T& Vector<T>::Iterator::operator*() const
	{
		// can't dereference an Iterator with no owner
		if(mOwner == nullptr)
		{
			throw std::exception("Error -- can't dereference an Iterator that has no owner!");
		}

		// can't dereference on an out-of-bounds
		if(mIndex >= mOwner->mSize)
		{
			throw std::exception("Error -- can't dereference an Iterator that is out-of-bounds of its Vector!");
		}

		return mOwner->mArray[mIndex];
	}

#pragma endregion

}
