
#include "pch.h"
#include "CppUnitTest.h"

#include "Foo.h"
#include "Datum.h"
#include "Scope.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DOGEngine;
using namespace UnitTests;
using namespace std;
using namespace glm;

namespace LibraryDesktopTest
{
	TEST_CLASS(DatumTest)
	{
	public:

		TEST_CLASS_INITIALIZE(ClassInit)
		{
#ifdef _DEBUG
			// grab snapshot of memory state at start of test
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
			_CrtMemCheckpoint(&sClassStartMemState);
#endif

			arraySize = 5;

			// allocate space for all of our external arrays
			ints = reinterpret_cast<int32_t*>(malloc(sizeof(int32_t) * arraySize));
			floats = reinterpret_cast<float*>(malloc(sizeof(float) * arraySize));
			strs = reinterpret_cast<string*>(malloc(sizeof(string) * arraySize));
			foos = reinterpret_cast<Foo**>(malloc(sizeof(Foo*) * arraySize));
			vecs = reinterpret_cast<vec4*>(malloc(sizeof(vec4) * arraySize));
			mats = reinterpret_cast<mat4x4*>(malloc(sizeof(mat4x4) * arraySize));
			scopes = reinterpret_cast<Scope*>(malloc(sizeof(Scope) * arraySize));

			int32_t x, y, z;
			for(uint32_t i = 0; i < arraySize; ++i)
			{
				x = i + 1;
				y = i + 2;
				z = i + 3;

				foos[i] = new Foo(i);
				ints[i] = i;
				floats[i] = (float)i;

				// placement new elements in the arrays
				new(strs + i)string(to_string(i));
				new(vecs + i)vec4(x, y, z, i);
				new(mats + i)mat4x4(
					x, x, x, x, 
					y, y, y, y, 
					z, z, z, z, 
					i, i, i, i);

				new(scopes + i)Scope(3);
				scopes[i].append(to_string(i)) = (int32_t)i;
			}
		}

		TEST_CLASS_CLEANUP(ClassCleanup)
		{
			for(uint32_t i = 0; i < arraySize; ++i)
			{
				// destruct elements that we can
				delete foos[i];
				scopes[i].~Scope();
				strs[i].~string();
				vecs[i].~vec4();
				mats[i].~mat4x4();
			}

			// free allocated space in our external arrays
			free(ints);
			free(floats);
			free(strs);
			free(foos);
			free(vecs);
			free(mats);
			free(scopes);

#ifdef _DEBUG
			_CrtMemState classEndMemState, classDiffMemState;

			// grab snapshot of of memory state at end of test and
			// compare it against the starting memory state
			_CrtMemCheckpoint(&classEndMemState);
			if(_CrtMemDifference(&classDiffMemState, &sClassStartMemState, &classEndMemState))
			{
				// memory leak if difference between starting and ending memory states
				_CrtMemDumpStatistics(&classDiffMemState);
				Assert::Fail(L"Memory leak detected!");
			}
#endif
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


		TEST_METHOD(DatumDefaultConstructor)
		{
			// verify that default constructor creates an empty Datum with no type
			Datum defaultDatum;
			Assert::IsTrue(defaultDatum.type() == Datum::DatumType::Unknown);
			Assert::IsTrue(defaultDatum.size() == 0);

			// verify that the constructor can take an enum value that sets Datum's type
			ConstructorTestHelper(Datum::DatumType::Unknown);
			ConstructorTestHelper(Datum::DatumType::Float);
			ConstructorTestHelper(Datum::DatumType::Integer);
			ConstructorTestHelper(Datum::DatumType::String);
			ConstructorTestHelper(Datum::DatumType::Pointer);
			ConstructorTestHelper(Datum::DatumType::Matrix);
			ConstructorTestHelper(Datum::DatumType::Vector);
			ConstructorTestHelper(Datum::DatumType::Table);
		}

		TEST_METHOD(DatumCopyConstructor)
		{
			CopyConstructorTestHelper<int32_t>(ints);
			CopyConstructorTestHelper<float>(floats);
			CopyConstructorTestHelper<string>(strs);
			CopyConstructorTestHelper<RTTI*>((RTTI**)foos);
			CopyConstructorTestHelper<mat4x4>(mats);
			CopyConstructorTestHelper<vec4>(vecs);
			CopyConstructorTestHelper<Scope>(scopes);

			// test with external datum
			Datum externalDatum;
			externalDatum.setStorage(ints, 3);
			Datum otherExternalDatum(externalDatum);
			testDatumEquivalency(externalDatum, otherExternalDatum);
		}

		TEST_METHOD(DatumMove)
		{
			// move semantics can change the type of a datum
			{
				Datum datum;
				datum = "hello";
				Assert::IsTrue(datum.type() == Datum::DatumType::String);

				Datum otherDatum;
				otherDatum = 100;
				Assert::IsTrue(otherDatum.type() == Datum::DatumType::Integer);

				otherDatum = std::move(datum);
				Assert::IsTrue(otherDatum.type() == Datum::DatumType::String);
			}

			// test with different types
			MoveTestHelper<int32_t>(ints, Datum::DatumType::Integer);
			MoveTestHelper<float>(floats, Datum::DatumType::Float);
			MoveTestHelper<string>(strs, Datum::DatumType::String);
			MoveTestHelper<RTTI*>((RTTI**)foos, Datum::DatumType::Pointer);
			MoveTestHelper<mat4x4>(mats, Datum::DatumType::Matrix);
			MoveTestHelper<vec4>(vecs, Datum::DatumType::Vector);
			MoveTestHelper<Scope>(scopes, Datum::DatumType::Table);

			// move semantics on external data
			// move construction -- external memory
			{
				Datum datum;
				datum.setStorage(ints, arraySize);
				Datum copiedDatum(datum);

				Datum otherDatum(std::move(datum));

				Assert::IsTrue(otherDatum == copiedDatum);
				Assert::IsTrue(otherDatum != datum);

				Assert::IsTrue(datum.type() == Datum::DatumType::Unknown);
				Assert::IsTrue(datum.size() == 0);
				Assert::IsTrue(datum.capacity() == 0);
				Assert::IsFalse(datum.isExternal());
				auto emptyGet = [&datum]{ datum.get<int32_t>(); };
				Assert::ExpectException<exception>(emptyGet);

				Assert::IsTrue(otherDatum.type() == Datum::DatumType::Integer);
				Assert::IsTrue(otherDatum.size() == arraySize);
				Assert::IsTrue(otherDatum.capacity() == arraySize);
				Assert::IsTrue(otherDatum.isExternal());
				Assert::IsTrue(otherDatum.get<int32_t>() == ints[0]);
			}

			// move assignment -- external memory
			{
				Datum datum;
				datum.setStorage(ints, arraySize);
				Datum copiedDatum(datum);

				Datum otherDatum;
				otherDatum.setStorage(ints, 2);

				otherDatum = std::move(datum);

				Assert::IsTrue(otherDatum == copiedDatum);
				Assert::IsTrue(otherDatum != datum);

				Assert::IsTrue(datum.type() == Datum::DatumType::Unknown);
				Assert::IsTrue(datum.size() == 0);
				Assert::IsTrue(datum.capacity() == 0);
				Assert::IsFalse(datum.isExternal());
				auto emptyGet = [&datum]{ datum.get<int32_t>(); };
				Assert::ExpectException<exception>(emptyGet);

				Assert::IsTrue(otherDatum.type() == Datum::DatumType::Integer);
				Assert::IsTrue(otherDatum.size() == arraySize);
				Assert::IsTrue(otherDatum.capacity() == arraySize);
				Assert::IsTrue(otherDatum.isExternal());
				Assert::IsTrue(otherDatum.get<int32_t>() == ints[0]);
			}
		}

		TEST_METHOD(DatumDestructor)
		{
			DestructorTestHelper<int32_t>(ints);
			DestructorTestHelper<float>(floats);
			DestructorTestHelper<string>(strs);
			DestructorTestHelper<RTTI*>((RTTI**)foos);
			DestructorTestHelper<mat4x4>(mats);
			DestructorTestHelper<vec4>(vecs);
			DestructorTestHelper<Scope>(scopes);
		}

		TEST_METHOD(DatumAssignmentOperator)
		{
			// test Datum to Datum assignment
			DatumAssignmentTestHelper<int32_t>(ints);
			DatumAssignmentTestHelper<float>(floats);
			DatumAssignmentTestHelper<string>(strs);
			DatumAssignmentTestHelper<RTTI*>((RTTI**)foos);
			DatumAssignmentTestHelper<mat4x4>(mats);
			DatumAssignmentTestHelper<vec4>(vecs);

			// test with scope -- can only do this with internals
			Datum datum;
			datum.pushBack(scopes[0]);
			datum.pushBack(scopes[1]);
			datum.pushBack(scopes[2]);

			Datum otherDatum;
			datum.pushBack(scopes[3]);
			datum.pushBack(scopes[4]);
			otherDatum = datum;
			testDatumEquivalency(datum, otherDatum);					// internal to internal

			// verify that a type mismatch on scalar assignment throws an exception
			Datum errorDatum(Datum::DatumType::Integer);
			auto mismatchAssign = [&errorDatum]{ errorDatum = "hello"; };
			Assert::ExpectException<exception>(mismatchAssign);

			// test Datum scalar assignment
			ScalarDatumAssignmentTestHelper<int32_t>(ints, Datum::DatumType::Integer);
			ScalarDatumAssignmentTestHelper<float>(floats, Datum::DatumType::Float);
			ScalarDatumAssignmentTestHelper<string>(strs, Datum::DatumType::String);
			ScalarDatumAssignmentTestHelper<RTTI*>((RTTI**)foos, Datum::DatumType::Pointer);
			ScalarDatumAssignmentTestHelper<mat4x4>(mats, Datum::DatumType::Matrix);
			ScalarDatumAssignmentTestHelper<vec4>(vecs, Datum::DatumType::Vector);
			ScalarDatumAssignmentTestHelper<Scope>(scopes, Datum::DatumType::Table);
		}

		TEST_METHOD(DatumEqualityOperators)
		{
			// test Datum to Datum comparison
			DatumComparisonTestHelper<int32_t>(ints, Datum::DatumType::Integer);
			DatumComparisonTestHelper<float>(floats, Datum::DatumType::Float);
			DatumComparisonTestHelper<string>(strs, Datum::DatumType::String);
			DatumComparisonTestHelper<RTTI*>((RTTI**)foos, Datum::DatumType::Pointer);
			DatumComparisonTestHelper<mat4x4>(mats, Datum::DatumType::Matrix);
			DatumComparisonTestHelper<vec4>(vecs, Datum::DatumType::Vector);
			DatumComparisonTestHelper<Scope>(scopes, Datum::DatumType::Table);

			// test Datum scalar comparison
			ScalarDatumComparisonTestHelper<int32_t>(ints);
			ScalarDatumComparisonTestHelper<float>(floats);
			ScalarDatumComparisonTestHelper<string>(strs);
			ScalarDatumComparisonTestHelper<RTTI*>((RTTI**)foos);
			ScalarDatumComparisonTestHelper<mat4x4>(mats);
			ScalarDatumComparisonTestHelper<vec4>(vecs);
			ScalarDatumComparisonTestHelper<Scope>(scopes);
		}

		TEST_METHOD(DatumReserve)
		{
			// verify that reserving on an untyped Datum throws an exception
			Datum datum;
			auto untypedReserve = [&datum]{ datum.reserve(5); };
			Assert::ExpectException<exception>(untypedReserve);

			ReserveTestHelper<int32_t>(ints, Datum::DatumType::Integer);
			ReserveTestHelper<float>(floats, Datum::DatumType::Float);
			ReserveTestHelper<string>(strs, Datum::DatumType::String);
			ReserveTestHelper<RTTI*>((RTTI**)foos, Datum::DatumType::Pointer);
			ReserveTestHelper<mat4x4>(mats, Datum::DatumType::Matrix);
			ReserveTestHelper<vec4>(vecs, Datum::DatumType::Vector);
			ReserveTestHelper<Scope>(scopes, Datum::DatumType::Table);

			// external throws exception
			Datum externalDatum;										// external throws exception
			externalDatum.setStorage(ints, 1);
			auto externalReserve = [&externalDatum]{ externalDatum.reserve(10); };
			Assert::ExpectException<exception>(externalReserve);
		}

		TEST_METHOD(DatumShrinkToFit)
		{
			// verify that shrinking an untyped datum throws an exception
			Datum datum;
			auto untypedShrink = [&datum]{ datum.shrinkToFit(); };
			Assert::ExpectException<exception>(untypedShrink);

			ShrinkToFitTestHelper<int32_t>(ints);
			ShrinkToFitTestHelper<float>(floats);
			ShrinkToFitTestHelper<string>(strs);
			ShrinkToFitTestHelper<RTTI*>((RTTI**)foos);
			ShrinkToFitTestHelper<mat4x4>(mats);
			ShrinkToFitTestHelper<vec4>(vecs);
			ShrinkToFitTestHelper<Scope>(scopes);

			// external throws exception
			Datum externalDatum;										// external throws exception
			externalDatum.setStorage(ints, 1);
			auto externalShrink = [&externalDatum]{ externalDatum.shrinkToFit(); };
			Assert::ExpectException<exception>(externalShrink);
		}

		TEST_METHOD(DatumPushBack)
		{
			// verify that pushing back with a type mismatch throws an exception
			Datum datum;
			datum.pushBack(1);
			Assert::IsTrue(datum.type() == Datum::DatumType::Integer);
			auto mismatchPushBack = [&datum]{ datum.pushBack("hello"); };
			Assert::ExpectException<exception>(mismatchPushBack);

			PushBackTestHelper<int32_t>(ints, Datum::DatumType::Integer);
			PushBackTestHelper<float>(floats, Datum::DatumType::Float);
			PushBackTestHelper<string>(strs, Datum::DatumType::String);
			PushBackTestHelper<RTTI*>((RTTI**)foos, Datum::DatumType::Pointer);
			PushBackTestHelper<mat4x4>(mats, Datum::DatumType::Matrix);
			PushBackTestHelper<vec4>(vecs, Datum::DatumType::Vector);
			PushBackTestHelper<Scope>(scopes, Datum::DatumType::Table);

			// external throws exception
			Datum externalDatum;
			externalDatum.setStorage(ints, 1);
			auto externalPushBack = [&externalDatum]{ externalDatum.pushBack(ints[2]); };
			Assert::ExpectException<exception>(externalPushBack);
		}

		TEST_METHOD(DatumRemove)
		{
			// verify that removing from an external Datum throws an exception
			Datum errorDatum;
			errorDatum.setStorage(strs, 5);
			auto externalRemove = [&errorDatum]{ errorDatum.remove("cow"); };
			Assert::ExpectException<exception>(externalRemove);

			// verify that removing from a type mismatch throws an exception
			Datum otherDatum;
			otherDatum.pushBack("hello");
			otherDatum.pushBack("world");
			auto mismatchRemove = [&otherDatum]{ otherDatum.remove(100); };
			Assert::ExpectException<exception>(mismatchRemove);

			// remove at throws exception when out of bounds
			auto boundsRemove = [&otherDatum]{ otherDatum.removeAt(10); };
			Assert::ExpectException<exception>(boundsRemove);

			// verify removing the data
			RemoveTestHelper<int32_t>(ints);
			RemoveTestHelper<float>(floats);
			RemoveTestHelper<string>(strs);
			RemoveTestHelper<RTTI*>((RTTI**)foos);
			RemoveTestHelper<mat4x4>(mats);
			RemoveTestHelper<vec4>(vecs);
			RemoveTestHelper<Scope>(scopes);
		}

		TEST_METHOD(DatumPopBack)
		{
			PopBackTestHelper<int32_t>(ints);
			PopBackTestHelper<float>(floats);
			PopBackTestHelper<string>(strs);
			PopBackTestHelper<RTTI*>((RTTI**)foos);
			PopBackTestHelper<mat4x4>(mats);
			PopBackTestHelper<vec4>(vecs);
			PopBackTestHelper<Scope>(scopes);

			// external throws exception
			Datum externalDatum;
			externalDatum.setStorage(ints, 1);
			auto externalPopBack = [&externalDatum]{ externalDatum.popBack(); };
			Assert::ExpectException<exception>(externalPopBack);
		}

		TEST_METHOD(DatumClear)
		{
			ClearTestHelper<int32_t>(ints);
			ClearTestHelper<float>(floats);
			ClearTestHelper<string>(strs);
			ClearTestHelper<RTTI*>((RTTI**)foos);
			ClearTestHelper<mat4x4>(mats);
			ClearTestHelper<vec4>(vecs);
			ClearTestHelper<Scope>(scopes);

			// external throws exception
			Datum externalDatum;
			externalDatum.setStorage(ints, 1);
			auto externalClear = [&externalDatum]{ externalDatum.clear(); };
			Assert::ExpectException<exception>(externalClear);
		}

		TEST_METHOD(DatumSetStorage)
		{
			SetStorageTestHelper<int32_t, float>(ints, floats, Datum::DatumType::Integer);
			SetStorageTestHelper<float, int32_t>(floats, ints, Datum::DatumType::Float);
			SetStorageTestHelper<string, float>(strs, floats, Datum::DatumType::String);
			SetStorageTestHelper<RTTI*, float>((RTTI**)foos, floats, Datum::DatumType::Pointer);
			SetStorageTestHelper<mat4x4, float>(mats, floats, Datum::DatumType::Matrix);
			SetStorageTestHelper<vec4, float>(vecs, floats, Datum::DatumType::Vector);
		}

		TEST_METHOD(DatumSet)
		{
			// verify that a type mismatch in set throws an exception
			Datum errorDatum(Datum::DatumType::Integer);
			errorDatum.pushBack(1);
			errorDatum.pushBack(2);
			errorDatum.pushBack(3);

			auto mismatchSet = [&errorDatum]{ errorDatum.set("hello", 0); };
			Assert::ExpectException<exception>(mismatchSet);

			// verify that setting does bounds checking (it won't dynamically grow the Datum)
			auto boundsSet = [&errorDatum]{ errorDatum.set(5, 10); };
			Assert::ExpectException<exception>(boundsSet);

			// test setting validly
			SetTestHelper<int32_t>(ints);
			SetTestHelper<float>(floats);
			SetTestHelper<string>(strs);
			SetTestHelper<mat4x4>(mats);
			SetTestHelper<vec4>(vecs);
			SetTestHelperRTTI((RTTI**)foos);
			SetTestHelperScope();
		}

		TEST_METHOD(DatumGet)
		{
			// verify that a type mismatch for get throws an exception
			Datum errorDatum(Datum::DatumType::Float);
			const Datum& constErrorDatum = errorDatum;

			errorDatum.pushBack(1.0f);
			auto mismatchGet = [&errorDatum]{ errorDatum.get<int32_t>(0); };
			Assert::ExpectException<exception>(mismatchGet);

			auto constMismatchGet = [&constErrorDatum]{ constErrorDatum.get<int32_t>(0); };
			Assert::ExpectException<exception>(constMismatchGet);

			// verify that getting outside the bounds of the Datum throws an exception
			errorDatum.pushBack(2.0f);
			errorDatum.pushBack(3.0f);
			auto boundsGet = [&errorDatum]{ errorDatum.get<float>(5); };
			Assert::ExpectException<exception>(boundsGet);

			auto constBoundsGet = [&constErrorDatum]{ constErrorDatum.get<float>(5); };
			Assert::ExpectException<exception>(constBoundsGet);

			// test setting validly
			GetTestHelper<int32_t>(ints);
			GetTestHelper<float>(floats);
			GetTestHelper<string>(strs);
			GetTestHelper<RTTI*>((RTTI**)foos);
			GetTestHelper<mat4x4>(mats);
			GetTestHelper<vec4>(vecs);
			GetTestHelper<Scope>(scopes);
		}

		TEST_METHOD(DatumBracketsOperator)
		{
			// verify that brackets operator on nontable types throws exception
			Datum errorDatum;
			const Datum& constErrorDatum = errorDatum;

			auto untypedBrackets = [&errorDatum]{ errorDatum[0]; };
			Assert::ExpectException<exception>(untypedBrackets);
			auto constUntypedBrackets = [&constErrorDatum]{ constErrorDatum[0]; };
			Assert::ExpectException<exception>(constUntypedBrackets);

			errorDatum.setType(Datum::DatumType::Integer);
			auto nonTableBrackets = [&errorDatum]{ errorDatum[0]; };
			Assert::ExpectException<exception>(nonTableBrackets);
			auto constNonTableBrackets = [&constErrorDatum]{ constErrorDatum[0]; };
			Assert::ExpectException<exception>(constNonTableBrackets);

			// verify that getting outside the bounds of the Datum throws an exception
			Datum scopeErrorDatum;
			const Datum& constScopeErrorDatum = scopeErrorDatum;
			scopeErrorDatum.pushBack(scopes[0]);

			auto boundsBrackets = [&scopeErrorDatum]{ scopeErrorDatum[2]; };
			Assert::ExpectException<exception>(boundsBrackets);
			auto constBoundsBrackets = [&constScopeErrorDatum]{ constScopeErrorDatum.get<float>(5); };
			Assert::ExpectException<exception>(constBoundsBrackets);

			// verify brackets
			Datum datum;
			const Datum& constDatum = datum;

			datum.pushBack(scopes[0]);
			datum.pushBack(scopes[1]);
			datum.pushBack(scopes[2]);
			Assert::IsTrue(datum[0] == scopes[0]);
			Assert::IsTrue(datum[1] == scopes[1]);
			Assert::IsTrue(datum[2] == scopes[2]);
			Assert::IsTrue(constDatum[0] == scopes[0]);
			Assert::IsTrue(constDatum[1] == scopes[1]);
			Assert::IsTrue(constDatum[2] == scopes[2]);
		}

		TEST_METHOD(DatumSetFromString)
		{
			// verify that setting data from string on untyped Datum throws exception
			Datum errorDatum;

			auto untypedSetFromString = [&errorDatum]{ errorDatum.setFromString("5"); };
			Assert::ExpectException<exception>(untypedSetFromString);

			// verify that setting from string for table Datums throws exception
			errorDatum.setType(Datum::DatumType::Table);
			auto tableSetFromString = [&errorDatum]{ errorDatum.setFromString("0"); };
			Assert::ExpectException<exception>(tableSetFromString);

			// verify that setting data from string does bounds checking
			Datum errorDatum_2;
			errorDatum_2.pushBack(1);
			auto boundsSetFromString = [&errorDatum_2]{ errorDatum_2.setFromString("5", 1); };
			Assert::ExpectException<exception>(boundsSetFromString);

			// test inserting different types by string
			SetFromStringTestHelper<int32_t>("5", 5);
			SetFromStringTestHelper<float>("10.123400", 10.1234f);
			SetFromStringTestHelper<string>("hello", "hello");
			SetFromStringTestHelper<vec4>(
				"vec4(1.000000, 2.000000, 3.000000, 4.000000)", 
				vec4(1, 2, 3, 4));
			SetFromStringTestHelper<mat4x4>(
				"mat4x4((1.000000, 2.000000, 3.000000, 4.000000), (5.000000, 6.000000, 7.000000, 8.000000), (9.000000, 10.000000, 11.000000, 12.000000), (13.000000, 14.000000, 15.000000, 16.000000))",
				mat4x4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));

			//SetFromStringTestHelper<RTTI*>("0", nullptr);		// TODO -- none???
			//SetFromStringTestHelper<Scope*>("0", nullptr);	// TODO -- none???
		}

		TEST_METHOD(DatumToString)
		{
			// verify that getting to string on untyped Datum throws exception
			Datum errorDatum;

			auto untypedToString = [&errorDatum]{ errorDatum.toString(); };
			Assert::ExpectException<exception>(untypedToString);

			// verify that getting to string on table Datums throws exception
			errorDatum.setType(Datum::DatumType::Table);
			auto tableToString = [&errorDatum]{ errorDatum.toString(); };
			Assert::ExpectException<exception>(tableToString);

			// verify that getting to string does bounds checking
			Datum errorDatum_2;
			errorDatum_2.pushBack(5);
			auto boundsToString = [&errorDatum_2]{ errorDatum_2.toString(1); };
			Assert::ExpectException<exception>(boundsToString);

			// test getting to string with different types
			ToStringTestHelper<int32_t>(10, "10");
			ToStringTestHelper<float>(10.1234f, "10.123400");
			ToStringTestHelper<string>("hello", "hello");
			ToStringTestHelper<vec4>(
				vec4(1, 2, 3, 4),
				"vec4(1.000000, 2.000000, 3.000000, 4.000000)");
			ToStringTestHelper<mat4x4>(
				mat4x4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16),
				"mat4x4((1.000000, 2.000000, 3.000000, 4.000000), (5.000000, 6.000000, 7.000000, 8.000000), (9.000000, 10.000000, 11.000000, 12.000000), (13.000000, 14.000000, 15.000000, 16.000000))");

			Foo* foo = new Foo(10);
			ToStringTestHelper<RTTI*>(foo, "10");
			delete foo;

			// TODO -- to string for scope???
		}

