
#pragma once

#include "RTTI.h"

namespace DOGEngine
{
	class Scope;

	/**
	 * A class that stores an array of values.
	 * The values are of a single type, chosen
	 * at runtime.
	 */
	class Datum final
	{
	public:

		/**
		 * Enumeration of possible types of Datum.
		 */
		enum class DatumType
		{
			Integer,
			Float,
			String,
			Pointer,
			Matrix,
			Vector,
			Table,
			Unknown
		};

		/**
		 * @brief Constructor. Constructs the Datum
		 *		  without size, capacity, or type
		 *		  (unless specified).
		 * 
		 * @param type The type of the Datum being
		 *			   created. Defaulted to Unknown.
		 */
		explicit Datum(DatumType type = DatumType::Unknown);

		/**
		 * @brief Copy constructor. Deep copies if
		 *		  'other' owns its memory.
		 *
		 * @param other The Datum from which we are
		 *				copying.
		 */
		Datum(const Datum& other);

		/**
		 * @brief Move constructor. Moves the data from
		 *		  one Datum to a new Datum. The original
		 *		  becomes empty.
		 *
		 * @param other The Datum being moved.
		 */
		Datum(Datum&& other);

		/**
		 * @brief Move assignment operator. Moves the data
		 *		  from one Datum to another Datum. The
		 *		  original becomes empty.
		 *
		 * @param other The Datum being moved.
		 *
		 * @return Returns a reference to this Datum.
		 */
		Datum& operator=(Datum&& other);

		/**
		 * @brief Destructor. Clears memory that the
		 *		  Datum owns.
		 */
		~Datum();

		/**
		 * @brief Assignment operator. Assigns this
		 *		  Datum the values from 'other'.
		 *		  Deep copies if 'other' owns its
		 *		  memory.
		 *
		 * @param other The Datum from which we are
		 *				copying.
		 */
		Datum& operator=(const Datum& other);

		/**
		 * @brief Assignment operator. Assigns an
		 *		  int to this Datum's first index
		 *		  as though it were a scalar.
		 *
		 * @param rhs The value being inserted.
		 *
		 * @note Sets the Datum's type if it is not
		 *		 already set.
		 *
		 * @exception Throws an exception if the Datum
		 *			  stores external memory with size
		 *			  0. Ensure that external Datums
		 *			  have a 0th element before assigning.
		 * @exception Throws an exception if the Datum's
		 *			  type does not match the type of the
		 *			  inserted value.
		 */
		Datum& operator=(const std::int32_t& rhs);

		/**
		 * @brief Assignment operator. Assigns a
		 *		  float to this Datum's first index
		 *		  as though it were a scalar.
		 *
		 * @param rhs The value being inserted.
		 *
		 * @note Sets the Datum's type if it is not
		 *		 already set.
		 *
		 * @exception Throws an exception if the Datum
		 *			  stores external memory with size
		 *			  0. Ensure that external Datums
		 *			  have a 0th element before assigning.
		 * @exception Throws an exception if the Datum's
		 *			  type does not match the type of the
		 *			  inserted value.
		 */
		Datum& operator=(const float& rhs);

		/**
		 * @brief Assignment operator. Assigns a standard
		 *		  string to this Datum's first index
		 *		  as though it were a scalar.
		 *
		 * @param rhs The value being inserted.
		 *
		 * @note Sets the Datum's type if it is not
		 *		 already set.
		 *
		 * @exception Throws an exception if the Datum
		 *			  stores external memory with size
		 *			  0. Ensure that external Datums
		 *			  have a 0th element before assigning.
		 * @exception Throws an exception if the Datum's
		 *			  type does not match the type of the
		 *			  inserted value.
		 */
		Datum& operator=(const std::string& rhs);

		/**
		 * @brief Assignment operator. Assigns an
		 *		  RTTI* to this Datum's first index
		 *		  as though it were a scalar.
		 *
		 * @param rhs The value being inserted.
		 *
		 * @note Sets the Datum's type if it is not
		 *		 already set.
		 *
		 * @exception Throws an exception if the Datum
		 *			  stores external memory with size
		 *			  0. Ensure that external Datums
		 *			  have a 0th element before assigning.
		 * @exception Throws an exception if the Datum's
		 *			  type does not match the type of the
		 *			  inserted value.
		 */
		Datum& operator=(RTTI* const& rhs);

