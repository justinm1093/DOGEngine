
#pragma once

#include "pch.h"

#include "SList.h"
#include "Vector.h"

namespace DOGEngine
{

#pragma region Hash Functors

	/**
	 * Templated functor that provides a default hash for
	 * non-pointer key types.
	 *
	 * You can specialize this template to provide custom
	 * hash functions for HashMap. HashMap expects that
	 * its hash functor implements a const operator().
	 */
	template <typename TKey>
	struct HashFunc
	{
		std::uint32_t operator()(const TKey& key) const;
	};

	/**
	 * Specialized template of HashFunc that implements a
	 * hash function for char* key types.
	 */
	template <>
	struct HashFunc<char*>
	{
		std::uint32_t operator()(char* const& key) const;
	};

	/**
	 * Specialized template of HashFunc that implements a
	 * hash function for std::string key types.
	 */
	template <>
	struct HashFunc<std::string>
	{
		std::uint32_t operator()(const std::string& key) const;
	};

#pragma endregion

#pragma region Compare Functors

	/**
	 * Templated functor that provides a default comparison
	 * of keys.
	 *
	 * You can specialize this template to provide custom
	 * comparison functions for HashMap. HashMap expects that
	 * its comparison functor implements a const operator().
	 */
	template <typename TKey>
	struct CompareFunc
	{
		bool operator()(const TKey& key, const TKey& otherKey) const;
	};

	/**
	 * Specialized template of CompareFunc that implements a
	 * comparison function for char* key types.
	 */
	template <>
	struct CompareFunc<char*>
	{
		bool operator()(char* const& key, char* const& otherKey) const;
	};

#pragma endregion

	/**
	 * Templated container class. Stores a hash map of
	 * unordered, unique key-value pairs.
	 *
	 * Implemented with chaining as a Vector of SLists.
	 */
	template <typename TKey, typename TValue, typename THash = HashFunc<TKey>, typename TComp = CompareFunc<TKey>>
	class HashMap final
	{

	private:

		typedef std::pair<TKey, TValue> PairType;
		typedef SList<PairType> ChainType;
		typedef  Vector<ChainType> Buckets;
		typedef typename ChainType::Iterator ChainIter;

	public:
		
		/**
		 * Iterator class that allows for stepping from entry
		 * to entry in the HashMap.
		 */
		class Iterator
		{
			friend class HashMap;
		public:

			/**
			 * @brief Default constructor. Creates a HashMap
			 *		  Iterator with no owner and pointing to
			 *		  no data.
			 */
			Iterator();

			/**
			 * @brief Copy constructor. Creates a HashMap
			 *		  Iterator whose owner and data point
			 *		  to the given Iterator's.
			 *
			 * @param other The HashMap Iterator from which
			 *				we copy.
			 */
			Iterator(const Iterator& other);
			
			/**
			 * @brief Destructor.
			 */
			~Iterator();

			/**
			 * @brief Assignment operator. Overwrites the
			 *		  Iterator's owner and data with the
			 *		  given Iterator's.
			 *
			 * @param other The HashMap Iterator from which
			 *				we copy.
			 */
			Iterator& operator=(const Iterator& other);

			/**
			 * @brief Equality operator. Says whether the
			 *		  Iterator is equivalent to another.
			 *
			 * @param other The HashMap Iterator with which
			 *				we are checking equality.
			 *
			 * @return Returns true if the two Iterators have
			 *		   the same owners and point to the same
			 *		   location in the same HashMap chain.
			 *		   Otherwise, returns false.
			 */
			bool operator==(const Iterator& other) const;

			/**
			 * @brief Inequality operator. Says whether the
			 *		  Iterator is not equivalent to another.
			 *
			 * @param other The HashMap Iterator with which
			 *				we are checking inequality.
			 *
			 * @return Returns true if the two Iterators do
			 *		   not have the same owners or if they
			 *		   point to different chain locations.
			 *		   Otherwise, returns false.
			 */
			bool operator!=(const Iterator& other) const;

