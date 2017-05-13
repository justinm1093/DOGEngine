
#pragma once

#include "RTTI.h"
#include "Attributed.h"
#include "Action.h"
#include "Reaction.h"

#include "WorldState.h"
#include "Factory.h"

namespace DOGEngine
{
	/**
	 * Attributed class representing an Entity in the
	 * game world. Holds Actions, and is a child of
	 * Sectors.
	 */
	class Sector;
	class Entity : public Attributed
	{
		RTTI_DECLARATIONS(Entity, Attributed)
		FACTORY_DECLARATION(Entity, Entity)

	public:

		/**
		 * @brief Constructor. Creates an Entity with
		 *		  the given name.
		 *
		 * @param name The name for this Entity. Defaults
		 *			   to empty string ""
		 */
		explicit Entity(const std::string& name = "");

		/**
		 * @brief Copy constructor.
		 *
		 * @param other The Entity we are copying.
		 */
		Entity(const Entity& other);

		/**
		 * @brief Copy assignment.
		 *
		 * @param other The Entity we are copying.
		 *
		 * @return Returns a reference to this.
		 */
		Entity& operator=(const Entity& other);

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
		Entity(Entity&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The Entity we are moving.
		 *				It is reset to a default state.
		 *
		 * @return Returns a reference to this.
		 */
		Entity& operator=(Entity&& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~Entity();

		/**
		 * @brief Update method for the simulation loop.
		 *		  Updates this object and calls update
		 *		  on child Actions.
		 *
		 * @param worldState Data of the current simulation
		 *					 state.
		 */
		virtual void update(WorldState& worldState);

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
		 * @brief Creates a child Reaction.
		 *
		 * @param className The type name of the child we
		 *					are creating as a string.
		 * @param objectName The name to give the new child
		 *					 Reaction.
		 *
		 * @return Returns a pointer to the new child object.
		 *		   If the child Action was not created, returns
		 *		   nullptr.
		 */
		Reaction* createReaction(const std::string& className, const std::string& objectName);

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

		/**
		 * @brief Retrieves this object's parent as a Sector.
		 *
		 * @return Returns a pointer to this object's parent
		 *		   Sector.
		 */
		Sector* getSector() const;

		/**
		 * @brief Retrieves this object's child Actions.
		 *
		 * @return Returns a reference to the Datum holding
		 *		   this object's child Actions.
		 */
		Datum& getActions() const;

		/**
		 * @brief Retrieves this object's child Reactions.
		 *
		 * @return Returns a reference to the Datum holding
		 *		   this object's child Reactions.
		 */
		Datum& getReactions() const;

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

		std::string mName;

	public:

		const static std::string sNameAttribute;
		const static std::string sActionsAttribute;
		const static std::string sReactionsAttribute;
	};
}