		/**
		 * @brief Assignment operator. Assigns a glm
		 *		  vec4 to this Datum's first index
		 *		  as though it were a scalar.
		 *
		 * @param rhs The value being inserted.
		 *
		 * @note Sets the Datum's type if it is not
		 *		 already set.
		 *
		 * @exception Throws an exception if the Datum
		 *			  stores external memory with size
		 *			  0. Ensure that external Datums
		 *			  have a 0th element before assigning.
		 * @exception Throws an exception if the Datum's
		 *			  type does not match the type of the
		 *			  inserted value.
		 */
		Datum& operator=(const glm::vec4& rhs);

		/**
		 * @brief Assignment operator. Assigns a glm
		 *		  mat4x4 to this Datum's first index
		 *		  as though it were a scalar.
		 *
		 * @param rhs The value being inserted.
		 *
		 * @note Sets the Datum's type if it is not
		 *		 already set.
		 *
		 * @exception Throws an exception if the Datum
		 *			  stores external memory with size
		 *			  0. Ensure that external Datums
		 *			  have a 0th element before assigning.
		 * @exception Throws an exception if the Datum's
		 *			  type does not match the type of the
		 *			  inserted value.
		 */
		Datum& operator=(const glm::mat4x4& rhs);

		/**
		 * @brief Assignment operator. Assigns a
		 *		  Scope* to this Datum's first index
		 *		  as though it were a scalar.
		 *
		 * @param rhs The value being inserted.
		 *
		 * @note Sets the Datum's type if it is not
		 *		 already set.
		 *
		 * @exception Throws an exception if the Datum
		 *			  stores external memory with size
		 *			  0. Ensure that external Datums
		 *			  have a 0th element before assigning.
		 * @exception Throws an exception if the Datum's
		 *			  type does not match the type of the
		 *			  inserted value.
		 */
		Datum& operator=(Scope& rhs);

		/**
		 * @brief Equality operator.
		 *
		 * @param other The Datum to which we are
		 *				comparing this Datum.
		 *
		 * @return Returns true if this Datum and
		 *		   'other' have the same size and
		 *		   store the same data in the same
		 *		   array locations. Otherwise,
		 *		   returns false.
		 */
		bool operator==(const Datum& other) const;

		/**
		 * @brief Equality operator. Compares the first
		 *		  element of this Datum as though it
		 *		  were a scalar value.
		 *
		 * @param rhs The int to which we are comparing.
		 *
		 * @return Returns true if the object at the 0th
		 *		   index is equivalent to 'rhs'. Otherwise,
		 *		   returns false.
		 *
		 * @exception Throws exception if there is nothing
		 *			  at the 0th element.
		 */
		bool operator==(const std::int32_t& rhs) const;

		/**
		 * @brief Equality operator. Compares the first
		 *		  element of this Datum as though it
		 *		  were a scalar value.
		 *
		 * @param rhs The float to which we are comparing.
		 *
		 * @return Returns true if the object at the 0th
		 *		   index is equivalent to 'rhs'. Otherwise,
		 *		   returns false.
		 *
		 * @exception Throws exception if there is nothing
		 *			  at the 0th element.
		 */
		bool operator==(const float& rhs) const;

		/**
		 * @brief Equality operator. Compares the first
		 *		  element of this Datum as though it
		 *		  were a scalar value.
		 *
		 * @param rhs The string to which we are comparing.
		 *
		 * @return Returns true if the object at the 0th
		 *		   index is equivalent to 'rhs'. Otherwise,
		 *		   returns false.
		 *
		 * @exception Throws exception if there is nothing
		 *			  at the 0th element.
		 */
		bool operator==(const std::string& rhs) const;

		/**
		 * @brief Equality operator. Compares the first
		 *		  element of this Datum as though it
		 *		  were a scalar value.
		 *
		 * @param rhs The RTTI* to which we are comparing.
		 *
		 * @return Returns true if the object at the 0th
		 *		   index is equivalent to 'rhs'. Otherwise,
		 *		   returns false.
		 *
		 * @exception Throws exception if there is nothing
		 *			  at the 0th element.
		 */
		bool operator==(RTTI* const& rhs) const;

		/**
		 * @brief Equality operator. Compares the first
		 *		  element of this Datum as though it
		 *		  were a scalar value.
		 *
		 * @param rhs The glm vec4 to which we are comparing.
		 *
		 * @return Returns true if the object at the 0th
		 *		   index is equivalent to 'rhs'. Otherwise,
		 *		   returns false.
		 *
		 * @exception Throws exception if there is nothing
		 *			  at the 0th element.
		 */
		bool operator==(const glm::vec4& rhs) const;

