
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

namespace LibraryDesktopTest
{
	TEST_CLASS(HashMapTest)
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

		TEST_METHOD(HashMapConstructor)
		{
			// verify that the default constructor creates an empty Hashmap
			// verify that the constructor throws an exception with 0 buckets

			// test with primitive key type
			auto primitiveConstructor = []{ HashMap<int32_t, int32_t> primitiveTest(0); };
			Assert::ExpectException<exception>(primitiveConstructor);

			HashMap<int32_t, int32_t> primitiveMap;
			Assert::IsTrue(primitiveMap.size() == 0);

			// test with pointer key type
			auto pointerConstructor = []{ HashMap<int32_t*, int32_t> pointerTest(0); };
			Assert::ExpectException<exception>(pointerConstructor);

			HashMap<int32_t*, int32_t> pointerMap;
			Assert::IsTrue(pointerMap.size() == 0);

			// test with char* type
			auto charConstructor = []{ HashMap<char*, int32_t> charTest(0); };
			Assert::ExpectException<exception>(charConstructor);

			HashMap<char*, int32_t> charMap;
			Assert::IsTrue(charMap.size() == 0);

			// test with string type
			auto stringConstructor = []{ HashMap<string, int32_t> stringTest(0); };
			Assert::ExpectException<exception>(stringConstructor);

			HashMap<string, int32_t> stringMap;
			Assert::IsTrue(stringMap.size() == 0);

			// test with custom type
			auto customConstructor = []{ HashMap<Foo, int32_t> customTest(0); };
			Assert::ExpectException<exception>(customConstructor);

			HashMap<Foo, int32_t> customMap;
			Assert::IsTrue(customMap.size() == 0);
		}

		TEST_METHOD(HashMapCopyConstructor)
		{
			// verify that copy construction deep copies the data with no memory leaks

			// test with primitive key
			HashMap<uint32_t, uint32_t> primitiveMap;
			for(uint32_t i = 0; i < 10; ++i)
			{
				primitiveMap[i] = i + 5;
			}

			HashMap<uint32_t, uint32_t> otherPrimitiveMap(primitiveMap);
			checkHashMapEquality(primitiveMap, otherPrimitiveMap);

			// test with pointer key
			HashMap<uint32_t*, uint32_t> pointerMap;
			for(uint32_t i = 0; i < 10; ++i)
			{
				pointerMap[&primitiveMap[i]] = i + 5;
			}

			HashMap<uint32_t*, uint32_t> otherPointerMap(pointerMap);
			checkHashMapEquality(pointerMap, otherPointerMap);

			// test with char* key
			HashMap<const char*, uint32_t> charMap;

			string helperString;
			for(uint32_t i = 0; i < 10; ++i)
			{
				helperString = (char)i;
				charMap[helperString.c_str()] = i + 5;
			}

			HashMap<const char*, uint32_t> otherCharMap(charMap);
			checkHashMapEquality(charMap, otherCharMap);

			// test with string key
			HashMap<string, uint32_t> stringMap;
			for(uint32_t i = 0; i < 10; ++i)
			{
				helperString = (char)i;
				stringMap[helperString] = i + 5;
			}

			HashMap<string, uint32_t> otherStringMap(stringMap);
			checkHashMapEquality(stringMap, otherStringMap);

			// test with custom key
			HashMap<Foo, uint32_t> customMap;
			for(uint32_t i = 0; i < 10; ++i)
			{
				customMap[Foo(i)] = i + 5;
			}

			HashMap<Foo, uint32_t> otherCustomMap(customMap);
			checkHashMapEquality(customMap, otherCustomMap);
		}

		TEST_METHOD(HashMapMove)
		{
			// move constructor
			{
				HashMap<string, int32_t> testMap;
				testMap["hello"] = 100;
				testMap["world"] = 200;
				testMap["error"] = 300;
				HashMap<string, int32_t> copiedMap(testMap);

				HashMap<string, int32_t> otherMap(std::move(testMap));

				Assert::IsTrue(otherMap == copiedMap);

				Assert::IsTrue(testMap.isEmpty());
				Assert::IsTrue(otherMap.size() == 3);

				Assert::IsTrue(otherMap["hello"] == 100);
				Assert::IsTrue(otherMap["world"] == 200);
				Assert::IsTrue(otherMap["error"] == 300);

				Assert::IsTrue(testMap["hello"] != 100);
				Assert::IsTrue(testMap["world"] != 200);
				Assert::IsTrue(testMap["error"] != 300);
			}

			// move assignment
			{
				HashMap<string, int32_t> testMap;
				testMap["hello"] = 100;
				testMap["world"] = 200;
				testMap["error"] = 300;
				HashMap<string, int32_t> copiedMap(testMap);

				HashMap<string, int32_t> otherMap;
				otherMap["other"] = 400;

				otherMap = std::move(testMap);

				Assert::IsTrue(otherMap == copiedMap);

				Assert::IsTrue(testMap.isEmpty());
				Assert::IsTrue(otherMap.size() == 3);

				Assert::IsTrue(otherMap["hello"] == 100);
				Assert::IsTrue(otherMap["world"] == 200);
				Assert::IsTrue(otherMap["error"] == 300);

				Assert::IsTrue(testMap["hello"] != 100);
				Assert::IsTrue(testMap["world"] != 200);
				Assert::IsTrue(testMap["error"] != 300);
			}
		}

