
#pragma once

namespace DOGEngine
{
	/**
	 * Interface that declares an abstract method for
	 * handling event delivery.
	 *
	 * Classes that inherit from this should subscribe
	 * and unsubscribe from Events on construction and
	 * deletion.
	 */
	class EventPublisher;
	class IEventSubscriber
	{
	public:

		/**
		 * @brief Handler method that EventPublishers
		 *		  call when they are delivered. Notify
		 *		  is invoked on each object subscribed
		 *		  to the event.
		 *
		 * @param e Pointer to the EventPublisher to
		 *			which we are subscribed. Type should 
		 *			be validated through RTTI.
		 */
		virtual void notify(const EventPublisher& e) = 0;
	};
}
