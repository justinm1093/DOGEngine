
#pragma once

#include "Action.h"

namespace DOGEngine
{
	/**
	 * Action class that creates a new Action of
	 * some type and has its immediate parent 
	 * adopt it.
	 *
	 * Assumes that its parent is an Entity or
	 * ActionList.
	 */
	class ActionCreateAction final : public Action
	{
		RTTI_DECLARATIONS(ActionCreateAction, Action)
		ACTION_FACTORY(ActionCreateAction)

	public:

		/**
		 * @brief Constructor.
		 *
		 * @param name The name of this object.
		 */
		explicit ActionCreateAction(const std::string& name = "");

		/**
		 * @brief Copy constructor.
		 *
		 * @param other The object being copied.
		 */
		ActionCreateAction(const ActionCreateAction& other);

		/**
		 * @brief Copy Assignment.
		 *
		 * @param other The object being copied.
		 *
		 * @return Returns a reference to this object.
		 */
		ActionCreateAction& operator=(const ActionCreateAction& other);

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
		ActionCreateAction(ActionCreateAction&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The object we are moving. It
		 *				is reset to a default state.
		 *
		 * @return Returns a reference to this.
		 */
		ActionCreateAction& operator=(ActionCreateAction&& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~ActionCreateAction();

		/**
		 * @brief Update method for the simulation loop.
		 *		  Creates a new Action on its parent of
		 *		  the saved class name with the saved
		 *		  instance name.
		 *
		 * @param worldState Data of the current simulation
		 *					 state.
		 */
		virtual void update(WorldState& worldState);

		/**
		 * @brief Sets the name of the Action type that this
		 *		  object creates.
		 *
		 * @param name The new type name.
		 */
		void setClassName(const std::string& name);

		/**
		 * @brief Sets the name that this object gives its
		 *		  created action.
		 *
		 * @param name The new created Action name.
		 */
		void setInstanceName(const std::string& name);

		/**
		 * @brief Retrieves the name of the type that this
		 *		  object creates.
		 *
		 * @return Returns the name of the Action type this
		 *		   object creates.
		 */
		std::string getClassName() const;

		/**
		 * @brief Retrieves the name that this object gives
		 *		  its created Action.
		 *
		 * @return Returns the name that this object gives
		 *		   its created Action.
		 */
		std::string getInstanceName() const;

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

		std::string mClassName;
		std::string mInstanceName;

	public:

		const static std::string sClassNameAttribute;
		const static std::string sInstanceNameAttribute;
	};
}