		TEST_METHOD(HashMapDestructor)
		{
			// verify that destruction of Hashmap does not result in memory leaks

			// test with primitive key type
			{
				HashMap<uint32_t, uint32_t> primitiveMap;
				primitiveMap.insert(PrimitivePair(0, 1));
				primitiveMap.insert(PrimitivePair(1, 1));
				primitiveMap.insert(PrimitivePair(2, 1));
			}

			// test with pointer key type
			{
				HashMap<uint32_t*, uint32_t> pointerMap;

				uint32_t key_1 = 1, key_2 = 2, key_3 = 3;
				pointerMap.insert(PointerPair(&key_1, 1));
				pointerMap.insert(PointerPair(&key_2, 2));
				pointerMap.insert(PointerPair(&key_3, 3));
			}

			// test with char* key type
			{
				HashMap<char*, uint32_t> charMap;
				charMap.insert(CharPair("hello", 1));
				charMap.insert(CharPair("world", 2));
				charMap.insert(CharPair("apple", 3));
			}

			// test with string key type
			{
				HashMap<string, uint32_t> stringMap;
				stringMap.insert(StringPair("hello", 1));
				stringMap.insert(StringPair("world", 2));
				stringMap.insert(StringPair("apple", 3));
			}

			// test with custom key type
			{
				HashMap<Foo, uint32_t> customMap;

				Foo foo_1(1), foo_2(2), foo_3(3);
				customMap.insert(CustomPair(foo_1, 1));
				customMap.insert(CustomPair(foo_2, 2));
				customMap.insert(CustomPair(foo_3, 3));
			}
		}

		TEST_METHOD(HashMapAssignmentOperator)
		{
			// verify that assignment deep copies the Hashmap with no memory leaks

			// test with primitive key
			HashMap<uint32_t, uint32_t> primitiveMap(7);
			for(uint32_t i = 0; i < 10; ++i)
			{
				primitiveMap[i] = i + 5;
			}

			HashMap<uint32_t, uint32_t> otherPrimitiveMap;
			for(uint32_t i = 0; i < 5; ++i)
			{
				otherPrimitiveMap[i + 7] = i + 7;
			}

			otherPrimitiveMap = primitiveMap;
			checkHashMapEquality(primitiveMap, otherPrimitiveMap);

			// test with pointer key
			HashMap<uint32_t*, uint32_t> pointerMap(7);
			for(uint32_t i = 0; i < 10; ++i)
			{
				pointerMap[&primitiveMap[i]] = i + 5;
			}

			HashMap<uint32_t*, uint32_t> otherPointerMap;
			for(uint32_t i = 0; i < 5; ++i)
			{
				otherPointerMap[&primitiveMap[i]] = i + 15;
			}

			otherPointerMap = pointerMap;
			checkHashMapEquality(pointerMap, otherPointerMap);

			// test with char* key
			HashMap<const char*, uint32_t> charMap(7);

			string helperString;
			for(uint32_t i = 0; i < 10; ++i)
			{
				helperString = (char)i;
				charMap[helperString.c_str()] = i;
			}

			HashMap<const char*, uint32_t> otherCharMap;
			otherCharMap["hello"] = 12345;
			otherCharMap["world"] = 67890;

			otherCharMap = charMap;
			checkHashMapEquality(charMap, otherCharMap);

			// test with string key
			HashMap<string, uint32_t> stringMap(7);
			for(uint32_t i = 0; i < 10; ++i)
			{
				helperString = (char)i;
				stringMap[helperString] = i;
			}

			HashMap<string, uint32_t> otherStringMap;
			otherStringMap["hello"] = 12;
			otherStringMap["darkness"] = 34;
			otherStringMap["my"] = 56;
			otherStringMap["old"] = 78;
			otherStringMap["friend"] = 90;

			otherStringMap = stringMap;
			checkHashMapEquality(stringMap, otherStringMap);

			// test with custom key
			HashMap<Foo, uint32_t> customMap(7);
			for(uint32_t i = 0; i < 10; ++i)
			{
				customMap[Foo(i << i)] = i;
			}

			HashMap<Foo, uint32_t> otherCustomMap;
			for(uint32_t i = 0; i < 5; ++i)
			{
				otherCustomMap[Foo(i)] = i + 12;
			}

			otherCustomMap = customMap;
			checkHashMapEquality(customMap, otherCustomMap);
		}

