
#include "pch.h"
#include "CppUnitTest.h"

#include "Factory.h"

#include "Foo.h"
#include "XmlParseFoo.h"
#include "AttributedFoo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace UnitTests;
using namespace DOGEngine;
using namespace std;
using namespace glm;

namespace LibraryDesktopTest
{
	TEST_CLASS(FactoryTest)
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


		TEST_METHOD(FactoryFind)
		{
			AttributedFoo::AttributedFooFactory* attributedFooFactory = new AttributedFoo::AttributedFooFactory();
			SharedDataFoo::SharedDataFooFactory* sharedFooFactory = new SharedDataFoo::SharedDataFooFactory();
			XmlParseFoo::XmlParseFooFactory* parseFooFactory = new XmlParseFoo::XmlParseFooFactory();

			// find returns registered factories
			Factory<RTTI>* foundFactory = Factory<RTTI>::find("AttributedFoo");
			Assert::IsTrue(foundFactory->getProductName() == "AttributedFoo");
			Assert::IsTrue(foundFactory->isRegistered());
			Assert::IsTrue(foundFactory != nullptr);

			foundFactory = Factory<RTTI>::find("SharedDataFoo");
			Assert::IsTrue(foundFactory->getProductName() == "SharedDataFoo");
			Assert::IsTrue(foundFactory->isRegistered());
			Assert::IsTrue(foundFactory != nullptr);

			foundFactory = Factory<RTTI>::find("XmlParseFoo");
			Assert::IsTrue(foundFactory->getProductName() == "XmlParseFoo");
			Assert::IsTrue(foundFactory->isRegistered());
			Assert::IsTrue(foundFactory != nullptr);

			// finding an unregistered factory returns nullptr
			delete attributedFooFactory;
			delete sharedFooFactory;
			delete parseFooFactory;

			foundFactory = Factory<RTTI>::find("AttributedFoo");
			Assert::IsTrue(foundFactory == nullptr);

			foundFactory = Factory<RTTI>::find("SharedDataFoo");
			Assert::IsTrue(foundFactory == nullptr);

			foundFactory = Factory<RTTI>::find("XmlParseFoo");
			Assert::IsTrue(foundFactory == nullptr);

			foundFactory = Factory<RTTI>::find("Foo");
			Assert::IsTrue(foundFactory == nullptr);

			// finding a concrete factory with a duplicate returns the registered one
			Foo::FooFactory* fooFactory = new Foo::FooFactory();
			Foo::FooFactory* otherFooFactory = new Foo::FooFactory();

			Assert::IsTrue(fooFactory->isRegistered());
			Assert::IsFalse(otherFooFactory->isRegistered());

			Assert::IsTrue(fooFactory == reinterpret_cast<Foo::FooFactory*>(Factory<RTTI>::find("Foo")));
			Assert::IsTrue(otherFooFactory != reinterpret_cast<Foo::FooFactory*>(Factory<RTTI>::find("Foo")));

			// deleting the registered instance but not the unregistered one will cause find to return null
			delete fooFactory;

			foundFactory = Factory<RTTI>::find("Foo");
			Assert::IsTrue(foundFactory == nullptr);

			delete otherFooFactory;
		}

		TEST_METHOD(FactoryCreate)
		{
			AttributedFoo::AttributedFooFactory attributedFooFactory;
			SharedDataFoo::SharedDataFooFactory sharedFooFactory;
			XmlParseFoo::XmlParseFooFactory parseFooFactory;

			// can create directly from factories
			RTTI* obj = attributedFooFactory.create();
			Assert::IsTrue(obj->Is(AttributedFoo::TypeIdClass()));
			delete obj;

			obj = sharedFooFactory.create();
			Assert::IsTrue(obj->Is(SharedDataFoo::TypeIdClass()));
			delete obj;

			obj = parseFooFactory.create();
			Assert::IsTrue(obj->Is(XmlParseFoo::TypeIdClass()));
			delete obj;

			// can create using factory's static method and a class name
			obj = Factory<RTTI>::create("AttributedFoo");
			Assert::IsTrue(obj->Is(AttributedFoo::TypeIdClass()));
			delete obj;

			obj = Factory<RTTI>::create("SharedDataFoo");
			Assert::IsTrue(obj->Is(SharedDataFoo::TypeIdClass()));
			delete obj;

			obj = Factory<RTTI>::create("XmlParseFoo");
			Assert::IsTrue(obj->Is(XmlParseFoo::TypeIdClass()));
			delete obj;

			// create using an unregistered type returns nullptr
			obj = Factory<RTTI>::create("Foo");
			Assert::IsTrue(obj == nullptr);

			// cleanup -- we created some stuff that allocates stuff statically
			Attributed::clearAttributeCache();
			XmlParseFoo::clearHandlerCaches();
		}

