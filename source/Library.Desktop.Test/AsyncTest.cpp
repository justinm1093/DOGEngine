
#include "pch.h"
#include "CppUnitTest.h"

#include "EventSubscriberFoo.h"
#include "Foo.h"

#include "Factory.h"

#include "World.h"
#include "WorldState.h"

#include "EventQueue.h"
#include "EventArgs.h"
#include "Event.h"

#include "ActionEvent.h"
#include "ActionUnsubscribe.h"
#include "ActionClearEvents.h"
#include "ReactionAttributed.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::chrono;
using namespace DOGEngine;
using namespace UnitTests;
using namespace std;

namespace LibraryDesktopTest
{
	TEST_CLASS(AsyncTest)
	{
	public:

		TEST_METHOD_INITIALIZE(Initialize)
		{
			// because we create so many subscribers to stress-test the system, the static list of subscribers grows in capacity during the test
			//		this causes the unit test to thing that there is a memory leak when there really isn't
			//		so we create a bunch of subscribers here to grow the static subscriber list
			PrepStaticSubscriberList<EventSubscriberFoo>(sNumSubs * 2);
			PrepStaticSubscriberList<ReactionAttributed>(sNumSubs);

			Attributed::clearAttributeCache();

#ifdef _DEBUG
			// grab snapshot of memory state at start of test
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
			_CrtMemCheckpoint(&sStartMemState);
#endif

			sActionEventFactory = new ActionEvent::ActionEventFactory();
			sActionUnsubscribeFactory = new ActionUnsubscribe::ActionUnsubscribeFactory();
			sActionClearEventsFactory = new ActionClearEvents::ActionClearEventsFactory();
			sReactionAttributedFactory = new ReactionAttributed::ReactionAttributedFactory();
		}

		TEST_METHOD_CLEANUP(Cleanup)
		{
			Attributed::clearAttributeCache();

			delete sActionEventFactory;
			delete sActionUnsubscribeFactory;
			delete sActionClearEventsFactory;
			delete sReactionAttributedFactory;

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


		TEST_METHOD(AsyncRTTI)
		{
			AsyncRTTIHelper<ActionUnsubscribe, Action>("ActionUnsubscribe", "Action");
			AsyncRTTIHelper<ActionClearEvents, Action>("ActionClearEvents", "Action");
		}

		TEST_METHOD(AsyncConstructors)
		{
			AsyncConstructorsHelper<ActionUnsubscribe>("Action");
			AsyncConstructorsHelper<ActionClearEvents>("Action");
		}
		
		TEST_METHOD(AsyncAssignment)
		{
			AsyncAssignmentHelper<ActionUnsubscribe>("Action");
			AsyncAssignmentHelper<ActionClearEvents>("Action");
		}

		TEST_METHOD(AsyncDestructor)
		{
			AsyncDestructorHelper<ActionUnsubscribe>();
			AsyncDestructorHelper<ActionClearEvents>();
		}

		TEST_METHOD(AsyncFactory)
		{
			AsyncFactoryHelper<ActionUnsubscribe, Action, ActionUnsubscribe::ActionUnsubscribeFactory>(sActionUnsubscribeFactory, "ActionUnsubscribe");
			AsyncFactoryHelper<ActionClearEvents, Action, ActionClearEvents::ActionClearEventsFactory>(sActionClearEventsFactory, "ActionClearEvents");
		}


		TEST_METHOD(AsyncDeliver)
		{
			for(uint32_t i = 0; i < sNumTests; ++i)
			{
				AsyncDeliverHelper();
			}
		}

		TEST_METHOD(AsyncUpdate)
		{
			// stress testing -- this one takes awhile
			for(uint32_t i = 0; i < sNumTests; ++i)
			{
				AsyncUpdateHelper();
			}
		}

		TEST_METHOD(AsyncSubscribes)
		{
			for(uint32_t i = 0; i < sNumTests; ++i)
			{
				AsyncSubscribesHelper();
			}
		}

		TEST_METHOD(AsyncUnsubscribes)
		{
			for(uint32_t i = 0; i < sNumTests; ++i)
			{
				AsyncUnsubscribesHelper();
			}
		}

		TEST_METHOD(AsyncUnsubscribesAll)
		{
			for(uint32_t i = 0; i < sNumTests; ++i)
			{
				AsyncUnsubscribesAllHelper();
			}
		}

		TEST_METHOD(AsyncActionUnsubscribe)
		{
			for(uint32_t i = 0; i < sNumTests; ++i)
			{
				AsyncActionUnsubscribeHelper();
			}
		}

		TEST_METHOD(AsyncEnqueues)
		{
			for(uint32_t i = 0; i < sNumTests; ++i)
			{
				AsyncEnqueuesHelper();
			}
		}

		TEST_METHOD(AsyncClearEvents)
		{
			for(uint32_t i = 0; i < sNumTests; ++i)
			{
				AsyncClearEventsHelper();
			}
		}

	private:

		template <typename DerivedT, typename BaseT>
		void AsyncRTTIHelper(const string& derivedTypeName, const string& baseTypeName)
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
		void AsyncConstructorsHelper(const string& defaultName)
		{
			// creating object with no name
			T objNoName;
			Assert::IsTrue(objNoName.getName() == defaultName);

			objNoName.setName("New Name");
			Assert::IsTrue(objNoName.getName() == "New Name");

			// creating an object with a name
			T obj("My Name");
			Assert::IsTrue(obj.getName() == "My Name");
			Assert::IsTrue(obj["name"].isExternal());
			Assert::IsTrue(obj["name"].get<string>() == obj.getName());

			obj.append("aux") = 100;
			Assert::IsTrue(obj.isAuxiliaryAttribute("aux"));
			Assert::IsTrue(obj["aux"].get<int32_t>() == 100);

			// copy constructor
			T copiedObj(obj);

			Assert::IsTrue(copiedObj.getName() == "My Name");
			Assert::IsTrue(copiedObj.getName() == obj.getName());
			Assert::IsTrue(copiedObj["this"].get<RTTI*>() != obj["this"].get<RTTI*>());
			Assert::IsTrue(&copiedObj["name"].get<string>() != &obj["name"].get<string>());

			Assert::IsTrue(copiedObj.isAuxiliaryAttribute("aux"));
			Assert::IsTrue(copiedObj["aux"].get<int32_t>() == 100);

			Assert::IsTrue(copiedObj == obj);

			// move constructor
			T movedObj(std::move(obj));

			Assert::IsTrue(obj.getName() == defaultName);
			Assert::IsTrue(movedObj.getName() == "My Name");
			Assert::IsTrue(movedObj.getName() == copiedObj.getName());
			Assert::IsTrue(movedObj["this"].get<RTTI*>() != obj["this"].get<RTTI*>());
			Assert::IsTrue(&movedObj["name"].get<string>() != &obj["name"].get<string>());

			Assert::IsFalse(obj.isAuxiliaryAttribute("aux"));
			Assert::IsTrue(movedObj.isAuxiliaryAttribute("aux"));
			Assert::IsTrue(movedObj["aux"].get<int32_t>() == 100);

			Assert::IsTrue(movedObj != obj);
			Assert::IsTrue(movedObj == copiedObj);
		}

		template <typename T>
		void AsyncAssignmentHelper(const string& defaultName)
		{
			T obj("My Name");
			Assert::IsTrue(obj.getName() == "My Name");
			Assert::IsTrue(obj["name"].isExternal());
			Assert::IsTrue(obj["name"].get<string>() == obj.getName());

			obj.append("aux") = 100;
			Assert::IsTrue(obj.isAuxiliaryAttribute("aux"));
			Assert::IsTrue(obj["aux"].get<int32_t>() == 100);

			// copy constructor
			T copiedObj;

			copiedObj.append("test") = "hello world";
			Assert::IsTrue(copiedObj.isAuxiliaryAttribute("test"));
			Assert::IsTrue(copiedObj["test"].get<string>() == "hello world");
			Assert::IsTrue(copiedObj.getName() == defaultName);

			copiedObj = obj;

			Assert::IsTrue(copiedObj.getName() == "My Name");
			Assert::IsTrue(copiedObj.getName() == obj.getName());
			Assert::IsTrue(copiedObj["this"].get<RTTI*>() != obj["this"].get<RTTI*>());
			Assert::IsTrue(&copiedObj["name"].get<string>() != &obj["name"].get<string>());

			Assert::IsTrue(copiedObj.isAuxiliaryAttribute("aux"));
			Assert::IsTrue(copiedObj["aux"].get<int32_t>() == 100);
			Assert::IsFalse(copiedObj.isAuxiliaryAttribute("test"));

			Assert::IsTrue(copiedObj == obj);

			// move constructor
			T movedObj;

			movedObj.append("test") = "bingo bango";
			Assert::IsTrue(movedObj.isAuxiliaryAttribute("test"));
			Assert::IsTrue(movedObj["test"].get<string>() == "bingo bango");
			Assert::IsTrue(movedObj.getName() == defaultName);

			movedObj = std::move(obj);

			Assert::IsTrue(obj.getName() == defaultName);
			Assert::IsTrue(movedObj.getName() == "My Name");
			Assert::IsTrue(movedObj.getName() == copiedObj.getName());
			Assert::IsTrue(movedObj["this"].get<RTTI*>() != obj["this"].get<RTTI*>());
			Assert::IsTrue(&movedObj["name"].get<string>() != &obj["name"].get<string>());

			Assert::IsFalse(obj.isAuxiliaryAttribute("aux"));
			Assert::IsTrue(movedObj.isAuxiliaryAttribute("aux"));
			Assert::IsTrue(movedObj["aux"].get<int32_t>() == 100);
			Assert::IsFalse(movedObj.isAuxiliaryAttribute("test"));

			Assert::IsTrue(movedObj != obj);
			Assert::IsTrue(movedObj == copiedObj);
		}

		template <typename T>
		void AsyncDestructorHelper()
		{
			T obj;
			obj.append("test_1") = "hello";
			obj.append("test_2") = "world";
		}

		template <typename ProductT, typename GroupT, typename FactoryT>
		void AsyncFactoryHelper(FactoryT* staticFactory, const string& productName)
		{
			// test factory registered
			FactoryT stackFactory;
			Assert::IsTrue(staticFactory->isRegistered());
			Assert::IsFalse(stackFactory.isRegistered());

			Assert::IsTrue(Factory<GroupT>::isFactoryRegistered(productName));
			Assert::IsFalse(Factory<GroupT>::isFactoryRegistered("NotAFactory"));

			Assert::IsTrue(Factory<GroupT>::numFactories() == 3);

			// test product name
			Assert::IsTrue(staticFactory->getProductName() == productName);

			// test product creation
			GroupT* obj = staticFactory->create();

			Assert::IsTrue(obj != nullptr);
			Assert::IsTrue(obj->Is(ProductT::TypeIdClass()));
			delete obj;

			GroupT* otherObj = Factory<GroupT>::create(productName);
			Assert::IsTrue(otherObj->Is(ProductT::TypeIdClass()));
			delete otherObj;
		}


		void AsyncDeliverHelper()
		{
			WorldState state;

			EventArgs args;
			args.setSubtype("test");
			args.setWorldState(state);

			args.append("arg_1") = 100;
			args.append("arg_2") = 5.0f;
			args.append("arg_3") = "hello world";

			Event<EventArgs> testEvent(args);

			ReactionAttributed reaction_1;
			ReactionAttributed reaction_2;
			ReactionAttributed reaction_3;
			ReactionAttributed reaction_4;
			ReactionAttributed reaction_5;
			reaction_1.addSubtype("test");
			reaction_2.addSubtype("test");
			reaction_3.addSubtype("test");
			reaction_4.addSubtype("test");

			// deliver event -- test async event processing independent of queue
			testEvent.deliver();

			// reactions 1-4 process event
			Assert::IsTrue(reaction_1["arg_1"] == 100);
			Assert::IsTrue(reaction_1["arg_2"] == 5.0f);
			Assert::IsTrue(reaction_1["arg_3"] == "hello world");

			Assert::IsTrue(reaction_2["arg_1"] == 100);
			Assert::IsTrue(reaction_2["arg_2"] == 5.0f);
			Assert::IsTrue(reaction_2["arg_3"] == "hello world");

			Assert::IsTrue(reaction_3["arg_1"] == 100);
			Assert::IsTrue(reaction_3["arg_2"] == 5.0f);
			Assert::IsTrue(reaction_3["arg_3"] == "hello world");

			Assert::IsTrue(reaction_4["arg_1"] == 100);
			Assert::IsTrue(reaction_4["arg_2"] == 5.0f);
			Assert::IsTrue(reaction_4["arg_3"] == "hello world");

			// reaction 5 does not process event
			Assert::IsTrue(!reaction_5.isAttribute("arg_1"));
			Assert::IsTrue(!reaction_5.isAttribute("arg_2"));
			Assert::IsTrue(!reaction_5.isAttribute("arg_3"));
		}

		void AsyncUpdateHelper()
		{
			// create a bunch of subscribers
			Assert::IsTrue(Event<Foo>::numSubscribers() == 0);
			EventSubscriberFoo* foosubs = reinterpret_cast<EventSubscriberFoo*>(malloc(sizeof(EventSubscriberFoo) * sNumSubs));
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				new(foosubs + i)EventSubscriberFoo();
			}
			Assert::IsTrue(Event<Foo>::numSubscribers() == sNumSubs);

			// enqueue honey-bunches of events
			EventQueue eventQueue;
			GameTime time;
			time.SetCurrentTime(high_resolution_clock::now());

			Assert::IsTrue(eventQueue.size() == 0);
			for(uint32_t i = 0; i < sNumEvents; ++i)
			{
				eventQueue.enqueue(*new Event<Foo>(Foo(i)), time);
				eventQueue.enqueue(*new Event<Foo>(Foo(i)), time, milliseconds(1000));
				eventQueue.enqueue(*new Event<Foo>(Foo(i)), time, milliseconds(5000));
			}
			Assert::IsTrue(eventQueue.size() == sNumEvents * 3);

			// deliver no batches
			time.SetCurrentTime(time.CurrentTime() - milliseconds(500));
			eventQueue.update(time);

			Assert::IsTrue(eventQueue.size() == sNumEvents * 3);
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				Assert::IsTrue(foosubs[i].getNumNotifies() == 0);
			}

			// deliver the first batches
			time.SetCurrentTime(time.CurrentTime() + milliseconds(1000));
			eventQueue.update(time);
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				Assert::IsTrue(foosubs[i].getNumNotifies() == sNumEvents);
			}