		TEST_METHOD(HashMapBracketsOperator)
		{
			// verify that brackets operator creates new entries in the list
			// verify that the brackets operator can assign to a value already in the list
			// verify that brackets operator returns the correct value
			// verify that the const brackets operator throws an exception for nonexistant items
			// verify that the const brackets operator returns the correct value

			// test with primitive key
			HashMap<uint32_t, Foo> primitiveMap;

			uint32_t intKey_1 = 1, intKey_2 = 2;
			Foo fooVal_1(50), fooVal_2(100), fooVal_3(150);

			Assert::IsFalse(primitiveMap.containsKey(intKey_1));
			primitiveMap[intKey_1];
			Assert::IsTrue(primitiveMap.containsKey(intKey_1));

			primitiveMap[intKey_1] = fooVal_1;
			Assert::IsTrue(primitiveMap.find(intKey_1)->second == fooVal_1);

			primitiveMap[intKey_1] = fooVal_2;
			Assert::IsTrue(primitiveMap.find(intKey_1)->second == fooVal_2);

			const HashMap<uint32_t, Foo>& constPrimitiveMap = primitiveMap;
			auto constPrimitiveBrackets = [&constPrimitiveMap, &intKey_2]{ constPrimitiveMap[intKey_2]; };
			Assert::ExpectException<exception>(constPrimitiveBrackets);

			primitiveMap[intKey_2] = fooVal_3;
			Assert::IsTrue(constPrimitiveMap[intKey_2] == fooVal_3);

			// test with pointer key
			HashMap<uint32_t*, Foo> pointerMap;

			Assert::IsFalse(pointerMap.containsKey(&intKey_1));
			pointerMap[&intKey_1];
			Assert::IsTrue(pointerMap.containsKey(&intKey_1));

			pointerMap[&intKey_1] = fooVal_1;
			Assert::IsTrue(pointerMap.find(&intKey_1)->second == fooVal_1);

			pointerMap[&intKey_1] = fooVal_2;
			Assert::IsTrue(pointerMap.find(&intKey_1)->second == fooVal_2);

			const HashMap<uint32_t*, Foo>& constPointerMap = pointerMap;
			auto constPointerBrackets = [&constPointerMap, &intKey_2]{ constPointerMap[&intKey_2]; };
			Assert::ExpectException<exception>(constPointerBrackets);

			pointerMap[&intKey_2] = fooVal_3;
			Assert::IsTrue(constPointerMap[&intKey_2] == fooVal_3);

			// test with char key
			HashMap<char*, Foo> charMap;

			Assert::IsFalse(charMap.containsKey("hello"));
			charMap["hello"];
			Assert::IsTrue(charMap.containsKey("hello"));

			charMap["hello"] = fooVal_1;
			Assert::IsTrue(charMap.find("hello")->second == fooVal_1);

			charMap["hello"] = fooVal_2;
			Assert::IsTrue(charMap.find("hello")->second == fooVal_2);

			const HashMap<char*, Foo>& constCharMap = charMap;
			auto constCharBrackets = [&constCharMap]{ constCharMap["world"]; };
			Assert::ExpectException<exception>(constCharBrackets);

			charMap["world"] = fooVal_3;
			Assert::IsTrue(constCharMap["world"] == fooVal_3);

			// test with string key
			HashMap<string, Foo> stringMap;

			Assert::IsFalse(stringMap.containsKey("hello"));
			stringMap["hello"];
			Assert::IsTrue(stringMap.containsKey("hello"));

			stringMap["hello"] = fooVal_1;
			Assert::IsTrue(stringMap.find("hello")->second == fooVal_1);

			stringMap["hello"] = fooVal_2;
			Assert::IsTrue(stringMap.find("hello")->second == fooVal_2);

			const HashMap<string, Foo>& constStringMap = stringMap;
			auto constStringBrackets = [&constStringMap]{ constStringMap["world"]; };
			Assert::ExpectException<exception>(constStringBrackets);

			stringMap["world"] = fooVal_3;
			Assert::IsTrue(constStringMap["world"] == fooVal_3);

			// test with custom key
			HashMap<Foo, Foo> customMap;

			Assert::IsFalse(customMap.containsKey(Foo(1)));
			customMap[Foo(1)];
			Assert::IsTrue(customMap.containsKey(Foo(1)));

			customMap[Foo(1)] = fooVal_1;
			Assert::IsTrue(customMap.find(Foo(1))->second == fooVal_1);

			customMap[Foo(1)] = fooVal_2;
			Assert::IsTrue(customMap.find(Foo(1))->second == fooVal_2);

			const HashMap<Foo, Foo>& constCustomMap = customMap;
			auto constCustomBrackets = [&constCustomMap]{ constCustomMap[Foo(10)]; };
			Assert::ExpectException<exception>(constCustomBrackets);

			customMap[Foo(10)] = fooVal_3;
			Assert::IsTrue(constCustomMap[Foo(10)] == fooVal_3);
		}

		TEST_METHOD(HashMapAt)
		{
			// verify that at throws an exception when requested item is not in the list
			// verify that at returns the correct item

			// test with primitive key
			HashMap<uint32_t, uint32_t> primitiveMap;
			const HashMap<uint32_t, uint32_t>& constPrimitiveMap = primitiveMap;

			uint32_t intKey_1 = 1, intKey_2 = 2;

			auto primitiveAt = [&primitiveMap, &intKey_1]{ primitiveMap.at(intKey_1); };
			auto constPrimitiveAt = [&constPrimitiveMap, &intKey_1]{ constPrimitiveMap.at(intKey_1); };
			Assert::ExpectException<exception>(primitiveAt);
			Assert::ExpectException<exception>(constPrimitiveAt);

			primitiveMap[intKey_1] = 250;
			primitiveMap[intKey_2] = 500;
			Assert::IsTrue(primitiveMap.at(intKey_1).second == 250);
			Assert::IsTrue(primitiveMap.at(intKey_2).second == 500);
			Assert::IsTrue(constPrimitiveMap.at(intKey_1).second == 250);
			Assert::IsTrue(constPrimitiveMap.at(intKey_2).second == 500);

			// test with pointer key
			HashMap<uint32_t*, uint32_t> pointerMap;
			const HashMap<uint32_t*, uint32_t>& constPointerMap = pointerMap;

			auto pointerAt = [&pointerMap, &intKey_1]{ pointerMap.at(&intKey_1); };
			auto constPointerAt = [&constPointerMap, &intKey_1]{ constPointerMap.at(&intKey_1); };
			Assert::ExpectException<exception>(pointerAt);
			Assert::ExpectException<exception>(constPointerAt);

			pointerMap[&intKey_1] = 250;
			pointerMap[&intKey_2] = 500;
			Assert::IsTrue(pointerMap.at(&intKey_1).second == 250);
			Assert::IsTrue(pointerMap.at(&intKey_2).second == 500);
			Assert::IsTrue(constPointerMap.at(&intKey_1).second == 250);
			Assert::IsTrue(constPointerMap.at(&intKey_2).second == 500);

			// test with char* key
			HashMap<char*, uint32_t> charMap;
			const HashMap<char*, uint32_t>& constCharMap = charMap;

			auto charAt = [&charMap]{ charMap.at("hello"); };
			auto constCharAt = [&constCharMap]{ constCharMap.at("hello"); };
			Assert::ExpectException<exception>(charAt);
			Assert::ExpectException<exception>(constCharAt);

			charMap["hello"] = 250;
			charMap["world"] = 500;
			Assert::IsTrue(charMap.at("hello").second == 250);
			Assert::IsTrue(charMap.at("world").second == 500);
			Assert::IsTrue(constCharMap.at("hello").second == 250);
			Assert::IsTrue(constCharMap.at("world").second == 500);

			// test with string key
			HashMap<string, uint32_t> stringMap;
			const HashMap<string, uint32_t>& constStringMap = stringMap;

			auto stringAt = [&stringMap]{ stringMap.at("hello"); };
			auto constStringAt = [&constStringMap]{ constStringMap.at("hello"); };
			Assert::ExpectException<exception>(stringAt);
			Assert::ExpectException<exception>(constStringAt);

			stringMap["hello"] = 250;
			stringMap["world"] = 500;
			Assert::IsTrue(stringMap.at("hello").second == 250);
			Assert::IsTrue(stringMap.at("world").second == 500);
			Assert::IsTrue(constStringMap.at("hello").second == 250);
			Assert::IsTrue(constStringMap.at("world").second == 500);

			// test with custom key
			HashMap<Foo, uint32_t> customMap;
			const HashMap<Foo, uint32_t>& constCustomMap = customMap;

			Foo fooKey_1(1), fooKey_2(2);

			auto customAt = [&customMap, &fooKey_1]{ customMap.at(fooKey_1); };
			auto constCustomAt = [&constCustomMap, &fooKey_1]{ constCustomMap.at(fooKey_1); };
			Assert::ExpectException<exception>(customAt);
			Assert::ExpectException<exception>(constCustomAt);

			customMap[fooKey_1] = 250;
			customMap[fooKey_2] = 500;
			Assert::IsTrue(customMap.at(fooKey_1).second == 250);
			Assert::IsTrue(customMap.at(fooKey_2).second == 500);
			Assert::IsTrue(constCustomMap.at(fooKey_1).second == 250);
			Assert::IsTrue(constCustomMap.at(fooKey_2).second == 500);
		}

