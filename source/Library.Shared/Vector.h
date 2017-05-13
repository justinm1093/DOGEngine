
#pragma once

#include "pch.h"

namespace DOGEngine
{
	/**
	 * Templated container class. Stores a dynamically-
	 * sized contiguous array of templated objects for
	 * random access of stored elements.
	 */
	template <typename T>
	class Vector final
	{
	public:

		/**
		 * Iterator class that allows for stepping
		 * through the array that Vector encapsulates.
		 */
		class Iterator
		{
			friend class Vector;
		public:

			/**
			 * @brief Default constructor. Creates a Vector
			 *		  Iterator with no owner and pointing to
			 *		  no data.
			 */
			Iterator();

			/**
			 * @brief Copy constructor. Creates a Vector Iterator
			 *		  whose owner and data location are the same
			 *		  as the given Iterator.
			 *
			 * @param other The Vector Iterator from which we copy
			 *				our member values.
			 */
			Iterator(const Iterator& other);

			/**
			 * @brief Destructor.
			 */
			~Iterator();

			/**
			 * @brief Assignment operator. Sets the owner and data
			 *		  location of an already existing Vector Iterator
			 *		  to those of the given Iterator.
			 *
			 * @param other The Vector Iterator from which we copy
			 *				our member values
			 *
			 * @return Returns a reference to the Iterator to which
			 *		   we are assigning.
			 */
			Iterator& operator=(const Iterator& other);

			/**
			 * @brief Equality operator. Says whether the Vector
			 *		  Iterator is equivalent to the given Iterator.
			 *
			 * @param other The Vector Iterator to which we compare.
			 *
			 * @return Returns true if the owners and data locations
			 *		   of this Iterator and the given Iterator are the
			 *		   same. Otherwise, returns false.
			 */
			bool operator==(const Iterator& other) const;

			/**
			 * @brief Inequality operator. Says whether the Vector
			 *		  Iterator is not equivalent to the given
			 *		  Iterator.
			 *
			 * @param other The Vector Iterator to which we compare.
			 *
			 * @return Returns true if the owners OR the data locations
			 *		   of this Iterator and the given Iterator are
			 *		   different. Otherwise, returns false.
			 */
			bool operator!=(const Iterator& other) const;

			/**
			 * @brief Prefix increment operator. Sets the Vector Iterator
			 *		  to point to the next element in its owning Vector's
			 *		  array.
			 *
			 * @return Returns a reference to the same Iterator on which it
			 *		   was called, pointing to the next element in the array.
			 *
			 * @note Incrementing an unowned Iterator is undefined.
			 */
			Iterator& operator++();

			/**
			 * @brief Postfix increment operator. Sets the Vector
			 *		  Iterator to point to the next element in its
			 *		  owning Vector's array.
			 *
			 * @return Returns a copy of the Iterator pointing to its
			 *		   previous data location.
			 *
			 * @note Incrementing an unowned Iterator is undefined.
			 */
			Iterator operator++(int);

			/**
			 * @brief Dereference operator.
			 *
			 * @return Returns a reference to the data to which this
			 *		   Vector Iterator points.
			 *
			 * @note Dereferencing throws an exception when done to 
			 *		 an Iterator with no owning Vector, or to an
			 *		 Iterator pointing beyond the end of its array.
			 */
			T& operator*();

			/**
			 * @brief Dereference operator.
			 *
			 * @return Returns a const reference to the data to which 
			 *		   this Vector Iterator points.
			 *
			 * @note Dereferencing throws an exception when done to
			 *		 an Iterator with no owning Vector, or to an
			 *		 Iterator pointing beyond the end of its array.
			 */
			const T& operator*() const;

		private:

			/**
			 * @brief Constructor. Creates a new Vector Iterator 
			 *		  belonging to the given Vector.
			 *
			 * @param owner The Vector that owns this Iterator.
			 * @param index The array index of the data to which
			 *				this Iterator points.
			 */
			Iterator(const Vector* owner, std::uint32_t index);

			const Vector* mOwner;
			std::uint32_t mIndex;
		};

		/**
		 * @brief Default constructor. Creates a new empty Vector
		 *		  with the given capacity.
		 *
		 * @param capacity The capacity at which the Vector is
		 *				   initialized. If no capacity is provided,
		 *				   it is defaulted to 16.
		 */
		explicit Vector(const std::uint32_t capacity = 16);

		/**
		 * @brief Copy constructor. Creates a new Vector deep-copied
		 *		  from the given Vector.
		 *
		 * @param other The Vector from which we are copying.
		 */
		Vector(const Vector& other);

