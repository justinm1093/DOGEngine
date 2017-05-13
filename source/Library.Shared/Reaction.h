
#pragma once

#include "ActionList.h"
#include "IEventSubscriber.h"

namespace DOGEngine
{
	/**
	 * Abstract class defining an Action that
	 * receives Event<EventArgs> notifications,
	 * and calls its ActionList update method.
	 *
	 * In this way, many actions can be executed
	 * in order when an event is fired.
	 */
	class Reaction : public ActionList, public IEventSubscriber
	{
		RTTI_DECLARATIONS(Reaction, ActionList)

	public:

		/**
		 * @brief Constructor.
		 *
		 * @param name The name of this object.
		 */
		explicit Reaction(const std::string& name = "");

		/**
		 * @brief Copy constructor.
		 *
		 * @param other The object being copied.
		 */
		Reaction(const Reaction& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~Reaction() = default;
	};

#define REACTION_FACTORY(DerivedReaction) FACTORY_DECLARATION(DerivedReaction, Reaction)
}