		TEST_METHOD(HashMapFind)
		{
			// verify that finding a key in an empty Hashmap returns end()
			// verify that finding a key not in the Hashmap returns end()
			// verify that we can find keys in the Hashmap

			// test with primitive key
			HashMap<uint32_t, uint32_t> primitiveMap;
			uint32_t intKey_1 = 10, intKey_2 = 20, intVal_1 = 1, intVal_2 = 2;

			Assert::IsTrue(primitiveMap.find(intKey_1) == primitiveMap.end());

			primitiveMap.insert(PrimitivePair(intKey_1, intVal_1));
			Assert::IsTrue(primitiveMap.find(intKey_2) == primitiveMap.end());
			primitiveMap.insert(PrimitivePair(intKey_2, intVal_2));

			HashMap<uint32_t, uint32_t>::Iterator primitiveIter = primitiveMap.find(intKey_1);
			Assert::IsTrue(primitiveIter != primitiveMap.end());
			Assert::IsTrue(primitiveIter->second == intVal_1);

			// test with pointer key
			HashMap<uint32_t*, uint32_t> pointerMap;
			Assert::IsTrue(pointerMap.find(&intKey_1) == pointerMap.end());

			pointerMap.insert(PointerPair(&intKey_1, intVal_1));
			Assert::IsTrue(pointerMap.find(&intKey_2) == pointerMap.end());
			pointerMap.insert(PointerPair(&intKey_2, intVal_2));

			HashMap<uint32_t*, uint32_t>::Iterator pointerIter = pointerMap.find(&intKey_1);
			Assert::IsTrue(pointerIter != pointerMap.end());
			Assert::IsTrue(pointerIter->second == intVal_1);

			// test with char* key
			HashMap<char*, uint32_t> charMap;
			Assert::IsTrue(charMap.find("Hello") == charMap.end());

			charMap.insert(CharPair("Hello", intVal_1));
			Assert::IsTrue(charMap.find("World") == charMap.end());
			charMap.insert(CharPair("World", intVal_2));

			HashMap<char*, uint32_t>::Iterator charIter = charMap.find("Hello");
			Assert::IsTrue(charIter != charMap.end());
			Assert::IsTrue(charIter->second == intVal_1);

			// test with string key
			HashMap<string, uint32_t> stringMap;
			Assert::IsTrue(stringMap.find("Hello") == stringMap.end());

			stringMap.insert(StringPair("Hello", intVal_1));
			Assert::IsTrue(stringMap.find("World") == stringMap.end());
			stringMap.insert(StringPair("World", intVal_2));

			HashMap<string, uint32_t>::Iterator stringIter = stringMap.find("Hello");
			Assert::IsTrue(stringIter != stringMap.end());
			Assert::IsTrue(stringIter->second == intVal_1);

			// test with custom key
			HashMap<Foo, uint32_t> customMap;

			Foo foo_1(1), foo_2(2);
			Assert::IsTrue(customMap.find(foo_1) == customMap.end());

			customMap.insert(CustomPair(foo_1, intVal_1));
			Assert::IsTrue(customMap.find(foo_2) == customMap.end());
			customMap.insert(CustomPair(foo_2, intVal_2));

			HashMap<Foo, uint32_t>::Iterator customIter = customMap.find(foo_1);
			Assert::IsTrue(customIter != customMap.end());
			Assert::IsTrue(customIter->second == intVal_1);
		}

