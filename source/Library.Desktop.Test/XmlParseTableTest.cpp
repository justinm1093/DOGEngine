
#include "pch.h"
#include "CppUnitTest.h"

#include "XmlParseMaster.h"
#include "SharedDataTable.h"

#include "XmlParseHelperTable.h"
#include "XmlParseHelperData.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DOGEngine;
using namespace std;
using namespace glm;

namespace LibraryDesktopTest
{
	TEST_CLASS(XmlParseTableTest)
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


		TEST_METHOD(XmlParseTableRTTI)
		{
			// test shared data derived class
			TestHelperRTTI<SharedDataTable, SharedData>("SharedDataTable", "SharedData");

			// test xml parse helper derived classes
			TestHelperRTTI<XmlParseHelperTable, IXmlParseHelper>("XmlParseHelperTable", "IXmlParseHelper");
			TestHelperRTTI<XmlParseHelperData, IXmlParseHelper>("XmlParseHelperData", "IXmlParseHelper");
		}

		TEST_METHOD(XmlParseTableScopes)
		{
			// parse data that is just scopes
			SharedDataTable tableData;
			XmlParseMaster master(tableData);

			XmlParseHelperTable helperScope;
			master.addHelper(helperScope);

			Assert::IsTrue(master.parseFromFile(sXmlFilePath_Scopes));

			// verify the data
			Assert::IsTrue(tableData.depth() == 0);
			Assert::IsTrue(tableData.getScope() != nullptr);
			Scope& parsedTable = *tableData.getScope();

			Assert::IsTrue(parsedTable.size() == 2);
			Assert::IsTrue(parsedTable["nested_1"].size() == 1);
			Assert::IsTrue(parsedTable["nested_1"][0]["bottom"].size() == 2);
			Assert::IsTrue(parsedTable["nested_1"][0]["bottom"][0].isEmpty());
			Assert::IsTrue(parsedTable["nested_1"][0]["bottom"][1].isEmpty());

			Assert::IsTrue(parsedTable["nested_2"].size() == 1);
			Assert::IsTrue(parsedTable["nested_2"][0].isEmpty());
		}

		TEST_METHOD(XmlParseTableInts)
		{
			// parse data that is just ints
			SharedDataTable tableData;
			XmlParseMaster master(tableData);

			XmlParseHelperTable helperScope;
			XmlParseHelperData helperData;
			master.addHelper(helperScope);
			master.addHelper(helperData);

			Assert::IsTrue(master.parseFromFile(sXmlFilePath_Ints));

			// verify the data
			Assert::IsTrue(tableData.depth() == 0);
			Assert::IsTrue(tableData.getScope() != nullptr);
			Scope& parsedTable = *tableData.getScope();

			Assert::IsTrue(parsedTable.size() == 5);
			Assert::IsTrue(parsedTable["health"] == 100);
			Assert::IsTrue(parsedTable["weight"] == 127);
			Assert::IsTrue(parsedTable["height"] == 69);
			Assert::IsTrue(parsedTable["age"] == 23);

			Assert::IsTrue(parsedTable["numbers"].size() == 3);
			Assert::IsTrue(parsedTable["numbers"].get<int32_t>() == 0);
			Assert::IsTrue(parsedTable["numbers"].get<int32_t>(1) == 1);
			Assert::IsTrue(parsedTable["numbers"].get<int32_t>(2) == 2);
		}

		TEST_METHOD(XmlParseTableFloats)
		{
			// parse data that is just floats
			SharedDataTable tableData;
			XmlParseMaster master(tableData);

			XmlParseHelperTable helperScope;
			XmlParseHelperData helperData;
			master.addHelper(helperScope);
			master.addHelper(helperData);

			Assert::IsTrue(master.parseFromFile(sXmlFilePath_Floats));

			// verify the data
			Assert::IsTrue(tableData.depth() == 0);
			Assert::IsTrue(tableData.getScope() != nullptr);
			Scope& parsedTable = *tableData.getScope();

			Assert::IsTrue(parsedTable.size() == 5);
			Assert::IsTrue(parsedTable["health"] == 100.0f);
			Assert::IsTrue(parsedTable["weight"] == 127.0f);
			Assert::IsTrue(parsedTable["height"] == 69.0f);
			Assert::IsTrue(parsedTable["age"] == 23.0f);

			Assert::IsTrue(parsedTable["numbers"].size() == 3);
			Assert::IsTrue(parsedTable["numbers"].get<float>() == 0.0f);
			Assert::IsTrue(parsedTable["numbers"].get<float>(1) == 1.0f);
			Assert::IsTrue(parsedTable["numbers"].get<float>(2) == 2.0f);
		}

