
#pragma once

#include "Entity.h"

namespace UnitTests
{
	class EntityFoo : public DOGEngine::Entity
	{
		RTTI_DECLARATIONS(EntityFoo, DOGEngine::Entity)
		FACTORY_DECLARATION(EntityFoo, DOGEngine::Entity)

	public:

		/**
		 * @brief Constructor. Creates an Entity with
		 *		  the given name.
		 *
		 * @param name The name for this Entity. Defaults
		 *			   to empty string ""
		 */
		explicit EntityFoo(const std::string& name = "");

		/**
		 * @brief Copy constructor.
		 *
		 * @param other The Entity we are copying.
		 */
		EntityFoo(const EntityFoo& other);

		/**
		 * @brief Copy assignment.
		 *
		 * @param other The Entity we are copying.
		 *
		 * @return Returns a reference to this.
		 */
		EntityFoo& operator=(const EntityFoo& other);

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
		 * @param other The Entity we are moving.
		 *				It is reset to a default state.
		 */
		EntityFoo(EntityFoo&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The Entity we are moving.
		 *				It is reset to a default state.
		 *
		 * @return Returns a reference to this.
		 */
		EntityFoo& operator=(EntityFoo&& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~EntityFoo();

		/**
		 * @brief Update method for the simulation loop.
		 *		  Updates this object and calls update
		 *		  on child Actions.
		 *
		 * @param worldState Data of the current simulation
		 *					 state.
		 */
		virtual void update(DOGEngine::WorldState& worldState) override;

	protected:

		/**
		 * @brief Populates the static map of
		 *		  this class' prescribed attribute
		 *		  names.
		 */
		virtual void initSignatures() override;

		/**
		 * @brief Popultes this class' prescribed
		 *		  attributes with default values.
		 */
		virtual void populate() override;

		/**
		 * @brief Updates this class' table's pointers
		 *		  to external storage.
		 */
		virtual void updateExternalStorage() override;
	};
}
