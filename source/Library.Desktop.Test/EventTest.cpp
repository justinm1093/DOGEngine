
#include "pch.h"
#include "CppUnitTest.h"

#include "EventSubscriberFoo.h"
#include "IEventSubscriber.h"
#include "EventPublisher.h"
#include "Event.h"

#include "EventQueue.h"

#include "Foo.h"
#include "EntityFoo.h"
#include "AttributedFoo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::chrono;
using namespace DOGEngine;
using namespace UnitTests;
using namespace std;
using namespace glm;

namespace LibraryDesktopTest
{
	TEST_CLASS(EventTest)
	{
	public:

		TEST_METHOD_INITIALIZE(Initialize)
		{
#ifdef _DEBUG
			// grab snapshot of memory state at start of test
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
			_CrtMemCheckpoint(&sStartMemState);
#endif
		}

		TEST_METHOD_CLEANUP(Cleanup)
		{
			Attributed::clearAttributeCache();

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


		TEST_METHOD(EventRTTI)
		{
			// test RTTI types for a few different event types
			EventRTTIHelper<Event<Foo>, EventPublisher, Foo>("Event<T>", "EventPublisher");
			EventRTTIHelper<Event<EntityFoo>, EventPublisher, EntityFoo>("Event<T>", "EventPublisher");
			EventRTTIHelper<Event<AttributedFoo>, EventPublisher, AttributedFoo>("Event<T>", "EventPublisher");

			// template types have different type id's
			Assert::IsTrue(Event<Foo>::TypeIdClass() != Event<int32_t>::TypeIdClass());
		}

		TEST_METHOD(EventConstructors)
		{
			EventConstructorsHelper();
			EventQueueConstructorsHelper();
		}

		TEST_METHOD(EventAssignment)
		{
			EventAssignmentHelper();
			EventQueueAssignmentHelper();
		}

		TEST_METHOD(EventDestructors)
		{
			// event class
			{
				Foo foo;
				Event<Foo> eventFoo(foo);
				eventFoo.setTime(high_resolution_clock::now(), milliseconds(4000));
			}

			// event subscriber
			{
				EventSubscriberFoo subFoo;
			}
		}

		TEST_METHOD(EventGetSetTime)
		{
			Foo foo;
			Event<Foo> eventFoo(foo);

			// time enqueued and delay are default values on construction
			Assert::IsTrue(eventFoo.timeEnqueued() == high_resolution_clock::time_point());
			Assert::IsTrue(eventFoo.delay() == milliseconds());

			// setting time enqueued and delay
			high_resolution_clock::time_point now = high_resolution_clock::now();
			milliseconds delay(3000);

			eventFoo.setTime(now, delay);

			Assert::IsTrue(eventFoo.timeEnqueued() == now);
			Assert::IsTrue(eventFoo.delay() == milliseconds(3000));

			// setting only time enqueued resets delay
			high_resolution_clock::time_point otherNow = high_resolution_clock::now();

			eventFoo.setTime(otherNow);

			Assert::IsTrue(eventFoo.timeEnqueued() == otherNow);
			Assert::IsTrue(eventFoo.delay() == milliseconds());
		}
		
		TEST_METHOD(EventSubscription)
		{
			Foo foo;
			Event<Foo> eventFoo(foo);

			int32_t num = 0;
			Event<int32_t> eventInt(num);

			// no subscribers
			Assert::IsTrue(Event<Foo>::numSubscribers() == 0);
			Assert::IsTrue(eventFoo.numSubscribers() == 0);

			Assert::IsTrue(Event<int32_t>::numSubscribers() == 0);
			Assert::IsTrue(eventInt.numSubscribers() == 0);

			// EventSubscriberFoo subscribes to Event<Foo> on creation
			EventSubscriberFoo* subFoo = new EventSubscriberFoo();

			Assert::IsTrue(Event<Foo>::numSubscribers() == 1);
			Assert::IsTrue(eventFoo.numSubscribers() == 1);

			Assert::IsTrue(Event<int32_t>::numSubscribers() == 0);
			Assert::IsTrue(eventInt.numSubscribers() == 0);

			{
				EventSubscriberFoo otherSubFoo;

				Assert::IsTrue(Event<Foo>::numSubscribers() == 2);
				Assert::IsTrue(eventFoo.numSubscribers() == 2);

				Assert::IsTrue(Event<int32_t>::numSubscribers() == 0);
				Assert::IsTrue(eventInt.numSubscribers() == 0);
			}

			// destruction of EventSubscriberFoo unsubscribes it (stack allocated obj above)
			Assert::IsTrue(Event<Foo>::numSubscribers() == 1);
			Assert::IsTrue(eventFoo.numSubscribers() == 1);

			Assert::IsTrue(Event<int32_t>::numSubscribers() == 0);
			Assert::IsTrue(eventInt.numSubscribers() == 0);

			delete subFoo;

			Assert::IsTrue(Event<Foo>::numSubscribers() == 0);
			Assert::IsTrue(eventFoo.numSubscribers() == 0);

			Assert::IsTrue(Event<int32_t>::numSubscribers() == 0);
			Assert::IsTrue(eventInt.numSubscribers() == 0);

			// unsubscribing all
			EventSubscriberFoo subFoo_1;
			EventSubscriberFoo subFoo_2;
			EventSubscriberFoo subFoo_3;

			Assert::IsTrue(Event<Foo>::numSubscribers() == 3);

			Event<Foo>::unsubscribeAll();

			Assert::IsTrue(Event<Foo>::numSubscribers() == 0);

			// unsubscribing and subscribing directly through the static methods
			EventSubscriberFoo subFoo_a;

			Assert::IsTrue(Event<Foo>::numSubscribers() == 1);
			Event<Foo>::unsubscribe(subFoo_a);
			Assert::IsTrue(Event<Foo>::numSubscribers() == 0);
			Event<Foo>::subscribe(subFoo_a);
			Assert::IsTrue(Event<Foo>::numSubscribers() == 1);
		}

		TEST_METHOD(EventDeliver)
		{
			EventSubscriberFoo subFoo;
			Assert::IsTrue(subFoo.getInt() == 0);

			EventSubscriberFoo otherSubFoo;
			Assert::IsTrue(otherSubFoo.getInt() == 0);

			Foo foo(10);
			Foo otherFoo(30);
			Event<Foo> eventFoo(foo);
			Event<Foo> otherFooEvent(otherFoo);

			int32_t val = 25;
			Event<int32_t> eventInt(val);

			Assert::IsTrue(eventInt.numSubscribers() == 0);
			Assert::IsTrue(eventFoo.numSubscribers() == 2);
			Assert::IsTrue(otherFooEvent.numSubscribers() == 2);

			// broadcasting int event
			eventInt.deliver();
			Assert::IsTrue(subFoo.getInt() == 0);
			Assert::IsTrue(otherSubFoo.getInt() == 0);

			// broadcasting foo event
			eventFoo.deliver();
			Assert::IsTrue(subFoo.getInt() == 10);
			Assert::IsTrue(otherSubFoo.getInt() == 10);

			otherFooEvent.deliver();
			Assert::IsTrue(subFoo.getInt() == 30);
			Assert::IsTrue(otherSubFoo.getInt() == 30);

			// broadcasting polymorphically
			Foo thirdFoo(500);
			EventPublisher* polymorphicEvent = new Event<Foo>(thirdFoo);
			Assert::IsTrue(polymorphicEvent->numSubscribers() == 2);

			polymorphicEvent->deliver();
			Assert::IsTrue(subFoo.getInt() == 500);
			Assert::IsTrue(otherSubFoo.getInt() == 500);

			delete polymorphicEvent;

			int32_t otherVal = 1000;
			EventPublisher* otherPolymorphicEvent = new Event<int32_t>(otherVal);
			Assert::IsTrue(otherPolymorphicEvent->numSubscribers() == 0);

			otherPolymorphicEvent->deliver();
			Assert::IsTrue(subFoo.getInt() == 500);
			Assert::IsTrue(otherSubFoo.getInt() == 500);

			delete otherPolymorphicEvent;
		}

		TEST_METHOD(EventIsExpired)
		{
			GameTime time;
			time.SetCurrentTime(high_resolution_clock::now());

			// creating event
			Foo foo;
			Event<Foo> fooEvent(foo);

			// set time to now and delay to 1 second
			fooEvent.setTime(high_resolution_clock::now(), milliseconds(1000));
			Assert::IsFalse(fooEvent.isExpired(time));

			// reset time so that "now" is passed the enqueue time + delay
			fooEvent.setTime(high_resolution_clock::time_point(), milliseconds(0));
			Assert::IsTrue(fooEvent.isExpired(time));
		}

		TEST_METHOD(EventEnqueue)
		{
			GameTime time;
			Foo foo;

			EventQueue eventQueue;

			Assert::IsTrue(eventQueue.size() == 0);
			Assert::IsTrue(eventQueue.isEmpty());

			// enqueuing an event
			Event<Foo>* fooEvent = new Event<Foo>(foo);
			eventQueue.enqueue(*fooEvent, time);

			Assert::IsTrue(eventQueue.size() == 1);
			Assert::IsFalse(eventQueue.isEmpty());

			// enqueuing another event
			Event<Foo>* otherFooEvent = new Event<Foo>(foo);
			eventQueue.enqueue(*otherFooEvent, time);

			Assert::IsTrue(eventQueue.size() == 2);
			Assert::IsFalse(eventQueue.isEmpty());

			// clearing events
			eventQueue.clear();

			Assert::IsTrue(eventQueue.size() == 0);
			Assert::IsTrue(eventQueue.isEmpty());
		}

		TEST_METHOD(EventSend)
		{
			Foo foo(100);

			EventSubscriberFoo subFoo;
			Assert::IsTrue(subFoo.getInt() == 0);

			EventQueue eventQueue;
			eventQueue.send(*new Event<Foo>(foo));

			Assert::IsTrue(subFoo.getInt() == foo.getValue());
		}

		TEST_METHOD(EventUpdate)
		{
			EventSubscriberFoo subFoo;
			EventSubscriberFoo otherSubFoo;

			Foo foo(10);
			Foo otherFoo(20);
			Foo thirdFoo(30);

			Event<Foo>* foovent = new Event<Foo>(foo);
			Event<Foo>* otherFoovent = new Event<Foo>(otherFoo);
			Event<Foo>* thirdFoovent = new Event<Foo>(thirdFoo);

			GameTime time;
			time.SetCurrentTime(high_resolution_clock::now());

			// add several events to the queue
			EventQueue eventQueue;
			eventQueue.enqueue(*foovent, time, milliseconds(5000));
			eventQueue.enqueue(*otherFoovent, time);
			eventQueue.enqueue(*thirdFoovent, time, milliseconds(6000));

			// expire the earliest event
			time.SetCurrentTime(time.CurrentTime() + milliseconds(1000));

			Assert::IsFalse(foovent->isExpired(time));
			Assert::IsTrue(otherFoovent->isExpired(time));
			Assert::IsFalse(thirdFoovent->isExpired(time));

			Assert::IsTrue(eventQueue.size() == 3);
			eventQueue.update(time);
			Assert::IsTrue(eventQueue.size() == 2);

			Assert::IsTrue(subFoo.getInt() == otherFoo.getValue());
			Assert::IsTrue(otherSubFoo.getInt() == otherFoo.getValue());

			// expire the next earliest event
			time.SetCurrentTime(time.CurrentTime() + milliseconds(4500));
			Assert::IsTrue(foovent->isExpired(time));
			Assert::IsTrue(otherFoovent->isExpired(time));
			Assert::IsFalse(thirdFoovent->isExpired(time));

			Assert::IsTrue(eventQueue.size() == 2);
			eventQueue.update(time);
			Assert::IsTrue(eventQueue.size() == 1);

			Assert::IsTrue(subFoo.getInt() == foo.getValue());
			Assert::IsTrue(otherSubFoo.getInt() == foo.getValue());

			// expire the last event
			time.SetCurrentTime(time.CurrentTime() + milliseconds(4000));
			Assert::IsTrue(foovent->isExpired(time));
			Assert::IsTrue(otherFoovent->isExpired(time));
			Assert::IsTrue(thirdFoovent->isExpired(time));

			Assert::IsTrue(eventQueue.size() == 1);
			eventQueue.update(time);
			Assert::IsTrue(eventQueue.size() == 0);

			Assert::IsTrue(subFoo.getInt() == thirdFoo.getValue());
			Assert::IsTrue(otherSubFoo.getInt() == thirdFoo.getValue());
		}

	private:

		template <typename DerivedT, typename BaseT, typename MessageT>
		void EventRTTIHelper(const string& derivedTypeName, const string& baseTypeName)
		{
			MessageT message;
			DerivedT obj(message);
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

		void EventConstructorsHelper()
		{
			Foo foo(5);
			Foo defaultFoo;

			high_resolution_clock::time_point now = high_resolution_clock::now();
			milliseconds delay(5000);

			Event<Foo> obj(foo);

			// default construction
			Assert::IsTrue(obj.timeEnqueued() == high_resolution_clock::time_point());
			Assert::IsTrue(obj.delay() == milliseconds());
			Assert::IsTrue(obj.message() == foo);

			obj.setTime(now, delay);

			Assert::IsTrue(obj.timeEnqueued() == now);
			Assert::IsTrue(obj.delay() == delay);

			// copy construction
			Event<Foo> copiedObj(obj);

			Assert::IsTrue(copiedObj.timeEnqueued() == now);
			Assert::IsTrue(copiedObj.delay() == delay);
			Assert::IsTrue(copiedObj.message() == foo);

			// move construction
			Event<Foo> movedObj(std::move(obj));

			Assert::IsTrue(movedObj.timeEnqueued() == now);
			Assert::IsTrue(movedObj.delay() == delay);
			Assert::IsTrue(movedObj.message() == foo);

			Assert::IsTrue(movedObj.timeEnqueued() == copiedObj.timeEnqueued());
			Assert::IsTrue(movedObj.delay() == copiedObj.delay());
			Assert::IsTrue(movedObj.message() == copiedObj.message());

			Assert::IsTrue(obj.timeEnqueued() == high_resolution_clock::time_point());
			Assert::IsTrue(obj.delay() == milliseconds());
			Assert::IsTrue(obj.message() == defaultFoo);
		}

		void EventQueueConstructorsHelper()
		{
			// event queue -- default constructor
			EventQueue eventQueue;
			Assert::IsTrue(eventQueue.size() == 0);
			Assert::IsTrue(eventQueue.isEmpty());

			Foo foo;
			GameTime time;
			eventQueue.enqueue(*new Event<Foo>(foo), time);

			Assert::IsTrue(eventQueue.size() == 1);
			Assert::IsFalse(eventQueue.isEmpty());

			// event queue -- copy constructor
			EventQueue copiedQueue(eventQueue);

			Assert::IsTrue(copiedQueue.size() == 1);
			Assert::IsFalse(copiedQueue.isEmpty());

			Assert::IsTrue(eventQueue.size() == 1);
			Assert::IsFalse(eventQueue.isEmpty());

			// event queue -- move constructor
			EventQueue movedQueue(std::move(eventQueue));

			Assert::IsTrue(movedQueue.size() == 1);
			Assert::IsFalse(movedQueue.isEmpty());

			Assert::IsTrue(eventQueue.size() == 0);
			Assert::IsTrue(eventQueue.isEmpty());
		}

		void EventAssignmentHelper()
		{
			Foo foo(5);
			Foo defaultFoo;

			high_resolution_clock::time_point now = high_resolution_clock::now();
			milliseconds delay(5000);

			Event<Foo> obj(foo);

			// default construction
			Assert::IsTrue(obj.timeEnqueued() == high_resolution_clock::time_point());
			Assert::IsTrue(obj.delay() == milliseconds());
			Assert::IsTrue(obj.message() == foo);

			obj.setTime(now, delay);

			Assert::IsTrue(obj.timeEnqueued() == now);
			Assert::IsTrue(obj.delay() == delay);

			// copy assignment
			Event<Foo> copiedObj(defaultFoo);

			Assert::IsTrue(copiedObj.timeEnqueued() == high_resolution_clock::time_point());
			Assert::IsTrue(copiedObj.delay() == milliseconds());
			Assert::IsTrue(copiedObj.message() == defaultFoo);

			copiedObj = obj;

			Assert::IsTrue(copiedObj.timeEnqueued() == now);
			Assert::IsTrue(copiedObj.delay() == delay);
			Assert::IsTrue(copiedObj.message() == foo);

			// move assignment
			Event<Foo> movedObj(defaultFoo);

			Assert::IsTrue(movedObj.timeEnqueued() == high_resolution_clock::time_point());
			Assert::IsTrue(movedObj.delay() == milliseconds());
			Assert::IsTrue(movedObj.message() == defaultFoo);

			movedObj = std::move(obj);

			Assert::IsTrue(movedObj.timeEnqueued() == now);
			Assert::IsTrue(movedObj.delay() == delay);
			Assert::IsTrue(movedObj.message() == foo);

			Assert::IsTrue(movedObj.timeEnqueued() == copiedObj.timeEnqueued());
			Assert::IsTrue(movedObj.delay() == copiedObj.delay());
			Assert::IsTrue(movedObj.message() == copiedObj.message());

			Assert::IsTrue(obj.timeEnqueued() == high_resolution_clock::time_point());
			Assert::IsTrue(obj.delay() == milliseconds());
			Assert::IsTrue(obj.message() == defaultFoo);
		}

		void EventQueueAssignmentHelper()
		{
			EventQueue eventQueue;
			Assert::IsTrue(eventQueue.size() == 0);
			Assert::IsTrue(eventQueue.isEmpty());

			Foo foo;
			GameTime time;
			eventQueue.enqueue(*new Event<Foo>(foo), time);

			Assert::IsTrue(eventQueue.size() == 1);
			Assert::IsFalse(eventQueue.isEmpty());

			// event queue -- copy assignment
			EventQueue copiedQueue;
			Assert::IsTrue(copiedQueue.size() == 0);
			Assert::IsTrue(copiedQueue.isEmpty());

			copiedQueue = eventQueue;

			Assert::IsTrue(copiedQueue.size() == 1);
			Assert::IsFalse(copiedQueue.isEmpty());

			Assert::IsTrue(eventQueue.size() == 1);
			Assert::IsFalse(eventQueue.isEmpty());

			// event queue -- move assignment
			EventQueue movedQueue;
			Assert::IsTrue(movedQueue.size() == 0);
			Assert::IsTrue(movedQueue.isEmpty());

			movedQueue = std::move(eventQueue);

			Assert::IsTrue(movedQueue.size() == 1);
			Assert::IsFalse(movedQueue.isEmpty());

			Assert::IsTrue(eventQueue.size() == 0);
			Assert::IsTrue(eventQueue.isEmpty());
		}

		//

		static _CrtMemState sStartMemState;
	};

	_CrtMemState EventTest::sStartMemState;
}

