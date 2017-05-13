
#pragma once

#include "RTTI.h"
#include "Attributed.h"

#include "WorldState.h"
#include "Sector.h"

#include "PendingDelete.h"
#include "EventQueue.h"

namespace DOGEngine
{
	/**
	 * Attributed class representing a World, the root
	 * of the simulation hierarchy. Holds Sectors.
	 */
	class World final : public Attributed
	{
		RTTI_DECLARATIONS(World, Attributed)

	public:

		/**
		 * @brief Constructor. Creates a World with
		 *		  the given name.
		 *
		 * @param name The name for this World. Defaults
		 *			   to empty string ""
		 */
		explicit World(const std::string& name = "");

		/**
		 * @brief Copy constructor.
		 *
		 * @param other The World we are copying.
		 */
		World(const World& other);

		/**
		 * @brief Copy assignment.
		 *
		 * @param other The World we are copying.
		 *
		 * @return Returns a reference to this.
		 */
		World& operator=(const World& other);

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
		 * @param other The World we are moving.
		 *				It is reset to a default state.
		 */
		World(World&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The World we are moving.
		 *				It is reset to a default state.
		 *
		 * @return Returns a reference to this.
		 */
		World& operator=(World&& other);

		/**
		 * @brief Destructor.
		 */
		~World();

		/**
		 * @brief Update method for the simulation loop.
		 *		  Updates this object and calls update
		 *		  on child Sectors.
		 */
		void update();

		/**
		 * @brief Creates a child Sector.
		 *
		 * @param name The name to give the new child Sector.
		 *
		 * @return Returns a pointer to the new child Sector.
		 */
		Sector* createSector(const std::string& name);

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
		 * @brief Retrieves the pending delete queue.
		 *
		 * @return Returns a reference to mPendingDelete
		 */
		PendingDelete& getPendingDelete();

		/**
		 * @brief Retrieves the event queue.
		 *
		 * @return Returns a reference to mEventQueue
		 */
		EventQueue& getEventQueue();

		/**
		 * @brief Retrieves the world state.
		 *
		 * @return Returns a reference to mState
		 */
		WorldState& getWorldState();

		/**
		 * @brief Retrieves the world state.
		 *
		 * @return Returns a const reference to mState
		 */
		const WorldState& getWorldState() const;

		/**
		 * @brief Retrieves this object's name. Convenience
		 *		  method to avoid table lookup.
		 *
		 * @return Returns this object's name.
		 */
		const std::string& getName() const;

		/**
		 * @brief Retrieves this object's child Sectors.
		 *
		 * @return Returns a reference to the Datum holding
		 *		   this object's child Sectors.
		 */
		Datum& getSectors() const;

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

		PendingDelete mPendingDelete;
		EventQueue mEventQueue;
		WorldState mState;

		std::string mName;

	public:

		const static std::string sNameAttribute;
		const static std::string sSectorsAttribute;
		const static std::string sActionsAttribute;
		const static std::string sReactionsAttribute;
	};
}
