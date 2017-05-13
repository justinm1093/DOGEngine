
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
	TEST_CLASS(VectorTest)
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

		TEST_METHOD(VectorDefaultConstructor)
		{
			// verify that the constructor (with no argument) creates a Vector of 0 size and non-0 capacity
			// verify that the constructor with an argument creates a Vector with 0 size and the given capacity

			// test with primitive type
			Vector<int32_t> primitiveVector;
			Assert::IsTrue(primitiveVector.size() == 0);
			Assert::IsTrue(primitiveVector.capacity() != 0);

			Vector<int32_t> otherPrimitiveVector(10);
			Assert::IsTrue(otherPrimitiveVector.size() == 0);
			Assert::IsTrue(otherPrimitiveVector.capacity() == 10);

			// test with custom type
			Vector<Foo> customVector;
			Assert::IsTrue(customVector.size() == 0);
			Assert::IsTrue(customVector.capacity() != 0);

			Vector<Foo> otherCustomVector(10);
			Assert::IsTrue(otherCustomVector.size() == 0);
			Assert::IsTrue(otherCustomVector.capacity() == 10);

			// test with pointer type
			Vector<Foo*> pointerVector;
			Assert::IsTrue(pointerVector.size() == 0);
			Assert::IsTrue(pointerVector.capacity() != 0);

			Vector<Foo*> otherPointerVector(10);
			Assert::IsTrue(otherPointerVector.size() == 0);
			Assert::IsTrue(otherPointerVector.capacity() == 10);
		}

		TEST_METHOD(VectorCopyConstructor)
		{
			// verify that copy-constructed Vectors have the same data as the original at different addresses

			// test with primitive type
			Vector<int32_t> primitiveVector;
			primitiveVector.pushBack(0);
			primitiveVector.pushBack(1);
			primitiveVector.pushBack(2);

			Vector<int32_t> otherPrimitiveVector(primitiveVector);
			checkVectorEquality(primitiveVector, otherPrimitiveVector);

			// test with custom type
			Vector<Foo> customVector;

			Foo foo_1(1), foo_2(2), foo_3(3);
			customVector.pushBack(foo_1);
			customVector.pushBack(foo_2);
			customVector.pushBack(foo_3);

			Vector<Foo> otherCustomVector(customVector);
			checkVectorEquality(customVector, otherCustomVector);

			// test with pointer type
			Vector<Foo*> pointerVector;
			pointerVector.pushBack(&foo_1);
			pointerVector.pushBack(&foo_2);
			pointerVector.pushBack(&foo_3);

			Vector<Foo*> otherPointerVector(pointerVector);
			checkVectorEquality(pointerVector, otherPointerVector);
		}

		TEST_METHOD(VectorMove)
		{
			// move constructor
			{
				Vector<int32_t> testVec;
				for(uint32_t i = 0; i < 5; ++i)
				{
					testVec.pushBack(i);
				}
				Vector<int32_t> copiedVec(testVec);

				Vector<int32_t> otherVec(std::move(testVec));

				checkVectorEquality(otherVec, copiedVec);

				Assert::IsTrue(testVec.size() == 0);
				Assert::IsTrue(testVec.capacity() == 0);

				for(int32_t i = 0; i < 5; ++i)
				{
					Assert::IsTrue(otherVec[i] == i);
				}
			}

			// move assignment
			{
				Vector<int32_t> testVec;
				for(uint32_t i = 0; i < 5; ++i)
				{
					testVec.pushBack(i);
				}
				Vector<int32_t> copiedVec(testVec);

				Vector<int32_t> otherVec;
				otherVec.pushBack(100);

				otherVec = std::move(testVec);

				checkVectorEquality(otherVec, copiedVec);

				Assert::IsTrue(testVec.size() == 0);
				Assert::IsTrue(testVec.capacity() == 0);

				for(int32_t i = 0; i < 5; ++i)
				{
					Assert::IsTrue(otherVec[i] == i);
				}
			}
		}

		TEST_METHOD(VectorDestructor)
		{
			// verify that the destructor destroys the Vector with no memory leaks

			// test with primitive type
			{
				Vector<int32_t> primitiveVector;
				primitiveVector.pushBack(1);
			}

			// test with custom type
			{
				Vector<Foo> customVector;
				customVector.pushBack(Foo(1));
			}

			// test with pointer type
			{
				Vector<Foo*> pointerVector;

				Foo foo(1);
				pointerVector.pushBack(&foo);
			}
		}

		TEST_METHOD(VectorAssignmentOperator)
		{
			// verify that assigned Vectors have the same data as the original at different addresses

			// test with primitive type
			Vector<int32_t> primitiveVector;
			primitiveVector.pushBack(0);
			primitiveVector.pushBack(1);
			primitiveVector.pushBack(2);

			Vector<int32_t> otherPrimitiveVector;
			otherPrimitiveVector = primitiveVector;
			checkVectorEquality(primitiveVector, otherPrimitiveVector);

			// test with custom type
			Vector<Foo> customVector;

			Foo foo_1(1), foo_2(2), foo_3(3);
			customVector.pushBack(foo_1);
			customVector.pushBack(foo_2);
			customVector.pushBack(foo_3);

			Vector<Foo> otherCustomVector;
			otherCustomVector = customVector;
			checkVectorEquality(customVector, otherCustomVector);

			// test with pointer type
			Vector<Foo*> pointerVector;
			pointerVector.pushBack(&foo_1);
			pointerVector.pushBack(&foo_2);
			pointerVector.pushBack(&foo_3);

			Vector<Foo*> otherPointerVector;
			otherPointerVector = pointerVector;
			checkVectorEquality(pointerVector, otherPointerVector);
		}

		TEST_METHOD(VectorBracketsOperator)
		{
			// verify that the brackets operator throws an exception for index out-of-bounds
			// verify that the brackets operator returns the proper data in the Vector

			// test with primitive type
			Vector<int32_t> primitiveVector;
			
			auto primitiveBrackets = [&primitiveVector]{ primitiveVector[0]; };
			Assert::ExpectException<exception>(primitiveBrackets);

			primitiveVector.pushBack(0);
			primitiveVector.pushBack(1);
			primitiveVector.pushBack(2);

			Assert::AreEqual(primitiveVector[0], 0);
			Assert::AreEqual(primitiveVector[1], 1);
			Assert::AreEqual(primitiveVector[2], 2);

			// test with custom type
			Vector<Foo> customVector;

			auto customBrackets = [&customVector]{ customVector[0]; };
			Assert::ExpectException<exception>(customBrackets);

			Foo foo_1(1), foo_2(2), foo_3(3);
			customVector.pushBack(foo_1);
			customVector.pushBack(foo_2);
			customVector.pushBack(foo_3);

			Assert::IsTrue(customVector[0] == foo_1);
			Assert::IsTrue(customVector[1] == foo_2);
			Assert::IsTrue(customVector[2] == foo_3);

			// test with pointer type
			Vector<Foo*> pointerVector;

			auto pointerBrackets = [&pointerVector]{ pointerVector[0]; };
			Assert::ExpectException<exception>(pointerBrackets);

			pointerVector.pushBack(&foo_1);
			pointerVector.pushBack(&foo_2);
			pointerVector.pushBack(&foo_3);

			Assert::IsTrue(pointerVector[0] == &foo_1);
			Assert::IsTrue(pointerVector[1] == &foo_2);
			Assert::IsTrue(pointerVector[2] == &foo_3);
		}

		TEST_METHOD(VectorFind)
		{
			// verify that finding an element in an empty Vector returns end()
			// verify that finding an element that does not exist in the Vector returns end()
			// verify that we find the first element in the Vector matching data

			// test with primitive type
			Vector<int32_t> primitiveVector;

			Assert::IsTrue(primitiveVector.find(1) == primitiveVector.end());

			primitiveVector.pushBack(1);
			primitiveVector.pushBack(1);
			Assert::IsTrue(primitiveVector.find(2) == primitiveVector.end());

			Assert::IsTrue(primitiveVector.find(1) == primitiveVector.begin());

			// test with custom type
			Vector<Foo> customVector;
			Foo foo_1(1), foo_2(2);

			Assert::IsTrue(customVector.find(foo_1) == customVector.end());

			customVector.pushBack(foo_1);
			customVector.pushBack(foo_1);
			Assert::IsTrue(customVector.find(foo_2) == customVector.end());

			Assert::IsTrue(customVector.find(foo_1) == customVector.begin());

			// test with pointer type
			Vector<Foo*> pointerVector;

			Assert::IsTrue(pointerVector.find(&foo_1) == pointerVector.end());

			pointerVector.pushBack(&foo_1);
			pointerVector.pushBack(&foo_1);
			Assert::IsTrue(pointerVector.find(&foo_2) == pointerVector.end());

			Assert::IsTrue(pointerVector.find(&foo_1) == pointerVector.begin());
		}

		TEST_METHOD(VectorAt)
		{
			// verify that 'at' throws an exception for out-of-bounds indicies
			// verify that 'at' returns the data at the index

			//// test with primitive type
			Vector<int32_t> primitiveVector;
			primitiveVector.pushBack(1);
			primitiveVector.pushBack(2);
			primitiveVector.pushBack(3);

			auto primitiveAt = [&primitiveVector]{ primitiveVector.at(primitiveVector.size() + 10); };
			Assert::ExpectException<exception>(primitiveAt);

			Assert::IsTrue(primitiveVector.at(1) == primitiveVector[1]);

			const Vector<int32_t>& constPrimitiveVector = primitiveVector;
			auto constPrimitiveAt = [&constPrimitiveVector]{ constPrimitiveVector.at(constPrimitiveVector.size() + 10); };
			Assert::ExpectException<exception>(constPrimitiveAt);

			Assert::IsTrue(constPrimitiveVector.at(1) == constPrimitiveVector[1]);

			// test with custom type
			Vector<Foo> customVector;

			Foo foo_1(1), foo_2(2), foo_3(3);
			customVector.pushBack(foo_1);
			customVector.pushBack(foo_2);
			customVector.pushBack(foo_3);

			auto customAt = [&customVector]{ customVector.at(customVector.size() + 10); };
			Assert::ExpectException<exception>(customAt);

			Assert::IsTrue(customVector.at(1) == customVector[1]);

			const Vector<Foo>& constCustomVector = customVector;
			auto constCustomAt = [&constCustomVector]{ constCustomVector.at(constCustomVector.size() + 10); };
			Assert::ExpectException<exception>(constCustomAt);

			Assert::IsTrue(constCustomVector.at(1) == constCustomVector[1]);

			// test with pointer type
			Vector<Foo*> pointerVector;
			pointerVector.pushBack(&foo_1);
			pointerVector.pushBack(&foo_2);
			pointerVector.pushBack(&foo_3);

			auto pointerAt = [&pointerVector]{ pointerVector.at(pointerVector.size() + 10); };
			Assert::ExpectException<exception>(pointerAt);

			Assert::IsTrue(pointerVector.at(1) == pointerVector[1]);

			const Vector<Foo*>& constPointerVector = pointerVector;
			auto constPointerAt = [&constPointerVector]{ constPointerVector.at(constPointerVector.size() + 10); };
			Assert::ExpectException<exception>(constPointerAt);

			Assert::IsTrue(constPointerVector.at(1) == constPointerVector[1]);
		}

		TEST_METHOD(VectorFront)
		{
			// verify that getting front() on an empty Vector throws an exception
			// verify that front returns the first element in the Vector

			// test with primitive type
			Vector<uint32_t> primitiveVector;
			auto primitiveFront = [&primitiveVector]{ primitiveVector.front(); };
			Assert::ExpectException<exception>(primitiveFront);

			Vector<uint32_t>& constPrimitiveVector = primitiveVector;
			auto constPrimitiveFront = [&constPrimitiveVector]{ constPrimitiveVector.front(); };
			Assert::ExpectException<exception>(constPrimitiveFront);

			primitiveVector.pushBack(0);
			primitiveVector.pushBack(1);

			Assert::AreEqual(primitiveVector.front(), primitiveVector[0]);
			Assert::AreEqual(constPrimitiveVector.front(), constPrimitiveVector[0]);

			// test with custom type
			Vector<Foo> customVector;
			auto customFront = [&customVector]{ customVector.front(); };
			Assert::ExpectException<exception>(customFront);

			Vector<Foo>& constCustomVector = customVector;
			auto constCustomFront = [&constCustomVector]{ constCustomVector.front(); };
			Assert::ExpectException<exception>(constCustomFront);

			Foo foo_1(1), foo_2(2);
			customVector.pushBack(foo_1);
			customVector.pushBack(foo_2);

			Assert::IsTrue(customVector.front() == customVector[0]);
			Assert::IsTrue(constCustomVector.front() == constCustomVector[0]);

			// test with pointer type
			Vector<Foo*> pointerVector;
			auto pointerFront = [&pointerVector]{ pointerVector.front(); };
			Assert::ExpectException<exception>(pointerFront);

			Vector<Foo*>& constPointerVector = pointerVector;
			auto constPointerFront = [&constPointerVector]{ constPointerVector.front(); };
			Assert::ExpectException<exception>(constPointerFront);

			pointerVector.pushBack(&foo_1);
			pointerVector.pushBack(&foo_2);

			Assert::IsTrue(pointerVector.front() == pointerVector[0]);
			Assert::IsTrue(constPointerVector.front() == constPointerVector[0]);
		}

		TEST_METHOD(VectorBack)
		{
			// verify that getting back() on an empty Vector throws an exception
			// verify that back returns the last element in the Vector

			// test with primitive type
			Vector<uint32_t> primitiveVector;
			auto primitiveBack = [&primitiveVector]{ primitiveVector.back(); };
			Assert::ExpectException<exception>(primitiveBack);

			Vector<uint32_t>& constPrimitiveVector = primitiveVector;
			auto constPrimitiveBack = [&constPrimitiveVector]{ constPrimitiveVector.back(); };
			Assert::ExpectException<exception>(constPrimitiveBack);

			primitiveVector.pushBack(0);
			primitiveVector.pushBack(1);

			Assert::AreEqual(primitiveVector.back(), primitiveVector[primitiveVector.size() - 1]);
			Assert::AreEqual(constPrimitiveVector.back(), constPrimitiveVector[constPrimitiveVector.size() - 1]);

			// test with custom type
			Vector<Foo> customVector;
			auto customBack = [&customVector]{ customVector.back(); };
			Assert::ExpectException<exception>(customBack);

			Vector<Foo>& constCustomVector = customVector;
			auto constCustomBack = [&constCustomVector]{ constCustomVector.back(); };
			Assert::ExpectException<exception>(constCustomBack);

			Foo foo_1(1), foo_2(2);
			customVector.pushBack(foo_1);
			customVector.pushBack(foo_2);

			Assert::IsTrue(customVector.back() == customVector[customVector.size() - 1]);
			Assert::IsTrue(constCustomVector.back() == constCustomVector[constCustomVector.size() - 1]);

			// test with pointer type
			Vector<Foo*> pointerVector;
			auto pointerBack = [&pointerVector]{ pointerVector.back(); };
			Assert::ExpectException<exception>(pointerBack);

			Vector<Foo*>& constPointerVector = pointerVector;
			auto constPointerBack = [&constPointerVector]{ constPointerVector.back(); };
			Assert::ExpectException<exception>(constPointerBack);

			pointerVector.pushBack(&foo_1);
			pointerVector.pushBack(&foo_2);

			Assert::IsTrue(pointerVector.back() == pointerVector[pointerVector.size() - 1]);
			Assert::IsTrue(constPointerVector.back() == constPointerVector[constPointerVector.size() - 1]);
		}

		TEST_METHOD(VectorPushBack)
		{
			// verify that pushing back changes the Vector's size and final element
			// verify that pushing back when at capacity increases capacity without memory leaks
			// verify that pushing back on a list with 0 capacity makes capacity 1

			// test with primitive type
			Vector<uint32_t> primitiveVector;
			uint32_t savedPrimitiveCapacity = primitiveVector.capacity();

			for(uint32_t i = 0; i < 20; ++i)
			{
				primitiveVector.pushBack(i);

				Assert::AreEqual(primitiveVector.size(), i + 1);
				Assert::IsTrue(primitiveVector.back() == i);
			}
			Assert::AreNotEqual(primitiveVector.capacity(), savedPrimitiveCapacity);

			Vector<uint32_t> otherPrimitiveVector(0);
			otherPrimitiveVector.pushBack(1);
			Assert::IsTrue(otherPrimitiveVector.capacity() == 1);

			// test with custom type
			Vector<Foo> customVector;
			uint32_t savedCustomCapacity = customVector.capacity();

			for(uint32_t i = 0; i < 20; ++i)
			{
				customVector.pushBack(Foo(i));
				
				Assert::AreEqual(customVector.size(), i + 1);
				Assert::IsTrue(customVector.back() == Foo(i));
			}
			Assert::AreNotEqual(customVector.capacity(), savedCustomCapacity);

			Vector<Foo> otherCustomVector(0);
			otherCustomVector.pushBack(Foo(1));
			Assert::IsTrue(otherCustomVector.capacity() == 1);

			// test with primitive type
			Vector<Foo*> pointerVector;
			uint32_t savedPointerCapacity = pointerVector.capacity();

			for(uint32_t i = 0; i < 20; ++i)
			{
				pointerVector.pushBack(&customVector[i]);

				Assert::AreEqual(pointerVector.size(), i + 1);
				Assert::IsTrue(pointerVector.back() == &customVector[i]);
			}
			Assert::AreNotEqual(pointerVector.capacity(), savedPointerCapacity);

			Vector<Foo*> otherPointerVector(0);
			otherPointerVector.pushBack(&otherCustomVector[0]);
			Assert::IsTrue(otherPointerVector.capacity() == 1);
		}

		TEST_METHOD(VectorPopBack)
		{
			// verify that popping back reduces the list's size without memory leaks
			// verify that the capacity does not change

			// test with primitive type
			Vector<int32_t> primitiveVector;
			for(uint32_t i = 0; i < 10; ++i)
			{
				primitiveVector.pushBack(i);
			}

			uint32_t expectedCapacity = primitiveVector.capacity();
			uint32_t expectedSize = primitiveVector.size();
			while(!primitiveVector.isEmpty())
			{
				primitiveVector.popBack();
				expectedSize--;

				Assert::AreEqual(expectedCapacity, primitiveVector.capacity());
				Assert::AreEqual(expectedSize, primitiveVector.size());
			}

			// test with custom type
			Vector<Foo> customVector;
			for(uint32_t i = 0; i < 10; ++i)
			{
				customVector.pushBack(Foo(i));
			}

			expectedCapacity = customVector.capacity();
			expectedSize = customVector.size();
			while(!customVector.isEmpty())
			{
				customVector.popBack();
				expectedSize--;

				Assert::AreEqual(expectedCapacity, customVector.capacity());
				Assert::AreEqual(expectedSize, customVector.size());
			}

			// test with pointer type
			Vector<Foo*> pointerVector;
			for(uint32_t i = 0; i < 10; ++i)
			{
				pointerVector.pushBack(nullptr);
			}

			expectedCapacity = pointerVector.capacity();
			expectedSize = pointerVector.size();
			while(!pointerVector.isEmpty())
			{
				pointerVector.popBack();
				expectedSize--;

				Assert::AreEqual(expectedCapacity, pointerVector.capacity());
				Assert::AreEqual(expectedSize, pointerVector.size());
			}
		}

		TEST_METHOD(VectorReserve)
		{
			// verify that reserve only changes the Vector's capacity when requesting more space than what is already present
			// verify that reserve preserves the Vector's contents

			// test with primitive type
			Vector<int32_t> primitiveVector;
			uint32_t defaultPrimitiveCapacity = primitiveVector.capacity();

			primitiveVector.reserve(defaultPrimitiveCapacity - 1);						// can't shrink the reserved space
			Assert::AreEqual(defaultPrimitiveCapacity, primitiveVector.capacity());

			primitiveVector.reserve(defaultPrimitiveCapacity + 1);						// but can grow the reserved space
			Assert::IsTrue(primitiveVector.capacity() > defaultPrimitiveCapacity);

			primitiveVector.pushBack(0);
			primitiveVector.pushBack(1);
			primitiveVector.pushBack(2);

			Vector<int32_t> otherPrimitiveVector(primitiveVector);
			primitiveVector.reserve(primitiveVector.capacity() + 1);
			checkVectorDataEquality(primitiveVector, otherPrimitiveVector);				// reserving preserves data in the Vector
			
			// test with custom type
			Vector<Foo> customVector;
			uint32_t defaultCustomCapacity = customVector.capacity();

			customVector.reserve(defaultCustomCapacity - 1);							// can't shrink the reserved space
			Assert::AreEqual(defaultCustomCapacity, customVector.capacity());

			customVector.reserve(defaultCustomCapacity + 1);							// but can grow the reserved space
			Assert::IsTrue(customVector.capacity() > defaultCustomCapacity);

			Foo foo_1(1), foo_2(2), foo_3(3);
			customVector.pushBack(foo_1);
			customVector.pushBack(foo_2);
			customVector.pushBack(foo_3);

			Vector<Foo> otherCustomVector(customVector);
			customVector.reserve(customVector.capacity() + 1);
			checkVectorDataEquality(customVector, otherCustomVector);					// reserving preserves data in the Vector

			// test with pointer type
			Vector<Foo*> pointerVector;
			uint32_t defaultPointerCapacity = pointerVector.capacity();

			pointerVector.reserve(defaultPointerCapacity - 1);							// can't shrink the reserved space
			Assert::AreEqual(defaultPointerCapacity, pointerVector.capacity());

			pointerVector.reserve(defaultPointerCapacity + 1);							// but can grow the reserved space
			Assert::IsTrue(pointerVector.capacity() > defaultPointerCapacity);

			pointerVector.pushBack(&foo_1);
			pointerVector.pushBack(&foo_2);
			pointerVector.pushBack(&foo_3);

			Vector<Foo*> otherPointerVector(pointerVector);
			pointerVector.reserve(pointerVector.capacity() + 1);
			checkVectorDataEquality(pointerVector, otherPointerVector);					// reserving preserves data in the Vector
		}

		TEST_METHOD(VectorShrinkToFit)
		{
			// verify that shrink to fit matches the Vector's size and capacity
			// verify that shrink to fit preserves the data

			// test with primitive type
			Vector<uint32_t> primitiveVector;
			primitiveVector.pushBack(1);
			primitiveVector.pushBack(2);
			primitiveVector.pushBack(3);

			Vector<uint32_t> otherPrimitiveVector = primitiveVector;
			primitiveVector.shrinkToFit();

			Assert::IsTrue(primitiveVector.size() == primitiveVector.capacity());
			checkVectorDataEquality(primitiveVector, otherPrimitiveVector);

			primitiveVector.clear();
			primitiveVector.shrinkToFit();
			Assert::IsTrue(primitiveVector.capacity() == 0);

			// test with custom type
			Vector<Foo> customVector;

			Foo foo_1(1), foo_2(2), foo_3(3);
			customVector.pushBack(foo_1);
			customVector.pushBack(foo_2);
			customVector.pushBack(foo_3);

			Vector<Foo> otherCustomVector = customVector;
			customVector.shrinkToFit();

			Assert::IsTrue(customVector.size() == customVector.capacity());
			checkVectorDataEquality(customVector, otherCustomVector);

			customVector.clear();
			customVector.shrinkToFit();
			Assert::IsTrue(customVector.capacity() == 0);

			// test with pointer type
			Vector<Foo*> pointerVector;
			pointerVector.pushBack(&foo_1);
			pointerVector.pushBack(&foo_2);
			pointerVector.pushBack(&foo_3);

			Vector<Foo*> otherPointerVector = pointerVector;
			pointerVector.shrinkToFit();

			Assert::IsTrue(pointerVector.size() == pointerVector.capacity());
			checkVectorDataEquality(pointerVector, otherPointerVector);

			pointerVector.clear();
			pointerVector.shrinkToFit();
			Assert::IsTrue(pointerVector.capacity() == 0);
		}

		TEST_METHOD(VectorRemove)
		{
			// we need to verify that both versions of remove behave properly
			testSingleRemove();
			testRangedRemove();
		}

		TEST_METHOD(VectorClear)
		{
			// verify that clear makes the Vector empty
			// verify that clearing the Vector has no memory leaks

			// test with primitive type
			Vector<int32_t> primitiveVector;
			for(uint32_t i = 0; i < 10; ++i)
			{
				primitiveVector.pushBack(i);
			}

			Assert::IsFalse(primitiveVector.isEmpty());
			primitiveVector.clear();
			Assert::IsTrue(primitiveVector.isEmpty());
			Assert::AreEqual(primitiveVector.size(), (uint32_t)0);

			// test with custom type
			Vector<Foo> customVector;
			for(uint32_t i = 0; i < 10; ++i)
			{
				customVector.pushBack(Foo(i));
			}

			Assert::IsFalse(customVector.isEmpty());
			customVector.clear();
			Assert::IsTrue(customVector.isEmpty());
			Assert::AreEqual(customVector.size(), (uint32_t)0);

			// test with pointer type
			Vector<Foo*> pointerVector;
			for(uint32_t i = 0; i < 10; ++i)
			{
				pointerVector.pushBack(nullptr);
			}

			Assert::IsFalse(pointerVector.isEmpty());
			pointerVector.clear();
			Assert::IsTrue(pointerVector.isEmpty());
			Assert::AreEqual(pointerVector.size(), (uint32_t)0);
		}

		TEST_METHOD(VectorSize)
		{
			// verify that size changes when we add things to the list

			// test with primitive type
			Vector<int32_t> primitiveVector;
			Assert::IsTrue(primitiveVector.size() == 0);
			for(uint32_t i = 0; i < 10; ++i)
			{
				primitiveVector.pushBack(i);
				Assert::IsTrue(primitiveVector.size() == i + 1);
			}

			// test with custom type
			Vector<Foo> customVector;
			Assert::IsTrue(customVector.size() == 0);
			for(uint32_t i = 0; i < 10; ++i)
			{
				customVector.pushBack(Foo(i));
				Assert::IsTrue(customVector.size() == i + 1);
			}

			// test with pointer type
			Vector<Foo*> pointerVector;
			Assert::IsTrue(pointerVector.size() == 0);
			for(uint32_t i = 0; i < 10; ++i)
			{
				pointerVector.pushBack(&customVector[i]);
				Assert::IsTrue(pointerVector.size() == i + 1);
			}

			// verify that removing items from the Vector reduces size

			// test with primitive type
			for(uint32_t i = 10; i > 5; --i)
			{
				primitiveVector.popBack();
				Assert::IsTrue(primitiveVector.size() == i - 1);
			}

			primitiveVector.remove(primitiveVector.begin(), primitiveVector.end());
			Assert::IsTrue(primitiveVector.size() == 0);

			// test with custom type
			for(uint32_t i = 10; i > 5; --i)
			{
				customVector.popBack();
				Assert::IsTrue(customVector.size() == i - 1);
			}

			customVector.remove(customVector.begin(), customVector.end());
			Assert::IsTrue(customVector.size() == 0);

			// test with pointer type
			for(uint32_t i = 10; i > 5; --i)
			{
				pointerVector.popBack();
				Assert::IsTrue(pointerVector.size() == i - 1);
			}

			pointerVector.remove(pointerVector.begin(), pointerVector.end());
			Assert::IsTrue(pointerVector.size() == 0);
		}

		TEST_METHOD(VectorCapacity)
		{
			// verify that the Vector's default capacity is nonzero
			// verify that the capacity grows as things are added to the Vector

			// test with primitive type
			Vector<int32_t> primitiveVector;
			uint32_t defaultPrimitiveCapacity = primitiveVector.capacity();
			Assert::IsTrue(defaultPrimitiveCapacity > 0);

			for(uint32_t i = 0; i < 20; ++i)
			{
				primitiveVector.pushBack(i);
			}

			Assert::IsTrue(primitiveVector.capacity() > defaultPrimitiveCapacity);

			// test with custom type
			Vector<Foo> customVector;
			uint32_t defaultCustomCapacity = customVector.capacity();
			Assert::IsTrue(defaultCustomCapacity > 0);

			for(uint32_t i = 0; i < 20; ++i)
			{
				customVector.pushBack(Foo(i));
			}

			Assert::IsTrue(customVector.capacity() > defaultCustomCapacity);

			// test with pointer type
			Vector<Foo*> pointerVector;
			uint32_t defaultPointerCapacity = pointerVector.capacity();
			Assert::IsTrue(defaultPointerCapacity > 0);

			for(uint32_t i = 0; i < 20; ++i)
			{
				pointerVector.pushBack(&customVector[i]);
			}

			Assert::IsTrue(pointerVector.capacity() > defaultPointerCapacity);
		}

		TEST_METHOD(VectorBegin)
		{
			// verify that begin and end are the same for an empty Vector
			// verify that begin points to the first element in the Vector
			// verify that begin is invalidated when the Vector reallocates

			// test with primitive type
			Vector<int32_t> primitiveVector;
			const Vector<int32_t>& constPrimitiveVector = primitiveVector;

			Assert::IsTrue(primitiveVector.begin() == primitiveVector.end());
			Assert::IsTrue(constPrimitiveVector.begin() == constPrimitiveVector.end());

			primitiveVector.pushBack(1);
			primitiveVector.pushBack(2);
			Assert::IsTrue(*primitiveVector.begin() == 1);
			Assert::IsTrue(*constPrimitiveVector.begin() == 1);

			Vector<int32_t>::Iterator oldPrimitiveBegin = primitiveVector.begin();
			const Vector<int32_t>::Iterator& oldConstPrimitiveBegin = oldPrimitiveBegin;
			primitiveVector.reserve(20);

			Assert::IsTrue(oldPrimitiveBegin == primitiveVector.begin());
			Assert::IsTrue(oldConstPrimitiveBegin == constPrimitiveVector.begin());

			// test with custom type
			Vector<Foo> customVector;
			const Vector<Foo>& constCustomVector = customVector;

			Assert::IsTrue(customVector.begin() == customVector.end());
			Assert::IsTrue(constCustomVector.begin() == constCustomVector.end());

			Foo foo_1(1), foo_2(2);
			customVector.pushBack(foo_1);
			customVector.pushBack(foo_2);
			Assert::IsTrue(*customVector.begin() == foo_1);
			Assert::IsTrue(*constCustomVector.begin() == foo_1);

			Vector<Foo>::Iterator oldCustomBegin = customVector.begin();
			const Vector<Foo>::Iterator& oldConstCustomBegin = oldCustomBegin;
			customVector.reserve(20);

			Assert::IsTrue(oldCustomBegin == customVector.begin());
			Assert::IsTrue(oldConstCustomBegin == constCustomVector.begin());

			// test with pointer type
			Vector<Foo*> pointerVector;
			const Vector<Foo*>& constPointerVector = pointerVector;

			Assert::IsTrue(pointerVector.begin() == pointerVector.end());
			Assert::IsTrue(constPointerVector.begin() == constPointerVector.end());


			pointerVector.pushBack(&foo_1);
			pointerVector.pushBack(&foo_2);
			Assert::IsTrue(*pointerVector.begin() == &foo_1);
			Assert::IsTrue(*constPointerVector.begin() == &foo_1);

			Vector<Foo*>::Iterator oldPointerBegin = pointerVector.begin();
			const Vector<Foo*>::Iterator& oldConstPointerBegin = oldPointerBegin;
			pointerVector.reserve(20);

			Assert::IsTrue(oldPointerBegin == pointerVector.begin());
			Assert::IsTrue(oldConstPointerBegin == constPointerVector.begin());
		}

		TEST_METHOD(VectorEnd)
		{
			// verify that end() points to no data and can't be dereferenced
			// verify that an end() Iterator can be invalidated by altering the Vector

			// test with primitive type
			Vector<int32_t> primitiveVector;
			const Vector<int32_t>& constPrimitiveVector = primitiveVector;
			primitiveVector.pushBack(1);

			auto primitiveEnd = [&primitiveVector]{ *primitiveVector.end(); };
			auto constPrimitiveEnd = [&constPrimitiveVector]{ *constPrimitiveVector.end(); };
			Assert::ExpectException<exception>(primitiveEnd);
			Assert::ExpectException<exception>(constPrimitiveEnd);

			Vector<int32_t>::Iterator oldPrimitiveEnd = primitiveVector.end();
			const Vector<int32_t>::Iterator oldConstPrimitiveEnd = constPrimitiveVector.end();

			primitiveVector.pushBack(2);
			Assert::IsFalse(oldPrimitiveEnd == primitiveVector.end());
			Assert::IsFalse(oldConstPrimitiveEnd == constPrimitiveVector.end());

			primitiveVector.popBack();
			Assert::IsTrue(oldPrimitiveEnd == primitiveVector.end());
			Assert::IsTrue(oldConstPrimitiveEnd == constPrimitiveVector.end());

			primitiveVector.popBack();
			Assert::IsFalse(oldPrimitiveEnd == primitiveVector.end());
			Assert::IsFalse(oldConstPrimitiveEnd == constPrimitiveVector.end());

			// test with custom type
			Foo foo_1(1), foo_2(2);
			Vector<Foo> customVector;
			const Vector<Foo>& constCustomVector = customVector;
			customVector.pushBack(foo_1);

			auto customEnd = [&customVector]{ *customVector.end(); };
			auto constCustomEnd = [&constCustomVector]{ *constCustomVector.end(); };
			Assert::ExpectException<exception>(customEnd);
			Assert::ExpectException<exception>(constCustomEnd);

			Vector<Foo>::Iterator oldCustomEnd = customVector.end();
			const Vector<Foo>::Iterator oldConstCustomEnd = constCustomVector.end();

			customVector.pushBack(foo_2);
			Assert::IsFalse(oldCustomEnd == customVector.end());
			Assert::IsFalse(oldConstCustomEnd == constCustomVector.end());

			customVector.popBack();
			Assert::IsTrue(oldCustomEnd == customVector.end());
			Assert::IsTrue(oldConstCustomEnd == constCustomVector.end());

			customVector.popBack();
			Assert::IsFalse(oldCustomEnd == customVector.end());
			Assert::IsFalse(oldConstCustomEnd == constCustomVector.end());

			// test with pointer type
			Vector<Foo*> pointerVector;
			const Vector<Foo*>& constPointerVector = pointerVector;
			pointerVector.pushBack(&foo_1);

			auto pointerEnd = [&pointerVector]{ *pointerVector.end(); };
			auto constPointerEnd = [&constPointerVector]{ *constPointerVector.end(); };
			Assert::ExpectException<exception>(pointerEnd);
			Assert::ExpectException<exception>(constPointerEnd);

			Vector<Foo*>::Iterator oldPointerEnd = pointerVector.end();
			const Vector<Foo*>::Iterator oldConstPointerEnd = constPointerVector.end();

			pointerVector.pushBack(&foo_2);
			Assert::IsFalse(oldPointerEnd == pointerVector.end());
			Assert::IsFalse(oldConstPointerEnd == constPointerVector.end());

			pointerVector.popBack();
			Assert::IsTrue(oldPointerEnd == pointerVector.end());
			Assert::IsTrue(oldConstPointerEnd == constPointerVector.end());

			pointerVector.popBack();
			Assert::IsFalse(oldPointerEnd == pointerVector.end());
			Assert::IsFalse(oldConstPointerEnd == constPointerVector.end());
		}

	private:

		template <typename T>
		void checkVectorEquality(Vector<T>& originalList, Vector<T>& copiedList)
		{
			// Vector equality is based on the capacity and size
			Assert::AreEqual(originalList.capacity(), copiedList.capacity());
			Assert::AreEqual(originalList.size(), copiedList.size());

			// Vector equality is also based on the data being in the same place in the array
			checkVectorDataEquality(originalList, copiedList);
		}

		template <typename T>
		void checkVectorDataEquality(Vector<T>& originalList, Vector<T>& copiedList)
		{
			uint32_t vectorSize = originalList.size();
			for(uint32_t i = 0; i < vectorSize; ++i)
			{
				Assert::IsFalse(&originalList[i] == &copiedList[i]);
				Assert::IsTrue(originalList[i] == copiedList[i]);
			}
		}

		void testSingleRemove()
		{
			// verify that removing a single element that does not exist returns false
			// verify that removing a single element removes the first instance of that element

			// test with primitive type
			Vector<int32_t> primitiveVector;
			primitiveVector.pushBack(1);
			primitiveVector.pushBack(2);
			primitiveVector.pushBack(1);

			Assert::IsTrue(primitiveVector.front() == 1);
			Assert::IsTrue(primitiveVector.size() == 3);

			Assert::IsFalse(primitiveVector.remove(3));
			Assert::IsTrue(primitiveVector.size() == 3);

			Assert::IsTrue(primitiveVector.remove(1));
			Assert::IsTrue(primitiveVector.size() == 2);

			Assert::IsFalse(primitiveVector.front() == 1);

			// test with custom type
			Vector<Foo> customVector;

			Foo foo_1(1), foo_2(2), foo_3(3);
			customVector.pushBack(foo_1);
			customVector.pushBack(foo_2);
			customVector.pushBack(foo_1);

			Assert::IsTrue(customVector.size() == 3);
			Assert::IsTrue(customVector.front() == foo_1);

			Assert::IsFalse(customVector.remove(foo_3));
			Assert::IsTrue(customVector.size() == 3);

			Assert::IsTrue(customVector.remove(foo_1));
			Assert::IsTrue(customVector.size() == 2);

			Assert::IsFalse(customVector.front() == foo_1);

			// test with pointer type
			Vector<Foo*> pointerVector;
			pointerVector.pushBack(&foo_1);
			pointerVector.pushBack(&foo_2);
			pointerVector.pushBack(&foo_1);

			Assert::IsTrue(pointerVector.size() == 3);
			Assert::IsTrue(pointerVector.front() == &foo_1);

			Assert::IsFalse(pointerVector.remove(&foo_3));
			Assert::IsTrue(pointerVector.size() == 3);

			Assert::IsTrue(pointerVector.remove(&foo_1));
			Assert::IsTrue(pointerVector.size() == 2);

			Assert::IsFalse(pointerVector.front() == &foo_1);
		}

		void testRangedRemove()
		{
			// verify that calling ranged remove with unowned Iterators throws an exception
			// verify that calling ranged remove when start > finish throws an exception
			// verify that calling ranged remove when start or end is passed the outside of the Vector is a noop
			// verify that calling ranged remove with valid inputs removes the range

			// test with primitive type
			Vector<int32_t> primitiveVector;
			Vector<int32_t> otherPrimitiveVector;

			primitiveVector.pushBack(1);
			primitiveVector.pushBack(2);
			primitiveVector.pushBack(3);
			primitiveVector.pushBack(4);
			Assert::IsTrue(primitiveVector.size() == 4);

			auto primitiveRemoveInvalidStart = [&primitiveVector, &otherPrimitiveVector]{ primitiveVector.remove(otherPrimitiveVector.begin(), primitiveVector.end()); };
			auto primitiveRemoveInvalidFinish = [&primitiveVector, &otherPrimitiveVector]{ primitiveVector.remove(primitiveVector.begin(), otherPrimitiveVector.end()); };
			auto primitiveRemoveInvalidRange = [&primitiveVector]{ primitiveVector.remove(primitiveVector.end(), primitiveVector.begin()); };
			Assert::ExpectException<exception>(primitiveRemoveInvalidStart);
			Assert::ExpectException<exception>(primitiveRemoveInvalidFinish);
			Assert::ExpectException<exception>(primitiveRemoveInvalidRange);

			primitiveVector.remove(primitiveVector.end(), ++primitiveVector.end());			// ranged remove with range beyond end() is a noop
			Assert::IsTrue(primitiveVector.size() == 4);

			primitiveVector.remove(primitiveVector.begin(), primitiveVector.begin());		// ranged remove with start == finish is a noop
			Assert::IsTrue(primitiveVector.size() == 4);

			primitiveVector.remove(primitiveVector.begin(), primitiveVector.find(3));		// ranged remove with range inside Vector is valid
			Assert::IsTrue(primitiveVector.size() == 2);
			Assert::IsTrue(primitiveVector.front() == 3);

			primitiveVector.remove(primitiveVector.begin(), ++primitiveVector.end());		// ranged remove with finish outside vector removes remaining list beyond start
			Assert::IsTrue(primitiveVector.isEmpty());

			// test with custom type
			Vector<Foo> customVector;
			Vector<Foo> otherCustomVector;

			Foo foo_1(1), foo_2(2), foo_3(3), foo_4(4);
			customVector.pushBack(foo_1);
			customVector.pushBack(foo_2);
			customVector.pushBack(foo_3);
			customVector.pushBack(foo_4);
			Assert::IsTrue(customVector.size() == 4);

			auto customRemoveInvalidStart = [&customVector, &otherCustomVector]{ customVector.remove(otherCustomVector.begin(), customVector.end()); };
			auto customRemoveInvalidFinish = [&customVector, &otherCustomVector]{ customVector.remove(customVector.begin(), otherCustomVector.end()); };
			auto customRemoveInvalidRange = [&customVector]{ customVector.remove(customVector.end(), customVector.begin()); };
			Assert::ExpectException<exception>(customRemoveInvalidStart);
			Assert::ExpectException<exception>(customRemoveInvalidFinish);
			Assert::ExpectException<exception>(customRemoveInvalidRange);

			customVector.remove(customVector.end(), ++customVector.end());			// ranged remove with range beyond end() is a noop
			Assert::IsTrue(customVector.size() == 4);

			customVector.remove(customVector.begin(), customVector.begin());		// ranged remove with start == finish is a noop
			Assert::IsTrue(customVector.size() == 4);

			customVector.remove(customVector.begin(), customVector.find(foo_3));	// ranged remove with range inside Vector is valid
			Assert::IsTrue(customVector.size() == 2);
			Assert::IsTrue(customVector.front() == foo_3);

			customVector.remove(customVector.begin(), ++customVector.end());		// ranged remove with finish outside vector removes remaining list beyond start
			Assert::IsTrue(customVector.isEmpty());

			// test with pointer type
			Vector<Foo*> pointerVector;
			Vector<Foo*> otherPointerVector;

			pointerVector.pushBack(&foo_1);
			pointerVector.pushBack(&foo_2);
			pointerVector.pushBack(&foo_3);
			pointerVector.pushBack(&foo_4);
			Assert::IsTrue(pointerVector.size() == 4);

			auto pointerRemoveInvalidStart = [&pointerVector, &otherPointerVector]{ pointerVector.remove(otherPointerVector.begin(), pointerVector.end()); };
			auto pointerRemoveInvalidFinish = [&pointerVector, &otherPointerVector]{ pointerVector.remove(pointerVector.begin(), otherPointerVector.end()); };
			auto pointerRemoveInvalidRange = [&pointerVector]{ pointerVector.remove(pointerVector.end(), pointerVector.begin()); };
			Assert::ExpectException<exception>(pointerRemoveInvalidStart);
			Assert::ExpectException<exception>(pointerRemoveInvalidFinish);
			Assert::ExpectException<exception>(pointerRemoveInvalidRange);

			pointerVector.remove(pointerVector.end(), ++pointerVector.end());			// ranged remove with range beyond end() is a noop
			Assert::IsTrue(pointerVector.size() == 4);

			pointerVector.remove(pointerVector.begin(), pointerVector.begin());			// ranged remove with start == finish is a noop
			Assert::IsTrue(pointerVector.size() == 4);

			pointerVector.remove(pointerVector.begin(), pointerVector.find(&foo_3));	// ranged remove with range inside Vector is valid
			Assert::IsTrue(pointerVector.size() == 2);
			Assert::IsTrue(pointerVector.front() == &foo_3);

			pointerVector.remove(pointerVector.begin(), ++pointerVector.end());			// ranged remove with finish outside vector removes remaining list beyond start
			Assert::IsTrue(pointerVector.isEmpty());
		}

		static _CrtMemState sStartMemState;

	};

	_CrtMemState VectorTest::sStartMemState;
}
