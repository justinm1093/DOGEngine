
#pragma once

#include "Factory.h"
#include "Attributed.h"
#include "RTTI.h"

#include "WorldState.h"

namespace DOGEngine
{
	/**
	 * Abstract class that defines some action that
	 * Entities may take.
	 */
	class Action : public Attributed
	{
		RTTI_DECLARATIONS(Action, Attributed)

	public:

		/**
		 * @brief Constructor.
		 *
		 * @param name The name of this object.
		 */
		explicit Action(const std::string& name = "");

		/**
		 * @brief Copy constructor.
		 *
		 * @param other The object being copied.
		 */
		Action(const Action& other);

		/**
		 * @brief Copy assignment.
		 *
		 * @param other The object being copied.
		 *
		 * @return Returns a reference to this.
		 */
		Action& operator=(const Action& other);

		/**
		 * @brief Move constructor.
		 *
		 * @param other The object being moved. It
		 *				is reset to a default state.
		 */
		Action(Action&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The object being moved. It
		 *				is reset to a default state.
		 *
		 * @return Returns a reference to this.
		 */
		Action& operator=(Action&& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~Action();

		/**
		 * @brief Update method for the simulation loop.
		 *
		 * @param worldState Data of the current simulation
		 *					 state.
		 */
		virtual void update(WorldState& worldState) = 0;
		
		/**
		 * @brief Sets this object's name.
		 *
		 * @param name This object's new name.
		 */
		void setName(const std::string& name);

		/**
		 * @brief Retrieves this object's name. Convenience
		 *		  method to avoid table lookup.
		 *
		 * @return Returns this object's name.
		 */
		const std::string& getName() const;

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

		std::string mName;

	public:

		const static std::string sNameAttribute;
		const static std::string sReactionsAttribute;
	};

#define ACTION_FACTORY(DerivedAction) FACTORY_DECLARATION(DerivedAction, Action)
}