		/**
		 * @brief Equality operator. Compares the first
		 *		  element of this Datum as though it
		 *		  were a scalar value.
		 *
		 * @param rhs The glm mat4x4 to which we are comparing.
		 *
		 * @return Returns true if the object at the 0th
		 *		   index is equivalent to 'rhs'. Otherwise,
		 *		   returns false.
		 *
	 	 * @exception Throws exception if there is nothing
		 *			  at the 0th element.
		 */
		bool operator==(const glm::mat4x4& rhs) const;

		/**
		* @brief Equality operator. Compares the first
		*		  element of this Datum as though it
		*		  were a scalar value.
		*
		* @param rhs The Scope to which we are comparing.
		*
		* @return Returns true if the object at the 0th
		*		   index is equivalent to 'rhs'. Otherwise,
		*		   returns false.
		*
		* @exception Throws exception if there is nothing
		*			  at the 0th element.
		*/
		bool operator==(const Scope& rhs) const;

		/**
		 * @brief Inequality operator.
		 *
		 * @param other The Datum to which we are
		 *				comparing this Datum.
		 *
		 * @return Returns true if this Datum and
		 *		   'other' have different sizes or
		 *		   types, or the data differs.
		 *		   Otherwise, returns false.
		 */
		bool operator!=(const Datum& other) const;

		/**
		 * @brief Inequality operator. Compares the first
		 *		  element of this Datum as though it
		 *		  were a scalar value.
		 *
		 * @param rhs The int to which we are comparing.
		 *
		 * @return Returns true if the object at the 0th
		 *		   index is not equivalent to 'rhs'. Otherwise,
		 *		   returns false.
		 *
		 * @exception Throws exception if there is nothing
		 *			  at the 0th element.
		 */
		bool operator!=(const std::int32_t& rhs) const;

		/**
		 * @brief Inequality operator. Compares the first
		 *		  element of this Datum as though it
		 *		  were a scalar value.
		 *
		 * @param rhs The float to which we are comparing.
		 *
		 * @return Returns true if the object at the 0th
		 *		   index is not equivalent to 'rhs'. Otherwise,
		 *		   returns false.
		 *
		 * @exception Throws exception if there is nothing
		 *			  at the 0th element.
		 */
		bool operator!=(const float& rhs) const;

		/**
		 * @brief Inequality operator. Compares the first
		 *		  element of this Datum as though it
		 *		  were a scalar value.
		 *
		 * @param rhs The string to which we are comparing.
		 *
		 * @return Returns true if the object at the 0th
		 *		   index is not equivalent to 'rhs'. Otherwise,
		 *		   returns false.
		 *
		 * @exception Throws exception if there is nothing
		 *			  at the 0th element.
		 */
		bool operator!=(const std::string& rhs) const;

		/**
		 * @brief Inequality operator. Compares the first
		 *		  element of this Datum as though it
		 *		  were a scalar value.
		 *
		 * @param rhs The RTTI* to which we are comparing.
		 *
		 * @return Returns true if the object at the 0th
		 *		   index is not equivalent to 'rhs'. Otherwise,
		 *		   returns false.
		 *
		 * @exception Throws exception if there is nothing
		 *			  at the 0th element.
		 */
		bool operator!=(RTTI* const& rhs) const;

		/**
		 * @brief Inequality operator. Compares the first
		 *		  element of this Datum as though it
		 *		  were a scalar value.
		 *
		 * @param rhs The glm vec4 to which we are comparing.
		 *
		 * @return Returns true if the object at the 0th
		 *		   index is not equivalent to 'rhs'. Otherwise,
		 *		   returns false.
		 *
		 * @exception Throws exception if there is nothing
		 *			  at the 0th element.
		 */
		bool operator!=(const glm::vec4& rhs) const;

		/**
		 * @brief Inequality operator. Compares the first
		 *		  element of this Datum as though it
		 *		  were a scalar value.
		 *
		 * @param rhs The glm mat4x4 to which we are comparing.
		 *
		 * @return Returns true if the object at the 0th
		 *		   index is not equivalent to 'rhs'. Otherwise,
		 *		   returns false.
		 *
		 * @exception Throws exception if there is nothing
		 *			  at the 0th element.
		 */
		bool operator!=(const glm::mat4x4& rhs) const;

		/**
		* @brief Inequality operator. Compares the first
		*		  element of this Datum as though it
		*		  were a scalar value.
		*
		* @param rhs The Scope* to which we are comparing.
		*
		* @return Returns true if the object at the 0th
		*		   index is not equivalent to 'rhs'. Otherwise,
		*		   returns false.
		*
		* @exception Throws exception if there is nothing
		*			  at the 0th element.
		*/
		bool operator!=(const Scope& rhs) const;

