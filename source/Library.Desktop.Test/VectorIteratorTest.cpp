
#include "pch.h"
#include "CppUnitTest.h"

#include "Foo.h"
#include "Vector.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DOGEngine;
using namespace UnitTests;
using namespace std;

namespace LibraryDesktopTest
{
	TEST_CLASS(VectorIteratorTest)
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

		TEST_METHOD(VectorIteratorConstructors)
		{
			// verify that default-constructed Iterators don't point to anything
			// verify that a copy-constructed Iterator points to something

			// test with primitive type
			Vector<int32_t> primitiveVector;

			Vector<int32_t>::Iterator defaultPrimitiveIter;
			Assert::IsTrue(primitiveVector.end() != defaultPrimitiveIter);

			primitiveVector.pushBack(1);
			Vector<int32_t>::Iterator copyPrimitiveIter = primitiveVector.begin();
			Assert::IsTrue(*copyPrimitiveIter == primitiveVector[0]);

			// test with custom type
			Vector<Foo> customVector;

			Vector<Foo>::Iterator defaultCustomIter;
			Assert::IsTrue(customVector.end() != defaultCustomIter);

			Foo foo(1);
			customVector.pushBack(foo);
			Vector<Foo>::Iterator copyCustomIter = customVector.begin();
			Assert::IsTrue(*copyCustomIter == customVector[0]);

			// test with pointer type
			Vector<Foo*> pointerVector;

			Vector<Foo*>::Iterator defaultPointerIter;
			Assert::IsTrue(pointerVector.end() != defaultPointerIter);

			pointerVector.pushBack(&foo);
			Vector<Foo*>::Iterator copyPointerIter = pointerVector.begin();
			Assert::IsTrue(*copyPointerIter == pointerVector[0]);
		}

		TEST_METHOD(VectorIteratorAssignmentOperator)
		{
			// verify that using the assignment operator sets an Iterator's owner and data position

			// test with primitive type
			Vector<int32_t> primitiveVector;
			primitiveVector.pushBack(1);

			Vector<int32_t>::Iterator primitiveIter = primitiveVector.begin();
			Vector<int32_t>::Iterator otherPrimitiveIter;
			otherPrimitiveIter = primitiveIter;

			Assert::IsTrue(*otherPrimitiveIter == primitiveVector[0]);

			// test with custom type
			Vector<Foo> customVector;
			Foo foo;
			customVector.pushBack(foo);

			Vector<Foo>::Iterator customIter = customVector.begin();
			Vector<Foo>::Iterator otherCustomIter;
			otherCustomIter = customIter;

			Assert::IsTrue(*otherCustomIter == customVector[0]);

			// test with pointer type
			Vector<Foo*> pointerVector;
			pointerVector.pushBack(&foo);

			Vector<Foo*>::Iterator pointerIter = pointerVector.begin();
			Vector<Foo*>::Iterator otherPointerIter;
			otherPointerIter = pointerIter;

			Assert::IsTrue(*otherPointerIter == pointerVector[0]);
		}