		TEST_METHOD(XmlParseTableStrings)
		{
			// parse data that is just strings
			SharedDataTable tableData;
			XmlParseMaster master(tableData);

			XmlParseHelperTable helperScope;
			XmlParseHelperData helperData;
			master.addHelper(helperScope);
			master.addHelper(helperData);

			Assert::IsTrue(master.parseFromFile(sXmlFilePath_Strings));

			// verify the data
			Assert::IsTrue(tableData.depth() == 0);
			Assert::IsTrue(tableData.getScope() != nullptr);
			Scope& parsedTable = *tableData.getScope();

			Assert::IsTrue(parsedTable.size() == 4);
			Assert::IsTrue(parsedTable["name_first"] == "Justin");
			Assert::IsTrue(parsedTable["name_last"] == "Moser");
			Assert::IsTrue(parsedTable["status"] == "Tired");

			Assert::IsTrue(parsedTable["hobbies"].size() == 2);
			Assert::IsTrue(parsedTable["hobbies"].get<string>() == "Drawing");
			Assert::IsTrue(parsedTable["hobbies"].get<string>(1) == "Rock Climbing");
		}

		TEST_METHOD(XmlParseTableVector)
		{
			// parse data that is just vectors
			SharedDataTable tableData;
			XmlParseMaster master(tableData);

			XmlParseHelperTable helperScope;
			XmlParseHelperData helperData;
			master.addHelper(helperScope);
			master.addHelper(helperData);

			Assert::IsTrue(master.parseFromFile(sXmlFilePath_Vectors));

			// verify the data
			Assert::IsTrue(tableData.depth() == 0);
			Assert::IsTrue(tableData.getScope() != nullptr);
			Scope& parsedTable = *tableData.getScope();

			Assert::IsTrue(parsedTable.size() == 1);
			Assert::IsTrue(parsedTable["vec"].type() == Datum::DatumType::Vector);
			Assert::IsTrue(parsedTable["vec"] == vec4(0, 1, 2, 3));
		}

		TEST_METHOD(XmlParseTableMatrix)
		{
			// parse data that is just matricies
			SharedDataTable tableData;
			XmlParseMaster master(tableData);

			XmlParseHelperTable helperScope;
			XmlParseHelperData helperData;
			master.addHelper(helperScope);
			master.addHelper(helperData);

			Assert::IsTrue(master.parseFromFile(sXmlFilePath_Matrix));

			// verify the data
			Assert::IsTrue(tableData.depth() == 0);
			Assert::IsTrue(tableData.getScope() != nullptr);
			Scope& parsedTable = *tableData.getScope();

			Assert::IsTrue(parsedTable.size() == 1);
			Assert::IsTrue(parsedTable["matrix"].type() == Datum::DatumType::Matrix);
			Assert::IsTrue(parsedTable["matrix"] == mat4x4(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));
		}

		TEST_METHOD(XmlParseTableFull)
		{
			// parse data from a full table xml file
			SharedDataTable tableData;
			XmlParseMaster master(tableData);

			XmlParseHelperTable helperScope;
			XmlParseHelperData helperData;
			master.addHelper(helperScope);
			master.addHelper(helperData);

			Assert::IsTrue(master.parseFromFile(sXmlFilePath_Full));

			// verify the data
			{
				Assert::IsTrue(tableData.depth() == 0);
				Assert::IsTrue(tableData.getScope() != nullptr);
				Scope& parsedTable = *tableData.getScope();

				Assert::IsTrue(parsedTable.size() == 3);
				Assert::IsTrue(parsedTable["test"] == "This works!");

				Assert::IsTrue(parsedTable["Person"].size() == 2);
				Scope& firstPerson = parsedTable["Person"][0];
				Scope& secondPerson = parsedTable["Person"][1];

				Assert::IsTrue(firstPerson["name"][0]["first"] == "Justin");
				Assert::IsTrue(firstPerson["name"][0]["middle"] == "William");
				Assert::IsTrue(firstPerson["name"][0]["last"] == "Moser");
				Assert::IsTrue(firstPerson["age"] == 23);
				Assert::IsTrue(firstPerson["height"] == 5.75f);
				Assert::IsTrue(firstPerson["weight"] == 127.0f);
				Assert::IsTrue(firstPerson["direction"] == vec4(0, 1, 2, 3));
				Assert::IsTrue(firstPerson["matrix"] == mat4x4(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));

				Assert::IsTrue(secondPerson["name"][0]["first"] == "Brandon");
				Assert::IsTrue(secondPerson["name"][0]["middle"] == "Gerard");
				Assert::IsTrue(secondPerson["name"][0]["last"] == "Moser");
				Assert::IsTrue(secondPerson["age"] == 19);
				Assert::IsTrue(secondPerson["direction"] == vec4(3, 2, 1, 0));
				Assert::IsTrue(secondPerson["matrix"] == mat4x4(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0));

				Scope& gameScope = parsedTable["Game"][0];
				Assert::IsTrue(gameScope["name"] == "The Draft");
				Assert::IsTrue(gameScope["genre"].get<string>() == "RTS");
				Assert::IsTrue(gameScope["genre"].get<string>(1) == "Swordfighting");
			}

			// test this with an additional parse
			Assert::IsTrue(master.parseFromFile(sXmlFilePath_Full));
		}