		/**
		 * @brief Reserves space in the Datum's
		 *		  array for the given number of
		 *		  elements of the Datum's type.
		 *
		 * @param reserveSize The new array size.
		 *
		 * @note Does nothing if we are reserving
		 *		 less space than we already have.
		 *
		 * @exception Throws exception when called
		 *			  against a Datum with no type.
		 * @exception Throws exception when called
		 *			  against a Datum storing
		 *			  external memory.
		 */
		void reserve(const std::uint32_t reserveSize);

		/**
		 * @brief Shrinks the Datum's capacity to
		 *		  match its current size.
		 *
		 * @exception Throws exception when called
		 *			  against a Datum with no type.
		 * @exception Throws exception when called
		 *			  against a Datum storing
		 *			  external memory.
		 */
		void shrinkToFit();

		/**
		 * @brief Makes the Datum point to memory owned by 
		 *		  another object. Datum type becomes Integer.
		 *
		 * @param data The external memory we will reference.
		 * @param size The size of the external memory block.
		 *
		 * @note Size and capacity will be set to 'size'.
		 * @note Size may not be the full size of the external
		 *		 block of memory. It just refers to the size of
		 *		 the block to which we have access.
		 *
		 * @exception Throws exception if the Datum previously
		 *			  owned its own allocated memory.
		 * @exception Throws exception if the Datum's type
		 *			  does not match the type of the external
		 *			  memory.
		 */
		void setStorage(std::int32_t* const& data, const std::uint32_t size);

		/**
		 * @brief Makes the Datum point to memory owned by
		 *		  another object. Datum type becomes Float.
		 *
		 * @param data The external memory we will reference.
		 * @param size The size of the external memory block.
		 *
		 * @note Size and capacity will be set to 'size'.
		 * @note Size may not be the full size of the external
		 *		 block of memory. It just refers to the size of
		 *		 the block to which we have access.
		 *
		 * @exception Throws exception if the Datum previously
		 *			  owned its own allocated memory.
		 * @exception Throws exception if the Datum's type
		 *			  does not match the type of the external
		 *			  memory.
		 */
		void setStorage(float* const& data, const std::uint32_t size);

		/**
		 * @brief Makes the Datum point to memory owned by
		 *		  another object. Datum type becomes String.
		 *
		 * @param data The external memory we will reference.
		 * @param size The size of the external memory block.
		 *
		 * @note Size and capacity will be set to 'size'.
		 * @note Size may not be the full size of the external
		 *		 block of memory. It just refers to the size of
		 *		 the block to which we have access.
		 *
		 * @exception Throws exception if the Datum previously
		 *			  owned its own allocated memory.
		 * @exception Throws exception if the Datum's type
		 *			  does not match the type of the external
		 *			  memory.
		 */
		void setStorage(std::string* const& data, const std::uint32_t size);

		/**
		 * @brief Makes the Datum point to memory owned by
		 *		  another object. Datum type becomes Pointer.
		 *
		 * @param data The external memory we will reference.
		 * @param size The size of the external memory block.
		 *
		 * @note Size and capacity will be set to 'size'.
		 * @note Size may not be the full size of the external
		 *		 block of memory. It just refers to the size of
		 *		 the block to which we have access.
		 *
		 * @exception Throws exception if the Datum previously
		 *			  owned its own allocated memory.
		 * @exception Throws exception if the Datum's type
		 *			  does not match the type of the external
		 *			  memory.
		 */
		void setStorage(RTTI** const& data, const std::uint32_t size);

		/**
		 * @brief Makes the Datum point to memory owned by
		 *		  another object. Datum type becomes Vector.
		 *
		 * @param data The external memory we will reference.
		 * @param size The size of the external memory block.
		 *
		 * @note Size and capacity will be set to 'size'.
		 * @note Size may not be the full size of the external
		 *		 block of memory. It just refers to the size of
		 *		 the block to which we have access.
		 *
		 * @exception Throws exception if the Datum previously
		 *			  owned its own allocated memory.
		 * @exception Throws exception if the Datum's type
		 *			  does not match the type of the external
		 *			  memory.
		 */
		void setStorage(glm::vec4* const& data, const std::uint32_t size);

		/**
		 * @brief Makes the Datum point to memory owned by
		 *		  another object. Datum type becomes Matrix.
		 *
		 * @param data The external memory we will reference.
		 * @param size The size of the external memory block.
		 *
		 * @note Size and capacity will be set to 'size'.
		 * @note Size may not be the full size of the external
		 *		 block of memory. It just refers to the size of
		 *		 the block to which we have access.
		 *
		 * @exception Throws exception if the Datum previously
		 *			  owned its own allocated memory.
		 * @exception Throws exception if the Datum's type
		 *			  does not match the type of the external
		 *			  memory.
		 */
		void setStorage(glm::mat4x4* const& data, const std::uint32_t size);

