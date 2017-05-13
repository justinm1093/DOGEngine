
#pragma once

#include "pch.h"

namespace DOGEngine
{
	/**
	 * Templated container class. Implements a singly-linked
	 * list to store a dynamically-sized list of data.
	 */
	template <typename T>
	class SList final
	{

	private:

		/**
		 * Node struct for the singly-linked list
		 * that SList encapsulates. Stores a pointer
		 * to the next node in the list and an entry
		 * of templated data.
		 */
		typedef struct Node
		{
			/**
			 * @brief Constructor. Creates new Node.
			 *
			 * @param data The data Node stores.
			 * @param next The next Node in the list.
			 */
			Node(const T& data, Node* next = nullptr) : mData(data), mNext(next){}

			Node* mNext;
			T mData;
		} Node;

	public:

		/**
		 * Iterator class that allows for stepping
		 * through the singly-linked list that SList
		 * encapsulates.
		 */
		class Iterator
		{
			friend class SList;
		public:
			
			/**
			 * @brief Default constructor. Creates an
			 *		  uninitialized Iterator with no Node
			 *		  pointer and no owning SList.
			 *
			 * @note The default constructor creates an
			 *		 Iterator with no owner. This is not
			 *		 equivalent to the end() of an SList.
			 */
			Iterator();

			/**
			 * @brief Copy constructor. Creates a new Iterator
			 *		  with the same SList owner and node as the
			 *		  given Iterator.
			 *
			 * @param other The other Iterator from which we
			 *				are copying values.
			 */
			Iterator(const Iterator& other);

			/**
			 * @brief Destructor.
			 */
			~Iterator();

			/**
			 * @brief Assignment operator. Sets the Iterator's
			 *		  owner and current Node to those of the
			 *		  given Iterator.
			 *
			 * @param other The other Iterator from which we
			 *				are copying values.
			 */
			Iterator& operator=(const Iterator& other);

			/**
			 * @brief Equality operator. Says whether the
			 *		  Iterator is equal to another Iterator.
			 *
			 * @param other The other Iterator to which we
			 *				are comparing this Iterator.
			 *
			 * @return Returns true if the Iterators have
			 *		   the same owner and point to the same
			 *		   Node. Otherwise, returns false.
		 	 */
			bool operator==(const Iterator& other) const;

			/**
			 * @brief Inequality operator. Says whether the
			 *		  Iterator is not equal to another Iterator.
			 *
			 * @param other The other Iterator to which we
			 *				are comparing this Iterator.
			 *
			 * @return Returns false if the Iterators have
			 *		   the same owner and point to the same
			 *		   Node. Otherwise, returns true.
			 */
			bool operator!=(const Iterator& other) const;

			/**
			 * @brief Prefix increment operator. Sets the
			 *		  Iterator to point to the next Node in
			 *		  its owning SList.
			 *
			 * @return Returns the same Iterator on which it
			 *		   was called, pointing to the next Node.
			 *
			 * @note Does nothing if called on an Iterator
			 *		 that points to its SList's end.
			 */
			Iterator& operator++();

			/**
			 * @brief Postfix increment operator. Sets the
			 *		  Iterator to point to the next Node.
			 *
			 * @return Returns a copy of the Iterator that
			 *		   points to its Node from before the
			 *		   increment.
			 *
			 * @note Does nothing if called on an Iterator
			 *		 that points to its SList's end.
			 */
			Iterator operator++(int);

			/**
			 * @brief Dereference operator.
			 *
			 * @return Returns a reference to the data stored
			 *		   at the Iterator's current Node.
			 *
			 * @note Throws an exception if the Iterator does
			 *		 not point to a Node (such as the end of
			 *		 an SList). It does not check if the data
			 *		 stored at its Node is valid or not.
			 */
			T& operator*();

