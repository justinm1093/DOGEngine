
#pragma once

#include "Action.h"
#include "RTTI.h"

namespace DOGEngine
{
	/**
	 * Action class that holds other Actions
	 * and calls on their updates each frame.
	 */
	class ActionList : public Action
	{
		RTTI_DECLARATIONS(ActionList, Action)
		ACTION_FACTORY(ActionList)

	public:

		/**
		 * @brief Constructor.
		 *
		 * @param name The name of this object.
		 */
		explicit ActionList(const std::string& name = "");

		/**
		 * @brief Copy constructor.
		 *
		 * @param other The object being copied.
		 */
		ActionList(const ActionList& other);

		/**
		 * @brief Copy Assignment.
		 *
		 * @param other The object being copied.
		 *
		 * @return Returns a reference to this object.
		 */
		ActionList& operator=(const ActionList& other);

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
		ActionList(ActionList&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The object we are moving. It
		 *				is reset to a default state.
		 *
		 * @return Returns a reference to this.
		 */
		ActionList& operator=(ActionList&& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~ActionList();

		/**
		 * @brief Update method for the simulation loop.
		 *		  Updates this object and calls update
		 *		  on child Actions.
		 *
		 * @param worldState Data of the current simulation
		 *					 state.
		 */
		virtual void update(WorldState& worldState) override;

		/**
		 * @brief Creates a child Action.
		 *
		 * @param className The type name of the child we
		 *					are creating as a string.
		 * @param objectName The name to give the new child
		 *					 Action.
		 *
		 * @return Returns a pointer to the new child object.
		 *		   If the child Action was not created, returns
		 *		   nullptr.
		 */
		Action* createAction(const std::string& className, const std::string& objectName);

		/**
		 * @brief Retrieves this object's child Actions.
		 *
		 * @return Returns a reference to the Datum holding
		 *		   this object's child Actions.
		 */
		Datum& getActions() const;

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

	public:

		const static std::string sActionsAttribute;
	};
}
