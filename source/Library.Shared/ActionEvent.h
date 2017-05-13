
#pragma once

#include "Action.h"

namespace DOGEngine
{
	/**
	 * Action class that posts EventArg Events
	 * to the World's event queue.
	 *
	 * Events are posted with the subtype and
	 * millisecond delay defined by this object.
	 */
	class ActionEvent : public Action
	{
		RTTI_DECLARATIONS(ActionEvent, Action)
		ACTION_FACTORY(ActionEvent)

	public:

		/**
		 * @brief Constructor.
		 *
		 * @param name The name of this object.
		 */
		explicit ActionEvent(const std::string& name = "");

		/**
		 * @brief Copy constructor.
		 *
		 * @param other The object being copied.
		 */
		ActionEvent(const ActionEvent& other);

		/**
		 * @brief Copy assignment.
		 *
		 * @param other The object being copied.
		 *
		 * @return Returns a reference to this.
		 */
		ActionEvent& operator=(const ActionEvent& other);

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
		ActionEvent(ActionEvent&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The object being moved. It
		 *				is reset to a default state.
		 *
		 * @return Returns a reference to this.
		 */
		ActionEvent& operator=(ActionEvent&& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~ActionEvent();

		/**
		 * @brief Update method for the simulation loop.
		 *		  Posts an event with this object's subtype
		 *		  and delay to the World's event queue.
		 *
		 * @param worldState Data of the current simulation
		 *					 state.
		 */
		virtual void update(WorldState& worldState) override;

		/**
		 * @brief Sets the subtype that this object gives
		 *		  to outgoing events.
		 *
		 * @param subtype The new subtype.
		 */
		void setSubtype(const std::string& subtype);

		/**
		 * @brief Sets the millisecond delay that this object
		 *		  gives to outgoing events.
		 *
		 * @param delay The new delay in milliseconds.
		 */
		void setDelay(const std::int32_t delay);

		/**
		 * @brief Retrieves the subtype this object gives
		 *		  to outgoing events.
		 *
		 * @return Returns mSubtype
		 */
		const std::string& getSubtype() const;

		/**
		 * @brief Retrieves the millisecond delay this object
		 *		  gives to outgoing events.
		 *
		 * @return Returns mDelay
		 */
		std::int32_t getDelay() const;

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

	private:

		std::string mSubtype;
		std::int32_t mDelay;

	public:

		const static std::string sSubtypeAttribute;
		const static std::string sDelayAttribute;
	};
}