		/**
		 * @brief Adds a new int to the end of the Datum array.
		 *
		 * @param data The data being added.
		 *
		 * @note If there is no room for the data, the array
		 *		 automatically grows in capacity.
		 * @note Sets the Datum's type if this is the first
		 *		 addition.
		 *
		 * @exception Throws exception if called against a
		 *			  Datum referencing external memory.
		 */
		void pushBack(const std::int32_t& data);

		/**
		 * @brief Adds a new float to the end of the Datum array.
		 *
		 * @param data The data being added.
		 *
		 * @note If there is no room for the data, the array
		 *		 automatically grows in capacity.
		 * @note Sets the Datum's type if this is the first
		 *		 addition.
		 *
		 * @exception Throws exception if called against a
		 *			  Datum referencing external memory.
		 */
		void pushBack(const float& data);

		/**
		 * @brief Adds a new string to the end of the Datum array.
		 *
		 * @param data The data being added.
		 *
		 * @note If there is no room for the data, the array
		 *		 automatically grows in capacity.
		 * @note Sets the Datum's type if this is the first
		 *		 addition.
	 	 *
		 * @exception Throws exception if called against a
		 *			  Datum referencing external memory.
		 */
		void pushBack(const std::string& data);

		/**
		 * @brief Adds a new RTTI* to the end of the Datum array.
		 *
		 * @param data The data being added.
		 *
		 * @note If there is no room for the data, the array
		 *		 automatically grows in capacity.
		 * @note Sets the Datum's type if this is the first
		 *		 addition.
		 *
		 * @exception Throws exception if called against a
		 *			  Datum referencing external memory.
		 */
		void pushBack(RTTI* const& data);

		/**
		 * @brief Adds a new glm vec4 to the end of the Datum array.
		 *
		 * @param data The data being added.
		 *
		 * @note If there is no room for the data, the array
		 *		 automatically grows in capacity.
		 * @note Sets the Datum's type if this is the first
		 *		 addition.
		 *
		 * @exception Throws exception if called against a
		 *			  Datum referencing external memory.
		 */
		void pushBack(const glm::vec4& data);

		/**
		 * @brief Adds a new glm mat4x4 to the end of the Datum array.
		 *
		 * @param data The data being added.
		 *
		 * @note If there is no room for the data, the array
		 *		 automatically grows in capacity.
		 * @note Sets the Datum's type if this is the first
		 *		 addition.
		 *
		 * @exception Throws exception if called against a
		 *			  Datum referencing external memory.
		 */
		void pushBack(const glm::mat4x4& data);

		/**
		 * @brief Adds a new Scope* to the end of the Datum array.
		 *
		 * @param data The data being added.
		 *
		 * @note If there is no room for the data, the array
		 *		 automatically grows in capacity.
		 * @note Sets the Datum's type if this is the first
		 *		 addition.
		 *
		 * @exception Throws exception if called against a
		 *			  Datum referencing external memory.
		 */
		void pushBack(Scope& data);

		/**
		* @brief Removes the first instance of
		*		  the given data.
		*
		* @param data The data to remove.
		*
		* @return Returns true if the data was
		*		   found and removed. Otherwise,
		*		   returns false.
		*/
		bool remove(const std::int32_t& data);

		/**
		 * @brief Removes the first instance of
		 *		  the given data.
		 *
		 * @param data The data to remove.
		 *
		 * @return Returns true if the data was
		 *		   found and removed. Otherwise,
		 *		   returns false.
		 */
		bool remove(const float& data);

		/**
		 * @brief Removes the first instance of
		 *		  the given data.
		 *
		 * @param data The data to remove.
		 *
		 * @return Returns true if the data was
		 *		   found and removed. Otherwise,
		 *		   returns false.
		 */
		bool remove(const std::string& data);

		/**
		 * @brief Removes the first instance of
		 *		  the given data.
		 *
		 * @param data The data to remove.
		 *
		 * @return Returns true if the data was
		 *		   found and removed. Otherwise,
		 *		   returns false.
		 */
		bool remove(RTTI* const& data);
		
		/**
		 * @brief Removes the first instance of
		 *		  the given data.
		 *
		 * @param data The data to remove.
		 *
		 * @return Returns true if the data was
		 *		   found and removed. Otherwise,
		 *		   returns false.
		 */
		bool remove(const glm::vec4& data);
		
		/**
		 * @brief Removes the first instance of
		 *		  the given data.
		 *
		 * @param data The data to remove.
		 *
		 * @return Returns true if the data was
		 *		   found and removed. Otherwise,
		 *		   returns false.
		 */
		bool remove(const glm::mat4x4& data);
		