		TEST_METHOD(HashMapInsert)
		{
			// verify that inserting an element to the Hashmap adds the data
			// verify that inserting something with the same key leaves the Hashmap unchanged
			// verify the return of insert

			// test with primitive key
			uint32_t intKey = 10, intVal_1 = 1, intVal_2 = 2;
			HashMap<uint32_t, uint32_t> primitiveMap;
			Assert::IsTrue(primitiveMap.size() == 0);

			Assert::IsTrue(primitiveMap.insert(PrimitivePair(intKey, intVal_1))->second == intVal_1);
			Assert::IsTrue(primitiveMap.size() == 1);

			primitiveMap.insert(PrimitivePair(intKey, intVal_2));
			Assert::IsTrue(primitiveMap.size() == 1);

			Assert::IsTrue(primitiveMap.find(intKey)->second == intVal_1);

			// test with pointer key
			HashMap<uint32_t*, uint32_t> pointerMap;
			Assert::IsTrue(pointerMap.size() == 0);

			Assert::IsTrue(pointerMap.insert(PointerPair(&intKey, intVal_1))->second == intVal_1);
			Assert::IsTrue(pointerMap.size() == 1);

			pointerMap.insert(PointerPair(&intKey, intVal_2));
			Assert::IsTrue(pointerMap.size() == 1);

			Assert::IsTrue(pointerMap.find(&intKey)->second == intVal_1);

			// test with char* key
			HashMap<char*, uint32_t> charMap;
			Assert::IsTrue(charMap.size() == 0);

			Assert::IsTrue(charMap.insert(CharPair("Hello", intVal_1))->second == intVal_1);
			Assert::IsTrue(charMap.size() == 1);

			charMap.insert(CharPair("Hello", intVal_2));
			Assert::IsTrue(charMap.size() == 1);

			Assert::IsTrue(charMap.find("Hello")->second == intVal_1);

			// test with string key
			HashMap<string, uint32_t> stringMap;
			Assert::IsTrue(stringMap.size() == 0);

			Assert::IsTrue(stringMap.insert(StringPair("World", intVal_1))->second == intVal_1);
			Assert::IsTrue(stringMap.size() == 1);

			stringMap.insert(StringPair("World", intVal_2));
			Assert::IsTrue(stringMap.size() == 1);

			Assert::IsTrue(stringMap.find("World")->second == intVal_1);

			// test with custom key
			HashMap<Foo, uint32_t> customMap;
			Assert::IsTrue(customMap.size() == 0);

			Foo foo(1);
			Assert::IsTrue(customMap.insert(CustomPair(foo, intVal_1))->second == intVal_1);
			Assert::IsTrue(customMap.size() == 1);

			customMap.insert(CustomPair(foo, intVal_2));
			Assert::IsTrue(customMap.size() == 1);

			Assert::IsTrue(customMap.find(foo)->second == intVal_1);
		}

		TEST_METHOD(HashMapRemove)
		{
			// verify that removing an item not in the Hashmap is a noop
			// verify that removing an item reduces the Hashmap's size

			// test with primitive key
			HashMap<uint32_t, uint32_t> primitiveMap;

			uint32_t intKey_1 = 1, intKey_2 = 2;
			primitiveMap.insert(PrimitivePair(intKey_1, 0));

			Assert::IsTrue(primitiveMap.size() == 1);
			primitiveMap.remove(intKey_2);
			Assert::IsTrue(primitiveMap.size() == 1);

			Assert::IsTrue(primitiveMap.containsKey(intKey_1));
			primitiveMap.remove(intKey_1);

			Assert::IsFalse(primitiveMap.containsKey(intKey_1));
			Assert::IsTrue(primitiveMap.find(intKey_1) == primitiveMap.end());
			Assert::IsTrue(primitiveMap.size() == 0);

			// test with pointer key
			HashMap<uint32_t*, uint32_t> pointerMap;
			pointerMap.insert(PointerPair(&intKey_1, 0));

			Assert::IsTrue(pointerMap.size() == 1);
			pointerMap.remove(&intKey_2);
			Assert::IsTrue(pointerMap.size() == 1);

			Assert::IsTrue(pointerMap.containsKey(&intKey_1));
			pointerMap.remove(&intKey_1);

			Assert::IsFalse(pointerMap.containsKey(&intKey_1));
			Assert::IsTrue(pointerMap.find(&intKey_1) == pointerMap.end());
			Assert::IsTrue(pointerMap.size() == 0);

			// test with char* key
			HashMap<char*, uint32_t> charMap;
			charMap.insert(CharPair("Hello", 0));

			Assert::IsTrue(charMap.size() == 1);
			charMap.remove("World");
			Assert::IsTrue(charMap.size() == 1);

			Assert::IsTrue(charMap.containsKey("Hello"));
			charMap.remove("Hello");

			Assert::IsFalse(charMap.containsKey("Hello"));
			Assert::IsTrue(charMap.find("Hello") == charMap.end());
			Assert::IsTrue(charMap.size() == 0);

			// test with string key
			HashMap<string, uint32_t> stringMap;
			stringMap.insert(StringPair("Hello", 0));

			Assert::IsTrue(stringMap.size() == 1);
			stringMap.remove("World");
			Assert::IsTrue(stringMap.size() == 1);

			Assert::IsTrue(stringMap.containsKey("Hello"));
			stringMap.remove("Hello");

			Assert::IsFalse(stringMap.containsKey("Hello"));
			Assert::IsTrue(stringMap.find("Hello") == stringMap.end());
			Assert::IsTrue(stringMap.size() == 0);

			// test with custom key
			HashMap<Foo, uint32_t> customMap;
			customMap.insert(CustomPair(Foo(1), 0));

			Assert::IsTrue(customMap.size() == 1);
			customMap.remove(Foo(2));
			Assert::IsTrue(customMap.size() == 1);

			Assert::IsTrue(customMap.containsKey(Foo(1)));
			customMap.remove(Foo(1));

			Assert::IsFalse(customMap.containsKey(Foo(1)));
			Assert::IsTrue(customMap.find(Foo(1)) == customMap.end());
			Assert::IsTrue(customMap.size() == 0);
		}

