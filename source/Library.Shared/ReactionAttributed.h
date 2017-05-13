
#pragma once

#include "Reaction.h"

namespace DOGEngine
{
	/**
	 * Attributed class that receives Event<EventArgs>
	 * notifications and executes child Actions.
	 *
	 * ReactionAttributed has an array of event subtypes.
	 * The subtype of incoming events is validated against
	 * this list.
	 *
	 * Child actions are executed in order if the list
	 * of handleable subtypes contains the subtype of
	 * the incoming event.
	 */
	class ReactionAttributed : public Reaction
	{
		RTTI_DECLARATIONS(ReactionAttributed, Reaction)
		REACTION_FACTORY(ReactionAttributed)

	public:

		/**
		 * @brief Constructor.
		 *
		 * @param name The name of this object.
		 */
		explicit ReactionAttributed(const std::string& name = "");

		/**
		 * @brief Copy constructor.
		 *
		 * @param other The object being copied.
		 */
		ReactionAttributed(const ReactionAttributed& other);

		/**
		 * @brief Copy assignment.
		 *
		 * @param other The object being copied.
		 *
		 * @return Returns a reference to this.
		 */
		ReactionAttributed& operator=(const ReactionAttributed& other);

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
		ReactionAttributed(ReactionAttributed&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The object being moved. It
		 *				is reset to a default state.
		 *
		 * @return Returns a reference to this.
		 */
		ReactionAttributed& operator=(ReactionAttributed&& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~ReactionAttributed();

		/**
		 * @brief Receives events of type Event<EventArgs>.
		 *		  Validates the event's subtype against a list
		 *		  of subtypes this object can handle. Calls on
		 *		  child Actions if the event is handled.
		 *
		 * @param e The incoming event.
		 */
		virtual void notify(const EventPublisher& e) override;

		/**
		 * @brief Adds a new event subtype to this object's
		 *		  list of subtypes.
		 *
		 * @param subtype The subtype being added.
		 */
		void addSubtype(const std::string& subtype);

		/**
		 * @brief Removes a handled event subtype from
		 *		  this object's list of subtypes.
		 *
		 * @param subtype The subtype being removed.
		 *
		 * @return Returns true if the subtype was removed.
		 *		   Otherwise, returns false.
		 */
		bool removeSubtype(const std::string& subtype);

		/**
		 * @brief Retrieves the Datum holding this object's
		 *		  list of subtypes.
		 *
		 * @return Returns a const reference to the subtypes
		 *		   Datum.
		 */
		const Datum& getSubtypes() const;

		/**
		 * @brief Says whether this object handles events
		 *		  of the given subtype.
		 *
		 * @param subtype The subtype we are asking about.
		 *
		 * @return Returns true if the given subtype is
		 *		   in this object's list of subtypes.
		 *		   Otherwise, returns false.
		 */
		bool isSubtype(const std::string& subtype) const;

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

		std::mutex mMutex;

	public:

		const static std::string sSubtypeAttribute;
	};
}