		TEST_METHOD(VectorIteratorEqualityOperators)
		{
			// verify that equality operator returns true if both owner and data address are the same
			// verify that inequality operator returns true if either owner or data address are different

			// test with primitive type
			Vector<int32_t> primitiveVector;
			Vector<int32_t> otherPrimitiveVector;
			primitiveVector.pushBack(1);
			otherPrimitiveVector.pushBack(1);

			Vector<int32_t>::Iterator primitiveIter = primitiveVector.begin();				// different owners are not equivalent
			Vector<int32_t>::Iterator otherPrimitiveIter = otherPrimitiveVector.begin();	//		(even if data is the same)
			Assert::IsFalse(primitiveIter == otherPrimitiveIter);
			Assert::IsTrue(primitiveIter != otherPrimitiveIter);

			otherPrimitiveIter = primitiveVector.end();										// same owner, different data address are not equivalent
			Assert::IsFalse(primitiveIter == otherPrimitiveIter);
			Assert::IsTrue(primitiveIter != otherPrimitiveIter);

			otherPrimitiveIter = primitiveVector.begin();									// same owner, same data address are equivalent
			Assert::IsTrue(primitiveIter == otherPrimitiveIter);
			Assert::IsFalse(primitiveIter != otherPrimitiveIter);

			// test with custom type
			Vector<Foo> customVector;
			Vector<Foo> otherCustomVector;

			Foo foo(1);
			customVector.pushBack(foo);
			otherCustomVector.pushBack(foo);

			Vector<Foo>::Iterator customIter = customVector.begin();				// different owners are not equivalent
			Vector<Foo>::Iterator otherCustomIter = otherCustomVector.begin();		//		(can't really have same data address here)
			Assert::IsFalse(customIter == otherCustomIter);
			Assert::IsTrue(customIter != otherCustomIter);

			otherCustomIter = customVector.end();									// same owner, different data address are not equivalent
			Assert::IsFalse(customIter == otherCustomIter);
			Assert::IsTrue(customIter != otherCustomIter);

			otherCustomIter = customVector.begin();									// same owner, same data address are equivalent
			Assert::IsTrue(customIter == otherCustomIter);
			Assert::IsFalse(customIter != otherCustomIter);

			// test with pointer type
			Vector<Foo*> pointerVector;
			Vector<Foo*> otherPointerVector;
			pointerVector.pushBack(&foo);
			otherPointerVector.pushBack(&foo);

			Vector<Foo*>::Iterator pointerIter = pointerVector.begin();				// different owners are not equivalent
			Vector<Foo*>::Iterator otherPointerIter = otherPointerVector.begin();	//		(can't really have same data address here)
			Assert::IsFalse(pointerIter == otherPointerIter);
			Assert::IsTrue(pointerIter != otherPointerIter);

			otherPointerIter = pointerVector.end();									// same owner, different data address are not equivalent
			Assert::IsFalse(pointerIter == otherPointerIter);
			Assert::IsTrue(pointerIter != otherPointerIter);

			otherPointerIter = pointerVector.begin();								// same owner, same data address are equivalent
			Assert::IsTrue(pointerIter == otherPointerIter);
			Assert::IsFalse(pointerIter != otherPointerIter);
		}

		TEST_METHOD(VectorIteratorIncrementOperators)
		{
			// verify that prefix increment returns the changed Iterator
			// verify that postfix increment returns the unchanged Iterator but changes the Iterator
			// verify that we cannot increment beyond the end of a Vector

			// test with primitive type
			Vector<int32_t> primitiveVector;
			primitiveVector.pushBack(1);
			primitiveVector.pushBack(2);
			Vector<int32_t>::Iterator primitiveIter = primitiveVector.begin();

			Assert::IsTrue(*(primitiveIter++) == primitiveVector.front());			// post increment returns unchanged Iterator
			Assert::IsTrue(*primitiveIter == primitiveVector.back());				// but changes the invoking Iterator

			Vector<int32_t>::Iterator primitiveEndIter = primitiveVector.end();
			Assert::IsTrue(++primitiveIter == primitiveEndIter);					// pre increment returns the changed Iterator

			Assert::IsTrue(primitiveIter++ == primitiveEndIter);					// we cannot increment beyond end()
			Assert::IsTrue(primitiveIter == primitiveEndIter);

			primitiveIter = primitiveEndIter;
			Assert::IsTrue(++primitiveIter == primitiveEndIter);

			// test with custom type
			Vector<Foo> customVector;

			Foo foo_1(1), foo_2(2);
			customVector.pushBack(foo_1);
			customVector.pushBack(foo_2);
			Vector<Foo>::Iterator customIter = customVector.begin();

			Assert::IsTrue(*(customIter++) == customVector.front());			// post increment returns unchanged Iterator
			Assert::IsTrue(*customIter == customVector.back());					// but changes the invoking Iterator

			Vector<Foo>::Iterator customEndIter = customVector.end();
			Assert::IsTrue(++customIter == customEndIter);						// pre increment returns the changed Iterator

			Assert::IsTrue(customIter++ == customEndIter);						// we cannot increment beyond end()
			Assert::IsTrue(customIter == customEndIter);

			customIter = customEndIter;
			Assert::IsTrue(++customIter == customEndIter);

			// test with pointer type
			Vector<Foo*> pointerVector;

			pointerVector.pushBack(&foo_1);
			pointerVector.pushBack(&foo_2);
			Vector<Foo*>::Iterator pointerIter = pointerVector.begin();

			Assert::IsTrue(*(pointerIter++) == pointerVector.front());			// post increment returns unchanged Iterator
			Assert::IsTrue(*pointerIter == pointerVector.back());				// but changes the invoking Iterator

			Vector<Foo*>::Iterator pointerEndIter = pointerVector.end();
			Assert::IsTrue(++pointerIter == pointerEndIter);					// pre increment returns the changed Iterator

			Assert::IsTrue(pointerIter++ == pointerEndIter);					// we cannot increment beyond end()
			Assert::IsTrue(pointerIter == pointerEndIter);

			pointerIter = pointerEndIter;
			Assert::IsTrue(++pointerIter == pointerEndIter);
		}

