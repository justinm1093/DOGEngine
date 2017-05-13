
#include "pch.h"
#include "CppUnitTest.h"

#include "XmlParseMaster.h"
#include "XmlParseFoo.h"
#include "Scope.h"

#include "XmlParseHelperData.h"
#include "XmlParseHelperTable.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DOGEngine;
using namespace UnitTests;
using namespace std;

namespace LibraryDesktopTest
{
	TEST_CLASS(XmlParseTest)
	{
	public:

		TEST_CLASS_INITIALIZE(InitializeClass)
		{
			sXmlFilePath_1 = "../../../../../assets/xml/test.xml";
			sXmlFilePath_2 = "../../../../../assets/xml/test_2.xml";
		}

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
			XmlParseFoo::clearHandlerCaches();
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


		TEST_METHOD(XmlParseRTTI)
		{
			// rtti test for master
			{
				SharedDataFoo fooData;
				XmlParseMaster master(fooData);
				RTTI* rtti = (RTTI*)(&master);

				// test type id's
				Assert::IsTrue(XmlParseMaster::TypeIdClass() == master.TypeIdInstance());
				Assert::IsTrue(rtti->TypeIdInstance() == master.TypeIdInstance());

				// test type name
				Assert::IsTrue(XmlParseMaster::TypeName() == "XmlParseMaster");
				
				// test query interface
				Assert::IsTrue(rtti->QueryInterface(XmlParseMaster::TypeIdClass()) != nullptr);

				// test is with string
				Assert::IsTrue(master.Is("XmlParseMaster"));
				Assert::IsTrue(rtti->Is("XmlParseMaster"));

				// test is with int
				Assert::IsTrue(master.Is(XmlParseMaster::TypeIdClass()));
				Assert::IsTrue(rtti->Is(XmlParseMaster::TypeIdClass()));

				// test as
				Assert::IsTrue(rtti->As<XmlParseMaster>() != nullptr);
			}

			// rtti test for helper
			{
				XmlParseFoo foo;
				IXmlParseHelper* helper = (IXmlParseHelper*)(&foo);
				RTTI* rtti = (RTTI*)(&foo);

				// test type id's
				Assert::IsTrue(XmlParseFoo::TypeIdClass() == foo.TypeIdInstance());
				Assert::IsTrue(IXmlParseHelper::TypeIdClass() != foo.TypeIdInstance());

				Assert::IsTrue(helper->TypeIdInstance() == foo.TypeIdInstance());
				Assert::IsTrue(rtti->TypeIdInstance() == foo.TypeIdInstance());

				// test type name
				Assert::IsTrue(XmlParseFoo::TypeName() == "XmlParseFoo");
				Assert::IsTrue(IXmlParseHelper::TypeName() == "IXmlParseHelper");

				Assert::IsTrue(foo.TypeName() == "XmlParseFoo");
				Assert::IsTrue(helper->TypeName() == "IXmlParseHelper");

				// test query interface
				Assert::IsTrue(rtti->QueryInterface(XmlParseFoo::TypeIdClass()) != nullptr);
				Assert::IsTrue(helper->QueryInterface(XmlParseFoo::TypeIdClass()) != nullptr);
				Assert::IsTrue(foo.QueryInterface(IXmlParseHelper::TypeIdClass()) != nullptr);

				// test is with string
				Assert::IsTrue(foo.Is("IXmlParseHelper"));
				Assert::IsTrue(foo.Is("XmlParseFoo"));
				Assert::IsTrue(helper->Is("IXmlParseHelper"));
				Assert::IsTrue(helper->Is("XmlParseFoo"));
				Assert::IsTrue(rtti->Is("IXmlParseHelper"));
				Assert::IsTrue(rtti->Is("XmlParseFoo"));

				// test is with int
				Assert::IsTrue(foo.Is(IXmlParseHelper::TypeIdClass()));
				Assert::IsTrue(foo.Is(XmlParseFoo::TypeIdClass()));
				Assert::IsTrue(helper->Is(IXmlParseHelper::TypeIdClass()));
				Assert::IsTrue(helper->Is(XmlParseFoo::TypeIdClass()));
				Assert::IsTrue(rtti->Is(IXmlParseHelper::TypeIdClass()));
				Assert::IsTrue(rtti->Is(XmlParseFoo::TypeIdClass()));

				// test as
				Assert::IsTrue(foo.As<IXmlParseHelper>() != nullptr);
				Assert::IsTrue(helper->As<XmlParseFoo>() != nullptr);
				Assert::IsTrue(rtti->As<IXmlParseHelper>() != nullptr);
				Assert::IsTrue(rtti->As<XmlParseFoo>() != nullptr);
			}

			// rtti test for shared data
			{
				SharedDataFoo data;

				RTTI* rtti = (RTTI*)(&data);

				// test type id's
				Assert::IsTrue(SharedDataFoo::TypeIdClass() == data.TypeIdInstance());
				Assert::IsTrue(rtti->TypeIdInstance() == data.TypeIdInstance());

				// test type name
				Assert::IsTrue(SharedData::TypeName() == "SharedData");

				Assert::IsTrue(SharedDataFoo::TypeName() == "SharedDataFoo");
				Assert::IsTrue(data.TypeName() == "SharedDataFoo");

				// test query interface
				Assert::IsTrue(rtti->QueryInterface(SharedDataFoo::TypeIdClass()) != nullptr);

				// test is with string
				Assert::IsTrue(rtti->Is("SharedData"));
				Assert::IsTrue(rtti->Is("SharedDataFoo"));
				Assert::IsTrue(data.Is("SharedData"));
				
				// test is with int
				Assert::IsTrue(rtti->Is(SharedData::TypeIdClass()));
				Assert::IsTrue(rtti->Is(SharedDataFoo::TypeIdClass()));
				Assert::IsTrue(data.Is(SharedData::TypeIdClass()));
				
				// test as
				Assert::IsTrue(rtti->As<SharedData>() != nullptr);
				Assert::IsTrue(rtti->As<SharedDataFoo>() != nullptr);
			}
		}

		TEST_METHOD(XmlParseConstructor)
		{
			// constructing a parse master
			SharedDataFoo fooData;
			XmlParseMaster defaultMaster(fooData);

			Assert::IsTrue(defaultMaster.getSharedData() != nullptr);
			Assert::IsTrue(defaultMaster.getSharedData()->Is("SharedData"));

			Assert::IsTrue(defaultMaster.numHelpers() == 0);
			Assert::IsFalse(defaultMaster.isClone());
		}

		TEST_METHOD(XmlParseDestructor)
		{
			// no memory leaks, parse masters are not responsible for their own shared data
			{
				SharedDataFoo* data = new SharedDataFoo();
				XmlParseMaster master(*data);

				delete data;
			}

			// parse masters are not responsible for their helpers
			{
				SharedDataFoo fooData;
				XmlParseMaster* master = new XmlParseMaster(fooData);
				XmlParseFoo* foo = new XmlParseFoo();
				
				master->addHelper(*foo);

				delete foo;
				delete master;
			}

			// cloned parse masters are responsible for their helpers
			{
				SharedDataFoo fooData;
				XmlParseMaster* master = new XmlParseMaster(fooData);
				XmlParseFoo* foo = new XmlParseFoo();

				master->addHelper(*foo);
				XmlParseMaster* clone = master->clone();

				Assert::IsTrue(clone->numHelpers() == 1);

				delete foo;
				delete master;

				delete clone;
			}
		}

		TEST_METHOD(XmlParseClone)
		{
			// parse master clones have different shared data than the original
			SharedDataFoo fooData;
			XmlParseMaster master(fooData);

			XmlParseMaster* clone = master.clone();
			Assert::IsTrue(clone->isClone());
			Assert::IsFalse(master.isClone());

			Assert::IsTrue(master.getSharedData() != clone->getSharedData());

			delete clone;

			// cloning with a helper
			XmlParseFoo foo;
			master.addHelper(foo);

			XmlParseMaster* otherClone = master.clone();

			Assert::IsTrue(otherClone->numHelpers() == 1);
			Assert::IsTrue(otherClone->hasHelper(XmlParseFoo::TypeIdClass()));

			delete otherClone;
		}

		TEST_METHOD(XmlParseInitialize)
		{
			// initializing the parse master initializes the shared data and helpers
			SharedDataFoo* fooData = new SharedDataFoo();
			fooData->incrementNumElementsParsed();
			fooData->incrementDepth();

			Assert::IsTrue(fooData->numElementsParsed() == 1);
			Assert::IsTrue(fooData->depth() == 1);

			XmlParseMaster master(*fooData);
			master.initialize();

			Assert::IsTrue(fooData->numElementsParsed() == 0);
			Assert::IsTrue(fooData->depth() == 0);

			delete fooData;
		}

		TEST_METHOD(XmlParseAddHelper)
		{
			SharedDataFoo fooData;
			XmlParseMaster master(fooData);
			XmlParseFoo foo;

			Assert::IsTrue(master.numHelpers() == 0);
			Assert::IsFalse(master.hasHelper(XmlParseFoo::TypeIdClass()));

			// can't add a helper to a clone
			XmlParseMaster* clone = master.clone();
			Assert::IsFalse(clone->addHelper(foo));

			// adding a helper
			master.addHelper(foo);
			Assert::IsTrue(master.numHelpers() == 1);
			Assert::IsTrue(master.hasHelper(XmlParseFoo::TypeIdClass()));

			// can only add helpers of unique types (can't add another foo helper)
			XmlParseFoo otherFoo;
			Assert::IsFalse(master.addHelper(otherFoo));

			Assert::IsTrue(master.numHelpers() == 1);
			Assert::IsTrue(master.hasHelper(XmlParseFoo::TypeIdClass()));

			delete clone;
		}

		TEST_METHOD(XmlParseRemoveHelper)
		{
			// removing a helper that is not in the list returns false
			SharedDataFoo fooData;
			XmlParseMaster master(fooData);

			XmlParseFoo foo;

			Assert::IsFalse(master.removeHelper(foo));

			master.addHelper(foo);

			// can't remove a helper from a clone
			XmlParseMaster* clone = master.clone();
			Assert::IsFalse(clone->removeHelper(foo));

			// removing a helper whose address is in the parse master removes it
			Assert::IsTrue(master.numHelpers() == 1);
			Assert::IsTrue(master.hasHelper(XmlParseFoo::TypeIdClass()));

			Assert::IsTrue(master.removeHelper(foo));

			Assert::IsTrue(master.numHelpers() == 0);
			Assert::IsFalse(master.hasHelper(XmlParseFoo::TypeIdClass()));

			// removing a helper of the same type as one in the list does not remove it
			XmlParseFoo otherFoo;
			master.addHelper(foo);

			Assert::IsTrue(master.numHelpers() == 1);
			Assert::IsTrue(master.hasHelper(XmlParseFoo::TypeIdClass()));

			Assert::IsFalse(master.removeHelper(otherFoo));

			Assert::IsTrue(master.numHelpers() == 1);
			Assert::IsTrue(master.hasHelper(XmlParseFoo::TypeIdClass()));

			delete clone;
		}

		TEST_METHOD(XmlParseParse)
		{
			SharedDataFoo fooData;
			XmlParseMaster master(fooData);

			// parsing with no buffer throws exception
			auto parseNullBuffer = [&master]{ master.parse(nullptr, 0, true); };
			Assert::ExpectException<exception>(parseNullBuffer);

			// parsing with null shared data throws exception
			XmlParseMaster otherMaster(fooData);
			auto parseNullData = [&master]{ master.parse(nullptr, 0, true); };
			Assert::ExpectException<exception>(parseNullData);

			// parsing from file with bad filepath returns false
			master.setSharedData(fooData);
			Assert::IsFalse(master.parseFromFile("hello.xml"));

			// parse from file with foo helper
			XmlParseFoo foo;
			master.addHelper(foo);

			Assert::IsTrue(master.parseFromFile(sXmlFilePath_1));
			{
				Assert::IsTrue(fooData.numElementsParsed() == 7);
				Scope& fooTable = fooData.getTable();

				Assert::IsTrue(fooTable["key"] == "value");

				Assert::IsTrue(fooTable["People"].size() == 2);
				Scope& firstPerson = fooTable["People"][0];
				Scope& secondPerson = fooTable["People"][1];

				Assert::IsTrue(firstPerson["firstname"] == "Justin");
				Assert::IsTrue(firstPerson["lastname"] == "Moser");
				Assert::IsTrue(firstPerson["age"] == 23);

				Assert::IsTrue(secondPerson["firstname"] == "Brandon");
				Assert::IsTrue(secondPerson["lastname"] == "Moser");
				Assert::IsTrue(secondPerson["age"] == 19);
			}

			// verify that parsing successfully resets the state before performing its operation (would return false if it didn't)
			Assert::IsTrue(master.parseFromFile(sXmlFilePath_2));
			{
				Assert::IsTrue(fooData.numElementsParsed() == 3);
				Scope& fooTable = fooData.getTable();

				Assert::IsTrue(fooTable["People"].size() == 1);
				Scope& person = fooTable["People"][0];

				Assert::IsTrue(person["firstname"] == "Steals");
				Assert::IsTrue(person["lastname"] == "McGee");
				Assert::IsTrue(person["profession"] == "Thief");
			}
		}

		TEST_METHOD(XmlParseSetSharedData)
		{
			// new shared data have default state and no owner
			SharedDataFoo sharedData_1;
			SharedDataFoo sharedData_2;

			Assert::IsTrue(sharedData_1.getXmlParseMaster() == nullptr);
			Assert::IsTrue(sharedData_2.getXmlParseMaster() == nullptr);

			Assert::IsTrue(sharedData_1.numElementsParsed() == 0);
			Assert::IsTrue(sharedData_2.numElementsParsed() == 0);

			Assert::IsTrue(sharedData_1.depth() == 0);
			Assert::IsTrue(sharedData_2.depth() == 0);

			// new parse masters must have shared data when created
			XmlParseMaster master_1(sharedData_1);
			XmlParseMaster master_2(sharedData_2);

			Assert::IsTrue(master_1.getSharedData() == &sharedData_1);
			Assert::IsTrue(master_2.getSharedData() == &sharedData_2);

			Assert::IsTrue(sharedData_1.getXmlParseMaster() == &master_1);
			Assert::IsTrue(sharedData_2.getXmlParseMaster() == &master_2);

			// setting owned shared data on parse master changes data's owner
			master_1.setSharedData(sharedData_2);

			Assert::IsTrue(sharedData_2.getXmlParseMaster() == &master_1);
			Assert::IsTrue(master_1.getSharedData() == &sharedData_2);

			Assert::IsTrue(master_2.getSharedData() == nullptr);
			Assert::IsTrue(sharedData_1.getXmlParseMaster() == nullptr);

			// clones delete their shared data when setting
			XmlParseMaster otherMaster(sharedData_1);
			XmlParseMaster* clone = otherMaster.clone();

			clone->setSharedData(*new SharedDataFoo());
			delete clone;
		}

		TEST_METHOD(XmlParseGetHelpers)
		{
			SharedDataFoo fooData;
			XmlParseMaster master(fooData);

			{
				XmlParseFoo* helperFoo = new XmlParseFoo();
				XmlParseHelperData* helperData = new XmlParseHelperData();
				XmlParseHelperTable* helperTable = new XmlParseHelperTable();
				master.addHelper(*helperFoo);
				master.addHelper(*helperData);
				master.addHelper(*helperTable);
			}

			Vector<IXmlParseHelper*> helpers = master.getHelpers();
			for(auto& helper : helpers)
			{
				delete helper;
			}
		}

	private:

		//

		static _CrtMemState sStartMemState;
		static string sXmlFilePath_1;
		static string sXmlFilePath_2;

	};

	_CrtMemState XmlParseTest::sStartMemState;
	string XmlParseTest::sXmlFilePath_1;
	string XmlParseTest::sXmlFilePath_2;
}

