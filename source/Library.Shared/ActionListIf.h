
#pragma once

#include "ActionList.h"
#include "RTTI.h"

namespace DOGEngine
{
	/**
	 * Action class that represents a condition-based
	 * branch in code.
	 * 
	 * Non-zero values in the "condition" result in
	 * execution of the "then" block (which can be
	 * any Action, including ActionList or another
	 * ActionListIf).
	 *
	 * Zero values in the "condition" result in
	 * execution of the "else" block (which can
	 * similarly be of any Action type).
	 *
	 * The "then" and "else" blocks need not be set.
	 * If either is null and would be executed, the
	 * branch is not taken.
	 */
	class ActionListIf final : public ActionList
	{
		RTTI_DECLARATIONS(ActionListIf, ActionList)
		ACTION_FACTORY(ActionListIf)

	public:

		/**
		 * @brief Constructor.
		 *
		 * @param name The name of this object.
		 */
		explicit ActionListIf(const std::string& name = "");

		/**
		 * @brief Copy constructor.
		 *
		 * @param other The object being copied.
		 */
		ActionListIf(const ActionListIf& other);

		/**
		 * @brief Copy Assignment.
		 *
		 * @param other The object being copied.
		 *
		 * @return Returns a reference to this object.
		 */
		ActionListIf& operator=(const ActionListIf& other);

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
		ActionListIf(ActionListIf&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The object we are moving. It
		 *				is reset to a default state.
		 *
		 * @return Returns a reference to this.
		 */
		ActionListIf& operator=(ActionListIf&& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~ActionListIf();

		/**
		 * @brief Update method for the simulation loop.
		 *		  Chooses an Action to execute based on
		 *		  some condition. Non-zero executes the
		 *		  "then" Action. Zero executes the "else"
		 *		  Action.
		 *
		 * @param worldState Data of the current simulation
		 *					 state.
		 */
		virtual void update(WorldState& worldState) override;

		/**
		 * @brief Creates an Action of the given type with
		 *		  the given name for the "then" block.
		 *
		 * @param className The name of the Action type for
		 *					the "then" block.
		 * @param objName The name of the new "then" block
		 *				  Action.
		 *
		 * @return Returns a pointer to the new child object.
		 *		   If the child Action was not created, returns
		 *		   nullptr.
		 */
		Action* createThenBlock(const std::string& className, const std::string& objName);

		/**
		 * @brief Creates an Action of the given type with
		 *		  the given name for the "else" block.
		 *
		 * @param className The name of the Action type for
		 *					the "else" block.
		 * @param objName The name of the new "else" block
		 *				  Action.
		 *
		 * @return Returns a pointer to the new child object.
		 *		   If the child Action was not created, returns
		 *		   nullptr.
		 */
		Action* createElseBlock(const std::string& className, const std::string& objName);

		/**
		 * @brief Retrieves the Action executed on non-zero
		 *		  conditions.
		 *
		 * @return Returns a pointer to the "then" attribute's
		 *		   Action.
		 */
		Action* getThenBlock() const;

		/**
		 * @brief Retrieves the Action executed on zero
		 *		  conditions.
		 *
		 * @return Returns a pointer to the "else" attribute's
		 *		   Action.
		 */
		Action* getElseBlock() const;

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

		std::int32_t mCondition;	// TODO -- eventually this is an expression action???
		Scope* mThenBlock;
		Scope* mElseBlock;

	public:

		const static std::string sConditionAttribute;
		const static std::string sThenAttribute;
		const static std::string sElseAttribute;
	};
}