		TEST_METHOD(HashMapClear)
		{
			// verify that clearing data sets size to 0
			// verify that clearing data leaks no memory

			// test with primitive key
			HashMap<uint32_t, Foo> primitiveMap;
			for(uint32_t i = 0; i < 25; ++i)
			{
				primitiveMap.insert(pair<uint32_t, Foo>(i, Foo(i)));
			}

			Assert::IsFalse(primitiveMap.isEmpty());

			primitiveMap.clear();
			Assert::IsTrue(primitiveMap.isEmpty());
			Assert::IsTrue(primitiveMap.size() == 0);

			// test with pointer key
			Vector<uint32_t> helperVector(25);
			HashMap<uint32_t*, Foo> pointerMap;
			for(uint32_t i = 0; i < 25; ++i)
			{
				helperVector.pushBack(i);
				pointerMap.insert(pair<uint32_t*, Foo>(&helperVector[i], Foo(i)));
			}

			Assert::IsFalse(pointerMap.isEmpty());

			pointerMap.clear();
			Assert::IsTrue(pointerMap.isEmpty());
			Assert::IsTrue(pointerMap.size() == 0);

			// test with char* key
			HashMap<const char*, Foo> charMap;
			string helperString;
			for(uint32_t i = 0; i < 25; ++i)
			{
				helperString = (char)i;
				charMap.insert(pair<const char*, Foo>(helperString.c_str(), Foo(i)));
			}

			Assert::IsFalse(charMap.isEmpty());

			charMap.clear();
			Assert::IsTrue(charMap.isEmpty());
			Assert::IsTrue(charMap.size() == 0);

			// test with string key
			HashMap<string, Foo> stringMap;
			for(uint32_t i = 0; i < 25; ++i)
			{
				helperString = (char)i;
				stringMap.insert(pair<string, Foo>(helperString, Foo(i)));
			}

			Assert::IsFalse(stringMap.isEmpty());

			stringMap.clear();
			Assert::IsTrue(stringMap.isEmpty());
			Assert::IsTrue(stringMap.size() == 0);

			// test with custom key
			HashMap<Foo, Foo> customMap;
			for(uint32_t i = 0; i < 25; ++i)
			{
				customMap.insert(pair<Foo, Foo>(Foo(i), Foo(i + 5)));
			}

			Assert::IsFalse(customMap.isEmpty());

			customMap.clear();
			Assert::IsTrue(customMap.isEmpty());
			Assert::IsTrue(customMap.size() == 0);
		}

		TEST_METHOD(HashMapSize)
		{
			// verify that adding items to Hashmap increases size
			// verify that removing items from Hashmap decreases size

			// test with primitive key
			HashMap<uint32_t, uint32_t> primitiveMap;
			uint32_t intKey_1 = 1, intKey_2 = 2;

			Assert::IsTrue(primitiveMap.size() == 0);

			primitiveMap[intKey_1] = 500;
			Assert::IsTrue(primitiveMap.size() == 1);
			primitiveMap.insert(PrimitivePair(intKey_2, 250));
			Assert::IsTrue(primitiveMap.size() == 2);

			primitiveMap.remove(intKey_1);
			Assert::IsTrue(primitiveMap.size() == 1);
			primitiveMap.remove(intKey_2);
			Assert::IsTrue(primitiveMap.size() == 0);

			// test with pointer key
			HashMap<uint32_t*, uint32_t> pointerMap;

			Assert::IsTrue(pointerMap.size() == 0);

			pointerMap[&intKey_1] = 500;
			Assert::IsTrue(pointerMap.size() == 1);
			pointerMap.insert(PointerPair(&intKey_2, 250));
			Assert::IsTrue(pointerMap.size() == 2);

			pointerMap.remove(&intKey_1);
			Assert::IsTrue(pointerMap.size() == 1);
			pointerMap.remove(&intKey_2);
			Assert::IsTrue(pointerMap.size() == 0);

			// test with char* key
			HashMap<char*, uint32_t> charMap;

			Assert::IsTrue(charMap.size() == 0);

			charMap["hello"] = 500;
			Assert::IsTrue(charMap.size() == 1);
			charMap.insert(CharPair("world", 250));
			Assert::IsTrue(charMap.size() == 2);

			charMap.remove("hello");
			Assert::IsTrue(charMap.size() == 1);
			charMap.remove("world");
			Assert::IsTrue(charMap.size() == 0);

			// test with string key
			HashMap<string, uint32_t> stringMap;

			Assert::IsTrue(stringMap.size() == 0);

			stringMap["hello"] = 500;
			Assert::IsTrue(stringMap.size() == 1);
			stringMap.insert(StringPair("world", 250));
			Assert::IsTrue(stringMap.size() == 2);

			stringMap.remove("hello");
			Assert::IsTrue(stringMap.size() == 1);
			stringMap.remove("world");
			Assert::IsTrue(stringMap.size() == 0);

			// test with custom key
			HashMap<Foo, uint32_t> customMap;

			Assert::IsTrue(customMap.size() == 0);

			customMap[Foo(10)] = 500;
			Assert::IsTrue(customMap.size() == 1);
			customMap.insert(CustomPair(Foo(20), 250));
			Assert::IsTrue(customMap.size() == 2);

			customMap.remove(Foo(10));
			Assert::IsTrue(customMap.size() == 1);
			customMap.remove(Foo(20));
			Assert::IsTrue(customMap.size() == 0);
		}

		TEST_METHOD(HashMapContainsKey)
		{
			// verify that containsKey is correct

			// test with primitive key
			uint32_t intKey_1 = 1, intKey_2 = 2;
			HashMap<uint32_t, uint32_t> primitiveMap;

			primitiveMap.insert(PrimitivePair(intKey_1, 1));
			Assert::IsTrue(primitiveMap.containsKey(intKey_1));
			Assert::IsFalse(primitiveMap.containsKey(intKey_2));

			// test with pointer key
			HashMap<uint32_t*, uint32_t> pointerMap;

			pointerMap.insert(PointerPair(&intKey_1, 2));
			Assert::IsTrue(pointerMap.containsKey(&intKey_1));
			Assert::IsFalse(pointerMap.containsKey(&intKey_2));

			// test with char* key
			HashMap<char*, uint32_t> charMap;
			char* charKey_1 = "hello";
			char* charKey_2 = "world";

			charMap.insert(CharPair(charKey_1, 3));
			Assert::IsTrue(charMap.containsKey(charKey_1));
			Assert::IsFalse(charMap.containsKey(charKey_2));

			// test with string key
			HashMap<string, uint32_t> stringMap;
			string stringKey_1 = "hello";
			string stringKey_2 = "world";

			stringMap.insert(StringPair(stringKey_1, 4));
			Assert::IsTrue(stringMap.containsKey(stringKey_1));
			Assert::IsFalse(stringMap.containsKey(stringKey_2));

			// test with custom key
			HashMap<Foo, uint32_t> customMap;
			Foo fooKey_1(1), fooKey_2(2);

			customMap.insert(CustomPair(fooKey_1, 5));
			Assert::IsTrue(customMap.containsKey(fooKey_1));
			Assert::IsFalse(customMap.containsKey(fooKey_2));
		}

