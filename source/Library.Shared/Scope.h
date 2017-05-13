
#pragma once

#include "HashMap.h"
#include "Vector.h"
#include "Datum.h"

#include "RTTI.h"

namespace DOGEngine
{
	/**
	 * Class that stores an associative table of string-Datum
	 * pairs. The Datums may store Scope* in their arrays,
	 * creating a recursive tree of named data fields.
	 *
	 * Scopes also keep an ordered array that tracks the order
	 * of data insertions.
	 */
	class Scope : public RTTI
	{
		RTTI_DECLARATIONS(Scope, RTTI)

	protected:

		typedef std::pair<std::string, Datum> PairType;
		typedef HashMap<std::string, Datum> TableMap;	
		typedef TableMap::Iterator MapIter;
		typedef Vector<PairType*> TableVector;

	public:

		/**
		 * @brief Constructor.
		 *
		 * @param size The number of entries one expects the new
		 *			   Scope to store. The Scope's array allocates
		 *			   this many elements initially. Defaulted to
		 *			   0.
		 */
		explicit Scope(std::uint32_t size = 0);

		/**
		 * @brief Copy constructor. Recursive deep copy the other
		 *		  Scope and all of its children.
		 *
		 * @param other The Scope we are copying.
		 *
		 * @note The parent of 'other' is not copied. This Scope
		 *		 is initialized with no parent.
		 */
		Scope(const Scope& other);

		/**
		 * @brief Assignment operator. Recursive deep copy assignment
		 *		  from the other Scope.
		 *
		 * @param other The Scope we are copying.
		 *
		 * @return Returns a reference to this Scope after assignment.
		 *
		 * @note Assigning clears all memory allocated for this object
		 *		 before copying the new data over.
		 *
		 * @note Assigning does not change this Scope's parent. The
		 *		 parent of 'other' does not take ownership.
		 */
		Scope& operator=(const Scope& other);

		/**
		 * @brief Virtual copy constructor. Deep copies
		 *		  this object.
		 *
		 * @return Returns a deep copy of this object
		 *		   as a pointer to a Scope
		 */
		virtual Scope* copy();

		/**
		 * @brief Move constructor. Moves the data from one Scope into
		 *		  a new Scope. The original Scope becomes empty.
		 *
		 * @param other The Scope being moved.
		 */
		Scope(Scope&& other);

		/**
		 * @brief Move assignment operator. Moves the data from one Scope
		 *		  into another Scope. The original Scope becomes empty.
		 *
		 * @param other The Scope being moved.
		 *
		 * @return Returns a reference to this Scope.
		 */
		Scope& operator=(Scope&& other);

		/**
		 * @brief Destructor. Recursive destruction and deallocation
		 *		  of this Scope and all of its children.
		 *
		 * @note Detaches this Scope from its parent (avoids cases
		 *		 where parents think they own a child that no longer
		 *		 exists).
		 */
		virtual ~Scope();

		/**
		 * @brief Equality method. Compares this Scope with an RTTI.
		 *		  The comparison is recursive.
		 *
		 * @param rhs The RTTI* to which we are comparing.
		 *
		 * @return Returns true if 'rhs' is a Scope with the same
		 *		   named fields and the same data in those fields.
		 *		   Returns false if 'rhs' is not a Scope or the 
		 *		   structure or data of either Scope tree differs.
		 */
		virtual bool Equals(const RTTI* rhs) const override;

		/**
		 * @brief Equality operator. The comparison is recursive.
		 *
		 * @param other The Scope to which we are comparing.
		 *
		 * @return Returns true if 'other' has the same named fields 
		 *		   and the same data in those fields Returns false if 
		 *		   'other' is not a Scope or the structure or data of 
		 *		   either Scope tree differs.
		 */
		bool operator==(const Scope& other) const;

		/**
		 * @brief Inequality operator. The comparison is recursive.
		 *
		 * @param other The Scope to which we are comparing.
		 *
		 * @return Returns true if there is any difference in the
		 *		   hierarchy of Scopes, field names, or data. Returns
		 *		   false if the Scopes are equivalent.
		 */
		bool operator!=(const Scope& other) const;

		/**
		 * @brief Square brackets operator. Gets or creates the Datum
		 *		  associated with the given key.
		 *
		 * @param key The string key to look up.
		 *
		 * @return Returns a reference to a Datum object. If the key
		 *		   is already in the Scope, the Datum already there
		 *		   is returned. If the key is not in the Scope, a new
		 *		   empty Datum returned.
		 *
		 * @exception Throws an exception if the key is an empty
		 *			  string ""
		 */
		Datum& operator[](const std::string& key);

		/**
		 * @brief Square brackets operator. Gets the Datum inserted
		 *		  in the given order.
		 *
		 * @param index The place in the array from which to retrieve
		 *				the Datum.
		 *
		 * @return Returns a reference to a Datum object. The order of
		 *		   this Datum's insertion is the given index.
		 *
		 * @exception Throws an exception if the index is larger than
		 *			  size.
		 */
		Datum& operator[](const std::uint32_t index);

		/**
		 * @brief Gets the Datum inserted in the given order.
		 *
		 * @param index The place in the array from which to retrieve
		 *				the Datum.
		 *
		 * @return Returns a reference to a Datum object. The order of
		 *		   this Datum's insertion is the given index.
		 *
		 * @exception Throws an exception if the index is larger than
		 *			  size.
		 */
		Datum& at(const std::uint32_t index);

		/**
		 * @brief Retrieves the Datum object associated with the given
		 *		  key if it exists.
		 *
		 * @param key The string key to look up.
		 *
		 * @return Returns a pointer to the Datum object if the key is
		 *		   in the Scope. Otherwise, returns nullptr.
		 */
		Datum* find(const std::string& key) const;