		/**
		 * @brief Assignment operator. Clears the Vector's old array
		 *		  before deep-copying the data from the given Vector
		 *		  into this Vector.
		 *
		 * @param other The Vector from which we are copying data.
		 *
		 * @return Returns a reference to this Vector in its copied state.
		 */
		Vector& operator=(const Vector& other);

		/**
		 * @brief Move constructor. Moves the data from one Vector into
		 *		  a new Vector. The original Vector becomes empty.
		 *
		 * @param other The Vector being moved.
		 */
		Vector(Vector&& other);

		/**
		 * @brief Move assignment operator. Moves the data from one Vector
		 *		  into another Vector. The original Vector becomes empty.
		 *
		 * @param other The Vector being moved.
		 *
		 * @return Returns a reference to this Vector.
		 */
		Vector& operator=(Vector&& other);

		/**
		 * @brief Destructor. Destroys all array elements before freeing
		 *		  the Vector's reserved space.
		 */
		~Vector();

		/**
		 * @brief Brackets operator. Retrieves the data from the
		 *		  Vector at the given index.
		 *
		 * @param index The offset from the start of the Vector's
		 *				array from which to retrieve data.
		 *
		 * @return Returns a reference to the requested array element.
		 *
		 * @note Throws an exception if the index is equal to or
		 *		 greater than the array's size (out-of-bounds).
		 */
		T& operator[](const std::uint32_t index);

		/**
		 * @brief Brackets operator. Retrieves the data from the
		 *		  Vector at the given index.
		 *
		 * @param index The offset from the start of the Vector's
		 *				array from which to retrieve data.
		 *
		 * @return Returns a const reference to the requested array 
		 *		   element.
		 *
		 * @note Throws an exception if the index is equal to or
		 *		 greater than the array's size (out-of-bounds).
		 */
		const T& operator[](const std::uint32_t index) const;

		/**
		 * @brief Allocates a block of memory in which the Vector will
		 *		  store its array. Preserves data that may already be
		 *		  in the array.
		 *
		 * @param reserveSize The number of elements for which we are
		 *					  requesting space. For instance, a value 
		 *					  of 50 requests space for a 50 element
		 *					  array of the Vector's templated type.
		 *
		 * @note If the requested space is smaller than the current
		 *		 array allocation, this method does nothing.
		 */
		void reserve(const std::uint32_t reserveSize);

		/**
		 * @brief Shrinks the capacity of the Vector to size.
		 */
		void shrinkToFit();

		/**
		 * @brief Searches for an array element matching the
		 *		  given data.
		 *
		 * @param data The data for which we are searching in
		 *			   the Vector.
		 *
		 * @return Returns an Iterator pointing to the first
		 *		   occurance of the given data in the Vector
		 *		   array. If the data is not in the Vector,
		 *		   the returned Iterator points to end().
		 */
		Iterator find(const T& data) const;

		/**
		 * @brief Retrieves the data from the Vector at the given 
		 *		  index.
		 *
		 * @param index The offset from the start of the Vector's
		 *				array from which to retrieve data.
		 *
		 * @return Returns a reference to the requested array element.
		 *
		 * @note Throws an exception if the index is equal to or
		 *		 greater than the array's size (out-of-bounds).
		 */
		T& at(const std::uint32_t index);

		/**
		 * @brief Retrieves the data from the Vector at the given
		 *		  index.
		 *
		 * @param index The offset from the start of the Vector's
		 *				array from which to retrieve data.
		 *
		 * @return Returns a const reference to the requested array 
		 *		   element.
		 *
		 * @note Throws an exception if the index is equal to or
		 *		 greater than the array's size (out-of-bounds).
		 */
		const T& at(const std::uint32_t index) const;

		/**
		 * @brief Retrieves the data kept in the first element
		 *		  of the Vector's array.
		 *
		 * @return Returns a reference to the data kept in the
		 *		   Vector array's first element.
		 *
		 * @note Throws an exception if invoked against an empty
		 *		 list.
		 */
		T& front();

		/**
		 * @brief Retrieves the data kept in the first element
		 *		  of the Vector's array.
		 *
		 * @return Returns a const reference to the data kept in 
		 *		   the Vector array's first element.
		 *
		 * @note Throws an exception if invoked against an empty
		 *		 list.
		 */
		const T& front() const;

