
#include "pch.h"
#include "CppUnitTest.h"

#include "Foo.h"
#include "HashMap.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DOGEngine;
using namespace UnitTests;
using namespace std;

template <>
struct HashFunc<Foo>
{
	std::uint32_t operator()(const Foo& key) const
	{
		int32_t fooVal = key.getValue();
		const char* byteArray = reinterpret_cast<const char*>(&fooVal);

		uint32_t hashValue = 0;
		for(std::uint32_t i = 0; i < sizeof(fooVal); ++i)
		{
			hashValue += 13 + byteArray[i];
		}

		return hashValue;
	}
};

template <>
struct HashFunc<Foo*>
{
	std::uint32_t operator()(Foo* const& key) const
	{
		int32_t fooVal = key->getValue();
		const char* byteArray = reinterpret_cast<const char*>(&fooVal);

		uint32_t hashValue = 0;
		for(std::uint32_t i = 0; i < sizeof(fooVal); ++i)
		{
			hashValue += 13 + byteArray[i];
		}

		return hashValue;
	}
};

namespace LibraryDesktopTest
{
	TEST_CLASS(HashMapIteratorTest)
	{
	public:

		typedef pair<uint32_t, uint32_t> PrimitivePair;
		typedef pair<uint32_t*, uint32_t> PointerPair;
		typedef pair<char*, uint32_t> CharPair;
		typedef pair<string, uint32_t> StringPair;
		typedef pair<Foo, uint32_t> CustomPair;

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

		TEST_METHOD(HashMapIteratorConstructors)
		{
			// verify that default constructed iterators point to nothing
			// verify that copy constructed iterators point to something

			// test with primitive key
			HashMap<uint32_t, uint32_t> primitiveMap;

			HashMap<uint32_t, uint32_t>::Iterator defaultPrimitiveIter;
			Assert::IsTrue(defaultPrimitiveIter != primitiveMap.end());

			primitiveMap[100] = 500;
			HashMap<uint32_t, uint32_t>::Iterator copyPrimitiveIter(primitiveMap.begin());
			Assert::IsTrue(copyPrimitiveIter->first == 100);
			Assert::IsTrue(copyPrimitiveIter->second == 500);

			// test with pointer key
			HashMap<uint32_t*, uint32_t> pointerMap;

			HashMap<uint32_t*, uint32_t>::Iterator defaultPointerIter;
			Assert::IsTrue(defaultPointerIter != pointerMap.end());

			uint32_t intKey = 123456789;
			pointerMap[&intKey] = 500;
			HashMap<uint32_t*, uint32_t>::Iterator copyPointerIter(pointerMap.begin());
			Assert::IsTrue(copyPointerIter->first == &intKey);
			Assert::IsTrue(copyPointerIter->second == 500);

			// test with char* key
			HashMap<char*, uint32_t> charMap;

			HashMap<char*, uint32_t>::Iterator defaultCharIter;
			Assert::IsTrue(defaultCharIter != charMap.end());

			charMap["hello"] = 500;
			HashMap<char*, uint32_t>::Iterator copyCharIter(charMap.begin());
			Assert::IsTrue(strcmp(copyCharIter->first, "hello") == 0);
			Assert::IsTrue(copyCharIter->second == 500);

			// test with string key
			HashMap<string, uint32_t> stringMap;

			HashMap<string, uint32_t>::Iterator defaultStringIter;
			Assert::IsTrue(defaultStringIter != stringMap.end());

			stringMap["hello"] = 500;
			HashMap<string, uint32_t>::Iterator copyStringIter(stringMap.begin());
			Assert::IsTrue(strcmp(copyStringIter->first.c_str(), "hello") == 0);
			Assert::IsTrue(copyStringIter->second == 500);

			// test with custom key
			HashMap<Foo, uint32_t> customMap;

			HashMap<Foo, uint32_t>::Iterator defaultCustomMap;
			Assert::IsTrue(defaultCustomMap != customMap.end());

			customMap[Foo(250)] = 500;
			HashMap<Foo, uint32_t>::Iterator copyCustomIter(customMap.begin());
			Assert::IsTrue(copyCustomIter->first == Foo(250));
			Assert::IsTrue(copyCustomIter->second == 500);
		}

		TEST_METHOD(HashMapIteratorAssignmentOperator)
		{
			// verify that the assignment operator sets the Iterator's position

			// test with primitive key
			HashMap<uint32_t, uint32_t> primitiveMap;
			primitiveMap[10] = 100;

			HashMap<uint32_t, uint32_t>::Iterator primitiveIter = primitiveMap.begin();
			HashMap<uint32_t, uint32_t>::Iterator otherPrimitiveIter;
			otherPrimitiveIter = primitiveIter;

			Assert::IsTrue(otherPrimitiveIter->first == 10);
			Assert::IsTrue(otherPrimitiveIter->second == 100);

			// test with pointer key
			HashMap<uint32_t*, uint32_t> pointerMap;

			uint32_t intKey = 10;
			pointerMap[&intKey] = 100;

			HashMap<uint32_t*, uint32_t>::Iterator pointerIter = pointerMap.begin();
			HashMap<uint32_t*, uint32_t>::Iterator otherPointerIter;
			otherPointerIter = pointerIter;

			Assert::IsTrue(otherPointerIter->first == &intKey);
			Assert::IsTrue(otherPointerIter->second == 100);

			// test with char* key
			HashMap<char*, uint32_t> charMap;
			charMap["hello"] = 100;

			HashMap<char*, uint32_t>::Iterator charIter = charMap.begin();
			HashMap<char*, uint32_t>::Iterator otherCharIter;
			otherCharIter = charIter;

			Assert::IsTrue(strcmp(otherCharIter->first, "hello") == 0);
			Assert::IsTrue(otherCharIter->second == 100);

			// test with string key
			HashMap<string, uint32_t> stringMap;
			stringMap["hello"] = 100;

			HashMap<string, uint32_t>::Iterator stringIter = stringMap.begin();
			HashMap<string, uint32_t>::Iterator otherStringIter;
			otherStringIter = stringIter;

			Assert::IsTrue(strcmp(otherStringIter->first.c_str(), "hello") == 0);
			Assert::IsTrue(otherStringIter->second == 100);

			// test with custom key
			HashMap<Foo, uint32_t> customMap;
			customMap[Foo(50)] = 100;

			HashMap<Foo, uint32_t>::Iterator customIter = customMap.begin();
			HashMap<Foo, uint32_t>::Iterator otherCustomIter;
			otherCustomIter = customIter;

			Assert::IsTrue(otherCustomIter->first == Foo(50));
			Assert::IsTrue(otherCustomIter->second == 100);
		}

		TEST_METHOD(HashMapIteratorEqualityOperators)
		{
			// verify that the equality operator returns true if the owner and data are the same
			// verify that the inequality operator returns true if either the owner or data are not the same

			// test with primitive key
			HashMap<uint32_t, uint32_t> primitiveMap;
			HashMap<uint32_t, uint32_t> otherPrimitiveMap;
			primitiveMap[500] = 10;
			otherPrimitiveMap[500] = 10;

			HashMap<uint32_t, uint32_t>::Iterator primitiveIter = primitiveMap.begin();
			HashMap<uint32_t, uint32_t>::Iterator otherPrimitiveIter = otherPrimitiveMap.begin();

			Assert::IsTrue(primitiveIter->first == otherPrimitiveIter->first);		// different owners -- inequal (even if data is the same)
			Assert::IsTrue(primitiveIter->second == otherPrimitiveIter->second);
			Assert::IsFalse(primitiveIter == otherPrimitiveIter);
			Assert::IsTrue(primitiveIter != otherPrimitiveIter);

			otherPrimitiveIter = primitiveMap.end();
			Assert::IsFalse(primitiveIter == otherPrimitiveIter);					// same owner, different data -- inequal
			Assert::IsTrue(primitiveIter != otherPrimitiveIter);

			otherPrimitiveIter = primitiveMap.begin();
			Assert::IsTrue(primitiveIter == otherPrimitiveIter);					// same owner, same data -- equal
			Assert::IsFalse(primitiveIter != otherPrimitiveIter);

			// test with pointer key
			HashMap<uint32_t*, uint32_t> pointerMap;
			HashMap<uint32_t*, uint32_t> otherPointerMap;

			uint32_t intKey = 1000;
			pointerMap[&intKey] = 10;
			otherPointerMap[&intKey] = 10;

			HashMap<uint32_t*, uint32_t>::Iterator pointerIter = pointerMap.begin();
			HashMap<uint32_t*, uint32_t>::Iterator otherPointerIter = otherPointerMap.begin();

			Assert::IsTrue(pointerIter->first == otherPointerIter->first);		// different owners -- inequal (even if data is the same)
			Assert::IsTrue(pointerIter->second == otherPointerIter->second);
			Assert::IsFalse(pointerIter == otherPointerIter);
			Assert::IsTrue(pointerIter != otherPointerIter);

			otherPointerIter = pointerMap.end();
			Assert::IsFalse(pointerIter == otherPointerIter);					// same owner, different data -- inequal
			Assert::IsTrue(pointerIter != otherPointerIter);

			otherPointerIter = pointerMap.begin();
			Assert::IsTrue(pointerIter == otherPointerIter);					// same owner, same data -- equal
			Assert::IsFalse(pointerIter != otherPointerIter);

			// test with char* key
			HashMap<char*, uint32_t> charMap;
			HashMap<char*, uint32_t> otherCharMap;
			charMap["hello"] = 10;
			otherCharMap["hello"] = 10;

			HashMap<char*, uint32_t>::Iterator charIter = charMap.begin();
			HashMap<char*, uint32_t>::Iterator otherCharIter = otherCharMap.begin();

			Assert::IsTrue(strcmp(charIter->first, otherCharIter->first) == 0);		// different owners -- inequal (even if data is the same)
			Assert::IsTrue(charIter->second == otherCharIter->second);
			Assert::IsFalse(charIter == otherCharIter);
			Assert::IsTrue(charIter != otherCharIter);

			otherCharIter = charMap.end();
			Assert::IsFalse(charIter == otherCharIter);								// same owner, different data -- inequal
			Assert::IsTrue(charIter != otherCharIter);

			otherCharIter = charMap.begin();
			Assert::IsTrue(charIter == otherCharIter);								// same owner, same data -- equal
			Assert::IsFalse(charIter != otherCharIter);

			// test with string key
			HashMap<string, uint32_t> stringMap;
			HashMap<string, uint32_t> otherStringMap;
			stringMap["world"] = 10;
			otherStringMap["world"] = 10;

			HashMap<string, uint32_t>::Iterator stringIter = stringMap.begin();
			HashMap<string, uint32_t>::Iterator otherStringIter = otherStringMap.begin();

			Assert::IsTrue(stringIter->first == otherStringIter->first);		// different owners -- inequal (even if data is the same)
			Assert::IsTrue(stringIter->second == otherStringIter->second);
			Assert::IsFalse(stringIter == otherStringIter);
			Assert::IsTrue(stringIter != otherStringIter);

			otherStringIter = stringMap.end();
			Assert::IsFalse(stringIter == otherStringIter);						// same owner, different data -- inequal
			Assert::IsTrue(stringIter != otherStringIter);

			otherStringIter = stringMap.begin();
			Assert::IsTrue(stringIter == otherStringIter);						// same owner, same data -- equal
			Assert::IsFalse(stringIter != otherStringIter);

			// test with custom key
			HashMap<Foo, uint32_t> customMap;
			HashMap<Foo, uint32_t> otherCustomMap;
			customMap[Foo(25)] = 10;
			otherCustomMap[Foo(25)] = 10;

			HashMap<Foo, uint32_t>::Iterator customIter = customMap.begin();
			HashMap<Foo, uint32_t>::Iterator otherCustomIter = otherCustomMap.begin();

			Assert::IsTrue(customIter->first == otherCustomIter->first);		// different owners -- inequal (even if data is the same)
			Assert::IsTrue(customIter->second == otherCustomIter->second);
			Assert::IsFalse(customIter == otherCustomIter);
			Assert::IsTrue(customIter != otherCustomIter);

			otherCustomIter = customMap.end();
			Assert::IsFalse(customIter == otherCustomIter);						// same owner, different data -- inequal
			Assert::IsTrue(customIter != otherCustomIter);

			otherCustomIter = customMap.begin();
			Assert::IsTrue(customIter == otherCustomIter);						// same owner, same data -- equal
			Assert::IsFalse(customIter != otherCustomIter);
		}

		TEST_METHOD(HashMapIteratorIncrementOperators)
		{
			// verify that incrementing unowned Iterators throw exceptions 
			// verify that the prefix increment returns the changed Iterator
			// verify that the postfix increment returns the unchanged Iterator but changes the Iterator
			// verify that incrementing an end() Iterator is a noop

			// test with primitive type
			HashMap<uint32_t, uint32_t> primitiveMap;
			primitiveMap[10] = 100;

			HashMap<uint32_t, uint32_t>::Iterator primitiveIter;
			auto primitivePrefixIncrement = [&primitiveIter]{ ++primitiveIter; };
			auto primitivePostfixIncrement = [&primitiveIter]{ primitiveIter++; };
			Assert::ExpectException<exception>(primitivePrefixIncrement);
			Assert::ExpectException<exception>(primitivePostfixIncrement);
			
			primitiveIter = primitiveMap.begin();
			Assert::IsTrue(*(primitiveIter++) == PrimitivePair(10, 100));		// postfix returns the unchanged Iterator
			Assert::IsTrue(primitiveIter == primitiveMap.end());				// but changes the Iterator itself

			primitiveIter = primitiveMap.begin();
			Assert::IsTrue(++primitiveIter == primitiveMap.end());				// prefix returns the changed Iterator

			Assert::IsTrue(primitiveIter++ == primitiveMap.end());				// any increment on an end() Iterator is noop
			Assert::IsTrue(primitiveIter == primitiveMap.end());
			Assert::IsTrue(++primitiveIter == primitiveMap.end());

			// test with pointer type
			HashMap<uint32_t*, uint32_t> pointerMap;

			uint32_t intKey = 10;
			pointerMap[&intKey] = 100;

			HashMap<uint32_t*, uint32_t>::Iterator pointerIter;
			auto pointerPrefixIncrement = [&pointerIter]{ ++pointerIter; };
			auto pointerPostfixIncrement = [&pointerIter]{ pointerIter++; };
			Assert::ExpectException<exception>(pointerPrefixIncrement);
			Assert::ExpectException<exception>(pointerPostfixIncrement);
			
			pointerIter = pointerMap.begin();
			Assert::IsTrue(*(pointerIter++) == PointerPair(&intKey, 100));	// postfix returns the unchanged Iterator
			Assert::IsTrue(pointerIter == pointerMap.end());				// but changes the Iterator itself

			pointerIter = pointerMap.begin();
			Assert::IsTrue(++pointerIter == pointerMap.end());				// prefix returns the changed Iterator

			Assert::IsTrue(pointerIter++ == pointerMap.end());				// any increment on an end() Iterator is noop
			Assert::IsTrue(pointerIter == pointerMap.end());
			Assert::IsTrue(++pointerIter == pointerMap.end());

			// test with char* type
			HashMap<char*, uint32_t> charMap;
			charMap["hello"] = 100;

			HashMap<char*, uint32_t>::Iterator charIter;
			auto charPrefixIncrement = [&charIter]{ ++charIter; };
			auto charPostfixIncrement = [&charIter]{ charIter++; };
			Assert::ExpectException<exception>(charPrefixIncrement);
			Assert::ExpectException<exception>(charPostfixIncrement);

			charIter = charMap.begin();
			Assert::IsTrue(*(charIter++) == CharPair("hello", 100));	// postfix returns the unchanged Iterator
			Assert::IsTrue(charIter == charMap.end());					// but changes the Iterator itself

			charIter = charMap.begin();
			Assert::IsTrue(++charIter == charMap.end());				// prefix returns the changed Iterator

			Assert::IsTrue(charIter++ == charMap.end());				// any increment on an end() Iterator is noop
			Assert::IsTrue(charIter == charMap.end());
			Assert::IsTrue(++charIter == charMap.end());

			// test with string type
			HashMap<string, uint32_t> stringMap;
			stringMap["amigo"] = 100;

			HashMap<string, uint32_t>::Iterator stringIter;
			auto stringPrefixIncrement = [&stringIter]{ ++stringIter; };
			auto stringPostfixIncrement = [&stringIter]{ stringIter++; };
			Assert::ExpectException<exception>(stringPrefixIncrement);
			Assert::ExpectException<exception>(stringPostfixIncrement);

			stringIter = stringMap.begin();
			Assert::IsTrue(*(stringIter++) == StringPair("amigo", 100));	// postfix returns the unchanged Iterator
			Assert::IsTrue(stringIter == stringMap.end());					// but changes the Iterator itself

			stringIter = stringMap.begin();
			Assert::IsTrue(++stringIter == stringMap.end());				// prefix returns the changed Iterator

			Assert::IsTrue(stringIter++ == stringMap.end());				// any increment on an end() Iterator is noop
			Assert::IsTrue(stringIter == stringMap.end());
			Assert::IsTrue(++stringIter == stringMap.end());

			// test with custom type
			HashMap<Foo, uint32_t> customMap;
			customMap[Foo(13)] = 100;

			HashMap<Foo, uint32_t>::Iterator customIter;
			auto customPrefixIncrement = [&customIter]{ ++customIter; };
			auto customPostfixIncrement = [&customIter]{ customIter++; };
			Assert::ExpectException<exception>(customPrefixIncrement);
			Assert::ExpectException<exception>(customPostfixIncrement);

			customIter = customMap.begin();
			Assert::IsTrue(*(customIter++) == CustomPair(Foo(13), 100));	// postfix returns the unchanged Iterator
			Assert::IsTrue(customIter == customMap.end());					// but changes the Iterator itself

			customIter = customMap.begin();
			Assert::IsTrue(++customIter == customMap.end());				// prefix returns the changed Iterator

			Assert::IsTrue(customIter++ == customMap.end());				// any increment on an end() Iterator is noop
			Assert::IsTrue(customIter == customMap.end());
			Assert::IsTrue(++customIter == customMap.end());
		}

		TEST_METHOD(HashMapIteratorDereferenceOperators)
		{
			// verify that both types of Iterator dereference work
			testStarDereference();
			testArrowDereference();
		}

	private:

		void testStarDereference()
		{
			// verify that dereferencing an ownerless Iterator throws an exception
			// verify that dereferencing the end() throws an exception
			// verify that dereferencing a valid Iterator returns data

			// test with primitive key
			HashMap<uint32_t, uint32_t> primitiveMap;
			const HashMap<uint32_t, uint32_t>& constPrimitiveMap = primitiveMap;
			HashMap<uint32_t, uint32_t>::Iterator primitiveIter;
			const HashMap<uint32_t, uint32_t>::Iterator& constPrimitiveIter = primitiveIter;

			auto primitiveDeref = [&primitiveIter]{ *primitiveIter; };							// deref ownerless Iterator -- exception
			auto constPrimitiveDeref = [&constPrimitiveIter]{ *constPrimitiveIter; };
			Assert::ExpectException<exception>(primitiveDeref);
			Assert::ExpectException<exception>(constPrimitiveDeref);

			auto primitiveEndDeref = [&primitiveMap]{ *(primitiveMap.end()); };					// deref end() / invalid Iterator -- exception
			auto constPrimitiveEndDeref = [&constPrimitiveMap]{ *(constPrimitiveMap.end()); };
			Assert::ExpectException<exception>(primitiveEndDeref);
			Assert::ExpectException<exception>(constPrimitiveEndDeref);

			primitiveMap[500] = 250;
			Assert::IsTrue(*(primitiveMap.begin()) == PrimitivePair(500, 250));					// deref valid Iterator -- returns data
			Assert::IsTrue(*(constPrimitiveMap.begin()) == PrimitivePair(500, 250));

			// test with pointer key
			HashMap<uint32_t*, uint32_t> pointerMap;
			const HashMap<uint32_t*, uint32_t>& constPointerMap = pointerMap;
			HashMap<uint32_t*, uint32_t>::Iterator pointerIter;
			const HashMap<uint32_t*, uint32_t>::Iterator& constPointerIter = pointerIter;

			auto pointerDeref = [&pointerIter]{ *pointerIter; };							// deref ownerless Iterator -- exception
			auto constPointerDeref = [&constPointerIter]{ *constPointerIter; };
			Assert::ExpectException<exception>(pointerDeref);
			Assert::ExpectException<exception>(constPointerDeref);

			auto pointerEndDeref = [&pointerMap]{ *(pointerMap.end()); };					// deref end() / invalid Iterator -- exception
			auto constPointerEndDeref = [&constPointerMap]{ *(constPointerMap.end()); };
			Assert::ExpectException<exception>(pointerEndDeref);
			Assert::ExpectException<exception>(constPointerEndDeref);

			uint32_t intKey = 500;
			pointerMap[&intKey] = 250;
			Assert::IsTrue(*(pointerMap.begin()) == PointerPair(&intKey, 250));				// deref valid Iterator -- returns data
			Assert::IsTrue(*(constPointerMap.begin()) == PointerPair(&intKey, 250));

			// test with char* key
			HashMap<char*, uint32_t> charMap;
			const HashMap<char*, uint32_t>& constCharMap = charMap;
			HashMap<char*, uint32_t>::Iterator charIter;
			const HashMap<char*, uint32_t>::Iterator& constCharIter = charIter;

			auto charDeref = [&charIter]{ *charIter; };									// deref ownerless Iterator -- exception
			auto constCharDeref = [&constCharIter]{ *constCharIter; };
			Assert::ExpectException<exception>(charDeref);
			Assert::ExpectException<exception>(constCharDeref);

			auto charEndDeref = [&charMap]{ *(charMap.end()); };						// deref end() / invalid Iterator -- exception
			auto constCharEndDeref = [&constCharMap]{ *(constCharMap.end()); };
			Assert::ExpectException<exception>(charEndDeref);
			Assert::ExpectException<exception>(constCharEndDeref);

			charMap["banana"] = 250;
			Assert::IsTrue(*(charMap.begin()) == CharPair("banana", 250));				// deref valid Iterator -- returns data
			Assert::IsTrue(*(constCharMap.begin()) == CharPair("banana", 250));

			// test with string key
			HashMap<string, uint32_t> stringMap;
			const HashMap<string, uint32_t>& constStringMap = stringMap;
			HashMap<string, uint32_t>::Iterator stringIter;
			const HashMap<string, uint32_t>::Iterator& constStringIter = stringIter;

			auto stringDeref = [&stringIter]{ *stringIter; };								// deref ownerless Iterator -- exception
			auto constStringDeref = [&constStringIter]{ *constStringIter; };
			Assert::ExpectException<exception>(stringDeref);
			Assert::ExpectException<exception>(constStringDeref);

			auto stringEndDeref = [&stringMap]{ *(stringMap.end()); };						// deref end() / invalid Iterator -- exception
			auto constStringEndDeref = [&constStringMap]{ *(constStringMap.end()); };
			Assert::ExpectException<exception>(stringEndDeref);
			Assert::ExpectException<exception>(constStringEndDeref);

			stringMap["tree"] = 250;
			Assert::IsTrue(*(stringMap.begin()) == StringPair("tree", 250));				// deref valid Iterator -- returns data
			Assert::IsTrue(*(constStringMap.begin()) == StringPair("tree", 250));

			// test with custom key
			HashMap<Foo, uint32_t> customMap;
			const HashMap<Foo, uint32_t>& constCustomMap = customMap;
			HashMap<Foo, uint32_t>::Iterator customIter;
			const HashMap<Foo, uint32_t>::Iterator& constCustomIter = customIter;

			auto customDeref = [&customIter]{ *customIter; };								// deref ownerless Iterator -- exception
			auto constCustomDeref = [&constCustomIter]{ *constCustomIter; };
			Assert::ExpectException<exception>(customDeref);
			Assert::ExpectException<exception>(constCustomDeref);

			auto customEndDeref = [&customMap]{ *(customMap.end()); };						// deref end() / invalid Iterator -- exception
			auto constCustomEndDeref = [&constCustomMap]{ *(constCustomMap.end()); };
			Assert::ExpectException<exception>(customEndDeref);
			Assert::ExpectException<exception>(constCustomEndDeref);

			customMap[Foo(123)] = 250;
			Assert::IsTrue(*(customMap.begin()) == CustomPair(Foo(123), 250));				// deref valid Iterator -- returns data
			Assert::IsTrue(*(constCustomMap.begin()) == CustomPair(Foo(123), 250));
		}

		void testArrowDereference()
		{
			// verify that dereferencing an ownerless Iterator throws an exception
			// verify that dereferencing the end() throws an exception
			// verify that dereferencing a valid Iterator returns data

			// test with primitive key
			HashMap<uint32_t, uint32_t> primitiveMap;
			const HashMap<uint32_t, uint32_t>& constPrimitiveMap = primitiveMap;
			HashMap<uint32_t, uint32_t>::Iterator primitiveIter;
			const HashMap<uint32_t, uint32_t>::Iterator& constPrimitiveIter = primitiveIter;

			auto primitiveDeref = [&primitiveIter]{ primitiveIter->first; };						// deref ownerless Iterator -- exception
			auto constPrimitiveDeref = [&constPrimitiveIter]{ constPrimitiveIter->first; };
			Assert::ExpectException<exception>(primitiveDeref);
			Assert::ExpectException<exception>(constPrimitiveDeref);

			auto primitiveEndDeref = [&primitiveMap]{ primitiveMap.end()->first; };					// deref end() / invalid Iterator -- exception
			auto constPrimitiveEndDeref = [&constPrimitiveMap]{ constPrimitiveMap.end()->first; };
			Assert::ExpectException<exception>(primitiveEndDeref);
			Assert::ExpectException<exception>(constPrimitiveEndDeref);

			primitiveMap[500] = 250;
			Assert::IsTrue(primitiveMap.begin()->first == 500);										// deref valid Iterator -- returns data
			Assert::IsTrue(primitiveMap.begin()->second == 250);
			Assert::IsTrue(constPrimitiveMap.begin()->first == 500);
			Assert::IsTrue(constPrimitiveMap.begin()->second == 250);

			// test with pointer key
			HashMap<uint32_t*, uint32_t> pointerMap;
			const HashMap<uint32_t*, uint32_t>& constPointerMap = pointerMap;
			HashMap<uint32_t*, uint32_t>::Iterator pointerIter;
			const HashMap<uint32_t*, uint32_t>::Iterator& constPointerIter = pointerIter;

			auto pointerDeref = [&pointerIter]{ pointerIter->first; };							// deref ownerless Iterator -- exception
			auto constPointerDeref = [&constPointerIter]{ constPointerIter->first; };
			Assert::ExpectException<exception>(pointerDeref);
			Assert::ExpectException<exception>(constPointerDeref);

			auto pointerEndDeref = [&pointerMap]{ pointerMap.end()->first; };					// deref end() / invalid Iterator -- exception
			auto constPointerEndDeref = [&constPointerMap]{ constPointerMap.end()->first; };
			Assert::ExpectException<exception>(pointerEndDeref);
			Assert::ExpectException<exception>(constPointerEndDeref);

			uint32_t intKey = 500;
			pointerMap[&intKey] = 250;
			Assert::IsTrue(pointerMap.begin()->first == &intKey);								// deref valid Iterator -- returns data
			Assert::IsTrue(pointerMap.begin()->second == 250);
			Assert::IsTrue(constPointerMap.begin()->first == &intKey);
			Assert::IsTrue(constPointerMap.begin()->second == 250);

			// test with char* key
			HashMap<char*, uint32_t> charMap;
			const HashMap<char*, uint32_t>& constCharMap = charMap;
			HashMap<char*, uint32_t>::Iterator charIter;
			const HashMap<char*, uint32_t>::Iterator& constCharIter = charIter;

			auto charDeref = [&charIter]{ charIter->first; };								// deref ownerless Iterator -- exception
			auto constCharDeref = [&constCharIter]{ constCharIter->first; };
			Assert::ExpectException<exception>(charDeref);
			Assert::ExpectException<exception>(constCharDeref);

			auto charEndDeref = [&charMap]{ charMap.end()->first; };						// deref end() / invalid Iterator -- exception
			auto constCharEndDeref = [&constCharMap]{ constCharMap.end()->first; };
			Assert::ExpectException<exception>(charEndDeref);
			Assert::ExpectException<exception>(constCharEndDeref);

			charMap["banana"] = 250;
			Assert::IsTrue(strcmp(charMap.begin()->first, "banana") == 0);					// deref valid Iterator -- returns data
			Assert::IsTrue(charMap.begin()->second == 250);
			Assert::IsTrue(strcmp(constCharMap.begin()->first, "banana") == 0);
			Assert::IsTrue(constCharMap.begin()->second == 250);

			// test with string key
			HashMap<string, uint32_t> stringMap;
			const HashMap<string, uint32_t>& constStringMap = stringMap;
			HashMap<string, uint32_t>::Iterator stringIter;
			const HashMap<string, uint32_t>::Iterator& constStringIter = stringIter;

			auto stringDeref = [&stringIter]{ stringIter->first; };							// deref ownerless Iterator -- exception
			auto constStringDeref = [&constStringIter]{ constStringIter->first; };
			Assert::ExpectException<exception>(stringDeref);
			Assert::ExpectException<exception>(constStringDeref);

			auto stringEndDeref = [&stringMap]{ stringMap.end()->first; };					// deref end() / invalid Iterator -- exception
			auto constStringEndDeref = [&constStringMap]{ constStringMap.end()->first; };
			Assert::ExpectException<exception>(stringEndDeref);
			Assert::ExpectException<exception>(constStringEndDeref);

			stringMap["banana"] = 250;
			Assert::IsTrue(stringMap.begin()->first == "banana");							// deref valid Iterator -- returns data
			Assert::IsTrue(stringMap.begin()->second == 250);
			Assert::IsTrue(constStringMap.begin()->first == "banana");
			Assert::IsTrue(constStringMap.begin()->second == 250);

			// test with custom key
			HashMap<Foo, uint32_t> customMap;
			const HashMap<Foo, uint32_t>& constCustomMap = customMap;
			HashMap<Foo, uint32_t>::Iterator customIter;
			const HashMap<Foo, uint32_t>::Iterator& constCustomIter = customIter;

			auto customDeref = [&customIter]{ customIter->first; };							// deref ownerless Iterator -- exception
			auto constCustomDeref = [&constCustomIter]{ constCustomIter->first; };
			Assert::ExpectException<exception>(customDeref);
			Assert::ExpectException<exception>(constCustomDeref);

			auto customEndDeref = [&customMap]{ customMap.end()->first; };					// deref end() / invalid Iterator -- exception
			auto constCustomEndDeref = [&constCustomMap]{ constCustomMap.end()->first; };
			Assert::ExpectException<exception>(customEndDeref);
			Assert::ExpectException<exception>(constCustomEndDeref);

			customMap[Foo(123)] = 250;
			Assert::IsTrue(customMap.begin()->first == Foo(123));							// deref valid Iterator -- returns data
			Assert::IsTrue(customMap.begin()->second == 250);
			Assert::IsTrue(constCustomMap.begin()->first == Foo(123));
			Assert::IsTrue(constCustomMap.begin()->second == 250);
		}

		static _CrtMemState sStartMemState;

	};

	_CrtMemState HashMapIteratorTest::sStartMemState;
}