		/**
		 * @brief Finds the closest occurance of the given key in this
		 *		  Scope or one of its ancestors.
		 *
		 * @param key The string key to look up.
		 * @param outScope Passed by reference for returning, if provided.
		 *				   Points to the Scope that contains the closest
		 *				   occurance of the given key.
		 *
		 * @return Returns a pointer to the Datum object if the key is in
		 *		   the Scope. Otherwise, returns nullptr.
		 */
		Datum* search(const std::string& key, const Scope** outScope = nullptr) const;

		/**
		 * @brief Retrieves the field name of the given child scope.
		 *
		 * @param other The Scope whose name for which we are looking.
		 *
		 * @return Returns the name of the field that contains the given
		 *		   Scope if it is a child of this Scope. If the given
		 *		   Scope is not a direct child of this Scope, an empty
		 *		   string is returned ""
		 */
		std::string findName(Scope* const other) const;

		/**
		 * @brief Gets or creates the Datum associated with the given 
		 *		  key.
		 *
		 * @param key The string key to look up.
		 *
		 * @return Returns a reference to a Datum object. If the key
		 *		   is already in the Scope, the Datum already there
		 *		   is returned. If the key is not in the Scope, a new
		 *		   empty Datum returned.
		 *
		 * @exception Throws an exception if the key is an empty
		 *			  string ""
		 */
		Datum& append(const std::string& key);

		/**
		 * @brief Attaches a new Scope as a child of this Scope.
		 *
		 * @param key The string key at which we will attach the
		 *			  new child.
		 *
		 * @return Returns a reference to the new Scope object.
		 *
		 * @note Use this method to append child Scopes instead of
		 *		 normal append() and Datum assignment. This method
		 *		 ensures responsibility for Scopes falls with their
		 *		 parent Scopes.
		 * @note If the key is already a part of the Scope and stores
		 *		 tables there, the new Scope is added to the back of
		 *		 that Datum's array. If the key is not in the Scope,
		 *		 a new entry is created and the Scope is added to it.
		 *
		 * @exception Throws exception if the key is an empty string
		 *			  ""
		 * @exception Throws exception if a Datum exists at the given
		 *			  key that is not of type Table
		 */
		Scope& appendScope(const std::string& key);

		/**
		 * @brief Clears all allocated memory of this object, including
		 *		  child Scopes. Recursive.
		 */
		void clear();

		/**
		 * @brief Attaches an already-existing Scope as a child
		 *		  of this Scope.
		 *
		 * @param key The string key at which we will attach the
		 *			  new child.
		 * @param child The incoming Scope.
		 *
		 * @note This method is very similar to Scope::appendScope in
		 *		 purpose, except that this deals specifically with
		 *		 Scopes that have already been created whereas
		 *		 appendScope creates new objects.
		 *
		 * @exception Throws exception if the key is an empty string
		 *			  ""
		 * @exception Throws exception if a Datum exists at the given
		 *			  key that is not of type Table
		 */
		void adopt(const std::string& key, Scope& child);

		/**
		 * @brief Detaches this Scope from its parent. The parent is
		 *		  no longer responsible for this Scope or its children.
		 */
		void orphan();

		/**
		 * @brief Gets the starting iterator for the Scope.
		 *
		 * @return Returns a Vector::Iterator pointing to the start
		 *		   of mVector.
		 */
		TableVector::Iterator begin();

		/**
		 * @brief Gets the ending iterator for the Scope.
		 *
		 * @return Returns a Vector::Iterator pointing to the end
		 *		   of mVector.
		 */
		TableVector::Iterator end();

		/**
		 * @brief Says whether the given Scope is an ancestor of this.
		 *
		 * @param scope The Scope we are testing child-relation to.
		 *
		 * @return Returns true if the given Scope is an ancestor on the
		 *		   parent chain. Otherwise, returns false.
		 *
		 * @note Scopes are considered their own ancestor (scope == this)
		 */
		bool isAncestor(const Scope& scope) const;

		/**
		 * @brief Retrieves the number of inserted string-Datum pairs.
		 *
		 * @return Returns the number of string-Datum pairs that have
		 *		   been added to this Scope.
		 */
		std::uint32_t size() const;

		/**
		 * @brief Retrieves the address of this Scope's parent.
		 *
		 * @return Returns the address of this Scope's parent.
		 */
		Scope* getParent() const;

		/**
		 * @brief Says whether this Scope has any inserted string-Datum
		 *		  pairs.
		 *
		 * @return Returns true if no items have been added. Otherwise,
		 *		   returns false.
		 */
		bool isEmpty() const;

		/**
		 * @brief Prints the contents of this Scope. This is a recursive
		 *		  call that also prints this Scope's children's contents.
		 *
		 * @return Returns the string representing the data in this Scope.
		 */
		virtual std::string ToString() const override;

	protected:

		TableMap mMap;
		TableVector mVector;

		Scope* mParent;

	private:

		/**
		 * @brief Handles the logic for recursive deep copies of Scopes.
		 *
		 * @param other The Scope we are copying.
		 */
		void performDeepCopy(const Scope& other);

		/**
		 * @brief Helper function for Scope::ToString that prints tabs
		 *		  according to the nesting of the current Scope in the
		 *		  hierarchy relative to the original ToString call.
		 *
		 * @return Returns a series of tabs '\t' equal to the nested
		 *		   level of the current Scope relative to the original
		 *		   Scope::ToString call.
		 */
		std::string addTabs() const;

		static std::uint32_t sNumTabs;
	};
}
