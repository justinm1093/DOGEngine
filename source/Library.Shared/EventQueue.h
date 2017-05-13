
#pragma once

#include "GameTime.h"
#include "EventPublisher.h"
#include "Vector.h"

namespace DOGEngine
{
	/**
	 * Manages a queue of EventPublisher objects.
	 * Delivers expired events to subscribers via
	 * the simulation update.
	 *
	 * Enqueued events must be heap-allocated, and
	 * are consumed by the EventQueue, so no action
	 * to delete the created Events is necessary.
	 */
	class EventQueue final
	{
	public:

		/**
		 * @brief Constructor.
		 */
		EventQueue();

		/**
		 * @brief Copy constructor.
		 *
		 * @param other The object being copied.
		 */
		EventQueue(const EventQueue& other);

		/**
		 * @brief Copy assignment.
		 *
		 * @param other The object being copied.
		 *
		 * @return Returns a reference to this.
		 */
		EventQueue& operator=(const EventQueue& other);

		/**
		 * @brief Move constructor.
		 *
		 * @param other The object being moved. It
		 *				is reset to a default state.
		 */
		EventQueue(EventQueue&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The object being moved. It
		 *				is reset to a default state.
		 *
		 * @return Returns a reference to this.
		 */
		EventQueue& operator=(EventQueue&& other);

		/**
		 * @brief Destructor.
		 */
		virtual ~EventQueue();

		/**
		 * @brief Delivers any expired events.
		 *
		 * @param gameTime Reference to the game's
		 *				   time keeper.
		 */
		void update(const GameTime& gameTime);

		/**
		 * @brief Adds an event to the queue.
		 *
		 * @param publisher The event being added.
		 * @param gameTime Reference to the game's
		 *				   time keeper.
		 * @param delay The millisecond delay to
		 *				this object's delivery.
		 *				Defaults to 0.
		 */
		void enqueue(EventPublisher& publisher, const GameTime& gameTime, std::chrono::milliseconds delay = std::chrono::milliseconds(0));

		/**
		 * @brief adds an event to the queue.
		 *
		 * @param publisher SharedPtr to the event
		 *					being added.
		 * @param gameTime Reference to the game's
		 *				   time keeper.
		 * @param delay The millisecond delay to
		 *
		 */
		void enqueue(const std::shared_ptr<EventPublisher>& publisher, const GameTime& gameTime, std::chrono::milliseconds delay = std::chrono::milliseconds(0));

		/**
		 * @brief Delivers an event immediately. The
		 *		  event is consumed in the process.
		 *
		 * @param publisher The event being delivered.
		 */
		void send(EventPublisher& publisher);

		/**
		 * @brief Delivers an event immediately. The
		 *		  event is given as a shared pointer,
		 *		  so it may be kept by the caller.
		 *
		 * @param publisher The event being delivered.
		 */
		void send(const std::shared_ptr<EventPublisher>& publisher);

		/**
		 * @brief Empties the contents of the queue.
		 *		  Expired events are NOT delivered.
		 */
		void clear();

		/**
		 * @brief Retrieves the number of events in the
		 *		  queue.
		 *
		 * @return Returns the size of the queue.
		 */
		std::uint32_t size() const;

		/**
		 * @brief Says whether the queue is empty.
		 *
		 * @return Returns true if the queue has no inserted
		 *		   events. Otherwise returns false.
		 */
		bool isEmpty() const;

	private:

		typedef Vector<std::shared_ptr<EventPublisher>> Events;
		Events mEvents;

		mutable std::mutex mMutex;
	};
}
