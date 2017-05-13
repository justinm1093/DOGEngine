
#include "pch.h"
#include "CppUnitTest.h"

#include "World.h"
#include "Sector.h"
#include "Entity.h"
#include "Event.h"

#include "EventArgs.h"
#include "ActionEvent.h"
#include "ReactionAttributed.h"

#include "ActionCreateAction.h"

#include "XmlParseMaster.h"
#include "XmlParseHelperSubfile.h"
#include "XmlParseHelperTable.h"
#include "XmlParseHelperData.h"
#include "SharedDataTable.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::chrono;
using namespace DOGEngine;
using namespace std;

namespace LibraryDesktopTest
{
	TEST_CLASS(ReactionTest)
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
			sReactionFactory = new ReactionAttributed::ReactionAttributedFactory();
			sActionCreateActionFactory = new ActionCreateAction::ActionCreateActionFactory();
		}

		TEST_METHOD_CLEANUP(Cleanup)
		{
			delete sEntityFactory;
			delete sReactionFactory;
			delete sActionListFactory;
			delete sActionCreateActionFactory;

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


		TEST_METHOD(ReactionRTTI)
		{
			ReactionRTTIHelper<ReactionAttributed, Reaction>("ReactionAttributed", "Reaction");
			ReactionRTTIHelper<EventArgs, Scope>("EventArgs", "Scope");
			ReactionRTTIHelper<ActionEvent, Action>("ActionEvent", "Action");
		}

		TEST_METHOD(ReactionConstructors)
		{
			ReactionConstructorsHelper<ActionEvent>();

			// test prescribed attributes
			ReactionAttributed reaction;
			Assert::IsTrue(reaction.isPrescribedAttribute(ReactionAttributed::sNameAttribute));
			Assert::IsTrue(reaction.isPrescribedAttribute(ReactionAttributed::sActionsAttribute));
			Assert::IsTrue(reaction.isPrescribedAttribute(ReactionAttributed::sSubtypeAttribute));

			ActionEvent eventAction;
			Assert::IsTrue(eventAction.isPrescribedAttribute(ActionEvent::sNameAttribute));
			Assert::IsTrue(eventAction.isPrescribedAttribute(ActionEvent::sSubtypeAttribute));
			Assert::IsTrue(eventAction.isPrescribedAttribute(ActionEvent::sDelayAttribute));

			// ReactionAttributed -- creating object
			ReactionAttributed obj;

			obj.addSubtype("type");
			Assert::IsTrue(obj.isSubtype("type"));

			obj["auxiliary"] = 100;
			Assert::IsTrue(obj.isAuxiliaryAttribute("auxiliary"));
			Assert::IsTrue(obj["auxiliary"].get<int32_t>() == 100);

			// copy constructor
			ReactionAttributed copiedObj(obj);

			Assert::IsTrue(copiedObj["this"] == &copiedObj);
			Assert::IsTrue(copiedObj["this"].get<RTTI*>() != obj["this"].get<RTTI*>());

			Assert::IsTrue(copiedObj.isSubtype("type"));
			Assert::IsTrue(obj.isSubtype("type"));

			Assert::IsTrue(copiedObj.isAuxiliaryAttribute("auxiliary"));
			Assert::IsTrue(copiedObj["auxiliary"].get<int32_t>() == 100);

			Assert::IsTrue(copiedObj == obj);

			// move constructor
			ReactionAttributed movedObj(std::move(obj));

			Assert::IsTrue(obj["this"] == &obj);
			Assert::IsTrue(movedObj["this"] == &movedObj);
			Assert::IsTrue(movedObj["this"].get<RTTI*>() == &movedObj);

			Assert::IsTrue(obj.isSubtype(""));
			Assert::IsFalse(obj.isSubtype("type"));
			Assert::IsTrue(movedObj.isSubtype("type"));

			Assert::IsFalse(obj.isAuxiliaryAttribute("auxiliary"));
			Assert::IsTrue(movedObj.isAuxiliaryAttribute("auxiliary"));
			Assert::IsTrue(movedObj["auxiliary"].get<int32_t>() == 100);

			Assert::IsTrue(movedObj != obj);
			Assert::IsTrue(movedObj == copiedObj);
		}

		TEST_METHOD(ReactionAssignment)
		{
			ReactionAssignmentHelper<ActionEvent>();

			// setting and getting stuff on this object
			ReactionAttributed obj;

			obj.addSubtype("type");
			Assert::IsTrue(obj.isSubtype("type"));

			obj["auxiliary"] = 100;
			Assert::IsTrue(obj.isAuxiliaryAttribute("auxiliary"));
			Assert::IsTrue(obj["auxiliary"].get<int32_t>() == 100);

			// copy assignment
			ReactionAttributed copiedObj;
			copiedObj.addSubtype("copied");

			copiedObj = obj;

			Assert::IsTrue(copiedObj["this"] == &copiedObj);
			Assert::IsTrue(copiedObj["this"].get<RTTI*>() != obj["this"].get<RTTI*>());

			Assert::IsTrue(obj.isSubtype("type"));
			Assert::IsTrue(copiedObj.isSubtype("type"));
			Assert::IsFalse(copiedObj.isSubtype("copied"));

			Assert::IsTrue(copiedObj.isAuxiliaryAttribute("auxiliary"));
			Assert::IsTrue(copiedObj["auxiliary"].get<int32_t>() == 100);

			Assert::IsTrue(copiedObj == obj);

			// move assignment
			ReactionAttributed movedObj;
			movedObj.addSubtype("moved");

			movedObj = std::move(obj);

			Assert::IsTrue(obj["this"] == &obj);
			Assert::IsTrue(movedObj["this"] == &movedObj);
			Assert::IsTrue(movedObj["this"].get<RTTI*>() == &movedObj);

			Assert::IsFalse(obj.isSubtype("type"));
			Assert::IsFalse(movedObj.isSubtype("moved"));
			Assert::IsTrue(movedObj.isSubtype("type"));
			
			Assert::IsFalse(obj.isAuxiliaryAttribute("auxiliary"));
			Assert::IsTrue(movedObj.isAuxiliaryAttribute("auxiliary"));
			Assert::IsTrue(movedObj["auxiliary"].get<int32_t>() == 100);

			Assert::IsTrue(movedObj != obj);
			Assert::IsTrue(movedObj == copiedObj);
		}

		TEST_METHOD(ReactionDestructor)
		{
			ReactionDestructorHelper<ReactionAttributed>();
			ReactionDestructorHelper<ActionEvent>();
			ReactionDestructorHelper<EventArgs>();
		}

		TEST_METHOD(ReactionSettersAndGetters)
		{
			// reaction attributed subtype
			{
				ReactionAttributed reaction("name");
				Assert::IsTrue(reaction.isSubtype(""));

				reaction.addSubtype("type");
				Assert::IsTrue(reaction.isSubtype("type"));
				Assert::IsTrue(reaction.isSubtype(""));

				ReactionAttributed copiedReaction(reaction);
				Assert::IsTrue(copiedReaction.getName() == "name");
				Assert::IsTrue(copiedReaction.isSubtype("type"));
				Assert::IsTrue(copiedReaction.isSubtype(""));
				Assert::IsTrue(&copiedReaction["name"].get<string>() != &reaction["name"].get<string>());
				Assert::IsTrue(&copiedReaction["subtype"].get<string>() != &reaction["subtype"].get<string>());

				ReactionAttributed movedReaction(std::move(reaction));
				Assert::IsTrue(movedReaction.getName() == "name");
				Assert::IsTrue(movedReaction.isSubtype(""));
				Assert::IsTrue(movedReaction.isSubtype("type"));
				Assert::IsTrue(reaction.getName() == "Action");
				Assert::IsTrue(reaction.isSubtype(""));
				Assert::IsTrue(&movedReaction["name"].get<string>() != &reaction["name"].get<string>());
				Assert::IsTrue(&movedReaction["subtype"].get<string>() != &reaction["subtype"].get<string>());
			}

			// action event subtype and delay
			{
				ActionEvent actionEvent("name");
				Assert::IsTrue(actionEvent.getSubtype() == "");
				Assert::IsTrue(actionEvent.getDelay() == 0);

				actionEvent.setSubtype("type");
				actionEvent.setDelay(1000);

				Assert::IsTrue(actionEvent.getSubtype() == "type");
				Assert::IsTrue(actionEvent.getDelay() == 1000);

				ActionEvent copied(actionEvent);
				Assert::IsTrue(copied.getName() == "name");
				Assert::IsTrue(copied.getSubtype() == "type");
				Assert::IsTrue(&copied["name"].get<string>() != &actionEvent["name"].get<string>());
				Assert::IsTrue(&copied["subtype"].get<string>() != &actionEvent["subtype"].get<string>());
				Assert::IsTrue(&copied["delay"].get<int32_t>() != &actionEvent["delay"].get<int32_t>());

				ActionEvent moved(std::move(actionEvent));
				Assert::IsTrue(moved.getName() == "name");
				Assert::IsTrue(moved.getSubtype() == "type");
				Assert::IsTrue(actionEvent.getName() == "Action");
				Assert::IsTrue(actionEvent.getSubtype() == "");
				Assert::IsTrue(&moved["name"].get<string>() != &actionEvent["name"].get<string>());
				Assert::IsTrue(&moved["subtype"].get<string>() != &actionEvent["subtype"].get<string>());
				Assert::IsTrue(&moved["delay"].get<int32_t>() != &actionEvent["delay"].get<int32_t>());
			}

			// event args
			{
				EventArgs args;
				Assert::IsTrue(args.getSubtype() == "");
				Assert::IsTrue(args.getWorldState() == nullptr);

				WorldState worldstate;
				args.setSubtype("type");
				args.setWorldState(worldstate);

				Assert::IsTrue(args.getSubtype() == "type");
				Assert::IsTrue(args.getWorldState() == &worldstate);

				EventArgs copied(args);
				Assert::IsTrue(copied.getWorldState() == &worldstate);
				Assert::IsTrue(copied.getSubtype() == "type");

				EventArgs moved(std::move(args));
				Assert::IsTrue(moved.getWorldState() == &worldstate);
				Assert::IsTrue(moved.getSubtype() == "type");
				Assert::IsTrue(args.getWorldState() == nullptr);
				Assert::IsTrue(args.getSubtype() == "");

			}
		}

		TEST_METHOD(ReactionVirtualCopy)
		{
			Scope scope;
			scope.adopt("ReactionAttributed", *new ReactionAttributed("test"));
			scope.adopt("ActionEvent", *new ActionEvent("test"));
			scope.adopt("EventArgs", *new EventArgs("test"));

			// copy preserves the type of child objects
			Scope copiedScope = scope;
			Assert::IsTrue(copiedScope["ReactionAttributed"][0].Is(ReactionAttributed::TypeIdClass()));
			Assert::IsTrue(copiedScope["ActionEvent"][0].Is(ActionEvent::TypeIdClass()));
			Assert::IsTrue(copiedScope["EventArgs"][0].Is(EventArgs::TypeIdClass()));
		}

		TEST_METHOD(ReactionSubscribe)
		{
			Assert::IsTrue(Event<EventArgs>::numSubscribers() == 0);

			ReactionAttributed obj;
			Assert::IsTrue(Event<EventArgs>::numSubscribers() == 1);

			{
				ReactionAttributed otherObj;
				Assert::IsTrue(Event<EventArgs>::numSubscribers() == 2);
			}

			Assert::IsTrue(Event<EventArgs>::numSubscribers() == 1);
		}

		TEST_METHOD(ActionEventUpdate)
		{
			World world;
			WorldState worldState;
			worldState.world = &world;

			ActionEvent action;
			action.setSubtype("Test");
			action.setDelay(1000);

			Assert::IsTrue(action.getSubtype() == "Test");
			Assert::IsTrue(action.getDelay() == 1000);

			action.append("argument_1") = 100;
			action.append("argument_2") = "Hello, World!";
			Assert::IsTrue(action.isAuxiliaryAttribute("argument_1"));
			Assert::IsTrue(action.isAuxiliaryAttribute("argument_2"));

			Assert::IsTrue(world.getEventQueue().isEmpty());
			Assert::IsTrue(world.getEventQueue().size() == 0);

			action.update(worldState);

			Assert::IsFalse(world.getEventQueue().isEmpty());
			Assert::IsTrue(world.getEventQueue().size() == 1);
		}

		TEST_METHOD(ReactionNotify)
		{
			ActionEvent* action_1 = new ActionEvent("ActionEvent");
			ActionEvent* action_2 = new ActionEvent("ActionEvent");
			ReactionAttributed* reaction_1 = new ReactionAttributed("Reaction");
			ReactionAttributed* reaction_2 = new ReactionAttributed("Reaction");

			// set some arguments for the actions to construct
			action_1->setSubtype("type");
			action_1->setDelay(1000);
			action_1->addAuxiliaryAttribute("arg_1") = 100;
			action_1->addAuxiliaryAttribute("arg_2") = "hello";

			action_2->setSubtype("other");
			action_2->setDelay(2000);
			action_2->addAuxiliaryAttribute("arg_1") = 1.0f;
			action_2->addAuxiliaryAttribute("arg_2") = "world";

			// set subtypes for reactions
			reaction_1->addSubtype("type");
			reaction_2->addSubtype("other");

			Assert::IsTrue(reaction_1->isSubtype(action_1->getSubtype()));
			Assert::IsTrue(reaction_2->isSubtype(action_2->getSubtype()));
			Assert::IsTrue(action_1->getSubtype() != action_2->getSubtype());

			// adopt into world for convenience in test
			World world;
			world.adopt("actions", *action_1);
			world.adopt("actions", *action_2);
			world.adopt("reaction", *reaction_1);
			world.adopt("reaction", *reaction_2);

			Assert::IsTrue(world.getEventQueue().size() == 0);
			
			// update to enqueue the events, then remove the ActionEvents from world
			world.update();
			Assert::IsTrue(world.getEventQueue().size() == 2);
			delete action_1;
			delete action_2;

			// update world time to deliver action_1's event
			GameTime& time = world.getWorldState().gameTime;
			time.SetCurrentTime(time.CurrentTime() + milliseconds(1500));
			world.update();

			Assert::IsTrue(world.getEventQueue().size() == 1);

			// verify that the message was delivered
			Assert::IsTrue(reaction_2->find("arg_1") == nullptr);
			Assert::IsTrue(reaction_2->find("arg_2") == nullptr);

			Assert::IsTrue(reaction_1->find("arg_1") != nullptr);
			Assert::IsTrue(reaction_1->find("arg_2") != nullptr);
			Assert::IsTrue((*reaction_1)["arg_1"] == 100);
			Assert::IsTrue((*reaction_1)["arg_2"] == "hello");

			// update world time to deliver action_2's event
			time.SetCurrentTime(time.CurrentTime() + milliseconds(1000));
			world.update();

			Assert::IsTrue(world.getEventQueue().size() == 0);

			// verify that the message was delivered
			Assert::IsTrue(reaction_2->find("arg_1") != nullptr);
			Assert::IsTrue(reaction_2->find("arg_2") != nullptr);
			Assert::IsTrue((*reaction_2)["arg_1"] == 1.0f);
			Assert::IsTrue((*reaction_2)["arg_2"] == "world");
		}

		TEST_METHOD(ReactionAddSubtype)
		{
			ReactionAttributed reaction;
			Assert::IsTrue(reaction.isSubtype(""));

			reaction.addSubtype("type");
			Assert::IsTrue(reaction.isSubtype("type"));

			reaction.addSubtype("other");
			Assert::IsTrue(reaction.isSubtype("other"));
		}

		TEST_METHOD(ReactionRemoveSubtype)
		{
			ReactionAttributed reaction;
			Assert::IsTrue(reaction.isSubtype(""));

			reaction.addSubtype("type");
			Assert::IsTrue(reaction.isSubtype("type"));
			Assert::IsTrue(reaction.removeSubtype("type"));
			Assert::IsFalse(reaction.isSubtype("type"));

			Assert::IsFalse(reaction.removeSubtype("type"));
		}

		TEST_METHOD(ReactionXmlParse)
		{
			SharedDataTable tableData;
			XmlParseMaster master(tableData);
			XmlParseHelperSubfile subfileHelper;
			XmlParseHelperTable tableHelper;
			XmlParseHelperData dataHelper;
			master.addHelper(subfileHelper);
			master.addHelper(tableHelper);
			master.addHelper(dataHelper);

			Assert::IsTrue(master.parseFromFile(sReactionPath));
			Scope* scope = master.getSharedData()->As<SharedDataTable>()->extractScope();

			// verify world stuff
			Assert::IsTrue((*scope)[World::sActionsAttribute][0].Is(ActionCreateAction::TypeIdClass()));

			Datum& worldReactions = (*scope)[World::sReactionsAttribute];
			Assert::IsTrue(worldReactions.size() == 2);
			Assert::IsTrue(worldReactions[0].As<ReactionAttributed>()->getName() == "World Reaction");
			Assert::IsTrue(worldReactions[0].As<ReactionAttributed>()->isSubtype("World"));
			Assert::IsTrue(worldReactions[0].As<ReactionAttributed>()->isSubtype("Other"));
			Assert::IsTrue(worldReactions[1].As<ReactionAttributed>()->getName() == "SubfileReaction");
			Assert::IsTrue(worldReactions[1].As<ReactionAttributed>()->isSubtype("Other"));
			Assert::IsTrue(worldReactions[1].As<ReactionAttributed>()->isSubtype("Subfile"));
			Assert::IsTrue(worldReactions[1].As<ReactionAttributed>()->isSubtype("Test"));

			// verify sector stuff
			Sector* sector = (*scope)[World::sSectorsAttribute][0].As<Sector>();
			Assert::IsTrue((*sector)[Sector::sActionsAttribute][0].Is(ActionCreateAction::TypeIdClass()));

			Datum& sectorReactions = (*sector)[Sector::sReactionsAttribute];
			Assert::IsTrue(sectorReactions.size() == 2);
			Assert::IsTrue(sectorReactions[0].As<ReactionAttributed>()->getName() == "Sector Reaction");
			Assert::IsTrue(sectorReactions[0].As<ReactionAttributed>()->isSubtype("World"));
			Assert::IsTrue(sectorReactions[0].As<ReactionAttributed>()->isSubtype("Other"));
			Assert::IsTrue(sectorReactions[1].As<ReactionAttributed>()->getName() == "SubfileReaction");
			Assert::IsTrue(sectorReactions[1].As<ReactionAttributed>()->isSubtype("Other"));
			Assert::IsTrue(sectorReactions[1].As<ReactionAttributed>()->isSubtype("Subfile"));
			Assert::IsTrue(sectorReactions[1].As<ReactionAttributed>()->isSubtype("Test"));

			// verify entity stuff
			Entity* entity = (*sector)[Sector::sEntitiesAttribute][0].As<Entity>();
			Assert::IsTrue((*entity)[Entity::sActionsAttribute][0].Is(ActionList::TypeIdClass()));
			Assert::IsTrue((*entity)[Entity::sActionsAttribute][1].Is(ActionCreateAction::TypeIdClass()));

			Datum& entityReactions = (*entity)[Entity::sReactionsAttribute];
			Assert::IsTrue(entityReactions.size() == 2);
			Assert::IsTrue(entityReactions[0].As<ReactionAttributed>()->getName() == "Entity Reaction");
			Assert::IsTrue(entityReactions[0].As<ReactionAttributed>()->isSubtype("World"));
			Assert::IsTrue(entityReactions[0].As<ReactionAttributed>()->isSubtype("Other"));
			Assert::IsTrue(entityReactions[1].As<ReactionAttributed>()->getName() == "SubfileReaction");
			Assert::IsTrue(entityReactions[1].As<ReactionAttributed>()->isSubtype("Other"));
			Assert::IsTrue(entityReactions[1].As<ReactionAttributed>()->isSubtype("Subfile"));
			Assert::IsTrue(entityReactions[1].As<ReactionAttributed>()->isSubtype("Test"));

			// verify action list stuff
			ActionList* action = (*entity)[Entity::sActionsAttribute][0].As<ActionList>();

			Assert::IsTrue((*action)[ActionList::sActionsAttribute][0].Is(ActionCreateAction::TypeIdClass()));

			Datum& actionReactions = (*action)[Action::sReactionsAttribute];
			Assert::IsTrue(actionReactions.size() == 2);
			Assert::IsTrue(actionReactions[0].As<ReactionAttributed>()->getName() == "List Reaction");
			Assert::IsTrue(actionReactions[0].As<ReactionAttributed>()->isSubtype("World"));
			Assert::IsTrue(actionReactions[0].As<ReactionAttributed>()->isSubtype("Other"));
			Assert::IsTrue(actionReactions[1].As<ReactionAttributed>()->getName() == "SubfileReaction");
			Assert::IsTrue(actionReactions[1].As<ReactionAttributed>()->isSubtype("Other"));
			Assert::IsTrue(actionReactions[1].As<ReactionAttributed>()->isSubtype("Subfile"));
			Assert::IsTrue(actionReactions[1].As<ReactionAttributed>()->isSubtype("Test"));

			delete scope;
		}

		TEST_METHOD(ReactionFactory)
		{
			// test factory registered
			ReactionAttributed::ReactionAttributedFactory otherFactory;
			Assert::IsTrue(sReactionFactory->isRegistered());
			Assert::IsFalse(otherFactory.isRegistered());

			Assert::IsTrue(Factory<Reaction>::isFactoryRegistered("ReactionAttributed"));
			Assert::IsFalse(Factory<Reaction>::isFactoryRegistered("NotAFactory"));

			Assert::IsTrue(Factory<Reaction>::numFactories() == 1);

			// test product name
			Assert::IsTrue(sReactionFactory->getProductName() == "ReactionAttributed");

			// test reaction creation
			Reaction* reaction = Factory<Reaction>::create("ReactionAttributed");

			Assert::IsTrue(reaction != nullptr);
			Assert::IsTrue(reaction->Is(ReactionAttributed::TypeIdClass()));

			delete reaction;
		}

	private:

		template <typename DerivedT, typename BaseT>
		void ReactionRTTIHelper(const string& derivedTypeName, const string& baseTypeName)
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
		void ReactionConstructorsHelper()
		{
			// creating object
			T obj;

			obj.setSubtype("type");
			Assert::IsTrue(obj["subtype"].isExternal());
			Assert::IsTrue(obj["subtype"].get<string>() == obj.getSubtype());

			obj["auxiliary"] = 100;
			Assert::IsTrue(obj.isAuxiliaryAttribute("auxiliary"));
			Assert::IsTrue(obj["auxiliary"].get<int32_t>() == 100);

			// copy constructor
			T copiedObj(obj);

			Assert::IsTrue(copiedObj["this"] == &copiedObj);
			Assert::IsTrue(copiedObj["this"].get<RTTI*>() != obj["this"].get<RTTI*>());

			Assert::IsTrue(copiedObj.getSubtype() == "type");
			Assert::IsTrue(copiedObj.getSubtype() == obj.getSubtype());
			Assert::IsTrue(&copiedObj["subtype"].get<string>() != &obj["subtype"].get<string>());

			Assert::IsTrue(copiedObj.isAuxiliaryAttribute("auxiliary"));
			Assert::IsTrue(copiedObj["auxiliary"].get<int32_t>() == 100);

			Assert::IsTrue(copiedObj == obj);

			// move constructor
			T movedObj(std::move(obj));

			Assert::IsTrue(obj["this"] == &obj);
			Assert::IsTrue(movedObj["this"] == &movedObj);
			Assert::IsTrue(movedObj["this"].get<RTTI*>() == &movedObj);

			Assert::IsTrue(obj.getSubtype() == "");
			Assert::IsTrue(movedObj.getSubtype() == "type");
			Assert::IsTrue(&movedObj["subtype"].get<string>() != &obj["subtype"].get<string>());

			Assert::IsFalse(obj.isAuxiliaryAttribute("auxiliary"));
			Assert::IsTrue(movedObj.isAuxiliaryAttribute("auxiliary"));
			Assert::IsTrue(movedObj["auxiliary"].get<int32_t>() == 100);

			Assert::IsTrue(movedObj != obj);
			Assert::IsTrue(movedObj == copiedObj);
		}

		template <typename T>
		void ReactionAssignmentHelper()
		{
			// setting and getting stuff on this object
			T obj;

			obj.setSubtype("type");
			Assert::IsTrue(obj["subtype"].isExternal());
			Assert::IsTrue(obj["subtype"].get<string>() == obj.getSubtype());

			obj["auxiliary"] = 100;
			Assert::IsTrue(obj.isAuxiliaryAttribute("auxiliary"));
			Assert::IsTrue(obj["auxiliary"].get<int32_t>() == 100);

			// copy assignment
			T copiedObj;
			copiedObj.setSubtype("copied");

			copiedObj = obj;

			Assert::IsTrue(copiedObj["this"] == &copiedObj);
			Assert::IsTrue(copiedObj["this"].get<RTTI*>() != obj["this"].get<RTTI*>());

			Assert::IsTrue(copiedObj.getSubtype() == "type");
			Assert::IsTrue(copiedObj.getSubtype() == obj.getSubtype());
			Assert::IsTrue(&copiedObj["subtype"].get<string>() != &obj["subtype"].get<string>());

			Assert::IsTrue(copiedObj.isAuxiliaryAttribute("auxiliary"));
			Assert::IsTrue(copiedObj["auxiliary"].get<int32_t>() == 100);

			Assert::IsTrue(copiedObj == obj);

			// move assignment
			T movedObj;
			movedObj.setSubtype("moved");

			movedObj = std::move(obj);

			Assert::IsTrue(obj["this"] == &obj);
			Assert::IsTrue(movedObj["this"] == &movedObj);
			Assert::IsTrue(movedObj["this"].get<RTTI*>() == &movedObj);

			Assert::IsTrue(movedObj.getSubtype() == "type");
			Assert::IsTrue(movedObj.getSubtype() == copiedObj.getSubtype());
			Assert::IsTrue(&movedObj["subtype"].get<string>() != &obj["subtype"].get<string>());
			Assert::IsTrue(obj.getSubtype() == "");

			Assert::IsFalse(obj.isAuxiliaryAttribute("auxiliary"));
			Assert::IsTrue(movedObj.isAuxiliaryAttribute("auxiliary"));
			Assert::IsTrue(movedObj["auxiliary"].get<int32_t>() == 100);

			Assert::IsTrue(movedObj != obj);
			Assert::IsTrue(movedObj == copiedObj);
		}

		template <typename T>
		void ReactionDestructorHelper()
		{
			T obj;
			obj["auxiliary_1"] = "hello";
			obj["auxiliary_2"] = "world";
		}

		//

		static _CrtMemState sStartMemState;

		const static string sReactionPath;

		static Entity::EntityFactory* sEntityFactory;
		static ActionList::ActionListFactory* sActionListFactory;
		static ReactionAttributed::ReactionAttributedFactory* sReactionFactory;
		static ActionCreateAction::ActionCreateActionFactory* sActionCreateActionFactory;
	};

	_CrtMemState ReactionTest::sStartMemState;

	const string ReactionTest::sReactionPath = "assets/xml/Table_action/reaction_root.xml";

	Entity::EntityFactory* ReactionTest::sEntityFactory;
	ActionList::ActionListFactory* ReactionTest::sActionListFactory;
	ReactionAttributed::ReactionAttributedFactory* ReactionTest::sReactionFactory;
	ActionCreateAction::ActionCreateActionFactory* ReactionTest::sActionCreateActionFactory;
}