		TEST_METHOD(XmlParseTableExtractScope)
		{
			Scope* extractedScope = nullptr;

			{
				// parse data from a full table xml file
				SharedDataTable tableData;
				XmlParseMaster master(tableData);

				XmlParseHelperTable helperScope;
				XmlParseHelperData helperData;
				master.addHelper(helperScope);
				master.addHelper(helperData);

				Assert::IsTrue(master.parseFromFile(sXmlFilePath_Full));

				extractedScope = tableData.extractScope();
			}
			
			Assert::IsTrue(extractedScope != nullptr);
			Assert::IsTrue(!extractedScope->isEmpty());
			delete extractedScope;
		}

		TEST_METHOD(XmlParseInvalid)
		{
			// parse data from a full table xml file
			SharedDataTable tableData;
			XmlParseMaster master(tableData);

			XmlParseHelperTable helperScope;
			XmlParseHelperData helperData;
			master.addHelper(helperScope);
			master.addHelper(helperData);

			// parsing with no name set on Scope tag throws exception
			auto parseNoSetName = [&master]{ master.parseFromFile(sXmlFilePath_NoSetName); };
			Assert::ExpectException<exception>(parseNoSetName);

			// parsing with no name attribute throws exception
			auto parseNoNameAttr = [&master]{ master.parseFromFile(sXmlFilePath_NoName); };
			Assert::ExpectException<exception>(parseNoNameAttr);

			// parsing with no value attribute throws exception
			auto parseNoValAttr = [&master]{ master.parseFromFile(sXmlFilePath_NoVal); };
			Assert::ExpectException<exception>(parseNoValAttr);
		}

	private:

		template <typename DerivedT, typename BaseT>
		void TestHelperRTTI(const string& derivedTypeName, const string& baseTypeName)
		{
			DerivedT obj;
			RTTI* rtti = (RTTI*)(&obj);

			// test type id's
			Assert::IsTrue(DerivedT::TypeIdClass() == obj.TypeIdInstance());
			Assert::IsTrue(rtti->TypeIdInstance() == obj.TypeIdInstance());

			// test type name
			Assert::IsTrue(BaseT::TypeName() == baseTypeName);

			Assert::IsTrue(DerivedT::TypeName() == derivedTypeName);
			Assert::IsTrue(obj.TypeName() == derivedTypeName);

			// test query interface
			Assert::IsTrue(rtti->QueryInterface(DerivedT::TypeIdClass()) != nullptr);

			// test is with string
			Assert::IsTrue(obj.Is(baseTypeName));
			Assert::IsTrue(rtti->Is(baseTypeName));

			Assert::IsTrue(obj.Is(derivedTypeName));
			Assert::IsTrue(rtti->Is(derivedTypeName));

			// test is with int
			Assert::IsTrue(obj.Is(BaseT::TypeIdClass()));
			Assert::IsTrue(rtti->Is(BaseT::TypeIdClass()));

			Assert::IsTrue(obj.Is(DerivedT::TypeIdClass()));
			Assert::IsTrue(rtti->Is(DerivedT::TypeIdClass()));

			// test as
			Assert::IsTrue(rtti->As<BaseT>() != nullptr);
			Assert::IsTrue(rtti->As<DerivedT>() != nullptr);
		}

		static _CrtMemState sStartMemState;

		const static string sXmlFilePath_Scopes;
		const static string sXmlFilePath_Ints;
		const static string sXmlFilePath_Floats;
		const static string sXmlFilePath_Strings;
		const static string sXmlFilePath_Vectors;
		const static string sXmlFilePath_Matrix;
		const static string sXmlFilePath_Full;

		const static string sXmlFilePath_NoSetName;
		const static string sXmlFilePath_NoName;
		const static string sXmlFilePath_NoVal;
	};

	_CrtMemState XmlParseTableTest::sStartMemState;

	const string XmlParseTableTest::sXmlFilePath_Scopes = "assets/xml/Table_valid/test_scopes.xml";
	const string XmlParseTableTest::sXmlFilePath_Ints = "assets/xml/Table_valid/test_ints.xml";
	const string XmlParseTableTest::sXmlFilePath_Floats = "assets/xml/Table_valid/test_floats.xml";
	const string XmlParseTableTest::sXmlFilePath_Strings = "assets/xml/Table_valid/test_strings.xml";
	const string XmlParseTableTest::sXmlFilePath_Vectors = "assets/xml/Table_valid/test_vectors.xml";
	const string XmlParseTableTest::sXmlFilePath_Matrix = "assets/xml/Table_valid/test_matrix.xml";
	const string XmlParseTableTest::sXmlFilePath_Full = "assets/xml/Table_valid/test_full.xml";

	const string XmlParseTableTest::sXmlFilePath_NoSetName = "assets/xml/Table_invalid/test_nosetname.xml";
	const string XmlParseTableTest::sXmlFilePath_NoName = "assets/xml/Table_invalid/test_noname.xml";
	const string XmlParseTableTest::sXmlFilePath_NoVal = "assets/xml/Table_invalid/test_noval.xml";
}

