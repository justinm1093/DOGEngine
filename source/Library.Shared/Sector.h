
#pragma once

#include "RTTI.h"
#include "Attributed.h"

#include "WorldState.h"
#include "Entity.h"

namespace DOGEngine
{
	/**
	 * Attributed class representing a Sector in the
	 * game world. Holds Entities, and is a child of
	 * Worlds.
	 */
	class World;
	class Sector final : public Attributed
	{
		RTTI_DECLARATIONS(Sector, Attributed)

	public:

		/**
		 * @brief Constructor. Creates a Sector with
		 *		  the given name.
		 *
		 * @param name The name for this Sector. Defaults
		 *			   to empty string ""
		 */
		explicit Sector(const std::string& name = "");

		/**
		 * @brief Copy constructor.
		 *
		 * @param other The World we are copying.
		 */
		Sector(const Sector& other);

		/**
		 * @brief Copy assignment.
		 *
		 * @param other The World we are copying.
		 *
		 * @return Returns a reference to this.
		 */
		Sector& operator=(const Sector& other);

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
		 * @param other The Sector we are moving.
		 *				It is reset to a default state.
		 */
		Sector(Sector&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The Sector we are moving.
		 *				It is reset to a default state.
		 *
		 * @return Returns a reference to this.
		 */
		Sector& operator=(Sector&& other);

		/**
		 * @brief Destructor.
		 */
		~Sector();

		/**
		 * @brief Update method for the simulation loop.
		 *		  Updates this object and calls update
		 *		  on child Entities.
		 *
		 * @param worldState Data of the current simulation
		 *					 state.
		 */
		void update(WorldState& worldState);

		/**
		 * @brief Creates a child Entity.
		 *
		 * @param className The type name of the child we
		 *					are creating as a string.
		 * @param objectName The name to give the new child
		 *					 Entity.
		 *
		 * @return Returns a pointer to the new child object.
		 *		   If the child Entity was not created, returns
		 *		   nullptr.
		 */
		Entity* createEntity(const std::string& className, const std::string& objectName);

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
		 * @brief Retrieves this object's parent as a World.
		 *
		 * @return Returns a pointer to this object's parent
		 *		   World.
		 */
		World* getWorld() const;

		/**
		 * @brief Retrieves this object's child Entities.
		 *
		 * @return Returns a reference to the Datum holding
		 *		   this object's child Entities.
		 */
		Datum& getEntities() const;

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
		const static std::string sEntitiesAttribute;
		const static std::string sActionsAttribute;
		const static std::string sReactionsAttribute;
	};
}
