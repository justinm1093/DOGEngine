
#include "pch.h"
#include "CppUnitTest.h"

#include "AttributedFoo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DOGEngine;
using namespace UnitTests;
using namespace std;

namespace LibraryDesktopTest
{
	TEST_CLASS(AttributedTest)
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


		TEST_METHOD(AttributedRTTI)
		{
			AttributedFoo foo;
			RTTI* rtti = (RTTI*)(&foo);

			// test type id's
			Assert::IsTrue(AttributedFoo::TypeIdClass() == foo.TypeIdInstance());
			Assert::IsTrue(Attributed::TypeIdClass() != foo.TypeIdInstance());

			// test type name
			Assert::IsTrue(Attributed::TypeName() == "Attributed");
			Assert::IsTrue(AttributedFoo::TypeName() == "AttributedFoo");

			// test query interface
			Assert::IsTrue(rtti->QueryInterface(AttributedFoo::TypeIdClass()) != nullptr);
			Assert::IsTrue(foo.QueryInterface(Attributed::TypeIdClass()) != nullptr);

			// test is with string
			Assert::IsTrue(foo.Is("Attributed"));
			Assert::IsTrue(foo.Is("AttributedFoo"));
			Assert::IsTrue(rtti->Is("Attributed"));
			Assert::IsTrue(rtti->Is("AttributedFoo"));
			Assert::IsFalse(foo.Is("Hello"));

			// test is with int
			Assert::IsTrue(foo.Is(Attributed::TypeIdClass()));
			Assert::IsTrue(foo.Is(AttributedFoo::TypeIdClass()));
			Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
			Assert::IsTrue(rtti->Is(AttributedFoo::TypeIdClass()));
			Assert::IsFalse(foo.Is(0));

			// test as
			Attributed* asAttributed = rtti->As<Attributed>();
			AttributedFoo* asAttributedFoo = rtti->As<AttributedFoo>();
			Assert::IsTrue(asAttributed != nullptr);
			Assert::IsTrue(asAttributedFoo != nullptr);
			Assert::IsTrue(asAttributed == &foo);
			Assert::IsTrue(asAttributedFoo == &foo);

			Attributed::clearAttributeCache();
		}

		TEST_METHOD(AttributedConstructor)
		{
			// creating an Attributed subclass auto-populates the table
			AttributedFoo foo;
			Assert::IsTrue(foo.size() != 0);
			Assert::IsTrue(foo.size() == 27);

			Assert::IsTrue(foo.prescribedSize() == 27);
			Assert::IsTrue(foo.auxiliarySize() == 0);
			Assert::IsTrue(foo.auxiliaryBeginIndex() == 27);

			// testing some externally stored value
			Assert::IsTrue(&foo.mIntField == &foo["int_field"].get<int32_t>());
			foo["int_field"] = 5;
			Assert::IsTrue(foo.mIntField == 5);

			Attributed::clearAttributeCache();
		}

		TEST_METHOD(AttributedCopy)
		{
			// test copy construction
			{
				AttributedFoo foo;
				foo.addAuxiliaryAttribute("auxiliary");

				AttributedFoo otherFoo(foo);

				Assert::IsTrue(foo["this"].get<RTTI*>() != otherFoo["this"].get<RTTI*>());

				Assert::IsTrue(otherFoo.isAuxiliaryAttribute("auxiliary"));
				Assert::IsTrue(foo == otherFoo);
			}

			// test assignment operator
			{
				AttributedFoo foo;
				foo.addAuxiliaryAttribute("foo_auxiliary");

				AttributedFoo otherFoo;
				otherFoo.addAuxiliaryAttribute("otherFoo_auxiliary");

				otherFoo = foo;

				Assert::IsTrue(foo["this"].get<RTTI*>() != otherFoo["this"].get<RTTI*>());

				Assert::IsTrue(otherFoo.isAuxiliaryAttribute("foo_auxiliary"));
				Assert::IsFalse(otherFoo.isAuxiliaryAttribute("otherFoo_auxiliary"));
				Assert::IsTrue(foo == otherFoo);
			}

			Attributed::clearAttributeCache();
		}

