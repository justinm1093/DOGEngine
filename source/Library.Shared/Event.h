
#pragma once

#include "pch.h"
#include "EventPublisher.h"

namespace DOGEngine
{
	/**
	 * Templated Event class that defines an Event
	 * capable of delivering some data-payload (the
	 * template argument).
	 *
	 * Each templated Event has its own static list
	 * of subscriber objects that are called when
	 * the event is delivered.
	 */
	template <typename T>
	class Event final : public EventPublisher
	{
		RTTI_DECLARATIONS(Event<T>, EventPublisher)

	public:

		/**
		 * @brief Constructor.
		 *
		 * @param message Reference to an object that
		 *				  this Event delivers. A payload.
		 */
		explicit Event(const T& message);

		
		/**
		 * @brief Copy constructor.
		 *
		 * @param other The object being copied.
		 */
		Event(const Event& other);

		/**
		 * @brief Copy assignment.
		 *
		 * @param other The object being copied.
		 *
		 * @return Returns a reference to this.
		 */
		Event& operator=(const Event& other);

		/**
		 * @brief Move constructor.
		 *
		 * @param other The object being moved. It
		 *				is reset to a default state.
		 */
		Event(Event&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The object being moved. It
		 *				is reset to a default state.
		 *
		 * @return Returns a reference to this.
		 */
		Event& operator=(Event&& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~Event();

		/**
		 * @brief Adds an EventSubscriber to the static
		 *		  list of subscribers.
		 *
		 * @param subscriber Reference to a subscriber
		 *					 object.
		 */
		static void subscribe(IEventSubscriber& subscriber);

		/**
		 * @brief Removes an EventSubscriber from the static
		 *		  list of subscribers.
		 *
		 * @param subscriber Reference to a subscriber
		 *					 object.
		 */
		static void unsubscribe(IEventSubscriber& subscriber);

		/**
		 * @brief Removes all subscriber objects from the
		 *		  static list of subscribers.
		 */
		static void unsubscribeAll();

		/**
		 * @brief Retrieves the number of subscribers to this
		 *		  event.
		 *
		 * @return Returns the size of this type's static
		 *		   subscriber list.
		 */
		static std::uint32_t numSubscribers();

		/**
		 * @brief Retrieves the data-payload for this object.
		 *
		 * @return Returns a const reference to the data that
		 *		   is carried with this Event.
		 */
		const T& message() const;

	private:

		T mMessage;

		static Subscribers sSubscribers;
		static std::mutex sMutex;
	};

	template <typename T> RTTI_DEFINITIONS(Event<T>)
	template <typename T> EventPublisher::Subscribers Event<T>::sSubscribers;
	template <typename T> std::mutex Event<T>::sMutex;
}

#include "Event.inl"