		TEST_METHOD(HashMapBegin)
		{
			// verify that an empty Hashmap's begin and end are the same
			// verify that begin points to data when data is added to the HashMap

			// test with primitive key
			HashMap<uint32_t, uint32_t> primitiveMap;
			const HashMap<uint32_t, uint32_t>& constPrimitiveMap = primitiveMap;

			Assert::IsTrue(primitiveMap.begin() == primitiveMap.end());
			Assert::IsTrue(constPrimitiveMap.begin() == constPrimitiveMap.end());

			primitiveMap.insert(PrimitivePair(0, 1));

			Assert::IsTrue(primitiveMap.begin() != primitiveMap.end());
			Assert::IsTrue(primitiveMap.begin()->first == 0);
			Assert::IsTrue(primitiveMap.begin()->second == 1);

			Assert::IsTrue(constPrimitiveMap.begin() != constPrimitiveMap.end());
			Assert::IsTrue(constPrimitiveMap.begin()->first == 0);
			Assert::IsTrue(constPrimitiveMap.begin()->second == 1);

			// test with pointer key
			HashMap<uint32_t*, uint32_t> pointerMap;
			const HashMap<uint32_t*, uint32_t>& constPointerMap = pointerMap;

			Assert::IsTrue(pointerMap.begin() == pointerMap.end());
			Assert::IsTrue(constPointerMap.begin() == constPointerMap.end());

			uint32_t key = 0;
			pointerMap.insert(PointerPair(&key, 1));

			Assert::IsTrue(pointerMap.begin() != pointerMap.end());
			Assert::IsTrue(pointerMap.begin()->first == &key);
			Assert::IsTrue(pointerMap.begin()->second == 1);

			Assert::IsTrue(constPointerMap.begin() != constPointerMap.end());
			Assert::IsTrue(constPointerMap.begin()->first == &key);
			Assert::IsTrue(constPointerMap.begin()->second == 1);

			// test with char* key
			HashMap<char*, uint32_t> charMap;
			const HashMap<char*, uint32_t>& constCharMap = charMap;

			Assert::IsTrue(charMap.begin() == charMap.end());
			Assert::IsTrue(constCharMap.begin() == constCharMap.end());

			charMap.insert(CharPair("Hello", 1));

			Assert::IsTrue(charMap.begin() != charMap.end());
			Assert::IsTrue(strcmp(charMap.begin()->first, "Hello") == 0);
			Assert::IsTrue(charMap.begin()->second == 1);

			Assert::IsTrue(constCharMap.begin() != constCharMap.end());
			Assert::IsTrue(strcmp(constCharMap.begin()->first, "Hello") == 0);
			Assert::IsTrue(constCharMap.begin()->second == 1);

			// test with string key
			HashMap<string, uint32_t> stringMap;
			const HashMap<string, uint32_t>& constStringMap = stringMap;

			Assert::IsTrue(stringMap.begin() == stringMap.end());
			Assert::IsTrue(constStringMap.begin() == constStringMap.end());

			stringMap.insert(StringPair("Hello", 1));

			Assert::IsTrue(stringMap.begin() != stringMap.end());
			Assert::IsTrue(stringMap.begin()->first == "Hello");
			Assert::IsTrue(stringMap.begin()->second == 1);

			Assert::IsTrue(constStringMap.begin() != constStringMap.end());
			Assert::IsTrue(constStringMap.begin()->first == "Hello");
			Assert::IsTrue(constStringMap.begin()->second == 1);

			// test with custom key
			HashMap<Foo, uint32_t> customMap;
			const HashMap<Foo, uint32_t>& constCustomMap = customMap;

			Assert::IsTrue(customMap.begin() == customMap.end());
			Assert::IsTrue(constCustomMap.begin() == constCustomMap.end());

			customMap.insert(CustomPair(Foo(20), 1));

			Assert::IsTrue(customMap.begin() != customMap.end());
			Assert::IsTrue(customMap.begin()->first == Foo(20));
			Assert::IsTrue(customMap.begin()->second == 1);

			Assert::IsTrue(constCustomMap.begin() != constCustomMap.end());
			Assert::IsTrue(constCustomMap.begin()->first == Foo(20));
			Assert::IsTrue(constCustomMap.begin()->second == 1);
		}