			/**
			 * @brief Dereference operator.
			 *
			 * @return Returns a const reference to the data 
			 *		   stored at the Iterator's current Node.
			 *
			 * @note Throws an exception if the Iterator does
			 *		 not point to a Node (such as the end of
			 *		 an SList). It does not check if the data
			 *		 stored at its Node is valid or not.
			 */
			const T& operator*() const;

		private:

			/**
			* @brief Constructor. Initializes a linked
			*		  list belonging to the given SList.
			*
			* @param owner The SList whose Nodes to
			*				which the Iterator will point.
			* @param node The Node to which the Iterator
			*			   will point.
			*/
			Iterator(const SList* owner, Node* node);

			const SList* mOwner;
			Node* mNode;
		};

		/**
		 * @brief Default constructor. Creates an 
		 *		  empty list with null front and 
		 *		  back.
		 */
		SList();

		/**
		 * @brief Copy constructor. Creates a new list
		 *		  that is deep-copied from the given
		 *		  list.
		 *
		 * @param other The SList from which the list
		 *				we are creating will be copied.
		 */
		SList(const SList& other);

		/**
		 * @brief Assignment operator. Deep-copies the
		 *		  given SList into an existing list.
		 *
		 * @param other The SList from which the existing
		 *				list copies its list items.
		 *
		 * @note Note that this operation replaces whatever
		 *		 may already be present in the target list.
		 */
		SList& operator=(const SList& other);

		/**
		 * @brief Move constructor. Moves the allocated data
		 *		  from one SList into a new SList. The other
		 *		  list becomes empty.
		 *
		 * @param other The SList being moved.
		 */
		SList(SList&& other);

		/**
		 * @brief Move assignment operator. Moves the allocated
		 *		  data from one SList into another SList. The other
		 *		  list becomes empty.
		 *
		 * @param other The SList being moved.
		 */
		SList& operator=(SList&& other);

		/**
		 * @brief Destructor. Removes all elements from
		 *		  the list with the clear() method before
		 *		  destroying the list. 
		 *
		 * @note If this is a list of pointers, those pointers 
		 *		 are NOT deallocated as they may be being
		 *		 used elsewhere.
		 */
		~SList();

		/**
		 * @brief Inserts the given item at the
		 *		  front of the list.
		 *
		 * @param data The new item being added
		 *			   to the list.
		 *
		 * @return Returns an Iterator to the
		 *		   inserted data.
		 */
		Iterator pushFront(const T& data);

		/**
		 * @brief Removes the item at the front
		 *		  of the list. 
		 *
		 * @note This method does not return a
		 *		 value. To GET the data kept at 
		 *		 the front of the list, use 
		 *		 front()
		 */
		void popFront();

		/**
		 * @brief Getter for the data at the front
		 *		  of the list.
		 *
		 * @return Returns a reference to the data 
		 *		   at the front of the list.
		 *
		 * @note Note that calling this on an
		 *		 empty list throws an exception.
		 */
		T& front();

		/**
		 * @brief Getter for the data at the front
		 *		  of the list.
		 *
		 * @return Returns a const reference to the
		 *		   data at the front of the list.
		 *
		 * @note Note that calling this on an
		 *		 empty list throws an exception.
		 */
		const T& front() const;

		/**
		 * @brief Inserts the given item at the back
		 *		  of the list.
		 *
		 * @param data The new item being added to
		 *			   the back of the list.
		 *
		 * @return Returns an Iterator to the data
		 *		   that was added.
		 */
		Iterator pushBack(const T& data);

		/**
		 * @brief Getter for the data at the back
		 *		  of the list.
		 *
		 * @return Returns a reference to the data
		 *		  at the back of the list.
		 *
		 * @note Note that calling this on an
		 *		 empty list throws an exception.
		 */
		T& back();

		/**
		 * @brief Getter for the data at the back
		 *		  of the list.
		 *
		 * @return Returns a const reference to the
		 *		   data at the back of the list.
		 *
		 * @note Note that calling this on an
		 *		 empty list throws an exception.
		 */
		const T& back() const;

