
#include "pch.h"
#include "CppUnitTest.h"

#include "Foo.h"
#include "SList.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DOGEngine;
using namespace UnitTests;
using namespace std;

namespace LibraryDesktopTest
{
	TEST_CLASS(SListIteratorTest)
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

		TEST_METHOD(SListIteratorConstructors)
		{
			// verify that a default constructed Iterator has no owner
			//		default-constructed Iterator have no Node but also have no owner, 
			//		so we expect it to be different than an Iterator with an owner and no Node

			// verify that a copy constructed Iterator will point to a node

			// test with primitive type
			SList<int32_t> primitiveList;
			
			SList<int32_t>::Iterator defaultPrimitiveIter;
			Assert::IsTrue(defaultPrimitiveIter != primitiveList.end());

			primitiveList.pushFront(1);
			SList<int32_t>::Iterator copyPrimitiveIter = primitiveList.begin();
			Assert::IsTrue(*copyPrimitiveIter == primitiveList.front());

			// test with custom type
			SList<Foo> customList;

			SList<Foo>::Iterator defaultCustomIter;
			Assert::IsTrue(defaultCustomIter != customList.end());

			Foo foo(1);
			customList.pushFront(foo);
			SList<Foo>::Iterator copyCustomIter = customList.begin();
			Assert::IsTrue(*copyCustomIter == customList.front());

			// test with pointer type
			SList<Foo*> pointerList;

			SList<Foo*>::Iterator defaultPointerIter;
			Assert::IsTrue(defaultPointerIter != pointerList.end());

			pointerList.pushFront(&foo);
			SList<Foo*>::Iterator copyPointerIter = pointerList.begin();
			Assert::IsTrue(*copyPointerIter == pointerList.front());
		}

		TEST_METHOD(SListIteratorAssignmentOperator)
		{
			// verify that assigning one Iterator to another Iterator sets the owner and node

			// test with primitive type	
			SList<int32_t> primitiveList;
			primitiveList.pushFront(1);

			SList<int32_t>::Iterator primitiveIter = primitiveList.begin();
			SList<int32_t>::Iterator otherPrimitiveIter;

			otherPrimitiveIter = primitiveIter;
			Assert::IsTrue(*otherPrimitiveIter == primitiveList.front());

			// test with custom type
			Foo foo(1);
			SList<Foo> customList;
			customList.pushFront(foo);

			SList<Foo>::Iterator customIter = customList.begin();
			SList<Foo>::Iterator otherCustomIter;

			otherCustomIter = customIter;
			Assert::IsTrue(*otherCustomIter == customList.front());

			// test with pointer type
			SList<Foo*> pointerList;
			pointerList.pushFront(&foo);

			SList<Foo*>::Iterator pointerIter = pointerList.begin();
			SList<Foo*>::Iterator otherPointerIter;

			otherPointerIter = pointerIter;
			Assert::IsTrue(*otherPointerIter == pointerList.front());
		}

		TEST_METHOD(SListIteratorEqualityOperators)
		{
			// verify that equality operator returns true only if both owner and node are the same
			// verify that inequality operator returns true if either owner and node are different

			// test with primitive type
			SList<int32_t> primitiveList;
			SList<int32_t> otherPrimitiveList;
			primitiveList.pushFront(1);
			otherPrimitiveList.pushFront(1);

			SList<int32_t>::Iterator primitiveIter = primitiveList.end();				// differnet owners, same Node are not equivalent
			SList<int32_t>::Iterator otherPrimitiveIter = otherPrimitiveList.end();
			Assert::IsFalse(primitiveIter == otherPrimitiveIter);
			Assert::IsTrue(primitiveIter != otherPrimitiveIter);

			primitiveIter = primitiveList.begin();										// different owners, different Nodes are not equivalent
			Assert::IsFalse(primitiveIter == otherPrimitiveIter);
			Assert::IsTrue(primitiveIter != otherPrimitiveIter);

			otherPrimitiveIter = primitiveList.end();									// same owners, different Nodes are not equivalent
			Assert::IsFalse(primitiveIter == otherPrimitiveIter);
			Assert::IsTrue(primitiveIter != otherPrimitiveIter);

			otherPrimitiveIter = primitiveList.begin();									// same owners, same Nodes are equivalent
			Assert::IsTrue(primitiveIter == otherPrimitiveIter);
			Assert::IsFalse(primitiveIter != otherPrimitiveIter);

			// test with custom type
			Foo foo(1);
			SList<Foo> customList;
			SList<Foo> otherCustomList;
			customList.pushFront(foo);
			otherCustomList.pushFront(foo);

			SList<Foo>::Iterator customIter = customList.end();						// differnet owners, same Node are not equivalent
			SList<Foo>::Iterator otherCustomIter = otherCustomList.end();
			Assert::IsFalse(customIter == otherCustomIter);
			Assert::IsTrue(customIter != otherCustomIter);

			customIter = customList.begin();										// different owners, different Nodes are not equivalent
			Assert::IsFalse(customIter == otherCustomIter);
			Assert::IsTrue(customIter != otherCustomIter);

			otherCustomIter = customList.end();										// same owners, different Nodes are not equivalent
			Assert::IsFalse(customIter == otherCustomIter);
			Assert::IsTrue(customIter != otherCustomIter);

			otherCustomIter = customList.begin();									// same owners, same Nodes are equivalent
			Assert::IsTrue(customIter == otherCustomIter);
			Assert::IsFalse(customIter != otherCustomIter);

			// test with pointer type
			SList<Foo*> pointerList;
			SList<Foo*> otherPointerList;
			pointerList.pushFront(&foo);
			otherPointerList.pushFront(&foo);

			SList<Foo*>::Iterator pointerIter = pointerList.end();					// differnet owners, same Node are not equivalent
			SList<Foo*>::Iterator otherPointerIter = otherPointerList.end();
			Assert::IsFalse(pointerIter == otherPointerIter);
			Assert::IsTrue(pointerIter != otherPointerIter);

			pointerIter = pointerList.begin();										// different owners, different Nodes are not equivalent
			Assert::IsFalse(pointerIter == otherPointerIter);
			Assert::IsTrue(pointerIter != otherPointerIter);

			otherPointerIter = pointerList.end();									// same owners, different Nodes are not equivalent
			Assert::IsFalse(pointerIter == otherPointerIter);
			Assert::IsTrue(pointerIter != otherPointerIter);

			otherPointerIter = pointerList.begin();									// same owners, same Nodes are equivalent
			Assert::IsTrue(pointerIter == otherPointerIter);
			Assert::IsFalse(pointerIter != otherPointerIter);
		}

