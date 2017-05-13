
#pragma once

#include "Scope.h"
#include "Datum.h"
#include "RTTI.h"

namespace DOGEngine
{
	/**
	 * Abstract class that defines several methods
	 * for creating Scopes with default fields.
	 * Attributed is abstract and cannot be created
	 * on its own.
	 *
	 * Instead, derive from Attributed and implement
	 * the abstract methods to define the field names
	 * and populate the table with default-values
	 * on construction.
	 *
	 * Attributed Scopes have a concept of prescribed
	 * attributes and auxiliary attributes. Prescribed
	 * means that the attribute is a part of the class
	 * by default and is created at construction.
	 *
	 * Auxiliary attributes are not associated with the
	 * type by default, but may be added to an object
	 * after it is created.
	 */
	class Attributed : public Scope
	{
		RTTI_DECLARATIONS(Attributed, Scope)

	public:

		/**
		 * @brief Constructor. Creates a new Scope table.
		 *		  If it has not already, initializes a list
		 *		  of prescribed attribute names for this type,
		 *		  and also populates the Attributed with
		 *		  prescribed attributes with default values.
		 *
		 * @param size The size of the table.
		 */
		explicit Attributed(std::uint32_t size = 0);

		/**
		 * @brief Copy constructor. Deep copies the
		 *		  given Attributed into a new Attributed.
		 *
		 * @param other The Attributed being copied.
		 *
		 * @exception Throws exception if the RTTI type of
		 *			  'other' does not match the type being
		 *			  constructed.
		 */
		Attributed(const Attributed& other);

		/**
		 * @brief Copy assignment operator. Deep copies the
		 *		  given Attributed into a new Attributed.
		 *
		 * @param other The Attributed being copied.
		 *
		 * @return Returns a reference to this Attributed.
		 *
		 * @exception Throws exception if the RTTI type of
		 *			  'other' does not match the type being
		 *			  assigned.
		 */
		Attributed& operator=(const Attributed& other);

		/**
		 * @brief Virtual copy constructor. Deep copies
		 *		  this object.
		 *
		 * @return Returns a deep copy of this object
		 *		   as a pointer to a Scope
		 */
		virtual Scope* copy() override;

		/**
		 * @brief Move constructor. Moves the data from one
		 *		  Attributed into a new Attributed. The
		 *		  original Attributed is set to its default
		 *		  values.
		 *
		 * @param other The Attributed being moved.
		 *
		 * @exception Throws exception if the RTTI type of
		 *			  'other' does not match the type being
		 *			  constructed.
		 */
		Attributed(Attributed&& other);

