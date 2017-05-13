
#pragma once

#include "Action.h"

namespace DOGEngine
{
	class ActionUnsubscribe final : public Action
	{
		RTTI_DECLARATIONS(ActionUnsubscribe, Action)
		ACTION_FACTORY(ActionUnsubscribe)
	
	public:

		/**
		 * @brief Constructor.
		 *
		 * @param name The name of this object.
		 */
		explicit ActionUnsubscribe(const std::string& name = "");

		/**
		 * @brief Copy constructor.
		 *
		 * @param other The object being copied.
		 */
		ActionUnsubscribe(const ActionUnsubscribe& other);

		/**
		 * @brief Copy Assignment.
		 *
		 * @param other The object being copied.
		 *
		 * @return Returns a reference to this object.
		 */
		ActionUnsubscribe& operator=(const ActionUnsubscribe& other);

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
		ActionUnsubscribe(ActionUnsubscribe&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The object we are moving. It
		 *				is reset to a default state.
		 *
		 * @return Returns a reference to this.
		 */
		ActionUnsubscribe& operator=(ActionUnsubscribe&& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~ActionUnsubscribe();

		/**
		 * @brief Update method for the simulation loop.
		 *		  Unsubscribes its parent from Event<EventArgs>
		 *		  if its parent is of type ReactionAttributed.
		 *
		 * @param worldState Data of the current simulation
		 *					 state.
		 */
		virtual void update(WorldState& worldState);
	};
}