		/**
		 * @brief Removes the first instance of
		 *		  the given data.
		 *
		 * @param data The data to remove.
		 *
		 * @return Returns true if the data was
		 *		   found and removed. Otherwise,
		 *		   returns false.
		 */
		bool remove(Scope& data);

		/**
		 * @brief Removes the element at the given
		 *		  array index.
		 *
		 * @index The spot in the array to remove.
		 */
		void removeAt(std::uint32_t index);

		/**
		 * @brief Removes the element at the back of
		 *		  the Datum array.
		 *
		 * @note Capacity is unchanged.
		 *
		 * @exception Throws exception if called against
		 *			  a Datum referencing external memory.
		 */
		void popBack();

		/**
		 * @brief Removes all elements from the Datum array.
		 *
		 * @note Capacity is unchanged.
		 *
		 * @exception Throws exception if called against a
		 *			  Datum referencing external memory.
		 */
		void clear();

		/**
		 * @brief Sets data in the Datum array at
		 *		  the given index.
		 *
		 * @param data The incoming integer.
		 * @param index The array element index to set.
		 *				Defaulted to 0.
		 *
		 * @exception Throws exception if the Datum's type
		 *			  does not match the incoming data's.
		 * @exception Throws exception if 'index' is out of
		 *			  bounds for the Datum's array.
		 */
		void set(const std::int32_t& data, const std::uint32_t index = 0);

		/**
		 * @brief Sets data in the Datum array at
		 *		  the given index.
		 *
		 * @param data The incoming float.
		 * @param index The array element index to set.
		 *				Defaulted to 0.
		 *
		 * @exception Throws exception if the Datum's type
		 *			  does not match the incoming data's.
		 * @exception Throws exception if 'index' is out of
		 *			  bounds for the Datum's array.
		 */
		void set(const float& data, const std::uint32_t index = 0);

		/**
		 * @brief Sets data in the Datum array at
		 *		  the given index.
		 *
		 * @param data The incoming string.
		 * @param index The array element index to set.
		 *				Defaulted to 0.
		 *
		 * @exception Throws exception if the Datum's type
		 *			  does not match the incoming data's.
		 * @exception Throws exception if 'index' is out of
		 *			  bounds for the Datum's array.
		 */
		void set(const std::string& data, const std::uint32_t index = 0);

		/**
		 * @brief Sets data in the Datum array at
		 *		  the given index.
		 *
		 * @param data The incoming RTTI*.
		 * @param index The array element index to set.
		 *				Defaulted to 0.
		 *
		 * @exception Throws exception if the Datum's type
		 *			  does not match the incoming data's.
		 * @exception Throws exception if 'index' is out of
		 *			  bounds for the Datum's array.
		 */
		void set(RTTI* const& data, const std::uint32_t index = 0);

		/**
		 * @brief Sets data in the Datum array at
		 *		  the given index.
		 *
		 * @param data The incoming glm vec4.
		 * @param index The array element index to set.
		 *				Defaulted to 0.
		 *
		 * @exception Throws exception if the Datum's type
		 *			  does not match the incoming data's.
		 * @exception Throws exception if 'index' is out of
		 *			  bounds for the Datum's array.
		 */
		void set(const glm::vec4& data, const std::uint32_t index = 0);

		/**
		 * @brief Sets data in the Datum array at
		 *		  the given index.
		 *
		 * @param data The incoming glm mat4x4.
		 * @param index The array element index to set.
		 *				Defaulted to 0.
		 *
		 * @exception Throws exception if the Datum's type
		 *			  does not match the incoming data's.
		 * @exception Throws exception if 'index' is out of
		 *			  bounds for the Datum's array.
		 */
		void set(const glm::mat4x4& data, const std::uint32_t index = 0);

		/**
		 * @brief Sets data in the Datum array at
		 *		  the given index.
		 *
		 * @param data The incoming Scope.
		 * @param index The array element index to set.
		 *				Defaulted to 0.
		 *
		 * @exception Throws exception if the Datum's type
	 	 *			  does not match the incoming data's.
		 * @exception Throws exception if 'index' is out of
		 *			  bounds for the Datum's array.
		 */
		void set(Scope& data, const std::uint32_t index = 0);

		/**
		 * @brief Retrieves the data from the Datum array
		 *		  at the given array location.
		 *
		 * @param index The Datum array index. Defaulted to 0.
		 *
		 * @return Returns a reference to the data at the
		 *		   array index.
		 *
		 * @exception Throws exception if the Datum's type
		 *			  does not match the return type.
		 * @exception Throws exception if 'index' is out of
		 *			  bounds for the Datum's array.
		 */
		template<typename T> T& get(const std::uint32_t index = 0);

