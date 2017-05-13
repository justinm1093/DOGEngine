
#pragma once

#include "Attributed.h"
#include "RTTI.h"
#include "Factory.h"

#define ATTRIBUTED_FOO_ARRAY_SIZE 5

namespace UnitTests
{
	/**
	 * Derived Attributed class with some
	 * default prescribed attributes.
	 */
	class AttributedFoo : public DOGEngine::Attributed
	{
		RTTI_DECLARATIONS(AttributedFoo, DOGEngine::Attributed)
		FACTORY_DECLARATION(AttributedFoo, DOGEngine::RTTI)

	public:

		/**
		 * @brief Constructor. Fills the list of prescribed
		 *		  attribute names if it is empty, and populates
		 *		  this object with default-valued prescribed
		 *		  attributes.
		 */
		AttributedFoo();

		/**
		 * @brief Copy constructor. Fills the list of prescribed
		 *		  attribute names if it is empty, and populates
		 *		  this object with default-valued prescribed
		 *		  attributes.
		 *
		 * @param other The AttributedFoo being copied.
		 *
		 * @exception Throws exception if the RTTI type of
		 *			  'other' does not match the type being
		 *			  constructed.
		 */
		AttributedFoo(const AttributedFoo& other);

		/**
		 * @brief Copy assignment operator.
		 *
		 * @param other The AttributedFoo being copied.
		 *
		 * @return Returns a referecen to this AttributedFoo.
		 *
		 * @exception Throws exception if the RTTI type of
		 *			  'other' does not match the type being
		 *			  assigned.
		 */
		AttributedFoo& operator=(const AttributedFoo& other);

		/**
		 * @brief Virtual copy constructor. Deep copies
		 *		  this object.
		 *
		 * @return Returns a deep copy of this object
		 *		   as a pointer to a Scope
		 */
		virtual Scope* copy() override;

		/**
		 * @brief Move constructor.
		 *
		 * @param other The AttributedFoo being moved.
		 *
		 * @exception Throws exception if the RTTI type of
		 *			  'other' does not match the type being
		 *			  constructed.
		 */
		AttributedFoo(AttributedFoo&& other);

		/**
		 * @brief Move assignment operator.
		 *
		 * @param other The AttributedFoo being moved.
		 *
		 * @return Returns a reference to this AttributedFoo.
		 *
		 * @exception Throws exception if the RTTI type of
		 *			  'other' does not match the type being
		 *			  assigned.
		 */
		AttributedFoo& operator=(AttributedFoo&& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~AttributedFoo();

	protected:

		/**
		* @brief Builds this object's list of prescribed attributes.
		*		  Overridden implementations should call their parent
		*		  class' implementation as well to receive signatures
		*		  for their parent class' attributes.
		*/
		virtual void initSignatures() override;

		/**
		* @brief Adds default-valued prescribed attributes to this
		*		  object's table. Overridden implementations should
		*		  call their parent class' implementation as well to
		*		  receive their parent class' default data.
		*/
		virtual void populate() override;

		/**
		 * @brief Updates this object's external storage pointers.
		 *		  This prevents a copied or moved object from having
		 *		  its external data pointing to members of another
		 *		  Attributed object. Overridden implementations should
		 *		  call their parent class' implementation as well.
		 */
		virtual void updateExternalStorage() override;

	public:

		// members -- public access for test convenience
		std::int32_t mIntField;
		float mFloatField;
		std::string mStringField;
		glm::vec4 mVectorField;
		glm::mat4x4 mMatrixField;
		RTTI* mRTTIField;

		std::int32_t mIntArray[ATTRIBUTED_FOO_ARRAY_SIZE];
		float mFloatArray[ATTRIBUTED_FOO_ARRAY_SIZE];
		std::string mStringArray[ATTRIBUTED_FOO_ARRAY_SIZE];
		glm::vec4 mVectorArray[ATTRIBUTED_FOO_ARRAY_SIZE];
		glm::mat4x4 mMatrixArray[ATTRIBUTED_FOO_ARRAY_SIZE];
		RTTI* mRTTIArray[ATTRIBUTED_FOO_ARRAY_SIZE];
	};
}