			/**
			 * @brief Prefix increment operator. Sets the
			 *		  Iterator to point to the next data
			 *		  element in the HashMap.
			 *
			 * @return Returns a reference to the changed
			 *		   Iterator.
			 *
			 * @note Incrementing an unowned Iterator throws
			 *		 an exception. Incrementing an Iterator
			 *		 equivalent to HashMap::end() is a no-op.
			 */
			Iterator& operator++();

			/**
			 * @brief Postfix increment operator. Sets the
			 *		  Iterator to point to the next data
			 *		  element in the HashMap.
			 *
			 * @return Returns an Iterator with the values
			 *		   of the invoking Iterator from before
			 *		   the increment.
			 *
			 * @note Incrementing an unowned Iterator throws
			 *		 an exception. Incrementing an Iterator
			 *		 equivalent to HashMap::end() is a no-op.
			 */
			Iterator operator++(int);

			/**
			 * @brief Star dereference operator.
			 *
			 * @return Returns a reference to a std::pair that
			 *		   is the key-value pair of the current
			 *		   HashMap element.
			 */
			PairType& operator*();

			/**
			 * @brief Star dereference operator.
			 *
			 * @return Returns a const reference to a std::pair
			 *		   that is the key-value pair of the current
			 *		   HashMap element.
			 */
			const PairType& operator*() const;

			/**
			 * @brief Arrow operator.
			 *
			 * @return Returns a pointer to the std::pair that is
			 *		   the key-value pair of the current HashMap
			 *		   element. The pointer allows us to directly
			 *		   access the std::pair's first and second
			 *		   items as though the Iterator were the pair
			 *		   itself.
			 */
			PairType* operator->();

			/**
			 * @brief Arrow operator.
			 *
			 * @return Returns a const pointer to the std::pair
			 *		   that is the key-value pair of the current
			 *		   HashMap element. The pointer allows us to
			 *		   directly access the std::pair's first and
			 *		   second items as though the Iterator was
			 *		   the pair itself.
			 */
			const PairType* operator->() const;

		private:

			/**
			 * @brief Constructor. Creates a HashMap Iterator
			 *		  with the given owner, bucket index, and
			 *		  chain position.
			 *
			 * @param owner The HashMap to which this Iterator
			 *				belongs.
			 * @param index The index of the chain to which
			 *				this Iterator points.
			 * @param chainIter The SList Iterator that points
			 *		  to this Iterator's position in a chain.
			 */
			Iterator(const HashMap* owner, std::uint32_t index, ChainIter chainIter);

			const HashMap* mOwner;	
			std::uint32_t mIndex;
			ChainIter mChainIter;
		};

		/**
		 * @brief Constructor. Creates an empty HashMap.
		 *		  If no argument is provided, the number
		 *		  of chains in the HashMap is defaulted
		 *		  to 13.
		 *
		 * @param numBuckets The number of array elements
		 *					 (chains) this HashMap has.
		 *
		 * @note Throws an exception if the number of
		 *		 chains is 0.
		 */
		explicit HashMap(std::uint32_t numBuckets = 13);

		/**
		 * @brief Copy constructor. Creates a HashMap
		 *		  whose size and data are copied from 
		 *		  another.
		 *
		 * @param other The other HashMap from which we
		 *				copy our size and data.
		 */
		HashMap(const HashMap& other);

		/**
		 * @brief Assignment operator. Overwrites the
		 *		  size and data of this HashMap with
		 *		  those of another.
		 *
		 * @param other The other HashMap from which we
		 *				copy our size and data.
		 *
		 * @return Returns a reference to the changed
		 *		   HashMap.
		 */
		HashMap& operator=(const HashMap& other);

		/**
		 * @brief Move constructor. Moves the data from one HashMap
		 *		  into a new HashMap. The original becomes empty.
		 *
		 * @param other The HashMap being moved.
		 */
		HashMap(HashMap&& other);