		/**
		 * @brief Retrieves the data from the Datum array
		 *		  at the given array location.
		 *
		 * @param index The Datum array index. Defaulted to 0.
		 *
		 * @return Returns a const reference to the data at the
		 *		   array index.
		 *
		 * @exception Throws exception if the Datum's type
		 *			  does not match the return type.
		 * @exception Throws exception if 'index' is out of
		 *			  bounds for the Datum's array.
		 */
		template<typename T> const T& get(const std::uint32_t index = 0) const;

		/**
		 * @brief Retreives the Datum with the given name at the
		 *		  0th Scope.
		 *
		 * @param name The name of the attribute to look up on
		 *			   the 0th Scope.
		 *
		 * @return Returns a reference to the Datum with the given
		 *		   name at the 0th Scope.
		 *
		 * @exception This Datum's type is not Table.
		 * @exception There is no Scope in this Datum.
		 */
		Datum& operator[](const std::string& name);

		/**
		 * @brief Retrieves the scope at the given location.
		 *
		 * @param index The Datum array index.
		 *
		 * @return Returns a reference to the Scope at the location.
		 *
		 * @exception Throws exception if the Datum's type is not
		 *			  Table.
		 * @exception Throws exception if the Datum's type
		 *			  does not match the return type.
		 * @exception Throws exception if 'index' is out of
		 *			  bounds for the Datum's array.
		 */
		Scope& operator[](std::uint32_t index);

		/**
		 * @brief Retrieves the scope at the given location.
		 *
		 * @param index The Datum array index.
		 *
		 * @return Returns a const reference to the Scope at the location.
		 *
		 * @exception Throws exception if the Datum's type is not
		 *			  Table.
		 * @exception Throws exception if the Datum's type
		 *			  does not match the return type.
		 * @exception Throws exception if 'index' is out of
		 *			  bounds for the Datum's array.
		 */
		const Scope& operator[](std::uint32_t index) const;
		
		/**
		 * @brief Pushes back data parsed from the given
		 *		  string. Requires that the Datum's type
		 *		  has been set previously.
		 *
		 * @param str The string to parse.
		 *
		 * @exception The Datum type is Pointer, Table,
		 *			  or Unknown.
		 */
		void pushBackFromString(const std::string& str);

		/**
		 * @brief Converts the given string to a value of
		 *		  the Datum's type and inserts it at the
		 *		  given index.
		 *
		 * @param str The incoming string to parse.
		 * @param index The index at which to place the
		 *				incoming data. Defaulted to 0.
		 *
		 * @exception Throws exception if the Datum has
		 *			  no type.
		 * @exception Throws exception if the Datum's type
		 *			  does not match the incoming data's.
		 * @exception Throws exception if 'index' is out of
		 *			  bounds for the Datum's array.
		 */
		void setFromString(const std::string& str, const std::uint32_t index = 0);

		/**
		 * @brief Retrieves data from the Datum's array
		 *		  as a string.
		 *
		 * @param index The array index of the object that
		 *				we retrieve as a string. Defaulted
		 *				to 0.
		 *
		 * @return Returns the data at the requested array
		 *		   location as a string.
		 *
		 * @exception Throws exception if the Datum has
		 *			  no type.
		 * @exception Throws exception if 'index' is out of
		 *			  bounds for the Datum's array.
		 */
		std::string toString(const std::uint32_t index = 0) const;

		/**
		 * @brief Sets the Datum's type enumeration.
		 *
		 * @param type The incoming type.
		 *
		 * @exception Throws exception if the Datum's type
		 *			  was already set. If the incoming type
		 *			  is the same as the Datum's current
		 *			  type, this method is a no-op.
		 */
		void setType(const DatumType type);

		/**
		 * @brief Retrieves the Datum's type.
		 *
		 * @return Returns the Datum's type.
		 */
		DatumType type() const;

		/**
		 * @brief Retrieves the Datum's size.
		 *
		 * @return Returns the number of elements
		 *		   in the Datum array.
		 */
		std::uint32_t size() const;

		/**
		 * @brief Retrieves the Datum array's capacity.
		 *
		 * @return Returns the size of the Datum's
		 *		   memory allocation.
		 */
		std::uint32_t capacity() const;

		/**
		 * @brief Says whether the Datum owns its memory.
		 *
		 * @return Returns true if the Datum references
		 *		   externally owned memory. Returns false
		 *		   if the Datum owns its own memory.
		 */
		bool isExternal() const;

