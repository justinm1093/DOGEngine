
#pragma once

#include "Action.h"

namespace DOGEngine
{
	class ActionClearEvents final : public Action
	{
		RTTI_DECLARATIONS(ActionClearEvents, Action)
		ACTION_FACTORY(ActionClearEvents)

	public:

		/**
		 * @brief Constructor.
		 *
		 * @param name The name of this object.
		 */
		explicit ActionClearEvents(const std::string& name = "");

		/**
		 * @brief Copy constructor.
		 *
		 * @param other The object being copied.
		 */
		ActionClearEvents(const ActionClearEvents& other);

		/**
		 * @brief Copy Assignment.
		 *
		 * @param other The object being copied.
		 *
		 * @return Returns a reference to this object.
		 */
		ActionClearEvents& operator=(const ActionClearEvents& other);

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
		 * @param other The object we are moving. It
		 *				is reset to a default state.
		 */
		ActionClearEvents(ActionClearEvents&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The object we are moving. It
		 *				is reset to a default state.
		 *
		 * @return Returns a reference to this.
		 */
		ActionClearEvents& operator=(ActionClearEvents&& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~ActionClearEvents();

		/**
		 * @brief Update method for the simulation loop.
		 *		  Clears the current World's event queue.
		 *
		 * @param worldState Data of the current simulation
		 *					 state.
		 */
		virtual void update(WorldState& worldState);
	};
}
