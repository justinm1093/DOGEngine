
#include "pch.h"
#include "CppUnitTest.h"

#include "GameClock.h"

#include "Factory.h"

#include "XmlParseMaster.h"
#include "XmlParseHelperSubfile.h"
#include "XmlParseHelperTable.h"
#include "XmlParseHelperData.h"
#include "SharedDataTable.h"

#include "WorldState.h"
#include "World.h"
#include "Sector.h"
#include "Entity.h"

#include "EntityFoo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::chrono;
using namespace DOGEngine;
using namespace UnitTests;
using namespace std;
using namespace glm;

namespace LibraryDesktopTest
{
	TEST_CLASS(EntityTest)
	{
	public:

		TEST_METHOD_INITIALIZE(Initialize)
		{
#ifdef _DEBUG
			// grab snapshot of memory state at start of test
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
			_CrtMemCheckpoint(&sStartMemState);
#endif

			sEntityFactory = new Entity::EntityFactory();
			sEntityFooFactory = new EntityFoo::EntityFooFactory();
		}

		TEST_METHOD_CLEANUP(Cleanup)
		{
			delete sEntityFactory;
			delete sEntityFooFactory;

			Attributed::clearAttributeCache();

			XmlParseHelperData::clearHandlerCache();
			XmlParseHelperTable::clearHandlerCache();

#ifdef _DEBUG
			_CrtMemState endMemState, diffMemState;

			// grab snapshot of of memory state at end of test and
			// compare it against the starting memory state
			_CrtMemCheckpoint(&endMemState);
			if(_CrtMemDifference(&diffMemState, &sStartMemState, &endMemState))
			{
				// memory leak if difference between starting and ending memory states
				_CrtMemDumpStatistics(&diffMemState);
				Assert::Fail(L"Memory leak detected!");
			}
#endif
		}


		TEST_METHOD(EntityRTTI)
		{
			EntityRTTIHelper<World, Attributed>("World", "Attributed");
			EntityRTTIHelper<Sector, Attributed>("Sector", "Attributed");
			EntityRTTIHelper<Entity, Attributed>("Entity", "Attributed");
			EntityRTTIHelper<EntityFoo, Entity>("EntityFoo", "Entity");

			EntityRTTIHelper<XmlParseHelperTable, IXmlParseHelper>("XmlParseHelperTable", "IXmlParseHelper");
		}

		TEST_METHOD(EntityConstructors)
		{
			// scope types
			EntityConstructorsHelper<World>("World");
			EntityConstructorsHelper<Sector>("Sector");
			EntityConstructorsHelper<Entity>("Entity");
			EntityConstructorsHelper<EntityFoo>("Entity");

			// world state
			WorldState worldState;
			Assert::IsTrue(worldState.world == nullptr);
			Assert::IsTrue(worldState.sector == nullptr);
			Assert::IsTrue(worldState.entity == nullptr);

			// test attributes of entities
			World world;
			Assert::IsTrue(world.isPrescribedAttribute("this"));
			Assert::IsTrue(world.isPrescribedAttribute("name"));
			Assert::IsTrue(world.isPrescribedAttribute("sectors"));

			Sector sector;
			Assert::IsTrue(sector.isPrescribedAttribute("this"));
			Assert::IsTrue(sector.isPrescribedAttribute("name"));
			Assert::IsTrue(sector.isPrescribedAttribute("entities"));

			Entity entity;
			Assert::IsTrue(entity.isPrescribedAttribute("this"));
			Assert::IsTrue(entity.isPrescribedAttribute("name"));

			EntityFoo foo;
			Assert::IsTrue(foo.isPrescribedAttribute("this"));
			Assert::IsTrue(foo.isPrescribedAttribute("name"));
			Assert::IsTrue(foo.isPrescribedAttribute("entity_foo_int"));
		}

		TEST_METHOD(EntityAssignment)
		{
			EntityAssignmentsHelper<World>("World");
			EntityAssignmentsHelper<Sector>("Sector");
			EntityAssignmentsHelper<Entity>("Entity");
			EntityAssignmentsHelper<EntityFoo>("Entity");
		}

		TEST_METHOD(EntityDestructors)
		{
			{
				World world;
				world.addAuxiliaryAttribute("hello");
				world.addAuxiliaryAttribute("me");
			}
			{
				Sector sector;
				sector.addAuxiliaryAttribute("hello");
				sector.addAuxiliaryAttribute("world");
			}
			{
				Entity entity;
				entity.addAuxiliaryAttribute("hello");
				entity.addAuxiliaryAttribute("world");
			}
			{
				EntityFoo foo;
				foo.addAuxiliaryAttribute("hello");
				foo.addAuxiliaryAttribute("world");
			}
			{
				WorldState worldState;
				World world;
				Sector sector;
				Entity entity;

				worldState.world = &world;
				worldState.sector = &sector;
				worldState.entity = &entity;
			}
		}

		TEST_METHOD(EntityWorldCreateSector)
		{
			// worlds have empty sectors datum at start
			World world;
			Datum& sectors = world.getSectors();

			Assert::IsTrue(sectors.isEmpty());
			Assert::IsTrue(sectors.type() == Datum::DatumType::Table);

			// world createSector adopts a new sector with the given name
			Sector* sector = world.createSector("Sector_1");

			Assert::IsTrue(sectors.size() == 1);
			Assert::IsTrue(sector->getName() == "Sector_1");
			Assert::IsTrue(sector->getParent() == &world);
			Assert::IsTrue(sector->getWorld() == &world);

			Sector* otherSector = world.createSector("OtherSector");

			Assert::IsTrue(sectors.size() == 2);
			Assert::IsTrue(otherSector->getName() == "OtherSector");
			Assert::IsTrue(otherSector->getParent() == &world);
			Assert::IsTrue(otherSector->getWorld() == &world);

			// copying from a created sector does not set its parent world
			Sector* copiedSector = new Sector(*sector);
			Assert::IsTrue(*copiedSector == *sector);
			Assert::IsTrue(copiedSector->getParent() != &world);
			Assert::IsTrue(copiedSector->getWorld() != &world);

			// moving from a created sector sets the parent world
			Sector* movedSector = new Sector(std::move(*sector));
			Assert::IsTrue(*movedSector != *sector);
			Assert::IsTrue(*movedSector == *copiedSector);
			Assert::IsTrue(movedSector->getParent() == &world);
			Assert::IsTrue(movedSector->getWorld() == &world);
			Assert::IsTrue(sector->getParent() == nullptr);
			Assert::IsTrue(sector->getWorld() == nullptr);

			delete copiedSector;
			delete sector;
		}

		TEST_METHOD(EntitySectorCreateEntity)
		{
			// sectors have empty entities datum at start
			Sector sector;
			Datum& entities = sector.getEntities();

			Assert::IsTrue(entities.isEmpty());
			Assert::IsTrue(entities.type() == Datum::DatumType::Table);

			// sector createEntity creates entities of given type with given name
			Entity* entity = sector.createEntity("Entity", "Entity_1");

			Assert::IsTrue(entity != nullptr);
			Assert::IsTrue(entities.size() == 1);
			Assert::IsTrue(entity->getName() == "Entity_1");
			Assert::IsTrue(entity->getParent() == &sector);
			Assert::IsTrue(entity->getSector() == &sector);

			Entity* foo = sector.createEntity("EntityFoo", "Foo");
			Assert::IsTrue(foo->Is(EntityFoo::TypeIdClass()));

			Assert::IsTrue(foo != nullptr);
			Assert::IsTrue(entities.size() == 2);
			Assert::IsTrue(foo->getName() == "Foo");
			Assert::IsTrue(foo->getParent() == &sector);
			Assert::IsTrue(foo->getSector() == &sector);

			// sector createEntity returns null if the entity is not created
			Entity* nullEntity = sector.createEntity("NotAnEntity", "Null");
			Assert::IsTrue(nullEntity == nullptr);
			Assert::IsTrue(entities.size() == 2);

			// copying from a created entity does not set its parent sector
			Entity* copiedEntity = new Entity(*entity);
			Assert::IsTrue(*copiedEntity == *entity);
			Assert::IsTrue(copiedEntity->getParent() != &sector);
			Assert::IsTrue(copiedEntity->getSector() != &sector);

			// moving from a created entity sets the parent sector
			Entity* movedEntity = new Entity(std::move(*entity));
			Assert::IsTrue(*movedEntity != *entity);
			Assert::IsTrue(*movedEntity == *copiedEntity);
			Assert::IsTrue(movedEntity->getParent() == &sector);
			Assert::IsTrue(movedEntity->getSector() == &sector);
			Assert::IsTrue(entity->getParent() == nullptr);
			Assert::IsTrue(entity->getSector() == nullptr);

			delete copiedEntity;
			delete entity;
		}

		TEST_METHOD(EntityUpdate)
		{
			GameClock clock;
			
			// create a tree of world-sector-entities to run loop on
			SharedDataTable sharedData;
			XmlParseMaster master(sharedData);

			XmlParseHelperTable helperTable;
			XmlParseHelperData helperData;
			master.addHelper(helperTable);
			master.addHelper(helperData);

			Assert::IsTrue(master.parseFromFile(sXmlFile_Full));
			World* world = sharedData.extractScope()->As<World>();
			Assert::IsTrue(world != nullptr);

			// run an update loop for an amount of seconds
			clock.Reset();
			milliseconds duration(3000);
			while(world->getWorldState().gameTime.TotalGameTime() < duration)
			{
				clock.UpdateGameTime(world->getWorldState().gameTime);
				world->update();
			}

			delete world;
		}

		TEST_METHOD(EntityParseXml)
		{
			SharedDataTable sharedData;
			XmlParseMaster master(sharedData);

			XmlParseHelperTable helperTable;
			XmlParseHelperData helperData;
			master.addHelper(helperTable);
			master.addHelper(helperData);

			// parse, then verify the data
 			Assert::IsTrue(master.parseFromFile(sXmlFile_Full));

			{
				// world
				Scope* extractedScope = sharedData.extractScope();
				Assert::IsTrue(extractedScope->Is(World::TypeIdClass()));

				World* world = extractedScope->As<World>();
				Assert::IsTrue(world->getName() == "Test World");
				Assert::IsTrue((*world)["worldInt"].get<int32_t>() == 1);

				Assert::IsTrue((*world)["sectors"].size() == 2);
				Assert::IsTrue((*world)["sectors"][0].Is(Sector::TypeIdClass()));
				Assert::IsTrue((*world)["sectors"][1].Is(Sector::TypeIdClass()));
				{
					// sector 1
					Sector* sector = (*world)["sectors"][0].As<Sector>();
					Assert::IsTrue(sector->getName() == "Test Sector 1");
					Assert::IsTrue((*sector)["sectorInt"].get<int32_t>() == 0);

					Assert::IsTrue((*sector)["entities"].size() == 2);
					Assert::IsTrue((*sector)["entities"][0].Is(Entity::TypeIdClass()));
					Assert::IsTrue((*sector)["entities"][1].Is(Entity::TypeIdClass()));

					{
						// entity 1
						Entity* entity = (*sector)["entities"][0].As<Entity>();
						Assert::IsTrue(entity->getName() == "Test Entity 1");
						Assert::IsTrue((*entity)["int"].get<int32_t>() == 100);
					}
					{
						// entity 2
						Entity* entity = (*sector)["entities"][1].As<Entity>();
						Assert::IsTrue(entity->Is(EntityFoo::TypeIdClass()));

						EntityFoo* foo = entity->As<EntityFoo>();
						Assert::IsTrue(foo->getName() == "Foo");

						Assert::IsTrue((*foo)["int"].get<int32_t>() == 200);
						Assert::IsTrue((*foo)["entity_foo_int"].size() == 1);
						Assert::IsTrue((*foo)["entity_foo_int"].get<int32_t>() == 10);
					}
				}
				{
					// sector 2
					Sector* sector = (*world)["sectors"][1].As<Sector>();
					Assert::IsTrue(sector->getName() == "Test Sector 2");
					Assert::IsTrue((*sector)["entities"].size() == 2);
					Assert::IsTrue((*sector)["entities"][0].Is(Entity::TypeIdClass()));
					Assert::IsTrue((*sector)["entities"][1].Is(Entity::TypeIdClass()));

					{
						// entity 1
						Entity* entity = (*sector)["entities"][0].As<Entity>();
						Assert::IsTrue(entity->getName() == "Test Entity 1");
						Assert::IsTrue((*entity)["int"].get<int32_t>() == 300);
					}
					{
						// entity 2
						Entity* entity = (*sector)["entities"][1].As<Entity>();
						Assert::IsTrue(entity->getName() == "Test Entity 2");
						Assert::IsTrue((*entity)["int"].get<int32_t>() == 400);
					}
				}

				delete extractedScope;
			}
		}

		TEST_METHOD(EntityParseXmlSubfiles)
		{
			SharedDataTable sharedData;
			XmlParseMaster master(sharedData);

			XmlParseHelperSubfile helperSubfile;
			XmlParseHelperTable helperTable;
			XmlParseHelperData helperData;
			master.addHelper(helperSubfile);
			master.addHelper(helperTable);
			master.addHelper(helperData);

			// parse, then verify the data
			Assert::IsTrue(master.parseFromFile(sXmlFile_Subfiles));

			{
				// world
				Scope* extractedScope = sharedData.extractScope();
				Assert::IsTrue(extractedScope->Is(World::TypeIdClass()));

				World* world = extractedScope->As<World>();
				Assert::IsTrue(world->getName() == "Test World");

				Assert::IsTrue((*world)["sectors"].size() == 1);
				Assert::IsTrue((*world)["sectors"][0].Is(Sector::TypeIdClass()));

				{
					// sector 1
					Sector* sector = (*world)["sectors"][0].As<Sector>();
					Assert::IsTrue(sector->getName() == "Test Sector");

					Assert::IsTrue((*sector)["entities"].size() == 2);
					Assert::IsTrue((*sector)["entities"][0].Is(Entity::TypeIdClass()));
					Assert::IsTrue((*sector)["entities"][1].Is(Entity::TypeIdClass()));

					{
						// entity 1
						Entity* entity = (*sector)["entities"][0].As<Entity>();
						Assert::IsTrue(entity->getName() == "Test Entity 1");
						Assert::IsTrue((*entity)["int"].get<int32_t>() == 100);
					}
					{
						// entity 2
						Entity* entity = (*sector)["entities"][1].As<Entity>();
						Assert::IsTrue(entity->getName() == "Test Entity 2");
						Assert::IsTrue((*entity)["int"].get<int32_t>() == 200);
					}
				}

				delete world;
			}
		}

	private:

		template <typename DerivedT, typename BaseT>
		void EntityRTTIHelper(const string& derivedTypeName, const string& baseTypeName)
		{
			DerivedT obj;
			RTTI* rtti = (RTTI*)(&obj);

			// test type id's
			Assert::IsTrue(DerivedT::TypeIdClass() == obj.TypeIdInstance());
			Assert::IsTrue(rtti->TypeIdInstance() == obj.TypeIdInstance());

			// test type name
			Assert::IsTrue(BaseT::TypeName() == baseTypeName);

			Assert::IsTrue(DerivedT::TypeName() == derivedTypeName);
			Assert::IsTrue(obj.TypeName() == derivedTypeName);

			// test query interface
			Assert::IsTrue(rtti->QueryInterface(DerivedT::TypeIdClass()) != nullptr);

			// test is with string
			Assert::IsTrue(obj.Is(baseTypeName));
			Assert::IsTrue(rtti->Is(baseTypeName));

			Assert::IsTrue(obj.Is(derivedTypeName));
			Assert::IsTrue(rtti->Is(derivedTypeName));

			// test is with int
			Assert::IsTrue(obj.Is(BaseT::TypeIdClass()));
			Assert::IsTrue(rtti->Is(BaseT::TypeIdClass()));

			Assert::IsTrue(obj.Is(DerivedT::TypeIdClass()));
			Assert::IsTrue(rtti->Is(DerivedT::TypeIdClass()));

			// test as
			Assert::IsTrue(rtti->As<BaseT>() != nullptr);
			Assert::IsTrue(rtti->As<DerivedT>() != nullptr);
		}

		template <typename T>
		void EntityConstructorsHelper(const string& defaultName)
		{
			// creating an object with no argument uses some default name
			T objNoName;
			Assert::IsTrue(objNoName.getName() == defaultName);

			// can set a name
			objNoName.setName("New Name");
			Assert::IsTrue(objNoName.getName() == "New Name");

			// creating an object with an argument uses that name
			T obj("MyName");
			Assert::IsTrue(obj.getName() == "MyName");
			Assert::IsTrue(obj["name"].isExternal());
			Assert::IsTrue(obj["name"].get<string>() == obj.getName());

			// copy constructor
			T copiedObj(obj);
			Assert::IsTrue(copiedObj.getName() == "MyName");
			Assert::IsTrue(copiedObj.getName() == obj.getName());
			Assert::IsTrue(copiedObj["this"].get<RTTI*>() != obj["this"].get<RTTI*>());
			Assert::IsTrue(&copiedObj["name"].get<string>() != &obj["name"].get<string>());

			Assert::IsTrue(copiedObj == obj);

			// move constructor
			T movedObj(std::move(obj));
			Assert::IsTrue(obj.getName() == defaultName);
			Assert::IsTrue(movedObj.getName() == "MyName");
			Assert::IsTrue(movedObj.getName() != obj.getName());
			Assert::IsTrue(movedObj["this"].get<RTTI*>() != obj["this"].get<RTTI*>());
			Assert::IsTrue(&movedObj["name"].get<string>() != &obj["name"].get<string>());

			Assert::IsTrue(movedObj != obj);
			Assert::IsTrue(movedObj == copiedObj);
		}

		template <typename T>
		void EntityAssignmentsHelper(const string& defaultName)
		{
			T obj("MyName");

			// copy assignment
			T copiedObj;
			Assert::IsTrue(copiedObj.getName() == defaultName);
			Assert::IsTrue(copiedObj != obj);

			copiedObj = obj;
			Assert::IsTrue(copiedObj.getName() == "MyName");
			Assert::IsTrue(copiedObj.getName() == obj.getName());
			Assert::IsTrue(copiedObj["this"].get<RTTI*>() != obj["this"].get<RTTI*>());
			Assert::IsTrue(&copiedObj["name"].get<string>() != &obj["name"].get<string>());

			Assert::IsTrue(copiedObj == obj);

			// move assignment
			T movedObj;
			Assert::IsTrue(movedObj.getName() == defaultName);
			Assert::IsTrue(movedObj != obj);

			movedObj = std::move(obj);
			Assert::IsTrue(obj.getName() == defaultName);
			Assert::IsTrue(movedObj.getName() == "MyName");
			Assert::IsTrue(movedObj.getName() != obj.getName());
			Assert::IsTrue(movedObj["this"].get<RTTI*>() != obj["this"].get<RTTI*>());
			Assert::IsTrue(&movedObj["name"].get<string>() != &obj["name"].get<string>());

			Assert::IsTrue(movedObj != obj);
			Assert::IsTrue(movedObj == copiedObj);
		}

		//

		static _CrtMemState sStartMemState;
		Entity::EntityFactory* sEntityFactory;
		EntityFoo::EntityFooFactory* sEntityFooFactory;

		const static string sXmlFile_Full;
		const static string sXmlFile_Subfiles;
	};

	_CrtMemState EntityTest::sStartMemState;
	const string EntityTest::sXmlFile_Full = "assets/xml/Table_entity/entity_full.xml";
	const string EntityTest::sXmlFile_Subfiles = "assets/xml/Table_entity/entity_subfile_root.xml";
}

