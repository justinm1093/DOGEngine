
#pragma once

#include "GameTime.h"

namespace DOGEngine
{
	class World;
	class Sector;
	class Entity;
	class Action;

	/**
	 * Container for simulation state data.
	 * Holds pointers to the current World,
	 * Sector, Entity, and Action, and a
	 * class that tracks time.
	 *
	 * Functionally does nothing on its own,
	 * but is passed by reference to others
	 * that act upon its stored data.
	 */
	struct WorldState final
	{
		/**
		 * @brief Constructor.
		 */
		WorldState() : 
			world(nullptr), 
			sector(nullptr), 
			entity(nullptr),
			action(nullptr),
			gameTime()
		{};

		World* world;
		Sector* sector;
		Entity* entity;
		Action* action;

		GameTime gameTime;
	};
}