		/**
		 * @brief Removes all elements from the list.
		 *		  
		 * @note This method removes and deallocates
		 *		 the elements of the list, NOT the
		 *		 pointers that those elements may be
		 *		 storing.
		 */
		void clear();

		/**
		 * @brief Getter for the size of the list.
		 *
		 * @return Returns the number of elements
		 *		   currently stored in the list.
		 */
		std::uint32_t size() const;

		/**
		 * @brief Says whether the SList is empty.
		 *
		 * @return Returns 'true' if the list has
		 *		   no elements. Otherwise returns
		 *		   false.
		 */
		bool isEmpty() const;

		/**
		 * @brief Gets an Iterator for the front
		 *		  of the list.
		 *
		 * @return Returns an SList Iterator that
		 *		   points to the first Node in the
		 *		   list.
		 */
		Iterator begin();

		/**
		 * @brief Gets an Iterator for the front
		 *		  of the list.
		 *
		 * @return Returns a const SList Iterator 
		 *		   that points to the first Node 
		 *		   in the list.
		 */
		const Iterator begin() const;

		/**
		 * @brief Gets an Iterator for the end of 
		 *		  the list.
		 *
		 * @return Returns an SList Iterator that 
		 *		   belongs to this list but points
		 *		   to null memory.
		 */
		Iterator end();

		/**
		 * @brief Gets an Iterator for the end of 
		 *		  the list.
		 *
		 * @return Returns a const SList Iterator
		 *		   that belongs to this list but
		 *		   points to null memory.
		 */
		const Iterator end() const;

		/**
		 * @brief Inserts a new list entry in the
		 *		  place after the given Iterator.
		 *
		 * @param iter The SList Iterator after which
		 *			   we are placing the incoming data.
		 * @param data The item being added to the list.
		 *
		 * @return Returns an Iterator to the inserted
		 *		   data.
		 *
		 * @note Throws an exception if the given Iterator
		 *		 does not belong to the list.
		 */
		Iterator insertAfter(const Iterator& iter, const T& data);

		/**
		 * @brief Searches for a list entry containing the
		 *		  given data.
		 *
		 * @param data The data for which we are searching.
		 *
		 * @return Returns an SList Iterator pointing to
		 *		   the first entry encountered that stores
		 *		   the given data. If the data is not in 
		 *		   the list, it returns the end() Iterator.
		 */
		Iterator find(const T& data);

		/**
		 * @brief Finds in the list the first list entry 
		 *		  that contains the given data.
		 *
		 * @param data The data for which we are searching.
		 *
		 * @return Returns a const SList Iterator pointing
		 *		   to the first entry encountered that
		 *		   stores the given data. If the data is not
		 *		   in the list, it returns the end() Iterator.
	 	 */
		const Iterator find(const T& data) const;

		/**
		 * @brief Removes from the list the first list entry 
		 *		  that contains the given data.
		 *
		 * @param data The data we are removing from the list.
		 *
		 * @return Returns true if the data exists in the
		 *		   list and is removed. Returns false if the
		 *		   given data could not be found (the list is
		 *		   unchanged).
		 */
		bool remove(const T& data);

		/**
		 * @brief Removes from the list the given Iterator.
		 *
		 * @param iter The Iterator being removed.
		 *
		 * @note Throws an exception if the given Iterator
		 *		 does not belong to this list.
		 */
		bool remove(Iterator& iter);

	private:

		/**
		 * @brief Performs a deep copy of the given
		 *		  list. Instead of assigning this list's
		 *		  front and back pointers to the other's,
		 *		  we create new Nodes whose data are the
		 *		  same.
		 *
		 * @param other The SList from which the list
		 *				we are creating will be copied.
		 */
		void performDeepCopy(const SList& other);

		std::uint32_t mCount;

		Node* mFront;
		Node* mBack;
	};
}

#include "SList.inl"
