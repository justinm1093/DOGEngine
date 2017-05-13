
#pragma once

#include "Action.h"

namespace DOGEngine
{
	/**
	 * Action class that marks for deletion an
	 * Action on parent with the given name.
	 * The object marked for deletion is destroyed
	 * at the end of the current frame.
	 *
	 * Assumes that its parent is an Entity or
	 * ActionList.
	 */
	class ActionDestroyAction final : public Action
	{
		RTTI_DECLARATIONS(ActionDestroyAction, Action)
		ACTION_FACTORY(ActionDestroyAction)

	public:

		/**
		 * @brief Constructor.
		 *
		 * @param name The name of this object.
		 */
		explicit ActionDestroyAction(const std::string& name = "");

		/**
		 * @brief Copy constructor.
		 *
		 * @param other The object being copied.
		 */
		ActionDestroyAction(const ActionDestroyAction& other);

		/**
		 * @brief Copy Assignment.
		 *
		 * @param other The object being copied.
		 *
		 * @return Returns a reference to this object.
		 */
		ActionDestroyAction& operator=(const ActionDestroyAction& other);

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
		ActionDestroyAction(ActionDestroyAction&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The object we are moving. It
		 *				is reset to a default state.
		 *
		 * @return Returns a reference to this.
		 */
		ActionDestroyAction& operator=(ActionDestroyAction&& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~ActionDestroyAction();

		/**
		 * @brief Update method for the simulation loop.
		 *		  Marks the Action on parent with the saved
		 *		  name for deletion at the end of this frame.
		 *
		 * @param worldState Data of the current simulation
		 *					 state.
		 */
		virtual void update(WorldState& worldState);

		/**
		 * @brief Sets the name of the Action for which this
		 *		  object will search to delete.
		 *
		 * @param target The new name for the delete search
		 *				 target.
		 */
		void setDeleteTarget(const std::string& target);

		/**
		 * @brief Retrieves the Action name for which this
		 *		  object searches to delete.
		 *
		 * @return Returns the Action name for which this
		 *		   object searches to delete.
		 */
		std::string getDeleteTarget() const;

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

		std::string mDeleteTarget;

	public:

		const static std::string sDeleteTargetAttribute;
	};
}
