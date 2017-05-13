
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
	TEST_CLASS(SListTest)
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

		TEST_METHOD(SListDefaultConstructor)
		{
			// verify that the default constructor creates an empty list

			// with primitive type
			SList<int32_t> primitiveList;
			Assert::IsTrue(primitiveList.isEmpty());

			// with custom type
			SList<Foo> customList;
			Assert::IsTrue(customList.isEmpty());

			// with pointer type
			SList<Foo*> pointerList;
			Assert::IsTrue(pointerList.isEmpty());
		}

		TEST_METHOD(SListCopyConstructor)
		{
			// verify that copied lists contain the same data at different node addresses

			// test with primitive type
			SList<int32_t> primitiveList;
			for(uint32_t i = 0; i < 5; ++i)
			{
				primitiveList.pushFront(i);
			}

			SList<int32_t> otherPrimitiveList(primitiveList);
			CheckListEquivalency<int32_t>(primitiveList, otherPrimitiveList);

			// test with custom type
			SList<Foo> customList;
			for(uint32_t i = 0; i < 5; ++i)
			{
				customList.pushFront(Foo(i));
			}

			SList<Foo> otherCustomList(customList);
			CheckListEquivalency<Foo>(customList, otherCustomList);

			// test with pointer type
			SList<Foo*> pointerList;
			for(uint32_t i = 0; i < 5; ++i)
			{
				pointerList.pushFront(nullptr);
			}

			SList<Foo*> otherPointerList(pointerList);
			CheckListEquivalency<Foo*>(pointerList, otherPointerList);
		}

		TEST_METHOD(SListMove)
		{
			// move construction
			{
				SList<int32_t> list;
				for(uint32_t i = 0; i < 5; ++i)
				{
					list.pushBack(i);
				}
				SList<int32_t> copiedList(list);

				SList<int32_t> otherList(std::move(list));

				CheckListEquivalency(copiedList, otherList);

				Assert::IsTrue(list.isEmpty());
				Assert::IsFalse(otherList.isEmpty());
				Assert::IsTrue(otherList.size() == 5);

				auto originalFront = [&list]{ list.front(); };
				auto originalBack = [&list]{ list.back(); };
				Assert::ExpectException<exception>(originalFront);
				Assert::ExpectException<exception>(originalBack);

				int32_t i = 0;
				for(auto& data : otherList)
				{
					Assert::IsTrue(data == i);
					++i;
				}
			}

			// move assignment
			{
				SList<int32_t> list;
				for(uint32_t i = 0; i < 5; ++i)
				{
					list.pushBack(i);
				}
				SList<int32_t> copiedList(list);

				SList<int32_t> otherList;
				otherList.pushBack(100);

				otherList = std::move(list);

				CheckListEquivalency(copiedList, otherList);

				Assert::IsTrue(list.isEmpty());
				Assert::IsFalse(otherList.isEmpty());
				Assert::IsTrue(otherList.size() == 5);

				auto originalFront = [&list]{ list.front(); };
				auto originalBack = [&list]{ list.back(); };
				Assert::ExpectException<exception>(originalFront);
				Assert::ExpectException<exception>(originalBack);

				int32_t i = 0;
				for(auto& data : otherList)
				{
					Assert::IsTrue(data == i);
					++i;
				}
			}
		}

		TEST_METHOD(SListDestructor)
		{
			// when test method finishes, detects memory leaks from list deallocation

			// test primitive type
			{
				SList<int32_t> primitiveList;
				primitiveList.pushFront(0);
			}

			// test custom type
			{
				SList<Foo> customList;
				customList.pushFront(Foo(0));
			}

			// test pointer type
			{
				Foo foo;
				SList<Foo*> pointerList;
				pointerList.pushFront(&foo);
			}
		}

		TEST_METHOD(SListAssignmentOperator)
		{
			// verify that assigned lists contain the same data at different node addresses from the original

			// test with primitive type
			SList<int32_t> primitiveList;
			for(uint32_t i = 0; i < 5; ++i)
			{
				primitiveList.pushFront(i);
			}

			SList<int32_t> otherPrimitiveList;
			otherPrimitiveList = primitiveList;
			CheckListEquivalency<int32_t>(primitiveList, otherPrimitiveList);

			// test with custom type
			SList<Foo> customList;
			for(uint32_t i = 0; i < 5; ++i)
			{
				customList.pushFront(Foo(i));
			}

			SList<Foo> otherCustomList;
			otherCustomList = customList;
			CheckListEquivalency<Foo>(customList, otherCustomList);

			// test with pointer type
			SList<Foo*> pointerList;
			for(uint32_t i = 0; i < 5; ++i)
			{
				pointerList.pushFront(nullptr);
			}

			SList<Foo*> otherPointerList;
			otherPointerList = pointerList;
			CheckListEquivalency<Foo*>(pointerList, otherPointerList);
		}

		TEST_METHOD(SListPopFront)
		{
			// verify that popping front reduces the list's size

			// test with primitive type
			SList<Foo> primitiveList;
			for(uint32_t i = 0; i < 5; ++i)
			{
				primitiveList.pushBack(Foo(i));
			}

			uint32_t expectedListSize = primitiveList.size();
			while(!primitiveList.isEmpty())
			{
				primitiveList.popFront();
				expectedListSize--;
				Assert::AreEqual(expectedListSize, primitiveList.size());
			}

			// test with custom type
			SList<Foo> customList;
			for(uint32_t i = 0; i < 5; ++i)
			{
				customList.pushBack(Foo(i));
			}

			expectedListSize = customList.size();
			while(!customList.isEmpty())
			{
				customList.popFront();
				expectedListSize--;
				Assert::AreEqual(expectedListSize, customList.size());
			}

			// test with pointer type
			SList<Foo*> pointerList;
			for(uint32_t i = 0; i < 5; ++i)
			{
				pointerList.pushBack(new Foo(i));
			}

			expectedListSize = pointerList.size();
			while(!pointerList.isEmpty())
			{
				delete pointerList.front();

				pointerList.popFront();
				expectedListSize--;
				Assert::AreEqual(expectedListSize, pointerList.size());
			}
		}

		TEST_METHOD(SListFront)
		{
			// verify that getting the front of empty lists throws exceptions
			// verify that pushing front changes front

			// test with primitive type
			SList<int32_t> primitiveList;
			const SList<int32_t>& primitiveListRef = primitiveList;

			auto primitiveFront = [&primitiveList]{ primitiveList.front(); };
			auto constPrimitiveFront = [&primitiveListRef]{ primitiveListRef.front(); };
			Assert::ExpectException<exception>(primitiveFront);
			Assert::ExpectException<exception>(constPrimitiveFront);

			int32_t primitiveData = 10;
			primitiveList.pushFront(primitiveData);
			Assert::AreEqual(primitiveList.front(), primitiveData);
			Assert::IsTrue(&(primitiveList.front()) == &(primitiveList.back()));

			// test with custom type
			SList<Foo> customList;
			const SList<Foo>& customListRef = customList;

			auto customFront = [&customList]{ customList.front(); };
			auto constCustomFront = [&customListRef]{ customListRef.front(); };
			Assert::ExpectException<exception>(customFront);
			Assert::ExpectException<exception>(constCustomFront);

			Foo customData(15);
			customList.pushFront(customData);
			Assert::IsTrue(customList.front() == customData);
			Assert::IsTrue(&(customList.front()) == &(customList.back()));

			// test with pointer type
			SList<Foo*> pointerList;
			const SList<Foo*>& pointerListRef = pointerList;

			auto pointerFront = [&pointerList]{ pointerList.front(); };
			auto constPointerFront = [&pointerListRef]{ pointerListRef.front(); };
			Assert::ExpectException<exception>(pointerFront);
			Assert::ExpectException<exception>(constPointerFront);

			Foo* pointerData = &customData;
			pointerList.pushFront(pointerData);
			Assert::IsTrue(pointerList.front() == pointerData);
			Assert::IsTrue(&(pointerList.front()) == &(pointerList.back()));
		}

		TEST_METHOD(SListBack)
		{
			// verify that getting the back of empty lists throws exceptions
			// verify that pushing back changes back

			// test with primitive type
			SList<int32_t> primitiveList;
			const SList<int32_t>& primitiveListRef = primitiveList;

			auto primitiveBack = [&primitiveList]{ primitiveList.back(); };
			auto constPrimitiveBack = [&primitiveListRef]{ primitiveListRef.back(); };
			Assert::ExpectException<exception>(primitiveBack);
			Assert::ExpectException<exception>(constPrimitiveBack);

			int32_t primitiveData = 10;
			primitiveList.pushBack(primitiveData);
			Assert::AreEqual(primitiveList.back(), primitiveData);
			Assert::IsTrue(&(primitiveList.back()) == &(primitiveList.front()));

			// test with custom type
			SList<Foo> customList;
			const SList<Foo>& customListRef = customList;

			auto customBack = [&customList]{ customList.back(); };
			auto constCustomBack = [&customListRef]{ customListRef.back(); };
			Assert::ExpectException<exception>(customBack);
			Assert::ExpectException<exception>(constCustomBack);

			Foo customData(15);
			customList.pushBack(customData);
			Assert::IsTrue(customList.back() == customData);
			Assert::IsTrue(&(customList.back()) == &(customList.front()));

			// test with pointer type
			SList<Foo*> pointerList;
			const SList<Foo*>& pointerListRef = pointerList;

			auto pointerBack = [&pointerList]{ pointerList.back(); };
			auto constPointerBack = [&pointerListRef]{ pointerListRef.back(); };
			Assert::ExpectException<exception>(pointerBack);
			Assert::ExpectException<exception>(constPointerBack);

			Foo* pointerData = &customData;
			pointerList.pushBack(pointerData);
			Assert::IsTrue(pointerList.back() == pointerData);
			Assert::IsTrue(&(pointerList.back()) == &(pointerList.front()));
		}

		TEST_METHOD(SListClear)
		{
			// fill a list with some data, then clear it

			// test with primitive type
			SList<int32_t> primitiveList;
			primitiveList.pushBack(5);

			Assert::IsFalse(primitiveList.isEmpty());
			primitiveList.clear();
			Assert::IsTrue(primitiveList.isEmpty());

			// test with custom type
			SList<Foo> customList;
			customList.pushBack(Foo(5));

			Assert::IsFalse(customList.isEmpty());
			customList.clear();
			Assert::IsTrue(customList.isEmpty());

			// test with pointer type
			SList<Foo*> pointerList;
			pointerList.pushBack(nullptr);

			Assert::IsFalse(pointerList.isEmpty());
			pointerList.clear();
			Assert::IsTrue(pointerList.isEmpty());
		}

		TEST_METHOD(SListSize)
		{
			// verify that the size changes when items are added or removed

			uint32_t expectedSizeAfterPop = 0;
			uint32_t expectedSizeAfterPush = 1;

			// test with primitive type
			SList<int32_t> primitiveList;
			Assert::AreEqual(expectedSizeAfterPop, primitiveList.size());

			primitiveList.pushFront(0);
			Assert::AreEqual(expectedSizeAfterPush, primitiveList.size());

			primitiveList.popFront();
			Assert::AreEqual(expectedSizeAfterPop, primitiveList.size());

			// test with custom type
			SList<Foo> customList;
			Assert::AreEqual(expectedSizeAfterPop, customList.size());

			customList.pushFront(Foo(0));
			Assert::AreEqual(expectedSizeAfterPush, customList.size());

			customList.popFront();
			Assert::AreEqual(expectedSizeAfterPop, customList.size());

			// test with pointer type
			SList<Foo*> pointerList;
			Assert::AreEqual(expectedSizeAfterPop, pointerList.size());

			pointerList.pushFront(nullptr);
			Assert::AreEqual(expectedSizeAfterPush, pointerList.size());

			pointerList.popFront();
			Assert::AreEqual(expectedSizeAfterPop, pointerList.size());
		}

		TEST_METHOD(SListBegin)
		{
			// verify that begin is end for an empty list
			// verify that begin points to the first item in the list

			// test with primitive types
			SList<int32_t> primitiveList;
			const SList<int32_t>& constPrimitiveList = primitiveList;

			Assert::IsTrue(primitiveList.begin() == primitiveList.end());
			Assert::IsTrue(constPrimitiveList.begin() == constPrimitiveList.end());

			primitiveList.pushBack(1);
			primitiveList.pushBack(2);
			Assert::IsTrue(*primitiveList.begin() == 1);
			Assert::IsTrue(*constPrimitiveList.begin() == 1);

			// test with custom types
			SList<Foo> customList;
			const SList<Foo>& constCustomList = customList;

			Assert::IsTrue(customList.begin() == customList.end());
			Assert::IsTrue(constCustomList.begin() == constCustomList.end());

			Foo foo_1(1), foo_2(2);
			customList.pushBack(foo_1);
			customList.pushBack(foo_2);
			Assert::IsTrue(*customList.begin() == foo_1);
			Assert::IsTrue(*constCustomList.begin() == foo_1);

			// test with pointer type
			SList<Foo*> pointerList;
			const SList<Foo*>& constPointerList = pointerList;

			Assert::IsTrue(pointerList.begin() == pointerList.end());
			Assert::IsTrue(constPointerList.begin() == constPointerList.end());

			pointerList.pushBack(&foo_1);
			pointerList.pushBack(&foo_2);
			Assert::IsTrue(*pointerList.begin() == &foo_1);
			Assert::IsTrue(*constPointerList.begin() == &foo_1);
		}

		TEST_METHOD(SListEnd)
		{
			// verify that the end() Iterator points to no data (null Node, can't deref)

			// test with primitive type
			SList<int32_t> primitiveList;
			SList<int32_t>::Iterator primitiveIter = primitiveList.end();

			auto primitiveEnd = [&primitiveIter]{ *primitiveIter; };
			Assert::ExpectException<exception>(primitiveEnd);

			const SList<int32_t>::Iterator constPrimitiveIter = primitiveIter;
			auto constPrimitiveEnd = [&constPrimitiveIter]{ *constPrimitiveIter; };
			Assert::ExpectException<exception>(constPrimitiveEnd);

			// test with custom type
			SList<Foo> customList;
			SList<Foo>::Iterator customIter = customList.end();

			auto customEnd = [&customIter]{ *customIter; };
			Assert::ExpectException<exception>(customEnd);

			const SList<Foo>::Iterator constCustomIter = customIter;
			auto constCustomEnd = [&constCustomIter]{ *constCustomIter; };
			Assert::ExpectException<exception>(constCustomEnd);

			// test with pointer type
			SList<Foo*> pointerList;
			SList<Foo*>::Iterator pointerIter = pointerList.end();

			auto pointerEnd = [&pointerIter]{ *pointerIter; };
			Assert::ExpectException<exception>(pointerEnd);

			const SList<Foo*>::Iterator constPointerIter = pointerIter;
			auto constPointerEnd = [&constPointerIter]{ *constPointerIter; };
			Assert::ExpectException<exception>(constPointerEnd);
		}

		TEST_METHOD(SListInsertAfter)
		{
			// verify that inserting data to a different list throws an error
			// verify that inserting to the list's end pushesBack
			// verify that inserting after actually does anything

			// test with primitive type
			SList<int32_t> primitiveList, otherPrimitiveList;
			SList<int32_t>::Iterator primitiveIter = primitiveList.begin();

			auto primitiveInsertAfter = [&otherPrimitiveList, &primitiveIter]{ otherPrimitiveList.insertAfter(primitiveIter, 0); };
			Assert::ExpectException<exception>(primitiveInsertAfter);

			//		inserting after end()
			primitiveIter = primitiveList.end();
			primitiveList.insertAfter(primitiveIter, 1);
			Assert::IsTrue(primitiveList.back() == 1);
			Assert::IsTrue(primitiveList.size() == 1);

			//		inserting after last element
			primitiveIter = primitiveList.begin();
			primitiveList.insertAfter(primitiveIter, 2);
			Assert::IsTrue(primitiveList.back() == 2);
			Assert::IsTrue(primitiveList.size() == 2);

			//		inserting after an Iterator
			Assert::IsTrue(*primitiveList.insertAfter(primitiveIter, 3) == 3);
			Assert::IsTrue(primitiveList.size() == 3);

			// test with custom type
			Foo foo_1(1), foo_2(2), foo_3(3);
			SList<Foo> customList, otherCustomList;
			SList<Foo>::Iterator customIter = customList.begin();

			auto customInsertAfter = [&otherCustomList, &customIter]{ otherCustomList.insertAfter(customIter, Foo(0)); };
			Assert::ExpectException<exception>(customInsertAfter);

			//		inserting after end()
			customIter = customList.end();
			customList.insertAfter(customIter, foo_1);
			Assert::IsTrue(customList.back() == foo_1);
			Assert::IsTrue(customList.size() == 1);

			//		inserting after last element
			customIter = customList.begin();
			customList.insertAfter(customIter, foo_2);
			Assert::IsTrue(customList.back() == foo_2);
			Assert::IsTrue(customList.size() == 2);

			//		inserting after an Iterator
			Assert::IsTrue(*customList.insertAfter(customIter, foo_3) == 3);
			Assert::IsTrue(customList.size() == 3);

			// test with pointer type
			SList<Foo*> pointerList, otherPointerList;
			SList<Foo*>::Iterator pointerIter = pointerList.begin();

			auto pointerInsertAfter = [&otherPointerList, &pointerIter, &foo_1]{ otherPointerList.insertAfter(pointerIter, &foo_1); };
			Assert::ExpectException<exception>(pointerInsertAfter);

			//		inserting after end()
			pointerIter = pointerList.end();
			pointerList.insertAfter(pointerIter, &foo_1);
			Assert::IsTrue(pointerList.back() == &foo_1);
			Assert::IsTrue(pointerList.size() == 1);

			//		insert after last element
			pointerIter = pointerList.begin();
			pointerList.insertAfter(pointerIter, &foo_2);
			Assert::IsTrue(pointerList.back() == &foo_2);
			Assert::IsTrue(pointerList.size() == 2);

			//		insert after an Iterator
			Assert::IsTrue(*pointerList.insertAfter(pointerIter, &foo_3) == &foo_3);
			Assert::IsTrue(pointerList.size() == 3);
		}

		TEST_METHOD(SListFind)
		{
			// verify that searching for an element that does not exist in the list returns end()
			// verify that we find the first instance of the requested element

			// test with primitive type
			SList<int32_t> primitiveList;
			primitiveList.pushBack(1);
			primitiveList.pushBack(2);
			primitiveList.pushBack(1);

			SList<int32_t>::Iterator primitiveIter = primitiveList.find(3);
			Assert::IsTrue(primitiveIter == primitiveList.end());

			primitiveIter = primitiveList.find(1);
			Assert::IsTrue(*primitiveIter == 1);
			Assert::IsTrue(primitiveIter == primitiveList.begin());

			const SList<int32_t>& constPrimitiveList = primitiveList;
			const SList<int32_t>::Iterator constPrimitiveIter_1 = constPrimitiveList.find(3);
			Assert::IsTrue(constPrimitiveIter_1 == constPrimitiveList.end());

			const SList<int32_t>::Iterator constPrimitiveIter_2 = constPrimitiveList.find(1);
			Assert::IsTrue(*constPrimitiveIter_2 == 1);
			Assert::IsTrue(constPrimitiveIter_2 == constPrimitiveList.begin());

			// test with custom type
			Foo foo_1(1), foo_2(2), foo_3(3);
			SList<Foo> customList;
			customList.pushBack(foo_1);
			customList.pushBack(foo_2);
			customList.pushBack(foo_1);

			SList<Foo>::Iterator customIter = customList.find(foo_3);
			Assert::IsTrue(customIter == customList.end());

			customIter = customList.find(foo_1);
			Assert::IsTrue(*customIter == foo_1);
			Assert::IsTrue(customIter == customList.begin());

			const SList<Foo>& constCustomList = customList;
			const SList<Foo>::Iterator constCustomIter_1 = constCustomList.find(foo_3);
			Assert::IsTrue(constCustomIter_1 == constCustomList.end());

			const SList<Foo>::Iterator constCustomIter_2 = constCustomList.find(foo_1);
			Assert::IsTrue(*constCustomIter_2 == foo_1);
			Assert::IsTrue(constCustomIter_2 == constCustomList.begin());

			// test with pointer type
			SList<Foo*> pointerList;
			pointerList.pushBack(&foo_1);
			pointerList.pushBack(&foo_2);
			pointerList.pushBack(&foo_1);

			SList<Foo*>::Iterator pointerIter = pointerList.find(&foo_3);
			Assert::IsTrue(pointerIter == pointerList.end());

			pointerIter = pointerList.find(&foo_1);
			Assert::IsTrue(*pointerIter == &foo_1);
			Assert::IsTrue(pointerIter == pointerList.begin());

			const SList<Foo*>& constPointerList = pointerList;
			const SList<Foo*>::Iterator constPointerIter_1 = constPointerList.find(&foo_3);
			Assert::IsTrue(constPointerIter_1 == constPointerList.end());

			const SList<Foo*>::Iterator constPointerIter_2 = constPointerList.find(&foo_1);
			Assert::IsTrue(*constPointerIter_2 == &foo_1);
			Assert::IsTrue(constPointerIter_2 == constPointerList.begin());
		}

		TEST_METHOD(SListRemove)
		{
			// verify that removing an item that does not exist returns false and lease list unchanged
			// verify that removing an item in the middle changes the list
			// verify that removing front changes front
			// verify that removing back changes back

			// test with primitive type
			SList<int32_t> primitiveList;
			primitiveList.pushBack(1);
			primitiveList.pushBack(2);
			primitiveList.pushBack(3);
			primitiveList.pushBack(4);
			Assert::IsTrue(primitiveList.size() == 4);

			Assert::IsFalse(primitiveList.remove(20));
			Assert::IsTrue(primitiveList.size() == 4);

			Assert::IsTrue(primitiveList.remove(2));
			Assert::IsTrue(primitiveList.size() == 3);
			Assert::IsTrue(primitiveList.find(2) == primitiveList.end());

			Assert::IsTrue(primitiveList.remove(1));
			Assert::IsTrue(primitiveList.front() == 3);
			Assert::IsTrue(primitiveList.size() == 2);
			Assert::IsTrue(primitiveList.find(1) == primitiveList.end());

			Assert::IsTrue(primitiveList.remove(4));
			Assert::IsTrue(primitiveList.back() == 3);
			Assert::IsTrue(primitiveList.size() == 1);
			Assert::IsTrue(primitiveList.find(4) == primitiveList.end());

			Assert::IsTrue(primitiveList.remove(3));
			Assert::IsTrue(primitiveList.isEmpty());

			// test with custom type
			Foo foo_1(1), foo_2(2), foo_3(3), foo_4(4), foo_5(5);
			SList<Foo> customList;
			customList.pushBack(foo_1);
			customList.pushBack(foo_2);
			customList.pushBack(foo_3);
			customList.pushBack(foo_4);
			Assert::IsTrue(customList.size() == 4);

			Assert::IsFalse(customList.remove(foo_5));
			Assert::IsTrue(customList.size() == 4);

			Assert::IsTrue(customList.remove(foo_2));
			Assert::IsTrue(customList.size() == 3);

			Assert::IsTrue(customList.remove(foo_1));
			Assert::IsTrue(customList.front() == foo_3);
			Assert::IsTrue(customList.size() == 2);

			Assert::IsTrue(customList.remove(foo_4));
			Assert::IsTrue(customList.back() == foo_3);
			Assert::IsTrue(customList.size() == 1);

			Assert::IsTrue(customList.remove(foo_3));
			Assert::IsTrue(customList.isEmpty());

			// test with pointer type
			SList<Foo*> pointerList;
			pointerList.pushBack(&foo_1);
			pointerList.pushBack(&foo_2);
			pointerList.pushBack(&foo_3);
			pointerList.pushBack(&foo_4);
			Assert::IsTrue(pointerList.size() == 4);

			Assert::IsFalse(pointerList.remove(&foo_5));
			Assert::IsTrue(pointerList.size() == 4);

			Assert::IsTrue(pointerList.remove(&foo_2));
			Assert::IsTrue(pointerList.size() == 3);

			Assert::IsTrue(pointerList.remove(&foo_1));
			Assert::IsTrue(pointerList.front() == &foo_3);
			Assert::IsTrue(pointerList.size() == 2);

			Assert::IsTrue(pointerList.remove(&foo_4));
			Assert::IsTrue(pointerList.back() == &foo_3);
			Assert::IsTrue(pointerList.size() == 1);

			Assert::IsTrue(pointerList.remove(&foo_3));
			Assert::IsTrue(pointerList.isEmpty());
		}

	private:

		template <typename T>
		void CheckListEquivalency(SList<T>& originalList, SList<T>& copiedList)
		{
			Assert::AreEqual(originalList.size(), copiedList.size());

			SList<T>::Iterator originalIter = originalList.begin();
			SList<T>::Iterator copiedIter = copiedList.begin();
			for(; originalIter != originalList.end(); ++originalIter, ++copiedIter)
			{
				Assert::IsFalse(originalIter == copiedIter);
				Assert::IsTrue(*originalIter == *copiedIter);
			}
		}

		static _CrtMemState sStartMemState;

	};

	_CrtMemState SListTest::sStartMemState;
}