		/**
		 * @brief Move assignment operator. Moves the data from
		 *		  one Attributed into a new Attributed. The
		 *		  original Attributed is set to its default
		 *		  values.
		 *
		 * @param other The Attributed being moved.
		 *
		 * @return Returns a reference to this Attributed.
		 *
		 * @exception Throws exception if the RTTI type of
		 *			  'other' does not match the type being
		 *			  assigned.
		 */
		Attributed& operator=(Attributed&& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~Attributed();

		/**
		 * @brief Equality method. Compares this Attributed with
		 *		  an RTTI. The comparison is recursive.
		 *
		 * @param rhs The RTTI* to which we are comparing.
		 *
		 * @return Returns true if 'rhs' is an Attributed with the
		 *		   same named fields and the same data in those fields.
		 *		   Returns false if 'rhs' is not a Scope or the 
		 *		   structure or data of either Scope tree differs.
		 *
		 * @note Attributed comparison ignores the "this" attribute.
		 */
		virtual bool Equals(const RTTI* rhs) const;

		/**
		 * @brief Equality operator. Compares this Attributed with an
		 *		  RTTI. The comparison is recursive.
		 *
		 * @param other The Attributed to which we are comparing.
		 *
		 * @return Returns true if 'other' has the same named fields
		 *		   and the same data in those fields Returns false if
		 *		   'other' is not a Scope or the structure or data of
		 *		   either Attributed tree differs.
		 *
		 * @note Attributed comparison ignores the "this" attribute.
		 */
		bool operator==(const Attributed& other) const;

		/**
		 * @brief Inequality operator. Compares this Attributed with an
		 *		  RTTI. The comparison is recursive.
		 *
		 * @param other The Attributed to which we are comparing.
		 *
		 * @return Returns true if there is any difference in the
		 *		   hierarchy of Scopes, field names, or data. Returns
		 *		   false if the Attributeds are equivalent.
		 *
		 * @note Attributed comparison ignores the "this" attribute.
		 */
		bool operator!=(const Attributed& other) const;
		
		/**
		 * @brief Adds a new attribute field to this Attributed.
		 *		  The new attribute is auxiliary, and not a part
		 *		  of the Attributed's fields by default.
		 *
		 * @param key The name of the attribute to add.
		 *
		 * @return Returns a reference to a Datum object. If the key
		 *		   is already in the Scope, the Datum already there
		 *		   is returned. If the key is not in the Scope, a new
		 *		   empty Datum returned.
		 *
		 * @exception Throws an exception if the key is an empty
		 *			  string ""
		 */
		Datum& addAuxiliaryAttribute(const std::string& key);

		/**
		 * @brief Retrieves the number of prescribed attributes in
		 *		  this object.
		 *
		 * @return Returns the number of prescribed attributes.
		 */
		std::uint32_t prescribedSize() const;

		/**
		 * @brief Retrieves the number of auxiliary attributes in
		 *		  this object.
		 *
		 * @return Returns the number of auxiliary attributes.
		 */
		std::uint32_t auxiliarySize() const;

		/**
		 * @brief Retrieves the index of the Scope vector region
		 *		  that stores auxiliary attributes.
		 *
		 * @return Returns the starting index of the auxiliary
		 *		   attribute region of the Scope vector.
		 */
		std::uint32_t auxiliaryBeginIndex() const;

		/**
		 * @brief Says whether the given attribute name is
		 *		  a part of the table.
		 *
		 * @param name The name for which we are searching.
		 *
		 * @return Returns true if the given name is a part of
		 *		   the table. Otherwise, returns false.
		 */
		bool isAttribute(const std::string& name) const;

		/**
		 * @brief Says whether the given attribute name is
		 *		  prescribed.
		 *
		 * @param name The name for which we are searching.
		 *
		 * @return Returns true if the name is of an attribute
		 *		   associated with this class by default.
		 *		   Otherwise, returns false.
		 */
		bool isPrescribedAttribute(const std::string& name) const;

		/**
		 * @brief Says whether the given attribute name is
		 *		  auxiliary.
		 *
		 * @param name The name for which we are searching.
		 *
		 * @return Returns true if the name is of an attribute
		 *		   that has been inserted but is not prescribed.
		 *		   Otherwise, returns false.
		 */
		bool isAuxiliaryAttribute(const std::string& name) const;

		/**
		 * @brief Clears the list of attribute names for this
		 *		  specific Attributed type.
		 */
		static void clearAttributeCache();

	protected:

		/**
		 * @brief Builds this object's list of prescribed attributes.
		 *		  Overridden implementations should call their parent
		 *		  class' implementation as well to receive signatures
		 *		  for their parent class' attributes.
		 */
		virtual void initSignatures();

		/**
		 * @brief Adds default-valued prescribed attributes to this
		 *		  object's table. Overridden implementations should
		 *		  call their parent class' implementation as well to
		 *		  receive their parent class' default data.
		 */
		virtual void populate();

		/**
		 * @brief Updates this object's external storage pointers.
		 *		  This prevents a copied or moved object from having
		 *		  its external data pointing to members of another
		 *		  Attributed object. Overridden implementations should
		 *		  call their parent class' implementation as well.
		 */
		virtual void updateExternalStorage();

		/**
		 * @brief Says whether the list of prescribed attribute
		 *		  names for this type have been initialized.
		 *
		 * @return Returns true if this type's prescribed
		 *		   attribute names list has been filled. Otherwise,
		 *		   returns false.
		 */
		bool areSignaturesInitialized() const;

		/**
		 * @brief Adds a prescribed attribute name to the class'
		 *		  list.
		 *
		 * @param name The prescribed attribute name.
		 */
		void addSignature(const std::string& name);

		/**
		 * @brief Creates a new entry in the table for child tables,
		 *		  but does not add a child table.
		 *
		 * @return Returns a reference to the Datum.
		 *
		 * @note To add child tables to an Attributed, you can make
		 *		 use of the Scope::appendScope method.
		 */
		void addTableEntry(const std::string& name);

		/**
		 * @brief Adds a prescribed integer attribute to the table.
		 *		  The attribute is set to some default value.
		 *
		 * @param name The name of the prescribed attribute.
		 * @param defaultValue The value with which to fill the
		 *		  attribute.
		 * @param size The number of elements in the attribute.
		 *		  Defaulted to 1.
		 */
		void addInternalAttribute(const std::string& name, const std::int32_t defaultValue, const std::uint32_t size = 1);

		/**
		 * @brief Adds a prescribed float attribute to the table.
		 *		  The attribute is set to some default value.
		 *
		 * @param name The name of the prescribed attribute.
		 * @param defaultValue The value with which to fill the
		 *		  attribute.
		 * @param size The number of elements in the attribute.
		 *		  Defaulted to 1.
		 */
		void addInternalAttribute(const std::string& name, const float defaultValue, const std::uint32_t size = 1);

		/**
		 * @brief Adds a prescribed string attribute to the table.
		 *		  The attribute is set to some default value.
		 *
		 * @param name The name of the prescribed attribute.
		 * @param defaultValue The value with which to fill the
		 *		  attribute.
		 * @param size The number of elements in the attribute.
		 *		  Defaulted to 1.
		 */
		void addInternalAttribute(const std::string& name, const std::string defaultValue, const std::uint32_t size = 1);

		/**
		 * @brief Adds a prescribed glm vector attribute to the table.
		 *		  The attribute is set to some default value.
		 *
		 * @param name The name of the prescribed attribute.
		 * @param defaultValue The value with which to fill the
		 *		  attribute.
		 * @param size The number of elements in the attribute.
		 *		  Defaulted to 1.
		 */
		void addInternalAttribute(const std::string& name, const glm::vec4 defaultValue, const std::uint32_t size = 1);

		/**
		 * @brief Adds a prescribed glm 4x4 matrix attribute to the table.
		 *		  The attribute is set to some default value.
		 *
		 * @param name The name of the prescribed attribute.
		 * @param defaultValue The value with which to fill the
		 *		  attribute.
		 * @param size The number of elements in the attribute.
		 *		  Defaulted to 1.
		 */
		void addInternalAttribute(const std::string& name, const glm::mat4x4 defaultValue, const std::uint32_t size = 1);

		/**
		 * @brief Adds a prescribed RTTI pointer attribute to the table.
		 *		  The attribute is set to some default value.
		 *
		 * @param name The name of the prescribed attribute.
		 * @param defaultValue The value with which to fill the
		 *		  attribute.
		 * @param size The number of elements in the attribute.
		 *		  Defaulted to 1.
		 */
		void addInternalAttribute(const std::string& name, RTTI* const defaultValue, const std::uint32_t size = 1);

		/**
		 * @brief Adds a prescribed integer attribute to the table.
		 *		  The attribute is set to some default value. The 
		 *		  attribute points to external storage.
		 *
		 * @param name The name of the prescribed attribute.
		 * @param storage The location of the external storage.
		 * @param defaultValue The value with which to fill the
		 *		  external storage.
		 * @param size The number of elements in the external storage.
		 *		  Defaulted to 1.
		 */
		void addExternalAttribute(const std::string& name, std::int32_t* const storage, const std::int32_t defaultValue, const std::uint32_t size = 1);

		/**
		 * @brief Adds a prescribed float attribute to the table.
		 *		  The attribute is set to some default value. The 
		 *		  attribute points to external storage.
		 *
		 * @param name The name of the prescribed attribute.
		 * @param storage The location of the external storage.
		 * @param defaultValue The value with which to fill the
		 *		  external storage.
		 * @param size The number of elements in the external storage.
		 *		  Defaulted to 1.
		 */
		void addExternalAttribute(const std::string& name, float* const storage, const float defaultValue, const std::uint32_t size = 1);

		/**
		 * @brief Adds a prescribed string attribute to the table.
		 *		  The attribute is set to some default value. The 
		 *		  attribute points to external storage.
		 *
		 * @param name The name of the prescribed attribute.
		 * @param storage The location of the external storage.
		 * @param defaultValue The value with which to fill the
		 *		  external storage.
		 * @param size The number of elements in the external storage.
		 *		  Defaulted to 1.
		 */
		void addExternalAttribute(const std::string& name, std::string* const storage, const std::string& defaultValue, const std::uint32_t size = 1);

		/**
		 * @brief Adds a prescribed glm vector attribute to the table.
		 *		  The attribute is set to some default value. The 
		 *		  attribute points to external storage.
		 *
		 * @param name The name of the prescribed attribute.
		 * @param storage The location of the external storage.
		 * @param defaultValue The value with which to fill the
		 *		  external storage.
		 * @param size The number of elements in the external storage.
		 *		  Defaulted to 1.
		 */
		void addExternalAttribute(const std::string& name, glm::vec4* const storage, const glm::vec4& defaultValue, const std::uint32_t size = 1);

		/**
		 * @brief Adds a prescribed glm 4x4 matrix attribute to the table.
		 *		  The attribute is set to some default value. The 
		 *		  attribute points to external storage.
		 *
		 * @param name The name of the prescribed attribute.
		 * @param storage The location of the external storage.
		 * @param defaultValue The value with which to fill the
		 *		  external storage.
		 * @param size The number of elements in the external storage.
		 *		  Defaulted to 1.
		 */
		void addExternalAttribute(const std::string& name, glm::mat4x4* const storage, const glm::mat4x4& defaultValue, const std::uint32_t size = 1);

		/**
		 * @brief Adds a prescribed RTTI pointer attribute to the table.
		 *		  The attribute is set to some default value. The 
		 *		  attribute points to external storage.
		 *
		 * @param name The name of the prescribed attribute.
		 * @param storage The location of the external storage.
		 * @param defaultValue The value with which to fill the
		 *		  external storage.
		 * @param size The number of elements in the external storage.
		 *		  Defaulted to 1.
		 */
		void addExternalAttribute(const std::string& name, RTTI** const storage, RTTI* const defaultValue, const std::uint32_t size = 1);

	private:

		/**
		 * @brief Adds the attribute to the table. Fills the
		 *		  attribute with a default value.
		 *
		 * @param name The name of the prescribed attribute.
		 * @param defaultValue The value with which to fill the
		 *		  attribute.
		 * @param size The number of elements in the external storage.
		 */
		template <typename T>
		void addInternalAttributeHelper(const std::string& name, const T defaultValue, const std::uint32_t size, const Datum::DatumType type);

		/**
		 * @brief Adds the externally stored attribute to the
		 *		  table. Fills the external storage with a
		 *		  default value.
		 *
		 * @param name The name of the prescribed attribute.
		 * @param storage The location of the external storage.
		 * @param defaultValue The value with which to fill the
		 *		  external storage.
		 * @param size The number of elements in the external storage.
		 */
		template <typename T>
		void addExternalAttributeHelper(const std::string& name, T* const storage, const T defaultValue, const std::uint32_t size);

		typedef Vector<std::string> SignatureList;
		typedef HashMap<std::uint64_t, SignatureList> SignatureMap;
		static SignatureMap sSignatureMap;
	};
}