		TEST_METHOD(DatumSetType)
		{
			// verify that setting the type of a nontyped Datum is valid
			// verify that setting the same type on a Datum is valid
			// verify that setting the type of a typed Datum throws an exception

			Datum datum;
			datum.setType(Datum::DatumType::Float);
			Assert::IsTrue(datum.type() == Datum::DatumType::Float);

			datum.setType(Datum::DatumType::Float);
			Assert::IsTrue(datum.type() == Datum::DatumType::Float);
			
			auto datumSetType = [&datum]{ datum.setType(Datum::DatumType::Unknown); };
			Assert::ExpectException<exception>(datumSetType);
		}

		TEST_METHOD(DatumPushBackFromString)
		{
			// table type throws exception
			Datum scopeDatum(Datum::DatumType::Table);
			auto scopePushBack = [&scopeDatum]{ scopeDatum.pushBackFromString("hello"); };
			Assert::ExpectException<exception>(scopePushBack);

			// pointer type throws exception
			Datum pointerDatum(Datum::DatumType::Pointer);
			auto pointerPushBack = [&pointerDatum]{ pointerDatum.pushBackFromString("hello"); };
			Assert::ExpectException<exception>(pointerPushBack);

			// no type throws exception
			Datum untypedDatum(Datum::DatumType::Unknown);
			auto untypedPushBack = [&untypedDatum]{ untypedDatum.pushBackFromString("0"); };
			Assert::ExpectException<exception>(untypedPushBack);

			// test other types

			// int
			Datum intDatum(Datum::DatumType::Integer);
			intDatum.pushBackFromString("0");
			intDatum.pushBackFromString("1");
			intDatum.pushBackFromString("2");
			Assert::IsTrue(intDatum.size() == 3);
			Assert::IsTrue(intDatum.get<int32_t>() == 0);
			Assert::IsTrue(intDatum.get<int32_t>(1) == 1);
			Assert::IsTrue(intDatum.get<int32_t>(2) == 2);

			// float
			Datum floatDatum(Datum::DatumType::Float);
			floatDatum.pushBackFromString("0.0");
			floatDatum.pushBackFromString("1.0");
			floatDatum.pushBackFromString("2.0");
			Assert::IsTrue(floatDatum.size() == 3);
			Assert::IsTrue(floatDatum.get<float>() == 0.0f);
			Assert::IsTrue(floatDatum.get<float>(1) == 1.0f);
			Assert::IsTrue(floatDatum.get<float>(2) == 2.0f);

			// string
			Datum strDatum(Datum::DatumType::String);
			strDatum.pushBackFromString("hello");
			strDatum.pushBackFromString("world");
			strDatum.pushBackFromString("error");
			Assert::IsTrue(strDatum.size() == 3);
			Assert::IsTrue(strDatum.get<string>() == "hello");
			Assert::IsTrue(strDatum.get<string>(1) == "world");
			Assert::IsTrue(strDatum.get<string>(2) == "error");

			// vector
			Datum vecDatum(Datum::DatumType::Vector);
			vecDatum.pushBackFromString("vec4(0, 1, 2, 3)");
			vecDatum.pushBackFromString("vec4(4, 5, 6, 7)");
			vecDatum.pushBackFromString("vec4(8, 9, 0, 1)");
			Assert::IsTrue(vecDatum.size() == 3);
			Assert::IsTrue(vecDatum.get<glm::vec4>() == glm::vec4(0, 1, 2, 3));
			Assert::IsTrue(vecDatum.get<glm::vec4>(1) == glm::vec4(4, 5, 6, 7));
			Assert::IsTrue(vecDatum.get<glm::vec4>(2) == glm::vec4(8, 9, 0, 1));

			// matrix
			Datum matDatum(Datum::DatumType::Matrix);
			matDatum.pushBackFromString("mat4x4((0, 1, 2, 3), (4, 5, 6, 7), (8, 9, 10, 11), (12, 13, 14, 15))");
			matDatum.pushBackFromString("mat4x4((1, 2, 3, 4), (5, 6, 7, 8), (9, 10, 11, 12), (13, 14, 15, 16))");
			matDatum.pushBackFromString("mat4x4((15, 14, 13, 12), (11, 10, 9, 8), (7, 6, 5, 4), (3, 2, 1, 0))");
			Assert::IsTrue(matDatum.size() == 3);
			Assert::IsTrue(matDatum.get<glm::mat4x4>() == glm::mat4x4(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));
			Assert::IsTrue(matDatum.get<glm::mat4x4>(1) == glm::mat4x4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
			Assert::IsTrue(matDatum.get<glm::mat4x4>(2) == glm::mat4x4(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0));
		}

	private:

		void ConstructorTestHelper(Datum::DatumType type)
		{
			Datum datum(type);
			Assert::IsTrue(datum.type() == type);
			Assert::IsTrue(datum.size() == 0);
		}

		template <typename T> 
		void CopyConstructorTestHelper(T* const& data)
		{
			// verify that copy constructed Datums store the same data
			Datum datum;
			datum.pushBack(data[0]);
			datum.pushBack(data[1]);
			datum.pushBack(data[2]);

			Datum otherDatum(datum);
			testDatumEquivalency(datum, otherDatum);
		}

		template <typename T>
		void MoveTestHelper(T* const& data, Datum::DatumType type)
		{
			// move constructor
			{
				// internal memory
				{
					Datum datum;
					datum.pushBack(data[0]);
					datum.pushBack(data[1]);
					datum.pushBack(data[2]);
					Datum copiedDatum(datum);

					Datum otherDatum(std::move(datum));

					Assert::IsTrue(otherDatum == copiedDatum);
					Assert::IsTrue(otherDatum != datum);

					Assert::IsTrue(datum.type() == Datum::DatumType::Unknown);
					Assert::IsTrue(datum.size() == 0);
					Assert::IsTrue(datum.capacity() == 0);
					Assert::IsFalse(datum.isExternal());
					auto emptyGet = [&datum]{ datum.get<T>(); };
					Assert::ExpectException<exception>(emptyGet);

					Assert::IsTrue(otherDatum.type() == type);
					Assert::IsTrue(otherDatum.size() == 3);
					Assert::IsTrue(otherDatum.capacity() == 4);
					Assert::IsFalse(otherDatum.isExternal());
					Assert::IsTrue(otherDatum.get<T>() == data[0]);
				}
			}

			// move assignment
			{
				// internal memory
				{
					Datum datum;
					datum.pushBack(data[0]);
					datum.pushBack(data[1]);
					datum.pushBack(data[2]);
					Datum copiedDatum(datum);

					Datum otherDatum;
					otherDatum.pushBack(data[3]);

					otherDatum = std::move(datum);

					Assert::IsTrue(otherDatum == copiedDatum);
					Assert::IsTrue(otherDatum != datum);

					Assert::IsTrue(datum.type() == Datum::DatumType::Unknown);
					Assert::IsTrue(datum.size() == 0);
					Assert::IsTrue(datum.capacity() == 0);
					Assert::IsFalse(datum.isExternal());
					auto emptyGet = [&datum]{ datum.get<T>(); };
					Assert::ExpectException<exception>(emptyGet);

					Assert::IsTrue(otherDatum.type() == type);
					Assert::IsTrue(otherDatum.size() == 3);
					Assert::IsTrue(otherDatum.capacity() == 4);
					Assert::IsFalse(otherDatum.isExternal());
					Assert::IsTrue(otherDatum.get<T>() == data[0]);
				}
			}
		}

		template <typename T>
		void DestructorTestHelper(T* const& data)
		{
			// verify that destruction has no memory leaks
			// destruction only does anything if the Datum owns its own memory and has a set type

			Datum datum;
			datum.pushBack(data[0]);
			datum.pushBack(data[1]);
			datum.pushBack(data[2]);
		}

		template <typename T> 
		void DatumAssignmentTestHelper(T* const& data)
		{
			// verify assigning an internal to another internal
			// verify assigning an internal to an external
			// verify assigning an external to an internal
			// verify assigning an external to another external

			Datum datum;
			datum.pushBack(data[0]);
			datum.pushBack(data[1]);
			datum.pushBack(data[2]);

			Datum otherDatum;
			datum.pushBack(data[3]);
			datum.pushBack(data[4]);
			otherDatum = datum;
			testDatumEquivalency(datum, otherDatum);					// internal to internal

			Datum externalDatum;
			externalDatum.setStorage(data, 3);
			otherDatum = externalDatum;
			testDatumEquivalency(externalDatum, otherDatum);			// external to internal

			Datum otherExternalDatum;
			otherExternalDatum.setStorage(data, 5);
			otherExternalDatum = externalDatum;
			testDatumEquivalency(externalDatum, otherExternalDatum);	// external to external

			externalDatum = datum;
			testDatumEquivalency(datum, externalDatum);					// internal to external
		}

		template <typename T> 
		void ScalarDatumAssignmentTestHelper(T* const& data, Datum::DatumType type)
		{
			// verify that assigning an untyped Datum sets its type
			// verify that assigning to an empty Datum sets size and capacity
			// verify that setting changes the stored value

			Datum datum;
			Assert::IsTrue(datum.size() == 0);
			Assert::IsTrue(datum.capacity() == 0);

			datum = data[0];
			Assert::IsTrue(datum.size() == 1);
			Assert::IsTrue(datum.capacity() == 1);
			Assert::IsTrue(datum.get<T>(0) == data[0]);
			Assert::IsTrue(datum.type() == type);

			datum = data[1];
			Assert::IsTrue(datum.get<T>(0) == data[1]);
		}

		template <typename T> 
		void DatumComparisonTestHelper(T* const& data, Datum::DatumType type)
		{
			// verify that if type is not the same, the Datums are inequal
			// verify that if the sizes are not the same, the Datums are inequal
			// verify that if the type and sizes are the same, but the data is not, the Datums are inequal
			// verify that if all the type, size, and data are the same, the Datums are equal

			Datum datum;
			Datum otherDatum(type);

			Assert::IsTrue(datum.type() != otherDatum.type());		// inequal type -- inequal
			Assert::IsTrue(datum.size() == otherDatum.size());
			Assert::IsFalse(datum == otherDatum);
			Assert::IsTrue(datum != otherDatum);

			datum.pushBack(data[0]);
			Assert::IsTrue(datum.type() == otherDatum.type());		// inequal size -- inequal
			Assert::IsTrue(datum.size() != otherDatum.size());
			Assert::IsFalse(datum == otherDatum);
			Assert::IsTrue(datum != otherDatum);

			otherDatum.pushBack(data[1]);
			Assert::IsTrue(datum.size() == otherDatum.size());		// inequal data -- inequal
			Assert::IsFalse(datum == otherDatum);
			Assert::IsTrue(datum != otherDatum);

			otherDatum = data[0];
			Assert::IsTrue(datum == otherDatum);					// equal all -- equal
			Assert::IsFalse(datum != otherDatum);
		}

		template <typename T> 
		void ScalarDatumComparisonTestHelper(T* const& data)
		{
			// verify that scalar comparison works

			Datum datum;
			datum.pushBack(data[0]);

			Assert::IsTrue(datum == data[0]);
			Assert::IsFalse(datum == data[1]);

			Assert::IsTrue(datum != data[1]);
			Assert::IsFalse(datum != data[0]);
		}

		template <typename T> 
		void ReserveTestHelper(T* const& data, Datum::DatumType type)
		{
			// verify that we can only reserve data when requesting more than is already present
			// verify that reserving preserves the Datum's contents
			// verify that reserving space for an external datum throws an exception

			Datum datum(type);
			uint32_t datumCapacity = datum.capacity();

			datum.reserve(5);											// can grow
			Assert::IsTrue(datum.capacity() > datumCapacity);
			datumCapacity = datum.capacity();
			datum.reserve(4);											// can't shrink
			Assert::IsTrue(datum.capacity() == datumCapacity);

			datum.pushBack(data[0]);									// preserves contents
			datum.pushBack(data[1]);
			Datum otherDatum = datum;
			otherDatum.reserve(10);
			Assert::IsTrue(datum == otherDatum);
		}

		template <typename T> 
		void ShrinkToFitTestHelper(T* const& data)
		{
			// verify that shrinking to fit matches capacity and size
			// verify that shrinking to fit preserves data
			// verify that shrinking to fit on an external Datum throws an exception

			Datum datum;
			datum.pushBack(data[0]);
			datum.pushBack(data[1]);
			datum.pushBack(data[2]);

			Assert::IsTrue(datum.capacity() != datum.size());
			datum.shrinkToFit();
			Assert::IsTrue(datum.capacity() == datum.size());			// matches size and capacity

			datum.popBack();											// preserves data (size and data same)
			datum.popBack();
			Datum otherDatum = datum;
			otherDatum.shrinkToFit();
			Assert::IsTrue(datum == otherDatum);
		}

		template <typename T> 
		void PushBackTestHelper(T* const& data, Datum::DatumType type)
		{
			// verify that pushing back on an untyped Datum sets its type	
			// verify that pushing back can grow capacity
			// verify that pushing back on an external Datum throws an exception

			Datum datum;
			Assert::IsTrue(datum.size() == 0);
			Assert::IsTrue(datum.capacity() == 0);
			Assert::IsTrue(datum.type() == Datum::DatumType::Unknown);

			datum.pushBack(data[0]);
			Assert::IsTrue(datum.size() == 1);
			Assert::IsTrue(datum.capacity() == 1);
			Assert::IsTrue(datum.type() == type);
		}

		template <typename T>
		void RemoveTestHelper(T* const& data)
		{
			Datum datum;
			datum.pushBack(data[0]);
			datum.pushBack(data[1]);
			datum.pushBack(data[2]);
			Assert::IsTrue(datum.size() == 3);

			// removing something that isn't there is a noop
			datum.remove(data[3]);
			Assert::IsTrue(datum.size() == 3);

			// removing reduces size and shuffles entries down
			datum.remove(data[0]);
			Assert::IsTrue(datum.size() == 2);
			Assert::IsTrue(datum.get<T>() == data[1]);
			Assert::IsTrue(datum.get<T>(1) == data[2]);

			// stress testing
			datum.pushBack(data[0]);

			// removing at does the same
			datum.removeAt(1);
			Assert::IsTrue(datum.size() == 2);
			Assert::IsTrue(datum.get<T>() == data[1]);
		}

		template <typename T> 
		void PopBackTestHelper(T* const& data)
		{
			// verify that popping back destroys elements without memory leaks
			// verify that popping back maintains capacity

			Datum datum;
			datum.pushBack(data[0]);
			Assert::IsTrue(datum.size() == 1);
			Assert::IsTrue(datum.capacity() == 1);

			datum.popBack();
			Assert::IsTrue(datum.size() == 0);
			Assert::IsTrue(datum.capacity() == 1);
		}

		template <typename T> 
		void ClearTestHelper(T* const& data)
		{
			// verify that clearing an external Datum throws an exception
			// verify that clear makes the Datum empty with no memory leaks
			// verify that clearing does not touch capacity

			Datum datum;
			datum.pushBack(data[0]);
			datum.pushBack(data[1]);
			datum.pushBack(data[2]);

			Assert::IsFalse(datum.isEmpty());
			Assert::IsTrue(datum.size() == 3);
			datum.clear();
			Assert::IsTrue(datum.isEmpty());
			Assert::IsTrue(datum.size() == 0);
			Assert::IsTrue(datum.capacity() > 0);
		}

		template <typename T_1, typename T_2> 
		void SetStorageTestHelper(T_1* const& data, T_2* const& otherData, Datum::DatumType type)
		{
			// verify that setting storage on a Datum with an internal allocation throws an exception
			// verify that setting storage changes size, capacity, and type
			// verify that the type cannot be overwritten

			Datum datum;
			Assert::IsTrue(datum.type() == Datum::DatumType::Unknown);
			Assert::IsTrue(datum.size() == 0);
			Assert::IsTrue(datum.capacity() == 0);

			datum.setStorage(data, 1);															// setting storage sets type, size, capacity
			Assert::IsTrue(datum.type() == type);
			Assert::IsTrue(datum.size() == 1);
			Assert::IsTrue(datum.capacity() == 1);

			auto setStorageType = [&datum, &otherData]{ datum.setStorage(otherData, 1); };		// cannot overwrite type
			Assert::ExpectException<exception>(setStorageType);

			Datum otherDatum(type);																// cannot replace allocated space
			otherDatum.reserve(5);
			auto setStorageReallocation = [&otherDatum, &data]{ otherDatum.setStorage(data, arraySize); };
			Assert::ExpectException<exception>(setStorageReallocation);
		}

		template <typename T> 
		void SetTestHelper(T* const& data)
		{
			// verify that setting changes the stored value
			// verify setting on external data

			Datum datum;
			datum.pushBack(data[0]);
			datum.pushBack(data[1]);

			datum.set(data[2], 0);
			datum.set(data[3], 1);
			Assert::IsTrue(datum.get<T>(0) == data[2]);
			Assert::IsTrue(datum.get<T>(1) == data[3]);

			Datum externalDatum;
			externalDatum.setStorage(data, 1);
			externalDatum.set(data[4]);
			Assert::IsTrue(externalDatum.get<T>(0) == data[4]);		// setting externally owned data changes it
			Assert::IsTrue(data[0] == data[4]);
		}

		void SetTestHelperRTTI(RTTI** data)
		{
			// verify that setting changes the stored value
			// verify setting on external data

			Datum datum;
			datum.pushBack(data[0]);
			datum.pushBack(data[1]);

			datum.set(data[2], 0);
			datum.set(data[3], 1);
			Assert::IsTrue(datum.get<RTTI*>(0) == data[2]);
			Assert::IsTrue(datum.get<RTTI*>(1) == data[3]);

			Datum externalDatum;
			externalDatum.setStorage(data, 1);

			delete externalDatum.get<RTTI*>();
			Foo* foo = new Foo(10);
			externalDatum.set(foo);
			Assert::IsTrue(externalDatum.get<RTTI*>(0) == foo);		// setting externally owned data changes it
			Assert::IsTrue(data[0] == foo);
		}

		void SetTestHelperScope()
		{
			// verify that setting changes the stored value
			Datum datum;
			datum.pushBack(scopes[0]);
			datum.pushBack(scopes[1]);

			datum.set(scopes[2], 0);
			datum.set(scopes[3], 1);
			Assert::IsTrue(datum.get<Scope>(0) == scopes[2]);
			Assert::IsTrue(datum.get<Scope>(1) == scopes[3]);
		}

		template <typename T> 
		void GetTestHelper(T* const& data)
		{
			// verify that validly getting returns the right value

			Datum datum;
			const Datum& constDatum = datum;

			datum.pushBack(data[0]);
			datum.pushBack(data[1]);
			datum.pushBack(data[2]);
			Assert::IsTrue(datum.get<T>(0) == data[0]);
			Assert::IsTrue(datum.get<T>(1) == data[1]);
			Assert::IsTrue(datum.get<T>(2) == data[2]);
			Assert::IsTrue(constDatum.get<T>(0) == data[0]);
			Assert::IsTrue(constDatum.get<T>(1) == data[1]);
			Assert::IsTrue(constDatum.get<T>(2) == data[2]);
		}

		template <typename T>
		void SetFromStringTestHelper(const string& str, const T& expectedValue)
		{
			// verify that setting from string sets the data to the expected value
			Datum datum;
			datum.pushBack(T());

			datum.setFromString(str);
			Assert::IsTrue(datum.get<T>() == expectedValue);
			//Assert::IsTrue(datum.toString() == str);
		}

		template <typename T>
		void ToStringTestHelper(const T& data, const string& expectedString)
		{
			// verify that to string returns the correct string

			Datum datum;
			datum.pushBack(data);
			Assert::IsTrue(datum.toString() == expectedString);
		}

		void testDatumEquivalency(const Datum& originalDatum, const Datum& copiedDatum)
		{
			// verify that all non-array members are the same

			Assert::IsTrue(originalDatum.type() == copiedDatum.type());
			Assert::IsTrue(originalDatum.size() == copiedDatum.size());
			Assert::IsTrue(originalDatum.capacity() == copiedDatum.capacity());
			Assert::IsTrue(originalDatum.isExternal() == copiedDatum.isExternal());

			Assert::IsTrue(originalDatum == copiedDatum);
		}


		static _CrtMemState sClassStartMemState;
		static _CrtMemState sStartMemState;

		static uint32_t arraySize;

		static int32_t* ints;
		static float* floats;
		static string* strs;
		static Foo** foos;
		static vec4* vecs;
		static mat4x4* mats;
		static Scope* scopes;

	};

	_CrtMemState DatumTest::sStartMemState;
	_CrtMemState DatumTest::sClassStartMemState;

	uint32_t DatumTest::arraySize;

	int32_t* DatumTest::ints;
	float* DatumTest::floats;
	string* DatumTest::strs;
	Foo** DatumTest::foos;
	vec4* DatumTest::vecs;
	mat4x4* DatumTest::mats;
	Scope* DatumTest::scopes;
}

