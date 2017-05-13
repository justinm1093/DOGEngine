
#pragma once

#include "Attributed.h"

namespace DOGEngine
{
	struct WorldState;

	/**
	 * Scope that holds data being deliverd
	 * via an Event<EventArgs>
	 */
	class EventArgs : public Scope
	{
		RTTI_DECLARATIONS(EventArgs, Scope)

	public:
		
		/**
		 * @brief Constructor.
		 *
		 * @param subtype The subtype of this object.
		 */
		explicit EventArgs(const std::string& subtype = "");

		/**
		 * @brief Copy constructor.
		 *
		 * @param other The object being copied.
		 */
		EventArgs(const EventArgs& other);

		/**
		 * @brief Copy assignment.
		 *
		 * @param other The object being copied.
		 *
		 * @return Returns a reference to this.
		 */
		EventArgs& operator=(const EventArgs& other);

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
		 * @param other The object being moved. It
		 *				is reset to a default state.
		 */
		EventArgs(EventArgs&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The object being moved. It
		 *				is reset to a default state.
		 *
		 * @return Returns a reference to this.
		 */
		EventArgs& operator=(EventArgs&& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~EventArgs();

		/**
		 * @brief Sets the event subtype of this object.
		 *
		 * @param subtype The new subtype.
		 */
		void setSubtype(const std::string& subtype);

		/**
		 * @brief Sets the WorldState of this object
		 *		  so subscribers can call their update.
		 *
		 * @param world Reference to the new WorldState.
		 */
		void setWorldState(WorldState& world);

		/**
		 * @brief Retrieves the subtype of this object.
		 *
		 * @return Returns mSubtype
		 */
		const std::string& getSubtype() const;

		/**
		 * @brief Retrieves the pointer to this object's
		 *		  WorldState.
		 *
		 * @return Returns mWorldState
		 */
		WorldState* getWorldState() const;

	private:

		std::string mSubtype;
		WorldState* mWorldState;
	};
}