		TEST_METHOD(VectorIteratorDereferenceOperators)
		{
			// verify that dereferencing an ownerless Iterator throws an exception
			// verify that dereferencing an Iterator out-of-bounds throws an exception
			// verify that dereferencing a valid Iterator returns a result

			// test with primitive type
			Vector<int32_t> primitiveVector;
			primitiveVector.pushBack(1);

			Vector<int32_t>::Iterator primitiveIter;
			const Vector<int32_t>::Iterator& constPrimitiveIter = primitiveIter;

			auto primitiveDeref = [&primitiveIter]{ *primitiveIter; };
			auto constPrimitiveDeref = [&constPrimitiveIter]{ *constPrimitiveIter; };
			Assert::ExpectException<exception>(primitiveDeref);
			Assert::ExpectException<exception>(constPrimitiveDeref);

			primitiveIter = primitiveVector.end();
			Assert::ExpectException<exception>(primitiveDeref);
			Assert::ExpectException<exception>(constPrimitiveDeref);

			primitiveIter = primitiveVector.begin();
			Assert::IsTrue(*primitiveIter == primitiveVector.front());
			Assert::IsTrue(*constPrimitiveIter == primitiveVector.front());

			// test with custom type
			Vector<Foo> customVector;
			Foo foo(1);
			customVector.pushBack(foo);

			Vector<Foo>::Iterator customIter;
			const Vector<Foo>::Iterator& constCustomIter = customIter;

			auto customDeref = [&customIter]{ *customIter; };
			auto constCustomDeref = [&constCustomIter]{ *constCustomIter; };
			Assert::ExpectException<exception>(customDeref);
			Assert::ExpectException<exception>(constCustomDeref);

			customIter = customVector.end();
			Assert::ExpectException<exception>(customDeref);
			Assert::ExpectException<exception>(constCustomDeref);

			customIter = customVector.begin();
			Assert::IsTrue(*customIter == customVector.front());
			Assert::IsTrue(*constCustomIter == customVector.front());

			// test with custom type
			Vector<Foo*> pointerVector;
			pointerVector.pushBack(&foo);

			Vector<Foo*>::Iterator pointerIter;
			const Vector<Foo*>::Iterator& constPointerIter = pointerIter;

			auto pointerDeref = [&pointerIter]{ *pointerIter; };
			auto constPointerDeref = [&constPointerIter]{ *constPointerIter; };
			Assert::ExpectException<exception>(pointerDeref);
			Assert::ExpectException<exception>(constPointerDeref);

			pointerIter = pointerVector.end();
			Assert::ExpectException<exception>(pointerDeref);
			Assert::ExpectException<exception>(constPointerDeref);

			pointerIter = pointerVector.begin();
			Assert::IsTrue(*pointerIter == pointerVector.front());
			Assert::IsTrue(*constPointerIter == pointerVector.front());
		}

	private:

		static _CrtMemState sStartMemState;

	};

	_CrtMemState VectorIteratorTest::sStartMemState;
}
