
#pragma once

#include "Scope.h"
#include "Vector.h"

namespace DOGEngine
{
	/**
	 * Static class that manages a queue of Scopes
	 * pending deletion at the end of the simulation
	 * loop's execution.
	 */
	class PendingDelete final
	{
	public:

		PendingDelete(const PendingDelete& other) = delete;
		PendingDelete& operator=(const PendingDelete& other) = delete;

		/**
		 * @brief Constructor.
		 */
		PendingDelete();

		/**
		 * @brief Move constructor.
		 *
		 * @param other The object being moved.
		 *				It is reset to a default
		 *				state.
		 */
		PendingDelete(PendingDelete&& other);

		/**
		 * @brief Move assignment.
		 *
		 * @param other The object being moved.
		 *				It is reset to a default
		 *				state.
		 *
		 * @return Returns a reference to this.
		 */
		PendingDelete& operator=(PendingDelete&& other);

		/**
		 * @brief Destructor.
		 */
		~PendingDelete();

		/**
		 * @brief Pushes the given Scope onto the pending
		 *		  delete queue.
		 *
		 * @param scope The incoming Scope.
		 *
		 * @note If the incoming Scope has an ancestor that
		 *		 is already on the queue, the Scope is not
		 *		 added (it will be deleted with its ancestor
		 *		 anyway).
		 * @note Likewise, if the incoming Scope has descendants
		 *		 on the queue, those descendants are removed (as
		 *		 they will be deleted along with their ancestor).
		 */
		void enqueue(Scope& scope);

		/**
		 * @brief Empties the pending delete queue. Objets in
		 *		  the queue are deleted from memory.
		 *
		 * @note This should be called at the end of the game
		 *		 loop each frame.
		 */
		void empty();

		/**
		 * @brief Clears the pending delete queue. Objects in
		 *		  the queue are not deleted.
		 */
		void clear();

		/**
		 * @brief Says whether the given Scope is pending deletion
		 *		  the next time clearQueue is called.
		 *
		 * @param scope The scope we are checking for pending
		 *				delete.
		 *
		 * @return Returns true if the Scope would be deleted the
		 *		   next time clearQueue is called. Otherwise, returns
		 *		   false.
		 *
		 * @note A Scope need not be actually inserted to the queue
		 *		 to be considered "pending deletion". A Scope is
		 *		 said to be "pending delete" if it or one of its
		 *		 ancestors is inserted to the queue.
		 */
		bool isPendingDelete(Scope& scope);

		/**
		 * @brief Says how many Scope objects are currently
		 *		  pushed onto the queue.
		 *
		 * @return Returns the number of objects in the queue.
		 */
		std::uint32_t size();

	private:

		Vector<Scope*> mPending;
		std::recursive_mutex mMutex;
	};
}
