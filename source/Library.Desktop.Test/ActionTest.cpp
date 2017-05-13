
#include "pch.h"
#include "CppUnitTest.h"

#include "World.h"
#include "Sector.h"
#include "Entity.h"

#include "Action.h"
#include "ActionList.h"
#include "ActionListIf.h"
#include "ActionCreateAction.h"
#include "ActionDestroyAction.h"

#include "XmlParseMaster.h"
#include "XmlParseHelperSubfile.h"
#include "XmlParseHelperTable.h"
#include "XmlParseHelperData.h"
#include "SharedDataTable.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DOGEngine;
using namespace std;
using namespace glm;

#define INIT_PARSER	SharedDataTable tableData;				\
					XmlParseMaster master(tableData);		\
					XmlParseHelperSubfile subfileHelper;	\
					XmlParseHelperTable tableHelper;		\
					XmlParseHelperData dataHelper;			\
					master.addHelper(subfileHelper);		\
					master.addHelper(tableHelper);			\
					master.addHelper(dataHelper);			\

namespace LibraryDesktopTest
{
	TEST_CLASS(ActionTest)
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
			sActionListFactory = new ActionList::ActionListFactory();
			sActionListIfFactory = new ActionListIf::ActionListIfFactory();
			sActionCreateActionFactory = new ActionCreateAction::ActionCreateActionFactory();
			sActionDestroyActionFactory = new ActionDestroyAction::ActionDestroyActionFactory();
		}

		TEST_METHOD_CLEANUP(Cleanup)
		{
			XmlParseHelperData::clearHandlerCache();
			XmlParseHelperTable::clearHandlerCache();
			Attributed::clearAttributeCache();

			delete sEntityFactory;
			delete sActionListFactory;
			delete sActionListIfFactory;
			delete sActionCreateActionFactory;
			delete sActionDestroyActionFactory;

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


		TEST_METHOD(ActionRTTI)
		{
			ActionRTTIHelper<ActionList, Action>("ActionList", "Action");
			ActionRTTIHelper<ActionList, Attributed>("ActionList", "Attributed");

			ActionRTTIHelper<ActionListIf, ActionList>("ActionListIf", "ActionList");
			ActionRTTIHelper<ActionListIf, Attributed>("ActionListIf", "Attributed");

			ActionRTTIHelper<ActionCreateAction, Action>("ActionCreateAction", "Action");
			ActionRTTIHelper<ActionCreateAction, Attributed>("ActionCreateAction", "Attributed");

			ActionRTTIHelper<ActionDestroyAction, Action>("ActionDestroyAction", "Action");
			ActionRTTIHelper<ActionDestroyAction, Attributed>("ActionDestroyAction", "Attributed");
		}

		TEST_METHOD(ActionConstructors)
		{
			ActionConstructorsHelper<ActionList>("Action");
			ActionConstructorsHelper<ActionListIf>("Action");
			ActionConstructorsHelper<ActionCreateAction>("Action");
			ActionConstructorsHelper<ActionDestroyAction>("Action");

			// test prescribed attributes
			ActionList actionList;
			Assert::IsTrue(actionList.isPrescribedAttribute("this"));
			Assert::IsTrue(actionList.isPrescribedAttribute("name"));
			Assert::IsTrue(actionList.isPrescribedAttribute("actions"));

			ActionListIf actionIf;
			Assert::IsTrue(actionIf.isPrescribedAttribute("this"));
			Assert::IsTrue(actionIf.isPrescribedAttribute("name"));
			Assert::IsTrue(actionIf.isPrescribedAttribute("condition"));
			Assert::IsTrue(actionIf.isPrescribedAttribute("then"));
			Assert::IsTrue(actionIf.isPrescribedAttribute("else"));

			ActionCreateAction createAction;
			Assert::IsTrue(createAction.isPrescribedAttribute("this"));
			Assert::IsTrue(createAction.isPrescribedAttribute("name"));
			Assert::IsTrue(createAction.isPrescribedAttribute("class_name"));
			Assert::IsTrue(createAction.isPrescribedAttribute("instance_name"));

			ActionDestroyAction destroyAction;
			// TODO
		}

		TEST_METHOD(ActionAssignment)
		{
			ActionAssignmentsHelper<ActionList>("Action");
			ActionAssignmentsHelper<ActionListIf>("Action");
			ActionAssignmentsHelper<ActionCreateAction>("Action");
			ActionAssignmentsHelper<ActionDestroyAction>("Action");
		}

		TEST_METHOD(ActionDestructor)
		{
			ActionDestructorHelper<ActionList>();
			ActionDestructorHelper<ActionListIf>();
			ActionDestructorHelper<ActionCreateAction>();
			ActionDestructorHelper<ActionDestroyAction>();
		}

		TEST_METHOD(ActionFactory)
		{
			// test factories
			Action* actionList = Factory<Action>::create("ActionList");
			Assert::IsTrue(actionList->Is(ActionList::TypeIdClass()));
			delete actionList;

			Action* actionIf = Factory<Action>::create("ActionListIf");
			Assert::IsTrue(actionIf->Is(ActionListIf::TypeIdClass()));
			delete actionIf;

			Action* actionCreate = Factory<Action>::create("ActionCreateAction");
			Assert::IsTrue(actionCreate->Is(ActionCreateAction::TypeIdClass()));
			delete actionCreate;

			Action* actionDestroy = Factory<Action>::create("ActionDestroyAction");
			Assert::IsTrue(actionDestroy->Is(ActionDestroyAction::TypeIdClass()));
			delete actionDestroy;

			Action* notAnAction = Factory<Action>::create("NotAnAction");
			Assert::IsTrue(notAnAction == nullptr);
		}

		TEST_METHOD(ActionCreateActionTest)
		{
			ActionCreateActionHelper<Entity>();
			ActionCreateActionHelper<ActionList>();
		}

		TEST_METHOD(ActionUpdate)
		{
			// action create action
			{
				ActionList* actionList = new ActionList();
				ActionCreateAction* createAction = actionList->createAction("ActionCreateAction", "Create")->As<ActionCreateAction>();

				createAction->setClassName("ActionListIf");
				createAction->setInstanceName("If");
				Assert::IsTrue(createAction->getClassName() == "ActionListIf");
				Assert::IsTrue(createAction->getInstanceName() == "If");

				World world;
				WorldState worldState;
				worldState.world = &world;

				actionList->update(worldState);

				// action create action's update enqueues itself for deletion
				Assert::IsTrue(world.getPendingDelete().isPendingDelete(*createAction));
				world.getPendingDelete().empty();

				Assert::IsTrue((*actionList)["actions"].size() == 1);
				Assert::IsTrue((*actionList)["actions"][0].Is(ActionListIf::TypeIdClass()));
				Assert::IsTrue((*actionList)["actions"][0].As<ActionListIf>()->getName() == "If");

				delete actionList;
			}

			// action destroy action
			{
				ActionList* actionList = new ActionList();
				ActionListIf* actionIf = actionList->createAction("ActionListIf", "If")->As<ActionListIf>();
				ActionDestroyAction* destroyAction = actionList->createAction("ActionDestroyAction", "Destroy")->As<ActionDestroyAction>();

				destroyAction->setDeleteTarget("If");
				Assert::IsTrue(destroyAction->getDeleteTarget() == "If");

				World world;
				WorldState worldState;
				worldState.world = &world;

				actionList->update(worldState);

				Assert::IsFalse(world.getPendingDelete().isPendingDelete(*actionList));
				Assert::IsTrue(world.getPendingDelete().isPendingDelete(*actionIf));
				Assert::IsTrue(world.getPendingDelete().isPendingDelete(*destroyAction));
				world.getPendingDelete().empty();

				Assert::IsTrue((*actionList)["actions"].size() == 0);

				delete actionList;
			}

			// action list if (true and false)
			{
				ActionListIf* actionIf = new ActionListIf();
				ActionList* thenAction = actionIf->createThenBlock("ActionList", "Then")->As<ActionList>();
				ActionList* elseAction = actionIf->createElseBlock("ActionList", "Else")->As<ActionList>();

				WorldState worldState;
				Assert::IsTrue(worldState.action == nullptr);

				// condition is not 0 -- then block taken
				(*actionIf)["condition"].set(1);
				Assert::IsTrue((*actionIf)["condition"].get<int32_t>() == 1);

				actionIf->update(worldState);
				Assert::IsTrue(worldState.action == thenAction);

				// condition is 0 -- else block taken
				(*actionIf)["condition"].set(0);
				Assert::IsTrue((*actionIf)["condition"].get<int32_t>() == 0);

				actionIf->update(worldState);
				Assert::IsTrue(worldState.action == elseAction);

				delete actionIf;
			}
		}

		TEST_METHOD(ActionCreateThenElseBlocks)
		{
			// if actions have no then or else blocks at start
			ActionListIf* actionIf = new ActionListIf();
			Assert::IsTrue((*actionIf)["then"].size() == 0);
			Assert::IsTrue((*actionIf)["else"].size() == 0);

			// can create then and else blocks by name
			Action* thenBlock = actionIf->createThenBlock("ActionList", "Then");
			Action* elseBlock = actionIf->createElseBlock("ActionList", "Else");

			Assert::IsTrue(thenBlock->getParent() == actionIf);
			Assert::IsTrue(elseBlock->getParent() == actionIf);
			Assert::IsTrue(&(*actionIf)["then"][0] == thenBlock);
			Assert::IsTrue(&(*actionIf)["else"][0] == elseBlock);
			Assert::IsTrue(actionIf->getThenBlock() == thenBlock);
			Assert::IsTrue(actionIf->getElseBlock() == elseBlock);

			// creating a new block replaces the old one
			Action* newThenBlock = actionIf->createThenBlock("ActionListIf", "Then");
			Action* newElseBlock = actionIf->createElseBlock("ActionListIf", "Else");

			Assert::IsTrue(newThenBlock->getParent() == actionIf);
			Assert::IsTrue(newElseBlock->getParent() == actionIf);
			Assert::IsTrue(&(*actionIf)["then"][0] == newThenBlock);
			Assert::IsTrue(&(*actionIf)["else"][0] == newElseBlock);
			Assert::IsTrue(actionIf->getThenBlock() == newThenBlock);
			Assert::IsTrue(actionIf->getElseBlock() == newElseBlock);

			// if the create block returns null, the original blocks are untouched
			Action* nullThen = actionIf->createThenBlock("NotAnAction", "Then");
			Action* nullElse = actionIf->createThenBlock("NotAnAction", "Else");

			Assert::IsTrue(nullThen == nullptr);
			Assert::IsTrue(nullElse == nullptr);
			Assert::IsTrue(&(*actionIf)["then"][0] == newThenBlock);
			Assert::IsTrue(&(*actionIf)["else"][0] == newElseBlock);
			Assert::IsTrue(actionIf->getThenBlock() == newThenBlock);
			Assert::IsTrue(actionIf->getElseBlock() == newElseBlock);

			// testing with copy semantics
			ActionListIf* copiedIf = new ActionListIf(*actionIf);

			Assert::IsTrue(&(*copiedIf)["then"][0] != newThenBlock);
			Assert::IsTrue(&(*copiedIf)["else"][0] != newElseBlock);
			Assert::IsTrue(copiedIf->getThenBlock() != newThenBlock);
			Assert::IsTrue(copiedIf->getElseBlock() != newElseBlock);

			Assert::IsTrue(copiedIf->getThenBlock()->Is(ActionListIf::TypeIdClass()));
			Assert::IsTrue(copiedIf->getElseBlock()->Is(ActionListIf::TypeIdClass()));

			Assert::IsTrue(copiedIf->Equals(actionIf));

			// testing with move semantics
			ActionListIf* movedIf = new ActionListIf(std::move(*actionIf));

			Assert::IsTrue(&(*movedIf)["then"][0] == newThenBlock);
			Assert::IsTrue(&(*movedIf)["else"][0] == newElseBlock);
			Assert::IsTrue(movedIf->getThenBlock() == newThenBlock);
			Assert::IsTrue(movedIf->getElseBlock() == newElseBlock);
			Assert::IsTrue(newThenBlock->getParent() == movedIf);
			Assert::IsTrue(newElseBlock->getParent() == movedIf);

			Assert::IsTrue((*actionIf)["then"].size() == 0);
			Assert::IsTrue((*actionIf)["else"].size() == 0);
			Assert::IsTrue(actionIf->getThenBlock() == nullptr);
			Assert::IsTrue(actionIf->getElseBlock() == nullptr);

			Assert::IsTrue(movedIf->Equals(copiedIf));
			Assert::IsFalse(movedIf->Equals(actionIf));

			delete actionIf;
			delete copiedIf;
			delete movedIf;
		}

		TEST_METHOD(ActionXmlParse)
		{
			// parse Action tags
			{
				INIT_PARSER
				Assert::IsTrue(master.parseFromFile(sActionPath));
				Scope* scope = master.getSharedData()->As<SharedDataTable>()->extractScope();

				Assert::IsTrue(scope != nullptr);
				Assert::IsTrue(scope->Is(Entity::TypeIdClass()));

				Assert::IsTrue((*scope)["actions"][0].Is(ActionList::TypeIdClass()));
				Assert::IsTrue((*scope)["actions"][0]["actions"][0].Is(ActionCreateAction::TypeIdClass()));

				delete scope;
			}

			// parse action lists
			{
				INIT_PARSER
				Assert::IsTrue(master.parseFromFile(sActionListPath));
				Scope* scope = master.getSharedData()->As<SharedDataTable>()->extractScope();

				Assert::IsTrue(scope != nullptr);
				Assert::IsTrue(scope->Is(ActionList::TypeIdClass()));

				Assert::IsTrue((*scope)["actions"][0].Is(ActionList::TypeIdClass()));
				Assert::IsTrue((*scope)["actions"][1].Is(ActionCreateAction::TypeIdClass()));
				Assert::IsTrue((*scope)["actions"][2].Is(ActionListIf::TypeIdClass()));
				Assert::IsTrue((*scope)["actions"][3].Is(ActionDestroyAction::TypeIdClass()));

				delete scope;
			}

			// parse action create action
			{
				INIT_PARSER
				Assert::IsTrue(master.parseFromFile(sActionCreatePath));
				Scope* scope = master.getSharedData()->As<SharedDataTable>()->extractScope();

				Assert::IsTrue(scope != nullptr);
				Assert::IsTrue(scope->Is(Entity::TypeIdClass()));

				Assert::IsTrue((*scope)["actions"][0].Is(ActionCreateAction::TypeIdClass()));
				Assert::IsTrue((*scope)["actions"][0][ActionCreateAction::sClassNameAttribute].get<string>() == "ActionList");
				Assert::IsTrue((*scope)["actions"][0][ActionCreateAction::sInstanceNameAttribute].get<string>() == "List");

				ActionCreateAction* createAction = (*scope)["actions"][0].As<ActionCreateAction>();
				Assert::IsTrue(createAction->getClassName() == "ActionList");
				Assert::IsTrue(createAction->getInstanceName() == "List");

				delete scope;
			}

			// parse action destroy action
			{
				INIT_PARSER
				Assert::IsTrue(master.parseFromFile(sActionDestroyPath));
				Scope* scope = master.getSharedData()->As<SharedDataTable>()->extractScope();

				Assert::IsTrue(scope != nullptr);
				Assert::IsTrue(scope->Is(Entity::TypeIdClass()));

				Assert::IsTrue((*scope)["actions"][0].Is(ActionDestroyAction::TypeIdClass()));
				Assert::IsTrue((*scope)["actions"][0][ActionDestroyAction::sDeleteTargetAttribute].get<string>() == "List");

				ActionDestroyAction* destroyAction = (*scope)["actions"][0].As<ActionDestroyAction>();
				Assert::IsTrue(destroyAction->getDeleteTarget() == "List");

				delete scope;
			}

			// parse if action
			{
				INIT_PARSER
				Assert::IsTrue(master.parseFromFile(sActionIfPath));
				Scope* scope = master.getSharedData()->As<SharedDataTable>()->extractScope();

				Assert::IsTrue(scope != nullptr);
				Assert::IsTrue(scope->Is(ActionListIf::TypeIdClass()));

				Assert::IsTrue((*scope)["condition"].get<int32_t>() == 1);

				ActionListIf* actionIf = scope->As<ActionListIf>();
				Assert::IsTrue(actionIf->getThenBlock()->Is(ActionList::TypeIdClass()));
				Assert::IsTrue(actionIf->getThenBlock()->getName() == "List");
				Assert::IsTrue(actionIf->getThenBlock() == &(*scope)["then"][0]);

				Assert::IsTrue(actionIf->getElseBlock()->Is(ActionListIf::TypeIdClass()));
				ActionListIf* nestedIf = actionIf->getElseBlock()->As<ActionListIf>();

				Assert::IsTrue(nestedIf->getName() == "NestedIf");
				Assert::IsTrue((*nestedIf)["condition"].get<int32_t>() == 2);

				Assert::IsTrue(nestedIf->getThenBlock()->Is(ActionCreateAction::TypeIdClass()));
				Assert::IsTrue(nestedIf->getThenBlock()->getName() == "Create");
				Assert::IsTrue(nestedIf->getThenBlock() == &(*nestedIf)["then"][0]);

				Assert::IsTrue(nestedIf->getElseBlock()->Is(ActionDestroyAction::TypeIdClass()));
				Assert::IsTrue(nestedIf->getElseBlock()->getName() == "Destroy");
				Assert::IsTrue(nestedIf->getElseBlock() == &(*nestedIf)["else"][0]);

				delete scope;
			}

			// parse subfiles
			{
				INIT_PARSER
				
				// parse for entity-root subfile
				Assert::IsTrue(master.parseFromFile(sSubfileEntityPath));
				Scope* scope = master.getSharedData()->As<SharedDataTable>()->extractScope();

				Assert::IsTrue(scope != nullptr);
				Assert::IsTrue(scope->Is(Entity::TypeIdClass()));

				Assert::IsTrue((*scope)["actions"].size() == 1);
				Assert::IsTrue((*scope)["actions"][0].Is(ActionListIf::TypeIdClass()));
				Assert::IsTrue((*scope)["actions"][0].As<ActionListIf>()->getName() == "If");
				Assert::IsTrue((*scope)["actions"][0].As<ActionListIf>()->getThenBlock()->getName() == "Then");
				Assert::IsTrue((*scope)["actions"][0].As<ActionListIf>()->getElseBlock()->getName() == "Else");
				Assert::IsTrue((*scope)["actions"][0].As<ActionListIf>()->getThenBlock()->Is(ActionList::TypeIdClass()));
				Assert::IsTrue((*scope)["actions"][0].As<ActionListIf>()->getElseBlock()->Is(ActionList::TypeIdClass()));

				delete scope;
				scope = nullptr;

				// parse for action list root subfile
				Assert::IsTrue(master.parseFromFile(sSubfileActionListPath));
				scope = master.getSharedData()->As<SharedDataTable>()->extractScope();

				Assert::IsTrue(scope != nullptr);
				Assert::IsTrue(scope->Is(ActionList::TypeIdClass()));

				Assert::IsTrue((*scope)["actions"].size() == 1);
				Assert::IsTrue((*scope)["actions"][0].Is(ActionListIf::TypeIdClass()));
				Assert::IsTrue((*scope)["actions"][0].As<ActionListIf>()->getName() == "If");
				Assert::IsTrue((*scope)["actions"][0].As<ActionListIf>()->getThenBlock()->getName() == "Then");
				Assert::IsTrue((*scope)["actions"][0].As<ActionListIf>()->getElseBlock()->getName() == "Else");
				Assert::IsTrue((*scope)["actions"][0].As<ActionListIf>()->getThenBlock()->Is(ActionList::TypeIdClass()));
				Assert::IsTrue((*scope)["actions"][0].As<ActionListIf>()->getElseBlock()->Is(ActionList::TypeIdClass()));

				delete scope;
			}
		}

		TEST_METHOD(ActionPendingDeleteQueue)
		{
			// emptying the queue with root objects
			{
				World world;

				Action* actionList = Factory<Action>::create("ActionList");
				Action* actionIf = Factory<Action>::create("ActionListIf");

				Assert::IsFalse(world.getPendingDelete().isPendingDelete(*actionList));
				Assert::IsFalse(world.getPendingDelete().isPendingDelete(*actionIf));

				// add to queue
				world.getPendingDelete().enqueue(*actionList);
				world.getPendingDelete().enqueue(*actionIf);

				Assert::IsTrue(world.getPendingDelete().isPendingDelete(*actionList));
				Assert::IsTrue(world.getPendingDelete().isPendingDelete(*actionIf));

				// clear queue
				world.getPendingDelete().empty();
			}

			// emptying the queue with child objects
			{
				World world;

				ActionList* actionList = Factory<Action>::create("ActionList")->As<ActionList>();
				Action* actionIf = actionList->createAction("ActionListIf", "If");

				Assert::IsTrue(actionIf != nullptr);
				Assert::IsTrue((*actionList)["actions"].size() == 1);
				Assert::IsTrue(&(*actionList)["actions"][0] == actionIf);

				world.getPendingDelete().enqueue(*actionIf);
				Assert::IsTrue(world.getPendingDelete().isPendingDelete(*actionIf));

				// empty the queue
				world.getPendingDelete().empty();
				Assert::IsTrue((*actionList)["actions"].size() == 0);

				delete actionList;
			}

			// adding a parent of an already inserted object to the queue (pushes parent, removes children of incoming scope)
			{
				World world;

				ActionList* actionList = Factory<Action>::create("ActionList")->As<ActionList>();
				Action* actionIf = actionList->createAction("ActionListIf", "If");
				Assert::IsTrue(actionIf != nullptr);
				
				world.getPendingDelete().enqueue(*actionIf);
				Assert::IsTrue(world.getPendingDelete().isPendingDelete(*actionIf));
				Assert::IsFalse(world.getPendingDelete().isPendingDelete(*actionList));

				Assert::IsTrue(world.getPendingDelete().size() == 1);

				world.getPendingDelete().enqueue(*actionList);
				Assert::IsTrue(world.getPendingDelete().isPendingDelete(*actionIf));
				Assert::IsTrue(world.getPendingDelete().isPendingDelete(*actionList));

				Assert::IsTrue(world.getPendingDelete().size() == 1);

				world.getPendingDelete().empty();

				Assert::IsTrue(world.getPendingDelete().size() == 0);
			}

			// adding a child of an already inserted object to the queue (changes nothing of the queue)
			{
				World world;

				ActionList* otherActionList = Factory<Action>::create("ActionList")->As<ActionList>();
				Action* otherActionIf = otherActionList->createAction("ActionListIf", "If");
				Assert::IsTrue(otherActionIf != nullptr);

				world.getPendingDelete().enqueue(*otherActionList);
				Assert::IsTrue(world.getPendingDelete().isPendingDelete(*otherActionList));
				Assert::IsTrue(world.getPendingDelete().isPendingDelete(*otherActionIf));

				Assert::IsTrue(world.getPendingDelete().size() == 1);

				world.getPendingDelete().enqueue(*otherActionIf);
				Assert::IsTrue(world.getPendingDelete().isPendingDelete(*otherActionList));
				Assert::IsTrue(world.getPendingDelete().isPendingDelete(*otherActionIf));

				Assert::IsTrue(world.getPendingDelete().size() == 1);

				world.getPendingDelete().empty();
				
				Assert::IsTrue(world.getPendingDelete().size() == 0);
			}

			// sanity check
			{
				World world;

				ActionList* actionList = new ActionList();
				Action* action = actionList->createAction("ActionList", "Action");

				ActionList* otherActionList = new ActionList();

				world.getPendingDelete().enqueue(*action);
				world.getPendingDelete().enqueue(*otherActionList);

				world.getPendingDelete().enqueue(*actionList);

				world.getPendingDelete().empty();
			}

			// clearing queue
			{
				World world;

				ActionList* actionList = new ActionList();

				Assert::IsTrue(world.getPendingDelete().size() == 0);
				world.getPendingDelete().enqueue(*actionList);
				Assert::IsTrue(world.getPendingDelete().size() == 1);

				world.getPendingDelete().clear();

				Assert::IsTrue(world.getPendingDelete().size() == 0);

				*actionList;

				delete actionList;
			}
		}

		TEST_METHOD(PendingDeleteMove)
		{
			World world;
			Sector* sector = world.createSector("Sector");
			Entity* entity = sector->createEntity("Entity", "Entity");
			ActionList* actionList = entity->createAction("ActionList", "List")->As<ActionList>();
			Action* action = actionList->createAction("ActionCreateAction", "Create");

			// enqueing delete marks enqueued subtree as pending delete
			world.getPendingDelete().enqueue(*entity);
			Assert::IsFalse(world.getPendingDelete().isPendingDelete(*sector));
			Assert::IsTrue(world.getPendingDelete().isPendingDelete(*entity));
			Assert::IsTrue(world.getPendingDelete().isPendingDelete(*actionList));
			Assert::IsTrue(world.getPendingDelete().isPendingDelete(*action));

			Assert::IsTrue(world.getPendingDelete().size() == 1);

			// can't add worlds as delete
			World* otherWorld = new World();
			world.getPendingDelete().enqueue(*otherWorld);
			Assert::IsFalse(world.getPendingDelete().isPendingDelete(*otherWorld));

			delete otherWorld;

			// can move delete
			World movedWorld(std::move(world));

			Assert::IsTrue(sector->getParent() == &movedWorld);

			Assert::IsTrue(world.getPendingDelete().size() == 0);
			Assert::IsTrue(movedWorld.getPendingDelete().size() == 1);

			Assert::IsFalse(world.getPendingDelete().isPendingDelete(*sector));
			Assert::IsFalse(world.getPendingDelete().isPendingDelete(*entity));
			Assert::IsFalse(world.getPendingDelete().isPendingDelete(*actionList));
			Assert::IsFalse(world.getPendingDelete().isPendingDelete(*action));

			Assert::IsFalse(movedWorld.getPendingDelete().isPendingDelete(*sector));
			Assert::IsTrue(movedWorld.getPendingDelete().isPendingDelete(*entity));
			Assert::IsTrue(movedWorld.getPendingDelete().isPendingDelete(*actionList));
			Assert::IsTrue(movedWorld.getPendingDelete().isPendingDelete(*action));

			movedWorld.getPendingDelete().empty();
		}

	private:

		template <typename DerivedT, typename BaseT>
		void ActionRTTIHelper(const string& derivedTypeName, const string& baseTypeName)
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
		void ActionConstructorsHelper(const string& defaultName)
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
		void ActionAssignmentsHelper(const string& defaultName)
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

		template <typename T>
		void ActionDestructorHelper()
		{
			T obj;
			obj.addAuxiliaryAttribute("hello");
			obj.addAuxiliaryAttribute("world");
		}

		template <typename T>
		void ActionCreateActionHelper()
		{
			T* obj = new T();

			Action* actionList = obj->createAction("ActionList", "FirstAction");
			Action* actionIf = obj->createAction("ActionListIf", "SecondAction");
			Action* notAnAction = obj->createAction("NotAnAction", "ThirdAction");

			Assert::IsTrue((*obj)["actions"].size() == 2);

			Assert::IsTrue(actionList->Is(ActionList::TypeIdClass()));
			Assert::IsTrue(actionList == &(*obj)["actions"][0]);
			Assert::IsTrue(actionList->getName() == "FirstAction");

			Assert::IsTrue(actionIf->Is(ActionListIf::TypeIdClass()));
			Assert::IsTrue(actionIf == &(*obj)["actions"][1]);
			Assert::IsTrue(actionIf->getName() == "SecondAction");

			Assert::IsTrue(notAnAction == nullptr);

			delete obj;
		}

		//

		static _CrtMemState sStartMemState;

		static Entity::EntityFactory* sEntityFactory;
		static ActionList::ActionListFactory* sActionListFactory;
		static ActionListIf::ActionListIfFactory* sActionListIfFactory;
		static ActionCreateAction::ActionCreateActionFactory* sActionCreateActionFactory;
		static ActionDestroyAction::ActionDestroyActionFactory* sActionDestroyActionFactory;

		const static string sActionPath;
		const static string sActionListPath;
		const static string sActionCreatePath;
		const static string sActionDestroyPath;
		const static string sActionIfPath;

		const static string sSubfileEntityPath;
		const static string sSubfileActionListPath;
	};

	_CrtMemState ActionTest::sStartMemState;

	Entity::EntityFactory* ActionTest::sEntityFactory;
	ActionList::ActionListFactory* ActionTest::sActionListFactory;
	ActionListIf::ActionListIfFactory* ActionTest::sActionListIfFactory;
	ActionCreateAction::ActionCreateActionFactory* ActionTest::sActionCreateActionFactory;
	ActionDestroyAction::ActionDestroyActionFactory* ActionTest::sActionDestroyActionFactory;

	const string ActionTest::sActionPath = "assets/xml/Table_action/action.xml";
	const string ActionTest::sActionListPath = "assets/xml/Table_action/actionList.xml";
	const string ActionTest::sActionCreatePath = "assets/xml/Table_action/actionCreate.xml";
	const string ActionTest::sActionDestroyPath = "assets/xml/Table_action/actionDestroy.xml";
	const string ActionTest::sActionIfPath = "assets/xml/Table_action/actionIf.xml";

	const string ActionTest::sSubfileEntityPath = "assets/xml/Table_action/subfileroot_entity.xml";
	const string ActionTest::sSubfileActionListPath = "assets/xml/Table_action/subfileroot_actionList.xml";
}