			// deliver the second batches
			time.SetCurrentTime(time.CurrentTime() + milliseconds(1000));
			eventQueue.update(time);
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				Assert::IsTrue(foosubs[i].getNumNotifies() == sNumEvents * 2);
			}

			// deliver the last batches
			time.SetCurrentTime(time.CurrentTime() + milliseconds(7000));
			eventQueue.update(time);
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				Assert::IsTrue(foosubs[i].getNumNotifies() == sNumEvents * 3);
			}

			// cleanup
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				foosubs[i].~EventSubscriberFoo();
			}
			free(foosubs);
		}

		void AsyncSubscribesHelper()
		{
			// create a bunch of subscribers
			Assert::IsTrue(Event<Foo>::numSubscribers() == 0);
			EventSubscriberFoo* foosubs = reinterpret_cast<EventSubscriberFoo*>(malloc(sizeof(EventSubscriberFoo) * sNumSubs));
			uint32_t* savedNumNotifies = reinterpret_cast<uint32_t*>(malloc(sizeof(uint32_t) * sNumSubs));
			uint32_t* savedChildNumNotifies = reinterpret_cast<uint32_t*>(malloc(sizeof(uint32_t) * sNumSubs));

			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				new(foosubs + i)EventSubscriberFoo(EventSubscriberFoo::SubscriberAction::SUBSCRIBE);
			}
			Assert::IsTrue(Event<Foo>::numSubscribers() == sNumSubs);

			// enqueue honey-bunches of events
			EventQueue eventQueue;
			GameTime time;
			time.SetCurrentTime(high_resolution_clock::now());

			Assert::IsTrue(eventQueue.size() == 0);
			for(uint32_t i = 0; i < sNumEvents; ++i)
			{
				eventQueue.enqueue(*new Event<Foo>(Foo(i)), time);
				eventQueue.enqueue(*new Event<Foo>(Foo(i)), time, milliseconds(1000));
			}
			Assert::IsTrue(eventQueue.size() == sNumEvents * 2);

			// deliver no batches
			time.SetCurrentTime(time.CurrentTime() - milliseconds(500));
			eventQueue.update(time);

			Assert::IsTrue(eventQueue.size() == sNumEvents * 2);
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				Assert::IsTrue(foosubs[i].getNumNotifies() == 0);
			}
			Assert::IsTrue(Event<Foo>::numSubscribers() == sNumSubs);

			// deliver first batch
			time.SetCurrentTime(time.CurrentTime() + milliseconds(1000));
			eventQueue.update(time);
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				// all the original subscribers received all events
				// all child subscribers received all events that were spun up after their creation
				savedNumNotifies[i] = foosubs[i].getNumNotifies();
				savedChildNumNotifies[i] = foosubs[i].getChildNumNotifies();

				Assert::IsTrue(savedNumNotifies[i] == sNumEvents);
				Assert::IsTrue(savedChildNumNotifies[i] <= sNumEvents);
			}
			Assert::IsTrue(Event<Foo>::numSubscribers() == sNumSubs * 2);		// each original subscriber created 1 new subscriber

																				// deliver second batch
			time.SetCurrentTime(time.CurrentTime() + milliseconds(1000));
			eventQueue.update(time);
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				Assert::IsTrue(foosubs[i].getNumNotifies() == savedNumNotifies[i] + sNumEvents);
				Assert::IsTrue(foosubs[i].getChildNumNotifies() == savedChildNumNotifies[i] + sNumEvents);
			}
			Assert::IsTrue(Event<Foo>::numSubscribers() == sNumSubs * 2);

			// cleanup
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				foosubs[i].~EventSubscriberFoo();
			}
			free(foosubs);
			free(savedNumNotifies);
			free(savedChildNumNotifies);
		}

		void AsyncUnsubscribesHelper()
		{
			// create a bunch of subscribers
			Assert::IsTrue(Event<Foo>::numSubscribers() == 0);
			EventSubscriberFoo* foosubs = reinterpret_cast<EventSubscriberFoo*>(malloc(sizeof(EventSubscriberFoo) * sNumSubs));
			uint32_t* savedNumNotifies = reinterpret_cast<uint32_t*>(malloc(sizeof(uint32_t) * sNumSubs));
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				new(foosubs + i)EventSubscriberFoo(EventSubscriberFoo::SubscriberAction::UNSUBSCRIBE);
			}
			Assert::IsTrue(Event<Foo>::numSubscribers() == sNumSubs);

			// enqueue honey-bunches of events
			EventQueue eventQueue;
			GameTime time;
			time.SetCurrentTime(high_resolution_clock::now());

			Assert::IsTrue(eventQueue.size() == 0);
			for(uint32_t i = 0; i < sNumEvents; ++i)
			{
				eventQueue.enqueue(*new Event<Foo>(Foo(i)), time);
				eventQueue.enqueue(*new Event<Foo>(Foo(i)), time, milliseconds(1000));
			}
			Assert::IsTrue(eventQueue.size() == sNumEvents * 2);

			// deliver no batches
			time.SetCurrentTime(time.CurrentTime() - milliseconds(500));
			eventQueue.update(time);

			Assert::IsTrue(eventQueue.size() == sNumEvents * 2);
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				Assert::IsTrue(foosubs[i].getNumNotifies() == 0);
			}

			// deliver first batch
			time.SetCurrentTime(time.CurrentTime() + milliseconds(1000));
			eventQueue.update(time);
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				savedNumNotifies[i] = foosubs[i].getNumNotifies();
				Assert::IsTrue(savedNumNotifies[i] < sNumEvents);
			}
			Assert::IsTrue(Event<Foo>::numSubscribers() == 0);

			// deliver second batch
			time.SetCurrentTime(time.CurrentTime() + milliseconds(1000));
			eventQueue.update(time);
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				// subscriber objects not notified any further since they were all unsubscribed
				Assert::IsTrue(savedNumNotifies[i] == foosubs[i].getNumNotifies());
			}

			// cleanup
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				foosubs[i].~EventSubscriberFoo();
			}
			free(foosubs);
			free(savedNumNotifies);
		}

		void AsyncUnsubscribesAllHelper()
		{
			// create a bunch of subscribers
			Assert::IsTrue(Event<Foo>::numSubscribers() == 0);
			EventSubscriberFoo* foosubs = reinterpret_cast<EventSubscriberFoo*>(malloc(sizeof(EventSubscriberFoo) * sNumSubs));
			uint32_t* savedNumNotifies = reinterpret_cast<uint32_t*>(malloc(sizeof(uint32_t) * sNumSubs));
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				new(foosubs + i)EventSubscriberFoo(EventSubscriberFoo::SubscriberAction::CLEAR_SUBSCRIBERS);
			}
			Assert::IsTrue(Event<Foo>::numSubscribers() == sNumSubs);

			// enqueue honey-bunches of events
			EventQueue eventQueue;
			GameTime time;
			time.SetCurrentTime(high_resolution_clock::now());

			Assert::IsTrue(eventQueue.size() == 0);
			for(uint32_t i = 0; i < sNumEvents; ++i)
			{
				eventQueue.enqueue(*new Event<Foo>(Foo(i)), time);
				eventQueue.enqueue(*new Event<Foo>(Foo(i)), time, milliseconds(1000));
			}
			Assert::IsTrue(eventQueue.size() == sNumEvents * 2);

			// deliver no batches
			time.SetCurrentTime(time.CurrentTime() - milliseconds(500));
			eventQueue.update(time);

			Assert::IsTrue(eventQueue.size() == sNumEvents * 2);
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				Assert::IsTrue(foosubs[i].getNumNotifies() == 0);
			}

			// deliver first batch
			time.SetCurrentTime(time.CurrentTime() + milliseconds(1000));
			eventQueue.update(time);
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				savedNumNotifies[i] = foosubs[i].getNumNotifies();
				Assert::IsTrue(savedNumNotifies[i] < sNumEvents);
			}
			Assert::IsTrue(Event<Foo>::numSubscribers() == 0);

			// deliver second batch
			time.SetCurrentTime(time.CurrentTime() + milliseconds(1000));
			eventQueue.update(time);
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				// subscriber objects not notified any further since they were all unsubscribed
				Assert::IsTrue(savedNumNotifies[i] == foosubs[i].getNumNotifies());
			}

			// cleanup
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				foosubs[i].~EventSubscriberFoo();
			}
			free(foosubs);
			free(savedNumNotifies);
		}

		void AsyncActionUnsubscribeHelper()
		{
			// create subscribers with an unsubscribe action
			World world;
			Datum& reactions = world.getReactions();
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				ReactionAttributed* reaction = world.createReaction("ReactionAttributed", "Reaction")->As<ReactionAttributed>();
				reaction->addSubtype("Subtype");

				reaction->createAction("ActionUnsubscribe", "Unsubscribe");
				Assert::IsTrue(reaction->getActions().size() == 1);
			}

			EventQueue& eventQueue = world.getEventQueue();
			GameTime& time = world.getWorldState().gameTime;

			int32_t* savedData = reinterpret_cast<int32_t*>(malloc(sizeof(int32_t) * sNumSubs));

			// enqueue honey-bunches of events
			time.SetCurrentTime(high_resolution_clock::now());
			Assert::IsTrue(eventQueue.size() == 0);
			for(uint32_t i = 0; i < sNumEvents; ++i)
			{
				EventArgs args;
				args.setSubtype("Subtype");
				args.setWorldState(world.getWorldState());
				args.append("arg") = static_cast<int32_t>(i) + 1;			// first batch has non-zero data

				EventArgs otherArgs;
				otherArgs.setSubtype("Subtype");
				otherArgs.setWorldState(world.getWorldState());
				otherArgs.append("arg") = 0;								// second batch has data = 0

				eventQueue.enqueue(*new Event<EventArgs>(args), time);
				eventQueue.enqueue(*new Event<EventArgs>(otherArgs), time, milliseconds(500));
			}
			Assert::IsTrue(Event<EventArgs>::numSubscribers() == sNumSubs);

			// deliver first batch -- first batch received, all subscribers unsubscribed
			time.SetCurrentTime(time.CurrentTime() + milliseconds(100));
			world.update();
			for(uint32_t i = 0; i < reactions.size(); ++i)
			{
				savedData[i] = reactions[i]["arg"].get<int32_t>();
				Assert::IsTrue(savedData[i] > 0);
			}
			Assert::IsTrue(Event<EventArgs>::numSubscribers() == 0);

			// deliver second batch -- second batch not received by any subscribers
			time.SetCurrentTime(time.CurrentTime() + milliseconds(1000));
			world.update();
			for(uint32_t i = 0; i < reactions.size(); ++i)
			{
				// the data on the subscriber was not changed
				Assert::IsTrue(reactions[i]["arg"].get<int32_t>() == savedData[i]);
			}

			free(savedData);
		}

		void AsyncEnqueuesHelper()
		{
			// create subscribers with an unsubscribe action
			World world;
			Datum& reactions = world.getReactions();
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				ReactionAttributed* reaction = world.createReaction("ReactionAttributed", "Reaction")->As<ReactionAttributed>();
				reaction->addSubtype("Subtype");

				ActionEvent* action = reaction->createAction("ActionEvent", "EventAction")->As<ActionEvent>();
				action->setSubtype("Duds");
				action->setDelay(1000);

				Assert::IsTrue(reaction->getActions().size() == 1);
			}

			EventQueue& eventQueue = world.getEventQueue();
			GameTime& time = world.getWorldState().gameTime;

			int32_t* savedData = reinterpret_cast<int32_t*>(malloc(sizeof(int32_t) * sNumSubs));

			// enqueue some events to kick things off
			time.SetCurrentTime(high_resolution_clock::now());
			Assert::IsTrue(eventQueue.size() == 0);
			for(uint32_t i = 0; i < 10; ++i)
			{
				EventArgs args;
				args.setSubtype("Subtype");
				args.setWorldState(world.getWorldState());
				args.append("arg") = static_cast<int32_t>(i) + 1;

				eventQueue.enqueue(*new Event<EventArgs>(args), time);
			}
			Assert::IsTrue(eventQueue.size() == 10);

			// deliver first batch
			time.SetCurrentTime(time.CurrentTime() + milliseconds(100));
			world.update();
			for(uint32_t i = 0; i < reactions.size(); ++i)
			{
				savedData[i] = reactions[i]["arg"].get<int32_t>();
				Assert::IsTrue(savedData[i] > 0);
			}
			Assert::IsTrue(eventQueue.size() == 10 * sNumSubs);

			// deliver created batch -- all duds that aren't picked up
			time.SetCurrentTime(time.CurrentTime() + milliseconds(5000));
			world.update();
			for(uint32_t i = 0; i < reactions.size(); ++i)
			{
				Assert::IsTrue(reactions[i]["arg"].get<int32_t>() == savedData[i]);
			}
			Assert::IsTrue(eventQueue.size() == 0);

			free(savedData);
		}

		void AsyncClearEventsHelper()
		{
			// create subscribers with an unsubscribe action
			World world;
			Datum& reactions = world.getReactions();
			for(uint32_t i = 0; i < sNumSubs; ++i)
			{
				ReactionAttributed* reaction = world.createReaction("ReactionAttributed", "Reaction")->As<ReactionAttributed>();
				reaction->addSubtype("Subtype");

				reaction->createAction("ActionClearEvents", "Unsubscribe");
				Assert::IsTrue(reaction->getActions().size() == 1);
			}

			EventQueue& eventQueue = world.getEventQueue();
			GameTime& time = world.getWorldState().gameTime;

			int32_t* savedData = reinterpret_cast<int32_t*>(malloc(sizeof(int32_t) * sNumSubs));

			// enqueue honey-bunches of events
			time.SetCurrentTime(high_resolution_clock::now());
			Assert::IsTrue(eventQueue.size() == 0);
			for(uint32_t i = 0; i < sNumEvents; ++i)
			{
				EventArgs args;
				args.setSubtype("Subtype");
				args.setWorldState(world.getWorldState());
				args.append("arg") = static_cast<int32_t>(i) + 1;			// first batch has non-zero data

				EventArgs otherArgs;
				otherArgs.setSubtype("Subtype");
				otherArgs.setWorldState(world.getWorldState());
				otherArgs.append("arg") = 0;								// second batch has data = 0

				eventQueue.enqueue(*new Event<EventArgs>(args), time);
				eventQueue.enqueue(*new Event<EventArgs>(otherArgs), time, milliseconds(500));
			}
			Assert::IsTrue(eventQueue.size() == sNumEvents * 2);

			// deliver first batch -- first batch received, all other events cleared
			time.SetCurrentTime(time.CurrentTime() + milliseconds(100));
			world.update();
			for(uint32_t i = 0; i < reactions.size(); ++i)
			{
				savedData[i] = reactions[i]["arg"].get<int32_t>();
				Assert::IsTrue(savedData[i] > 0);
			}
			Assert::IsTrue(eventQueue.size() == 0);

			// deliver second batch -- oh, wait. there is no second batch!
			time.SetCurrentTime(time.CurrentTime() + milliseconds(1000));
			world.update();
			for(uint32_t i = 0; i < reactions.size(); ++i)
			{
				Assert::IsTrue(reactions[i]["arg"].get<int32_t>() == savedData[i]);
			}

			free(savedData);
		}


		template <typename T>
		void PrepStaticSubscriberList(uint32_t size)
		{
			T* block = reinterpret_cast<T*>(malloc(sizeof(T) * size));

			for(uint32_t i = 0; i < size; ++i)
			{
				new(block + i)T();
			}
			for(uint32_t i = 0; i < size; ++i)
			{
				block[i].~T();
			}

			free(block);
		}

		//

		static _CrtMemState sStartMemState;

		static ActionEvent::ActionEventFactory* sActionEventFactory;
		static ActionUnsubscribe::ActionUnsubscribeFactory* sActionUnsubscribeFactory;
		static ActionClearEvents::ActionClearEventsFactory* sActionClearEventsFactory;
		static ReactionAttributed::ReactionAttributedFactory* sReactionAttributedFactory;

		static uint32_t sNumTests;
		static uint32_t sNumSubs;
		static uint32_t sNumEvents;
	};

	_CrtMemState AsyncTest::sStartMemState;

	ActionEvent::ActionEventFactory* AsyncTest::sActionEventFactory;
	ActionUnsubscribe::ActionUnsubscribeFactory* AsyncTest::sActionUnsubscribeFactory;
	ActionClearEvents::ActionClearEventsFactory* AsyncTest::sActionClearEventsFactory;
	ReactionAttributed::ReactionAttributedFactory* AsyncTest::sReactionAttributedFactory;

	uint32_t AsyncTest::sNumTests = 25;
	uint32_t AsyncTest::sNumSubs = 50;
	uint32_t AsyncTest::sNumEvents = 500;
}