		/**
		 * @brief Move assignment operator. Moves the data from one
		 *		  HashMap to another HashMap. The original becomes empty.
		 *
		 * @param other the HashMap being moved.
		 *
		 * @return Returns a reference to this HashMap.
		 */
		HashMap& operator=(HashMap&& other);

		/**
		 * @brief Destructor.
		 */
		~HashMap();

		/**
		 * @brief Equality operator.
		 *
		 * @param other The other HashMap.
		 *
		 * @return Returns true if the HashMaps have the
		 *		   same key value pairs in the same order.
		 *		   Otherwise, returns false.
		 */
		bool operator==(const HashMap& other) const;

		/**
		 * @brief Inequality operator.
		 *
		 * @param other The other HashMap.
		 *
		 * @return Returns true if the HashMaps have
		 *		   different sizes or there is a mismatch of
		 *		   data in the map. Otherwise, returns false.
		 */
		bool operator!=(const HashMap& other) const;

		/**
		 * @brief Brackets operator. Finds or inserts
		 *		  a HashMap element with the given key.
		 *
		 * @param key The key for which we are searching.
		 *
		 * @return Returns a reference to the HashMap's
		 *		   value-type associated with 'key'. 
		 *
		 * @note If an element with 'key' exists, this 
		 *		 returns the value associated with that 
		 *		 key. If 'key' is not in the list, then
		 *		 the HashMap creates a new element and
		 *		 returns the value-type for the new
		 *		 element (default-constructed).
		 */
		TValue& operator[](const TKey& key);

		/**
		 * @brief Brackets operator. Finds or inserts
		 *		  a HashMap element with the given key.
		 *
		 * @param key The key for which we are searching.
		 *
		 * @return Returns a const reference to the HashMap's
		 *		   value-type associated with 'key'.
		 *
		 * @note If an element with 'key' exists, this
		 *		 returns the value associated with that
		 *		 key. If 'key' is not in the list, then
		 *		 the HashMap creates a new element and
		 *		 returns the value-type for the new
		 *		 element (default-constructed).
		 */
		const TValue& operator[](const TKey& key) const;

		/**
		 * @brief Gets the std::pair associated with
		 *		  the given key.
		 *
		 * @param key The key for which we are searching.
		 *
		 * @return Returns a reference to the std::pair
		 *		   containing the given key and its
		 *		   associated value.
		 *
		 * @note Throws an exception if the given key is
		 *		 not in the HashMap.
		 */
		PairType& at(const TKey& key);

		/**
		 * @brief Gets the std::pair associated with
		 *		  the given key.
		 *
		 * @param key The key for which we are searching.
		 *
		 * @return Returns a const reference to the 
		 *		   std::pair containing the given key and 
		 *		   its associated value.
		 *
		 * @note Throws an exception if the given key is
		 *		 not in the HashMap.
		 */
		const PairType& at(const TKey& key) const;

		/**
		 * @brief Finds the element in the HashMap with
		 *		  the given key.
		 *
		 * @param key The key for which we are searching.
		 *
		 * @return Returns a HashMap Iterator pointing to
		 *		   the key-value pair with the given key.
		 *		   If the given key is not in the HashMap,
		 *		   the returned Iterator points to
		 *		   HashMap::end().
		 */
		Iterator find(const TKey& key) const;

		/**
		 * @brief Attempts to insert the given std::pair
		 *		  to the HashMap.
		 *
		 * @param data The std::pair that is the key-value
		 *			   pair we are trying to insert.
		 * @param outDidInsert Output variable that says whether
		 *					   the key-value pair was inserted.
		 *
		 * @return Returns an Iterator to the inserted
		 *		   key-value pair if the given key is not
		 *		   already in the HashMap. If the key is in
		 *		   the HashMap, returns an Iterator pointing
		 *		   to the key-value pair already there.
		 */
		Iterator insert(const PairType& data, bool* outDidInsert = nullptr);

		/**
		 * @brief Attempts to remove the HashMap element
		 *		  with the given key. If the key is not
		 *		  in the HashMap, this method is a no-op.
		 *
		 * @param key The key we are requesting be removed
		 *			  from the HashMap.
		 */
		void remove(const TKey& key);