		/**
		 * @brief Says whether the Datum is empty.
		 *
		 * @return Returns true if the Datum has
		 *		   no stored objects in its array.
		 *		   Otherwise, returns false.
		 */
		bool isEmpty() const;

	private:

		/**
		 * Union of the different types that Datum
		 * can store. Contains pointers of different 
		 * types to the block of memory at which Datum
		 * stores its data.
		 *
		 * Datum internally uses the void* pointer and
		 * casts it to the appropriate type. Pointers
		 * of the other types are provided so one can
		 * debug the value at that location.
		 */
		union DataPointers
		{
			void* v;

			int32_t* i;
			float* f;
			std::string* str;
			glm::vec4* vector;
			glm::mat4x4* matrix;
			RTTI** rtti;
			Scope** scope;
		};

		void performDeepCopy(const Datum& other);
		template <typename T> void performDeepCopyHelper(const Datum& other);
		void performDeepCopyHelperRTTI(const Datum& other);

		template <typename T> void assignmentHelper(const T& rhs, const DatumType expectedType);
		template <typename T> bool comparisonHelper(const Datum& other) const;
		bool comparisonHelperRTTI(const Datum& other) const;

		template <typename T> void reserveHelper(const std::uint32_t reserveSize);
		template <typename T> void shrinkToFitHelper();

		template <typename T> void setStorageHelper(T* const& data, const std::uint32_t size, const DatumType type);

		template <typename T> void pushBackHelper(const T& data, const DatumType expectedType);
		template <typename T> bool removeHelper(const T& data, const DatumType expectedType);
		template <typename T> void removeAtHelper(std::uint32_t index);
		template <typename T> void popBackHelper();
		template <typename T> void clearHelper();

		template <typename T> void setHelper(const T& data, const std::uint32_t index, const DatumType expectedType);
		template <typename T> T& getHelper(const std::uint32_t index, const DatumType expectedType) const;

		template <typename T> T dataFromString(const std::string& str);
		template <typename T> void pushBackFromStringHelper(const std::string& str);
		template <typename T> void setFromStringHelper(const std::string& str, const std::uint32_t index = 0);

		std::string toStringInt(const std::uint32_t index = 0) const;
		std::string toStringFloat(const std::uint32_t index = 0) const;
		std::string toStringString(const std::uint32_t index = 0) const;
		std::string toStringVector(const std::uint32_t index = 0) const;
		std::string toStringMatrix(const std::uint32_t index = 0) const;
		std::string toStringRTTI(const std::uint32_t index = 0) const;

		DataPointers mData;
		DatumType mType;

		std::uint32_t mSize;
		std::uint32_t mCapacity;

		bool mIsExternal;

		// static members storing function pointers
		typedef std::function<bool(const Datum&, const Datum&)> ComparisonFuncs;
		typedef std::function<void(Datum&, const std::uint32_t)> ReserveFuncs;
		typedef std::function<void(Datum&)> ShrinkToFitFuncs;
		typedef std::function<void(Datum&, std::uint32_t)> RemoveAtFuncs;
		typedef std::function<void(Datum&)> PopBackFuncs;
		typedef std::function<void(Datum&)> ClearFuncs;
		typedef std::function<void(Datum&, const std::string&)> PushFromStringFuncs;
		typedef std::function<void(Datum&, const std::string&, const std::uint32_t)> SetFromStringFuncs;
		typedef std::function<std::string(const Datum&, const std::uint32_t)> ToStringFuncs;
		typedef std::function<void(Datum&, const Datum&)> CopyFuncs;

		static ComparisonFuncs		sComparisonFuncs	[(std::uint32_t)DatumType::Unknown];
		static ReserveFuncs			sReserveFuncs		[(std::uint32_t)DatumType::Unknown];
		static ShrinkToFitFuncs		sShrinkToFitFuncs	[(std::uint32_t)DatumType::Unknown];
		static RemoveAtFuncs		sRemoveAtFuncs		[(std::uint32_t)DatumType::Unknown];
		static PopBackFuncs			sPopBackFuncs		[(std::uint32_t)DatumType::Unknown];
		static ClearFuncs			sClearFuncs			[(std::uint32_t)DatumType::Unknown];
		static PushFromStringFuncs	sPushFromStringFuncs[(std::uint32_t)DatumType::Unknown];
		static SetFromStringFuncs	sSetFromStringFuncs	[(std::uint32_t)DatumType::Unknown];
		static ToStringFuncs		sToStringFuncs		[(std::uint32_t)DatumType::Unknown];
		static CopyFuncs			sCopyFuncs			[(std::uint32_t)DatumType::Unknown];
	};
}
