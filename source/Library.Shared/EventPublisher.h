
#pragma once

#include "RTTI.h"
#include "Vector.h"

#include "IEventSubscriber.h"
#include "GameTime.h"

namespace DOGEngine
{
	/**
	 * Abstract base class for Events. Calls upon
	 * a list of subscribed handler objects when
	 * delivered.
	 *
	 * Events can be time-delayed, and will wait
	 * for the designated time to be delivered.
	 */
	class IEventSubscriber;
	class EventPublisher abstract : public RTTI
	{
		RTTI_DECLARATIONS(EventPublisher, RTTI)

	protected:

		typedef Vector<IEventSubscriber*> Subscribers;

	public:

		/**
		 * @brief Constructor.
		 *
		 * @param subscribers Reference to the derived class'
		 *					  static subscriber list.
		 */
		explicit EventPublisher(Subscribers& subscribers, std::mutex& mutex);

		/**
		 * @brief Copy constructor.
		 *
		 * @param other The object being copied.
		 */
		EventPublisher(const EventPublisher& other);

		/**
		 * @brief Copy assignment.
		 *
		 * @param other The object being copied.
		 *
		 * @return Returns a reference to this.
		 */
		EventPublisher& operator=(const EventPublisher& other);

		/**
		 * @brief Move constructor.
		 *
		 * @param other The object being moved. It
		 *				is reset to a default state.
		 */
		EventPublisher(EventPublisher&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The object being moved. It
		 *				is reset to a default state.
		 *
		 * @return Returns a reference to this.
		 */
		EventPublisher& operator=(EventPublisher&& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~EventPublisher();

		/**
		 * @brief Delivers this object to its 
		 *		  subscribers.
		 */
		void deliver();

		/**
		 * @brief Sets time-related member variables.
		 *
		 * @param timeEnqueued Says at what time this object
		 *					   was added to an EventQueue.
		 * @param delay Says how many milliseconds this object
		 *				will delay its delivery after being
		 *				added to an EventQueue. Defaults to 0.
		 */
		void setTime(const std::chrono::high_resolution_clock::time_point& timeEnqueued, const std::chrono::milliseconds& delay = std::chrono::milliseconds(0));

		/**
		 * @brief Retrieves the time at which this object was
		 *		  added to an EventQueue.
		 *
		 * @return Returns mTimeEnqueued
		 */
		const std::chrono::high_resolution_clock::time_point& timeEnqueued() const;

		/**
		 * @brief Retrieves the number of milliseconds this
		 *		  object delays delivery after being added
		 *		  to an EventQueue.
		 *
		 * @return Returns mDelay
		 */
		const std::chrono::milliseconds& delay() const;

		/**
		 * @brief Retrieves the number of objects subscribed
		 *		  to this type of event.
		 *
		 * @return Returns the size of the mSubscribers Vector.
		 */
		std::uint32_t numSubscribers() const;

		/**
		 * @brief Says whether this object will be delivered
		 *		  the next time its EventQueue updates.
		 *
		 * @param gameTime Reference to the game's time-keeper.
		 *
		 * @return Returns true if the game's current time is
		 *		   greater than the enqueued time and delay.
		 *		   Otherwise, returns false.
		 */
		bool isExpired(const GameTime& gameTime) const;

	protected:

		Subscribers* mSubscribers;
		std::mutex* mMutex;

	private:

		std::chrono::high_resolution_clock::time_point mTimeEnqueued;
		std::chrono::milliseconds mDelay;
	};
}