		TEST_METHOD(FactoryIterators)
		{
			{
				AttributedFoo::AttributedFooFactory* attributedFooFactory = new AttributedFoo::AttributedFooFactory();
				SharedDataFoo::SharedDataFooFactory* sharedFooFactory = new SharedDataFoo::SharedDataFooFactory();
				XmlParseFoo::XmlParseFooFactory* parseFooFactory = new XmlParseFoo::XmlParseFooFactory();
				Foo::FooFactory* fooFactory = new Foo::FooFactory();

				attributedFooFactory; sharedFooFactory; parseFooFactory; fooFactory;
			}

			uint32_t expectedNumFactories = Factory<RTTI>::numFactories();
			Assert::IsTrue(expectedNumFactories == 4);

			// can use the iterators to step through the abstract factory's contents
			//		such as for clearing the abstract factory's contents
			while(Factory<RTTI>::begin() != Factory<RTTI>::end())
			{
				Assert::IsTrue(expectedNumFactories == Factory<RTTI>::numFactories());
				delete Factory<RTTI>::begin()->second;

				--expectedNumFactories;
				Assert::IsTrue(expectedNumFactories == Factory<RTTI>::numFactories());
			}
		}

		TEST_METHOD(FactoryGetFactories)
		{
			{
				AttributedFoo::AttributedFooFactory* attributedFooFactory = new AttributedFoo::AttributedFooFactory();
				SharedDataFoo::SharedDataFooFactory* sharedFooFactory = new SharedDataFoo::SharedDataFooFactory();
				XmlParseFoo::XmlParseFooFactory* parseFooFactory = new XmlParseFoo::XmlParseFooFactory();
				Foo::FooFactory* fooFactory = new Foo::FooFactory();

				attributedFooFactory; sharedFooFactory; parseFooFactory; fooFactory;
			}

			HashMap<string, Factory<RTTI>*> factories = Factory<RTTI>::getFactories();
			for(auto& kvPair : factories)
			{
				delete kvPair.second;
			}
		}

		TEST_METHOD(FactoryNumFactories)
		{
			// at the start, the abstract factory has no size
			Assert::IsTrue(Factory<RTTI>::numFactories() == 0);

			// creating factories adds them to the abstract factory
			Foo::FooFactory* fooFactory = new Foo::FooFactory();
			Assert::IsTrue(Factory<RTTI>::numFactories() == 1);

			// destroying factories removes them from the abstract factory
			delete fooFactory;
			Assert::IsTrue(Factory<RTTI>::numFactories() == 0);
		}

		TEST_METHOD(FactoryProductName)
		{
			// all unit test foo types have factories with rtti as an abstract base type
			AttributedFoo::AttributedFooFactory attributedFooFactory;
			SharedDataFoo::SharedDataFooFactory sharedFooFactory;
			XmlParseFoo::XmlParseFooFactory parseFooFactory;
			Foo::FooFactory fooFactory;

			Assert::IsTrue(attributedFooFactory.isRegistered());
			Assert::IsTrue(sharedFooFactory.isRegistered());
			Assert::IsTrue(parseFooFactory.isRegistered());
			Assert::IsTrue(fooFactory.isRegistered());

			Assert::IsTrue(attributedFooFactory.getProductName() == "AttributedFoo");
			Assert::IsTrue(sharedFooFactory.getProductName() == "SharedDataFoo");
			Assert::IsTrue(parseFooFactory.getProductName() == "XmlParseFoo");
			Assert::IsTrue(fooFactory.getProductName() == "Foo");
		}

		TEST_METHOD(FactoryIsRegistered)
		{
			{
				Assert::IsTrue(Factory<RTTI>::numFactories() == 0);

				// creating a factory registers it with the abstract factory
				//		we can ask the factory if it is registered
				Foo::FooFactory* fooFactory = new Foo::FooFactory();
				Assert::IsTrue(fooFactory->isRegistered());

				Assert::IsTrue(Factory<RTTI>::numFactories() == 1);

				// creating another factory of an already registered type does not register the new factory
				//		the abstract factory's size does not increase, the contents of abstract factory do not change
				Foo::FooFactory* otherFooFactory = new Foo::FooFactory();
				Assert::IsFalse(otherFooFactory->isRegistered());

				Assert::IsTrue(Factory<RTTI>::numFactories() == 1);

				Foo::FooFactory* foundFactory = reinterpret_cast<Foo::FooFactory*>(Factory<RTTI>::find(otherFooFactory->getProductName()));
				Assert::IsTrue(otherFooFactory != foundFactory);
				Assert::IsTrue(fooFactory == foundFactory);

				// deleting an unregistered factory does not change the abstract factory's size or contents
				delete otherFooFactory;
				Assert::IsTrue(Factory<RTTI>::numFactories() == 1);

				foundFactory = reinterpret_cast<Foo::FooFactory*>(Factory<RTTI>::find(otherFooFactory->getProductName()));
				Assert::IsTrue(otherFooFactory != foundFactory);
				Assert::IsTrue(fooFactory == foundFactory);

				// deleting a registered factory does change the abstract factory's size
				delete fooFactory;
				Assert::IsTrue(Factory<RTTI>::numFactories() == 0);
			}

			{
				// all unit test foo types have factories with rtti as an abstract base type
				AttributedFoo::AttributedFooFactory attributedFooFactory;
				SharedDataFoo::SharedDataFooFactory sharedFooFactory;
				XmlParseFoo::XmlParseFooFactory parseFooFactory;
				Foo::FooFactory fooFactory;

				Assert::IsTrue(attributedFooFactory.isRegistered());
				Assert::IsTrue(sharedFooFactory.isRegistered());
				Assert::IsTrue(parseFooFactory.isRegistered());
				Assert::IsTrue(fooFactory.isRegistered());
			}
		}

	private:

		//

		static _CrtMemState sStartMemState;
	};

	_CrtMemState FactoryTest::sStartMemState;
}