		TEST_METHOD(HashMapEnd)
		{
			// verify that the end Iterator cannot be dereferenced

			// test with primitive key
			HashMap<uint32_t, uint32_t> primitiveMap;
			primitiveMap.insert(PrimitivePair(0, 1));

			auto primitiveEnd = [&primitiveMap]{ *(primitiveMap.end()); };
			Assert::ExpectException<exception>(primitiveEnd);

			const HashMap<uint32_t, uint32_t>& constPrimitiveMap = primitiveMap;
			auto constPrimitiveEnd = [&constPrimitiveMap]{ *(constPrimitiveMap.end()); };
			Assert::ExpectException<exception>(constPrimitiveEnd);

			// test with pointer key
			HashMap<uint32_t*, uint32_t> pointerMap;
			uint32_t key = 0;
			pointerMap.insert(PointerPair(&key, 1));

			auto pointerEnd = [&pointerMap]{ *(pointerMap.end()); };
			Assert::ExpectException<exception>(pointerEnd);

			const HashMap<uint32_t*, uint32_t>& constPointerMap = pointerMap;
			auto constPointerEnd = [&constPointerMap]{ *(constPointerMap.end()); };
			Assert::ExpectException<exception>(constPointerEnd);

			// test with char* key
			HashMap<char*, uint32_t> charMap;
			charMap.insert(CharPair("hello", 1));

			auto charEnd = [&charMap]{ *(charMap.end()); };
			Assert::ExpectException<exception>(charEnd);

			const HashMap<char*, uint32_t>& constCharMap = charMap;
			auto constCharEnd = [&constCharMap]{ *(constCharMap.end()); };
			Assert::ExpectException<exception>(constCharEnd);

			// test with string key
			HashMap<string, uint32_t> stringMap;
			stringMap.insert(StringPair("world", 1));

			auto stringEnd = [&stringMap]{ *(stringMap.end()); };
			Assert::ExpectException<exception>(stringEnd);

			const HashMap<string, uint32_t>& constStringMap = stringMap;
			auto constStringEnd = [&constStringMap]{ *(constStringMap.end()); };
			Assert::ExpectException<exception>(constStringEnd);
			
			// test with foo key
			HashMap<Foo, uint32_t> customMap;
			customMap.insert(CustomPair(Foo(20), 1));

			auto customEnd = [&customMap]{ *(customMap.end()); };
			Assert::ExpectException<exception>(customEnd);

			const HashMap<Foo, uint32_t>& constCustomMap = customMap;
			auto constCustomEnd = [&constCustomMap]{ *(constCustomMap.end()); };
			Assert::ExpectException<exception>(constCustomEnd);
		}

		TEST_METHOD(HashMapEquality)
		{
			// test with primitive
			HashMap<int32_t, int32_t> hashmap;
			HashMap<int32_t, int32_t> otherHashmap;
			for(uint32_t i = 0; i < 10; ++i)
			{
				hashmap[i] = i * 10;
				otherHashmap[i] = i * 10;
			}

			Assert::IsTrue(hashmap == otherHashmap);
			Assert::IsFalse(hashmap != otherHashmap);

			otherHashmap.remove(0);
			Assert::IsFalse(hashmap == otherHashmap);
			Assert::IsTrue(hashmap != otherHashmap);

			otherHashmap[0] = 1000;
			Assert::IsFalse(hashmap == otherHashmap);
			Assert::IsTrue(hashmap != otherHashmap);

			// test with pointer
			HashMap<int32_t*, int32_t> ptrHash;
			HashMap<int32_t*, int32_t> otherPtrHash;

			int32_t ptrs[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			for(uint32_t i = 0; i < 10; ++i)
			{
				ptrHash[ptrs + i] = i * 10;
				otherPtrHash[ptrs + i] = i * 10;
			}

			Assert::IsTrue(ptrHash == otherPtrHash);
			Assert::IsFalse(ptrHash != otherPtrHash);

			otherPtrHash.remove(ptrs);
			Assert::IsFalse(ptrHash == otherPtrHash);
			Assert::IsTrue(ptrHash != otherPtrHash);

			otherPtrHash[ptrs] = 1000;
			Assert::IsFalse(hashmap == otherHashmap);
			Assert::IsTrue(hashmap != otherHashmap);

			// test with char*
			HashMap<char*, int32_t> charHash;
			HashMap<char*, int32_t> otherCharHash;

			charHash["hello"] = 0;
			charHash["world"] = 1;
			charHash["error"] = 2;
			otherCharHash["hello"] = 0;
			otherCharHash["world"] = 1;
			otherCharHash["error"] = 2;

			Assert::IsTrue(charHash == otherCharHash);
			Assert::IsFalse(charHash != otherCharHash);

			otherCharHash.remove("hello");
			Assert::IsFalse(charHash == otherCharHash);
			Assert::IsTrue(charHash != otherCharHash);

			otherCharHash["hello"] = 1000;
			Assert::IsFalse(hashmap == otherHashmap);
			Assert::IsTrue(hashmap != otherHashmap);

			// test with string
			HashMap<string, int32_t> strHash;
			HashMap<string, int32_t> otherStrHash;
			for(uint32_t i = 0; i < 10; ++i)
			{
				strHash[to_string(i)] = i * 10;
				otherStrHash[to_string(i)] = i * 10;
			}

			Assert::IsTrue(strHash == otherStrHash);
			Assert::IsFalse(strHash != otherStrHash);

			otherStrHash.remove("0");
			Assert::IsFalse(strHash == otherStrHash);
			Assert::IsTrue(strHash != otherStrHash);

			otherStrHash["0"] = 1000;
			Assert::IsFalse(hashmap == otherHashmap);
			Assert::IsTrue(hashmap != otherHashmap);

			// test with custom
			HashMap<Foo, int32_t> customHash;
			HashMap<Foo, int32_t> otherCustomHash;

			for(uint32_t i = 0; i < 10; ++i)
			{
				customHash[Foo(i)] = i * 10;
				otherCustomHash[Foo(i)] = i * 10;
			}

			Assert::IsTrue(customHash == otherCustomHash);
			Assert::IsFalse(customHash != otherCustomHash);

			otherCustomHash.remove(Foo(0));
			Assert::IsFalse(customHash == otherCustomHash);
			Assert::IsTrue(customHash != otherCustomHash);

			otherCustomHash[Foo(0)] = 1000;
			Assert::IsFalse(hashmap == otherHashmap);
			Assert::IsTrue(hashmap != otherHashmap);
		}

	private:

		template <typename TKey, typename TValue, typename THash, typename TComp>
		void checkHashMapEquality(HashMap<TKey, TValue, THash, TComp>& originalMap, HashMap<TKey, TValue, THash, TComp>& copiedMap)
		{
			Assert::IsTrue(originalMap == copiedMap);
			Assert::IsFalse(originalMap != copiedMap);
		}

		static _CrtMemState sStartMemState;

	};

	_CrtMemState HashMapTest::sStartMemState;
}