		/**
		 * @brief Retrieves the data kept in the last element
		 *		  of the Vector's array.
		 *
		 * @return Returns a reference to the data kept in the 
		 *		   Vector array's last element.
		 *
		 * @note Throws an exception if invoked against an empty
		 *		 list.
		 */
		T& back();

		/**
		 * @brief Retrieves the data kept in the last element
		 *		  of the Vector's array.
		 *
		 * @return Returns a const reference to the data kept in 
		 *		   the Vector array's last element.
		 *
	 	 * @note Throws an exception if invoked against an empty
		 *		 list.
		 */
		const T& back() const;

		/**
		 * @brief Inserts the given data at the back of the Vector.
		 *
		 * @param data The data being added to the Vector.
		 *
		 * @note If the Vector's size and capacity are the same, 
		 *		 this method automatically allocates more space 
		 *		 for the Vector array using reserve(). The size
		 *		 of the reserved space grows by half of the old
		 *		 allocation's size.
		 */
		void pushBack(const T& data);

		/**
		 * @brief Removes the last element from the Vector.
		 *
		 * @note This method does not shrink the capacity of
		 *		 the vector.
		 */
		void popBack();

		/**
		 * @brief Removes from the Vector the first occurance 
		 *		  of the given data.
		 *
		 * @param data The data being removed from the Vector.
		 *
		 * @return Returns true if the data was in the Vector and
		 *		   was removed. Otherwise, returns false (the
		 *		   Vector is unchanged).
		 */
		bool remove(const T& data);

		/**
		 * @brief Removes from the Vector elements in the range
		 *		  of [start, finish).
		 *
		 * @param start An Iterator pointing to the start of the
		 *				range being removed from the Vector. The
		 *				start of the range is inclusive, and is
		 *				removed.
		 * @param finish An Iterator pointing to the end of the
		 *				 range being removed. The end of the range
		 *				 is exclusive, and is not removed.
		 *
		 * @note Throws an exception if either start or finish do
		 *		 not belong to the invoking Vector. Also throws an
		 *		 exception if start points to a location AFTER
		 *		 finish.
		 *
		 * @note No exceptions are thrown in cases where start or
		 *		 finish point beyond the end of the Vector's array.
		 *		 No exception is thrown if start and finish point to
		 *		 the same location. Nothing is removed and the Vector
		 *		 is unchanged in these cases.
		 */
		void remove(Iterator start, Iterator finish);

		/**
		 * @brief Clears the Vector of all stored elements.
		 */
		void clear();

		/**
		 * @brief Getter for the size of the Vector.
		 *
		 * @return Returns the number of elements currently
		 *		   being stored in the Vector.
		 */
		std::uint32_t size() const;

		/**
		 * @brief Getter for the capacity of the Vector.
		 *
		 * @return Returns the number of items for which
		 *		   the Vector has allocated space.
		 */
		std::uint32_t capacity() const;

		/**
		 * @brief Says whether the Vector has any stored 
		 *		  elements.
		 *
		 * @return Returns true if the Vector has no
		 *		   stored elements and size is 0. Otherwise,
		 *		   returns false.
		 */
		bool isEmpty() const;
		
		/**
		 * @brief Getter for an Iterator pointing to the
		 *		  first element in the Vector.
		 *
		 * @return Returns an Iterator pointing to the
		 *		   first item stored in the Vector.
		 */
		Iterator begin();

		/**
		 * @brief Getter for an Iterator pointing to the
		 *		  first element in the Vector.
		 *
		 * @return Returns a const Iterator pointing to 
		 *		   the first item stored in the Vector.
		 */
		const Iterator begin() const;

		/**
		 * @brief Getter for an Iterator pointing to a
		 *		  location beyond the end of the Vector's
		 *		  array.
		 *
		 * @return Returns an Iterator pointing to a location
		 *		   beyond the last element of the Vector array.
		 */
		Iterator end();

		/**
		 * @brief Getter for an Iterator pointing to a
		 *		  location beyond the end of the Vector's
		 *		  array.
		 *
		 * @return Returns a const Iterator pointing to a location
		 *		   beyond the last element of the Vector array.
		 */
		const Iterator end() const;

	private:

		/**
		 * @brief Reserves space identical in size to the provided
		 *		  Vector's reserved space. Then creates new array
		 *		  elements that are copy-constructed from the other
		 *		  Vector's array elements.
		 *
		 * @param other The Vector from which we are copying data.
		 */
		void performDeepCopy(const Vector& other);

		T* mArray;

		std::uint32_t mSize;
		std::uint32_t mCapacity;
	};
}

#include "Vector.inl"