		TEST_METHOD(SListIteratorIncrementOperators)
		{
			// verify that prefix increment returns the changed Iterator
			// verify that postfix increment returns the unchanged Iterator but changes the invoking Iterator
			// verify that any increment operator on an Iterator with null Node is a noop

			// test with primitive type
			SList<int32_t> primitiveList;
			primitiveList.pushBack(1);
			primitiveList.pushBack(2);
			SList<int32_t>::Iterator primitiveIter = primitiveList.begin();

			Assert::IsTrue(*(primitiveIter++) == primitiveList.front());	// post increment returns original value
			Assert::IsTrue(*primitiveIter == primitiveList.back());			// but changes the invoking Iterator

			Assert::IsTrue(++primitiveIter == primitiveList.end());			// pre-increment returns changed value

			Assert::IsTrue(++primitiveIter == primitiveList.end());			// increments on end() are noop
			Assert::IsTrue(primitiveIter++ == primitiveList.end());

			// test with custom type
			Foo foo_1(1), foo_2(2);
			SList<Foo> customList;
			customList.pushBack(foo_1);
			customList.pushBack(foo_2);
			SList<Foo>::Iterator customIter = customList.begin();

			Assert::IsTrue(*(customIter++) == customList.front());		// post increment returns original value
			Assert::IsTrue(*customIter == customList.back());			// but changes the invoking Iterator

			Assert::IsTrue(++customIter == customList.end());			// pre-increment returns changed value

			Assert::IsTrue(++customIter == customList.end());			// increments on end() are noop
			Assert::IsTrue(customIter++ == customList.end());

			// test with pointer type
			SList<Foo*> pointerList;
			pointerList.pushBack(&foo_1);
			pointerList.pushBack(&foo_2);
			SList<Foo*>::Iterator pointerIter = pointerList.begin();

			Assert::IsTrue(*(pointerIter++) == pointerList.front());	// post increment returns original value
			Assert::IsTrue(*pointerIter == pointerList.back());			// but changes the invoking Iterator

			Assert::IsTrue(++pointerIter == pointerList.end());			// pre-increment returns changed value

			Assert::IsTrue(++pointerIter == pointerList.end());			// increments on end() are noop
			Assert::IsTrue(pointerIter++ == pointerList.end());
		}

		TEST_METHOD(SListIteratorDereferenceOperator)
		{
			// verify that dereferencing Iterator pointing to null throws exception
			// verify that dereferencing valid Iterator returns Node's value

			// test with primitve type
			SList<int32_t> primitiveList;
			primitiveList.pushBack(1);

			auto primitiveIterDeref = [&primitiveList]{ *(primitiveList.end()); };
			Assert::ExpectException<exception>(primitiveIterDeref);

			SList<int32_t>::Iterator primitiveIter = primitiveList.begin();
			Assert::IsTrue(*primitiveIter == primitiveList.front());

			const SList<int32_t>& primitiveListRef = primitiveList;
			auto constPrimitiveIterDeref = [&primitiveListRef]{ *(primitiveListRef.end()); };
			Assert::ExpectException<exception>(constPrimitiveIterDeref);

			const SList<int32_t>::Iterator constPrimitiveIter = primitiveListRef.begin();
			Assert::IsTrue(*constPrimitiveIter == primitiveListRef.front());

			// test with custom type
			Foo foo(1);
			SList<Foo> customList;
			customList.pushBack(foo);

			auto customIterDeref = [&customList]{ *(customList.end()); };
			Assert::ExpectException<exception>(customIterDeref);

			SList<Foo>::Iterator customIter = customList.begin();
			Assert::IsTrue(*customIter == customList.front());

			const SList<Foo>& customListRef = customList;
			auto constCustomIterDeref = [&customListRef]{ *(customListRef.end()); };
			Assert::ExpectException<exception>(constCustomIterDeref);

			const SList<Foo>::Iterator constCustomIter = customListRef.begin();
			Assert::IsTrue(*constCustomIter == customListRef.front());

			// test with pointer type
			SList<Foo*> pointerList;
			pointerList.pushBack(&foo);

			auto pointerIterDeref = [&pointerList]{ *(pointerList.end()); };
			Assert::ExpectException<exception>(pointerIterDeref);

			SList<Foo*>::Iterator pointerIter = pointerList.begin();
			Assert::IsTrue(*pointerIter == pointerList.front());

			const SList<Foo*>& pointerListRef = pointerList;
			auto constPointerIterDeref = [&pointerListRef]{ *(pointerListRef.end()); };
			Assert::ExpectException<exception>(constPointerIterDeref);

			const SList<Foo*>::Iterator constPointerIter = pointerListRef.begin();
			Assert::IsTrue(*constPointerIter == pointerListRef.front());
		}

	private:

		static _CrtMemState sStartMemState;

	};

	_CrtMemState SListIteratorTest::sStartMemState;
}