		/**
		 * @brief Clears the HashMap of all stored elements.
		 */
		void clear();

		/**
		 * @brief Says whether the HashMap contains a 
		 *		  key-value pair with the given key.
		 *
		 * @param key The key for which we are searching in
		 *			  the HashMap.
		 *
		 * @return Returns true if the HashMap has a
		 *		   key-value pair with the given key.
		 *		   Otherwise, returns false.
		 */
		bool containsKey(const TKey& key) const;
		
		/**
		 * @brief Getter for the number of inserted elements
		 *		  in the HashMap.
		 *
		 * @return Returns the number of elements that have
		 *		   been added to the HashMap.
		 */
		std::uint32_t size() const;

		/**
		 * @brief Says whether the HashMap has any
		 *		  inserted elements.
		 *
		 * @return Returns true if the HashMap has no
		 *		   inserted elements. Otherwise, returns
		 *		   false.
		 */
		bool isEmpty() const;

		/**
		 * @brief Getter for a HashMap Iterator that points
		 *		  to the first key-value pair stored in the
		 *		  HashMap.
		 * 
		 * @return Returns an Iterator pointing to the first
		 *		   key-value pair stored in the HashMap. If
		 *		   the HashMap is empty, this is equivalent
		 *		   to HashMap::end().
		 *
		 * @note Because the HashMap is unordered, there is
		 *		 no guarantee that this Iterator points to 
		 *		 the item that was inserted first.
		 */
		Iterator begin();

		/**
		 * @brief Getter for a HashMap Iterator that points
		 *		  to the first key-value pair stored in the
		 *		  HashMap.
		 *
		 * @return Returns a const Iterator pointing to the 
		 *		   first key-value pair stored in the HashMap. 
		 *		   If the HashMap is empty, this is equivalent
		 *		   to HashMap::end().
		 *
		 * @note Because the HashMap is unordered, there is
		 *		 no guarantee that this Iterator points to
		 *		 the item that was inserted first.
		 */
		const Iterator begin() const;

		/**
		 * @brief Getter for a HashMap Iterator that points
		 *		  beyond the end of the HashMap's allocated
		 *		  storage space.
		 *
		 * @return Returns an Iterator pointing to no
		 *		   key-value pair in the HashMap.
		 *
		 * @note This Iterator is considered to be the end
		 *		 of the last chain in the HashMap. Note that
		 *		 the last chain is considered to be the last
		 *		 chain in the HashMap array, and not the last
		 *		 chain with inserted items.
		 */
		Iterator end();

		/**
		 * @brief Getter for a HashMap Iterator that points
		 *		  beyond the end of the HashMap's allocated
		 *		  storage space.
		 *
		 * @return Returns a const Iterator pointing to no
		 *		   key-value pair in the HashMap.
	 	 *
		 * @note This Iterator is considered to be the end
		 *		 of the last chain in the HashMap. Note that
		 *		 the last chain is considered to be the last
		 *		 chain in the HashMap array, and not the last
		 *		 chain with inserted items.
		 */
		const Iterator end() const;

	private:

		/**
		 * @brief Getter for the start of the next non-empty
		 *		  SList, starting from the given array index.
		 *
		 * @param index The starting point for the search for
		 *				the next non-empty SList. This index
		 *				is inclusive.
		 *
		 * @return Returns an SList Iterator pointing to the
		 *		   beginning of the first non-empty SList
		 *		   if there was a non-empty SList in the
		 *		   HashMap's array at or beyond the given
		 *		   index. If all SLists at or beyond the index
		 *		   are empty, the returned SList Iterator points
		 *		   to the end of the last bucket.
		 */
		ChainIter getNextChainBegin(std::uint32_t& index) const;

		Buckets mBuckets;
		std::uint32_t mSize;

		THash mHashFunc;
		TComp mCompFunc;
	};
}

#include "HashMap.inl"