		TEST_METHOD(AttributedMove)
		{
			// move constructor
			{
				// create original foo
				AttributedFoo foo;
				foo["int_field"] = 100;
				foo["float_field"] = 100.0f;
				foo["string_field"] = "hello";
				foo.addAuxiliaryAttribute("auxiliary") = "world";

				// copied foo (to verify)
				AttributedFoo copiedFoo(foo);

				// move original to new foo (original reverts to default AttributedFoo)
				AttributedFoo otherFoo(std::move(foo));

				Assert::IsTrue(otherFoo == copiedFoo);
				Assert::IsTrue(otherFoo != foo);

				Assert::IsFalse(foo.isAttribute("auxiliary"));
				Assert::IsTrue(otherFoo.isAttribute("auxiliary"));
				Assert::IsTrue(otherFoo.isAuxiliaryAttribute("auxiliary"));

				Assert::IsTrue(otherFoo["int_field"] == 100);
				Assert::IsTrue(otherFoo["float_field"] == 100.0f);
				Assert::IsTrue(otherFoo["string_field"] == "hello");
				Assert::IsTrue(otherFoo["auxiliary"] == "world");

				Assert::IsTrue(foo["int_field"] == 0);
				Assert::IsTrue(foo["float_field"] == 0.0f);
				Assert::IsTrue(foo["string_field"] == "");
			}

			// move assignment
			{
				// create original foo
				AttributedFoo foo;
				foo["int_field"] = 100;
				foo["float_field"] = 100.0f;
				foo["string_field"] = "hello";
				foo.addAuxiliaryAttribute("auxiliary") = "world";

				// copied foo (to verify)
				AttributedFoo copiedFoo(foo);

				// move original to new foo (original reverts to default AttributedFoo)
				AttributedFoo otherFoo;
				otherFoo["int_field"] = 200;
				otherFoo["float_field"] = 200.0f;
				otherFoo["string_field"] = "other";
				otherFoo.addAuxiliaryAttribute("other_auxiliary") = "error";

				otherFoo = std::move(foo);

				Assert::IsTrue(otherFoo == copiedFoo);
				Assert::IsTrue(otherFoo != foo);

				Assert::IsFalse(foo.isAttribute("auxiliary"));
				Assert::IsFalse(otherFoo.isAttribute("other_auxiliary"));
				Assert::IsTrue(otherFoo.isAttribute("auxiliary"));
				Assert::IsTrue(otherFoo.isAuxiliaryAttribute("auxiliary"));

				Assert::IsTrue(otherFoo["int_field"] == 100);
				Assert::IsTrue(otherFoo["float_field"] == 100.0f);
				Assert::IsTrue(otherFoo["string_field"] == "hello");
				Assert::IsTrue(otherFoo["auxiliary"] == "world");

				Assert::IsTrue(foo["int_field"] == 0);
				Assert::IsTrue(foo["float_field"] == 0.0f);
				Assert::IsTrue(foo["string_field"] == "");
			}

			Attributed::clearAttributeCache();
		}

		TEST_METHOD(AttributedDestructor)
		{
			{
				// test that the destructor does not leak memory
				AttributedFoo foo;

				foo.addAuxiliaryAttribute("hello");
				foo.addAuxiliaryAttribute("world");
				foo.addAuxiliaryAttribute("error");
			}

			Attributed::clearAttributeCache();
		}

		TEST_METHOD(AttributedEquality)
		{
			// test equality operators
			AttributedFoo foo;
			AttributedFoo otherFoo;

			Assert::IsTrue(foo == otherFoo);					// default data in each -- equal
			Assert::IsFalse(foo != otherFoo);

			foo.addAuxiliaryAttribute("auxiliary") = 100;		// auxiliary in one but not the other -- inequal
			Assert::IsFalse(foo == otherFoo);
			Assert::IsTrue(foo != otherFoo);

			otherFoo.addAuxiliaryAttribute("auxiliary") = 50;	// auxiliary in both, but different values -- inequal
			Assert::IsFalse(foo == otherFoo);
			Assert::IsTrue(foo != otherFoo);

			otherFoo["auxiliary"] = 100;						// equal data even in auxiliary -- equal
			Assert::IsTrue(foo == otherFoo);
			Assert::IsFalse(foo != otherFoo);

			Attributed::clearAttributeCache();
		}

		TEST_METHOD(AttributedAddAuxiliaryAttribute)
		{
			// adding an auxiliary attribute with no name throws exception
			AttributedFoo foo;
			auto emptyAuxiliary = [&foo]{ foo.addAuxiliaryAttribute(""); };
			Assert::ExpectException<exception>(emptyAuxiliary);

			// adding an auxiliary attribute changes the size of the Attributed
			Assert::IsTrue(foo.size() == 27);
			Assert::IsTrue(foo.prescribedSize() == 27);
			Assert::IsTrue(foo.auxiliarySize() == 0);
			Assert::IsTrue(foo.auxiliaryBeginIndex() == 27);

			foo.addAuxiliaryAttribute("auxiliary_1");
			foo.addAuxiliaryAttribute("auxiliary_2");
			foo.addAuxiliaryAttribute("auxiliary_3");

			Assert::IsTrue(foo.size() == 30);
			Assert::IsTrue(foo.prescribedSize() == 27);
			Assert::IsTrue(foo.auxiliarySize() == 3);
			Assert::IsTrue(foo.auxiliaryBeginIndex() == 27);

			Attributed::clearAttributeCache();
		}

		TEST_METHOD(AttributedIsAttribute)
		{
			AttributedFoo foo;
			foo.addAuxiliaryAttribute("auxiliary_attribute");

			// test isAttribute
			Assert::IsTrue(foo.isAttribute("this"));

			Assert::IsTrue(foo.isAttribute("internal_int"));
			Assert::IsTrue(foo.isAttribute("internal_float"));
			Assert::IsTrue(foo.isAttribute("internal_string"));
			Assert::IsTrue(foo.isAttribute("internal_vector"));
			Assert::IsTrue(foo.isAttribute("internal_matrix"));
			Assert::IsTrue(foo.isAttribute("internal_pointer"));

			Assert::IsTrue(foo.isAttribute("internal_int_array"));
			Assert::IsTrue(foo.isAttribute("internal_float_array"));
			Assert::IsTrue(foo.isAttribute("internal_string_array"));
			Assert::IsTrue(foo.isAttribute("internal_vector_array"));
			Assert::IsTrue(foo.isAttribute("internal_matrix_array"));
			Assert::IsTrue(foo.isAttribute("internal_pointer_array"));

			Assert::IsTrue(foo.isAttribute("int_field"));
			Assert::IsTrue(foo.isAttribute("float_field"));
			Assert::IsTrue(foo.isAttribute("string_field"));
			Assert::IsTrue(foo.isAttribute("vector_field"));
			Assert::IsTrue(foo.isAttribute("matrix_field"));
			Assert::IsTrue(foo.isAttribute("pointer_field"));

			Assert::IsTrue(foo.isAttribute("int_array"));
			Assert::IsTrue(foo.isAttribute("float_array"));
			Assert::IsTrue(foo.isAttribute("string_array"));
			Assert::IsTrue(foo.isAttribute("vector_array"));
			Assert::IsTrue(foo.isAttribute("matrix_array"));
			Assert::IsTrue(foo.isAttribute("pointer_array"));
			
			Assert::IsTrue(foo.isAttribute("auxiliary_attribute"));

			Assert::IsFalse(foo.isAttribute("not_attribute"));

			// test isPrescribedAttribute
			Assert::IsTrue(foo.isPrescribedAttribute("this"));

			Assert::IsTrue(foo.isPrescribedAttribute("internal_int"));
			Assert::IsTrue(foo.isPrescribedAttribute("internal_float"));
			Assert::IsTrue(foo.isPrescribedAttribute("internal_string"));
			Assert::IsTrue(foo.isPrescribedAttribute("internal_vector"));
			Assert::IsTrue(foo.isPrescribedAttribute("internal_matrix"));
			Assert::IsTrue(foo.isPrescribedAttribute("internal_pointer"));

			Assert::IsTrue(foo.isPrescribedAttribute("internal_int_array"));
			Assert::IsTrue(foo.isPrescribedAttribute("internal_float_array"));
			Assert::IsTrue(foo.isPrescribedAttribute("internal_string_array"));
			Assert::IsTrue(foo.isPrescribedAttribute("internal_vector_array"));
			Assert::IsTrue(foo.isPrescribedAttribute("internal_matrix_array"));
			Assert::IsTrue(foo.isPrescribedAttribute("internal_pointer_array"));

			Assert::IsTrue(foo.isPrescribedAttribute("int_field"));
			Assert::IsTrue(foo.isPrescribedAttribute("float_field"));
			Assert::IsTrue(foo.isPrescribedAttribute("string_field"));
			Assert::IsTrue(foo.isPrescribedAttribute("vector_field"));
			Assert::IsTrue(foo.isPrescribedAttribute("matrix_field"));
			Assert::IsTrue(foo.isPrescribedAttribute("pointer_field"));

			Assert::IsTrue(foo.isPrescribedAttribute("int_array"));
			Assert::IsTrue(foo.isPrescribedAttribute("float_array"));
			Assert::IsTrue(foo.isPrescribedAttribute("string_array"));
			Assert::IsTrue(foo.isPrescribedAttribute("vector_array"));
			Assert::IsTrue(foo.isPrescribedAttribute("matrix_array"));
			Assert::IsTrue(foo.isPrescribedAttribute("pointer_array"));

			Assert::IsFalse(foo.isPrescribedAttribute("auxiliary_attribute"));

			Assert::IsFalse(foo.isPrescribedAttribute("not_prescribed_attribute"));

			// test isAuxiliaryAttribute
			Assert::IsFalse(foo.isAuxiliaryAttribute("this"));

			Assert::IsFalse(foo.isAuxiliaryAttribute("internal_int"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("internal_float"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("internal_string"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("internal_vector"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("internal_matrix"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("internal_pointer"));

			Assert::IsFalse(foo.isAuxiliaryAttribute("internal_int_array"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("internal_float_array"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("internal_string_array"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("internal_vector_array"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("internal_matrix_array"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("internal_pointer_array"));

			Assert::IsFalse(foo.isAuxiliaryAttribute("int_field"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("float_field"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("string_field"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("vector_field"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("matrix_field"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("pointer_field"));

			Assert::IsFalse(foo.isAuxiliaryAttribute("int_array"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("float_array"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("string_array"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("vector_array"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("matrix_array"));
			Assert::IsFalse(foo.isAuxiliaryAttribute("pointer_array"));

			Assert::IsTrue(foo.isAuxiliaryAttribute("auxiliary_attribute"));

			Assert::IsFalse(foo.isAuxiliaryAttribute("not_prescribed_attribute"));

			Attributed::clearAttributeCache();
		}

	private:

		//

		static _CrtMemState sStartMemState;

	};

	_CrtMemState AttributedTest::sStartMemState;
}

