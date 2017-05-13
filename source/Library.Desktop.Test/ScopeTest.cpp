
#include "pch.h"
#include "CppUnitTest.h"

#include "Foo.h"
#include "Scope.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DOGEngine;
using namespace UnitTests;
using namespace std;

namespace LibraryDesktopTest
{
	TEST_CLASS(ScopeTest)
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


		TEST_METHOD(ScopeRTTI)
		{
			Scope scope;
			RTTI* rtti = (RTTI*)(&scope);

			// test query interface
			Assert::IsTrue(rtti->QueryInterface(Scope::TypeIdClass()) != nullptr);

			// test is with string
			Assert::IsTrue(rtti->Is("Scope"));
			Assert::IsFalse(rtti->Is("Hello"));

			// test is with int
			Assert::IsTrue(rtti->Is(Scope::TypeIdClass()));
			Assert::IsFalse(rtti->Is(0));

			// test as
			Scope* asScope = rtti->As<Scope>();
			Assert::IsFalse(asScope == nullptr);
			Assert::IsTrue(asScope == &scope);
		}

		TEST_METHOD(ScopeDefaultConstructor)
		{
			// verify that the default constructor creates an emtpy scope

			Scope scope;
			Assert::IsTrue(scope.size() == 0);
			Assert::IsTrue(scope.isEmpty());

			Scope otherScope(5);
			Assert::IsTrue(otherScope.size() == 0);
			Assert::IsTrue(scope.isEmpty());
		}

		TEST_METHOD(ScopeCopyConstructor)
		{
			Scope* brothersScope = new Scope(2);
			Scope* personScope_1 = new Scope(5);
			Scope* personScope_2 = new Scope(5);
			
			brothersScope->adopt("older", *personScope_1);
			brothersScope->adopt("younger", *personScope_2);

			personScope_1->append("name_first") = "Justin";
			personScope_1->append("name_last") = "Moser";
			personScope_1->append("age") = 23;
			personScope_1->append("height") = 5.7f;
			personScope_1->append("weight") = 124.0f;

			personScope_2->append("name_first") = "Brandom";
			personScope_2->append("name_last") = "Moser";
			personScope_2->append("age") = 19;
			personScope_2->append("height") = 5.4f;
			personScope_2->append("weight") = 150.0f;
			
			// verify recursive copy construction
			Scope otherBrothersScope(*brothersScope);
			Assert::IsTrue(*brothersScope == otherBrothersScope);
			Assert::IsFalse(*brothersScope != otherBrothersScope);

			delete brothersScope;
		}

		TEST_METHOD(ScopeMove)
		{
			// move constructor
			{
				// build out a scope tree
				Scope scope(2);
				Scope* personScope_1 = new Scope(5);
				Scope* personScope_2 = new Scope(5);

				scope.adopt("older", *personScope_1);
				scope.adopt("younger", *personScope_2);

				personScope_1->append("name_first") = "Justin";
				personScope_1->append("name_last") = "Moser";
				personScope_1->append("age") = 23;
				personScope_1->append("height") = 5.7f;
				personScope_1->append("weight") = 124.0f;

				personScope_2->append("name_first") = "Brandom";
				personScope_2->append("name_last") = "Moser";
				personScope_2->append("age") = 19;
				personScope_2->append("height") = 5.4f;
				personScope_2->append("weight") = 150.0f;

				// copied scope (for verification)
				Scope copiedScope(scope);

				// moved scope
				Scope otherScope(std::move(scope));

				// move construction makes scope equivalent to the original scope, but original scope is empty
				Assert::IsTrue(otherScope == copiedScope);
				Assert::IsTrue(otherScope != scope);
				Assert::IsTrue(scope.isEmpty());

				// handles child scopes and reparents them
				Assert::IsTrue(otherScope["older"] == *personScope_1);
				Assert::IsTrue(otherScope["younger"] == *personScope_2);
				Assert::IsTrue(personScope_1->getParent() == &otherScope);
				Assert::IsTrue(personScope_2->getParent() == &otherScope);
			}

			// move assignment
			{
				// build out a scope tree
				Scope scope(2);
				Scope* childScope_1 = new Scope(5);
				Scope* childScope_2 = new Scope(5);

				scope.adopt("older", *childScope_1);
				scope.adopt("younger", *childScope_2);

				childScope_1->append("name_first") = "Justin";
				childScope_1->append("name_last") = "Moser";
				childScope_1->append("age") = 23;
				childScope_1->append("height") = 5.7f;
				childScope_1->append("weight") = 124.0f;

				childScope_2->append("name_first") = "Brandom";
				childScope_2->append("name_last") = "Moser";
				childScope_2->append("age") = 19;
				childScope_2->append("height") = 5.4f;
				childScope_2->append("weight") = 150.0f;

				// copied scope (for verification)
				Scope copiedScope(scope);

				// moved scope
				Scope otherScope;
				Scope* childScope = new Scope();
				otherScope.adopt("scope", *childScope);
				otherScope["hello"] = 100;
				otherScope["world"] = 200;
				childScope->append("error") = 300;

				otherScope = std::move(scope);

				// move assignment makes scope equivalent to the original scope, but original scope is empty
				Assert::IsTrue(otherScope == copiedScope);
				Assert::IsTrue(otherScope != scope);
				Assert::IsTrue(scope.isEmpty());

				// handles child scopes and reparents them
				Assert::IsTrue(otherScope["older"] == *childScope_1);
				Assert::IsTrue(otherScope["younger"] == *childScope_2);
				Assert::IsTrue(childScope_1->getParent() == &otherScope);
				Assert::IsTrue(childScope_2->getParent() == &otherScope);
			}
		}

		TEST_METHOD(ScopeDestructor)
		{
			Scope* brothersScope = new Scope(2);
			Scope* personScope_1 = new Scope(5);
			Scope* personScope_2 = new Scope(5);

			brothersScope->adopt("older", *personScope_1);
			brothersScope->adopt("younger", *personScope_2);

			personScope_1->append("name_first") = "Justin";
			personScope_1->append("name_last") = "Moser";
			personScope_1->append("age") = 23;
			personScope_1->append("height") = 5.7f;
			personScope_1->append("weight") = 124.0f;

			personScope_2->append("name_first") = "Brandom";
			personScope_2->append("name_last") = "Moser";
			personScope_2->append("age") = 19;
			personScope_2->append("height") = 5.4f;
			personScope_2->append("weight") = 150.0f;

			// verify recursive delete
			delete brothersScope;
		}

		TEST_METHOD(ScopeAssignmentOperator)
		{
			Scope* brothersScope = new Scope(2);
			Scope* personScope_1 = new Scope(5);
			Scope* personScope_2 = new Scope(5);

			brothersScope->adopt("older", *personScope_1);
			brothersScope->adopt("younger", *personScope_2);

			personScope_1->append("name_first") = "Justin";
			personScope_1->append("name_last") = "Moser";
			personScope_1->append("age") = 23;
			personScope_1->append("height") = 5.7f;
			personScope_1->append("weight") = 124.0f;

			personScope_2->append("name_first") = "Brandom";
			personScope_2->append("name_last") = "Moser";
			personScope_2->append("age") = 19;
			personScope_2->append("height") = 5.4f;
			personScope_2->append("weight") = 150.0f;

			// verify recursive assignment
			Scope otherBrothersScope;
			otherBrothersScope = *brothersScope;
			Assert::IsTrue(*brothersScope == otherBrothersScope);
			Assert::IsFalse(*brothersScope != otherBrothersScope);

			delete brothersScope;
		}

		TEST_METHOD(ScopeEqualityOperators)
		{
			// verify equality operators
			Scope scope(2);
			Scope otherScope(2);

			Assert::IsTrue(scope == otherScope);		// no data in each, equal scopes
			Assert::IsFalse(scope != otherScope);

			scope["hello"] = 100;						// inequal size, inequal scopes
			Assert::IsFalse(scope == otherScope);
			Assert::IsTrue(scope != otherScope);

			otherScope["world"] = 100;					// same size but different labels, inequal scopes
			Assert::IsFalse(scope == otherScope);
			Assert::IsTrue(scope != otherScope);

			scope["world"] = 100;						// same size and labels but different data, inequal scopes
			otherScope["hello"] = 50;
			Assert::IsFalse(scope == otherScope);
			Assert::IsTrue(scope != otherScope);

			otherScope["hello"] = 100;					// equal (size, labels, data at labels), equal scopes
			Assert::IsTrue(scope == otherScope);
			Assert::IsFalse(scope != otherScope);

			// NOTE -- copy constructor and assignment operator test recursive equality
		}

		TEST_METHOD(ScopeBracketOperator)
		{
			TestBrackets();
			TestAtMethod();
		}

		TEST_METHOD(ScopeFind)
		{
			// finding a key that is not in the Scope returns nullptr
			Scope scope(3);
			Assert::IsTrue(scope.find("name") == nullptr);

			scope.append("name") = "Justin";
			scope.append("health") = 100;
			scope.append("age") = 23;

			// finding a key that is should return the correct Datum*
			Assert::IsTrue(*(scope.find("name")) == "Justin");
			Assert::IsTrue(*(scope.find("health")) == 100);
			Assert::IsTrue(*(scope.find("age")) == 23);

			Assert::IsTrue(scope.find("name") == &scope["name"]);
			Assert::IsTrue(scope.find("health") == &scope["health"]);
			Assert::IsTrue(scope.find("age") == &scope["age"]);
		}

		TEST_METHOD(ScopeSearch)
		{
			Scope rootScope;

			Scope& childScope_1 = rootScope.appendScope("child_1");
			Scope& childScope_2 = rootScope.appendScope("child_2");
			Scope& middleScope = childScope_2.appendScope("child");
			Scope& bottomScope = middleScope.appendScope("child");
			
			rootScope["head_value"] = 100;
			childScope_1["uncle_value"] = 200;
			childScope_2["value"] = 13;
			middleScope["value"] = 300;
			bottomScope["bottom_value"] = 400;
			
			const Scope* keyOwner = nullptr;
			Datum* keyDatum = nullptr;

			// searching for something not in the ancestry returns nullptr
			keyDatum = rootScope.search("not_in_ancestry", &keyOwner);
			Assert::IsTrue(keyOwner == nullptr);
			Assert::IsTrue(keyDatum == nullptr);

			keyDatum = bottomScope.search("not_in_ancestry", &keyOwner);
			Assert::IsTrue(keyOwner == nullptr);
			Assert::IsTrue(keyDatum == nullptr);

			// searching for something in the tree, but not on our branch (belongs to aunt / uncle / cousin)
			keyDatum = bottomScope.search("uncle_value", &keyOwner);
			Assert::IsTrue(keyOwner == nullptr);
			Assert::IsTrue(keyDatum == nullptr);

			// searching for something that exists in us (at our level)
			keyDatum = rootScope.search("head_value", &keyOwner);
			Assert::IsTrue(keyOwner == &rootScope);
			Assert::IsTrue(keyDatum != nullptr);
			Assert::IsTrue(*keyDatum == 100);
			
			// searching for something that exists above us (up ancestry chain)
			keyDatum = bottomScope.search("head_value", &keyOwner);
			Assert::IsTrue(keyOwner == &rootScope);
			Assert::IsTrue(keyDatum != nullptr);
			Assert::IsTrue(*keyDatum == 100);		

			// searching for something returns the closest instance
			//		from bottomScope, closest "value" lives in middleScope
			keyDatum = bottomScope.search("value", &keyOwner);
			Assert::IsTrue(keyOwner == &middleScope);
			Assert::IsTrue(keyDatum != nullptr);
			Assert::IsTrue(*keyDatum == 300);
		}

		TEST_METHOD(ScopeFindName)
		{
			// verify that finding the name for a scope that does not exist in the invoking Scope is ""
			// verify that finding the name for a scope returns the right string
			
			Scope rootScope;

			Scope& child_1a = rootScope.appendScope("child_1");
			Scope& child_1b = rootScope.appendScope("child_1");
			Scope& child_2 = rootScope.appendScope("child_2");
			Scope& child_bottom = child_2.appendScope("child");

			child_1a["value"] = 100;
			child_1b["value"] = 200;
			child_2["value"] = 500;
			child_bottom["value"] = 1000;

			// finding name of a scope not directly owned by caller returns ""
			Scope notInTree;
			Assert::IsTrue(rootScope.findName(&notInTree) == "");
			Assert::IsTrue(rootScope.findName(&child_bottom) == "");

			// finding name of scope returns the appropriate field name
			Assert::IsTrue(rootScope.findName(&child_1a) == "child_1");
			Assert::IsTrue(rootScope.findName(&child_1b) == "child_1");
			Assert::IsTrue(rootScope.findName(&child_2) == "child_2");
			Assert::IsTrue(child_2.findName(&child_bottom) == "child");
		}

		TEST_METHOD(ScopeAppend)
		{
			Datum datum, otherDatum;
			otherDatum.pushBack(10);

			Scope scope;
			Assert::IsTrue(scope.size() == 0);

			// append throws exception when given empty string
			auto scopeEmptyAppend = [&scope]{ scope.append(""); };
			Assert::ExpectException<exception>(scopeEmptyAppend);

			// append inserts a new field when the Scope doesn't have the key
			//		default datum created, size increased
			Datum& firstAppend = scope.append("value");
			Assert::IsTrue(firstAppend == datum);
			Assert::IsTrue(scope.size() == 1);

			// append returns the Datum already there if the key is in the Scope
			//		previous datum returned, nothing is inserted (but we can do things to the returned Datum)
			scope.append("value").pushBack(10);
			Assert::IsTrue(scope.append("value") == otherDatum);
			Assert::IsTrue(scope.size() == 1);
		}

		TEST_METHOD(ScopeAppendScope)
		{
			Scope errorScope;

			// appending scope with an empty key throws exception
			auto appendScopeEmpty = [&errorScope]{ errorScope.appendScope(""); };
			Assert::ExpectException<exception>(appendScopeEmpty);

			// appending scope with a key whose Datum is not of type Table throws exception
			errorScope["int_value"] = 100;
			Assert::IsTrue(errorScope["int_value"].type() == Datum::DatumType::Integer);

			auto appendScopeTypeMismatch = [&errorScope]{ errorScope.appendScope("int_value"); };
			Assert::ExpectException<exception>(appendScopeTypeMismatch);



			Scope scope;
			Assert::IsTrue(scope.size() == 0);

			// appending scope with a new key creates a new entry in the Scope and sets its type to Table
			Scope& appendedScope_1 = scope.appendScope("scopeKey");
			appendedScope_1["appendedKey"] = 500;
			Assert::IsTrue(scope.size() == 1);

			Assert::IsTrue(scope["scopeKey"].type() == Datum::DatumType::Table);
			Assert::IsTrue(scope["scopeKey"].size() == 1);

			// appending scope to a name already in the Scope (and with type Table) pushes the new scope back
			Scope& appendedScope_2 = scope.appendScope("scopeKey");
			Assert::IsTrue(scope.size() == 1);
			Assert::IsTrue(scope["scopeKey"].size() == 2);

			Assert::IsTrue(scope["scopeKey"][0] == appendedScope_1);
			Assert::IsTrue(scope["scopeKey"][1] == appendedScope_2);
		}

		TEST_METHOD(ScopeClear)
		{
			Scope* brothersScope = new Scope(2);

			Scope& personScope_1 = brothersScope->appendScope("older");
			Scope& personScope_2 = brothersScope->appendScope("younger");
			Scope& bottomScope = personScope_1.appendScope("child");

			personScope_1.append("name_first") = "Justin";
			personScope_1.append("name_last") = "Moser";
			personScope_1.append("age") = 23;
			personScope_1.append("height") = 5.7f;
			personScope_1.append("weight") = 124.0f;

			personScope_2.append("name_first") = "Brandom";
			personScope_2.append("name_last") = "Moser";
			personScope_2.append("age") = 19;
			personScope_2.append("height") = 5.4f;
			personScope_2.append("weight") = 150.0f;

			bottomScope.append("value") = 100;

			// no Scopes in the tree are empty
			Assert::IsTrue(!brothersScope->isEmpty());
			Assert::IsTrue(!personScope_1.isEmpty());
			Assert::IsTrue(!personScope_2.isEmpty());
			Assert::IsTrue(!bottomScope.isEmpty());

			// clearing from the bottom of the tree clears only that element
			Assert::IsTrue(bottomScope.size() == 1);
			bottomScope.clear();
			Assert::IsTrue(bottomScope.size() == 0);
			Assert::IsTrue(bottomScope.isEmpty());

			// clearing from the middle of the tree destroys child Scopes
			Assert::IsTrue(personScope_1.size() == 6);
			personScope_1.clear();
			Assert::IsTrue(personScope_1.size() == 0);
			Assert::IsTrue(personScope_1.isEmpty());

			// clearing from the head destroys everything
			Assert::IsTrue(brothersScope->size() == 2);
			brothersScope->clear();
			Assert::IsTrue(brothersScope->size() == 0);
			Assert::IsTrue(brothersScope->isEmpty());

			delete brothersScope;
		}

		TEST_METHOD(ScopeAdopt)
		{
			Scope* scope = new Scope();
			Scope* otherScope = new Scope();

			Scope& childScope = scope->appendScope("cool_guy");
			childScope["name_first"] = "Justin";
			childScope["name_last"] = "Moser";

			Assert::IsTrue(scope->size() == 1);
			Assert::IsTrue(scope->find("cool_guy")->size() == 1);

			// adopting to a key that is not of type Table throws exception
			//		throws exception because Justin Moser is, in fact, a cool guy
			//		(and because we attempted to adopt the containing Scope into a string field)
			Scope errorScope;
			errorScope["not_cool_guy"] = "The FIEA Bandit";
			auto mismatchAdopt = [&errorScope, &childScope]{ errorScope.adopt("not_cool_guy", childScope); };
			Assert::ExpectException<exception>(mismatchAdopt);

			// adopting with an empty key throws exception
			auto emptyAdopt = [&errorScope, &childScope]{ errorScope.adopt("", childScope); };
			Assert::ExpectException<exception>(emptyAdopt);



			// verify that we can adopt into a new field
			Assert::IsTrue(childScope.getParent() == scope);
			Assert::IsTrue(childScope.getParent() != otherScope);

			otherScope->adopt("student", childScope);

			Assert::IsTrue(childScope.getParent() == otherScope);
			Assert::IsTrue(childScope.getParent() != scope);

			// scope's size doesn't decrease, as we didn't delete the key-value pair
			//		it's still there, but the Datum at that key is now empty
			// otherScope's size increased, since we appended a new field
			Assert::IsTrue(scope->size() == 1);
			Assert::IsTrue(scope->find("cool_guy")->size() == 0);

			Assert::IsTrue(otherScope->size() == 1);
			Assert::IsTrue(otherScope->find("student")->size() == 1);

			// adopt into an existing field pushes back the item
			Assert::IsTrue(scope->getParent() == nullptr);
			otherScope->adopt("student", *scope);
			Assert::IsTrue(scope->getParent() == otherScope);

			Assert::IsTrue(otherScope->size() == 1);
			Assert::IsTrue(otherScope->find("student")->size() == 2);
			Assert::IsTrue(otherScope->find("student")->operator[](0) == childScope);
			Assert::IsTrue(otherScope->find("student")->operator[](1) == *scope);

			// since otherScope adopted scope, we no longer have a responsibility to delete it
			delete otherScope;
		}

		TEST_METHOD(ScopeOrphan)
		{
			Scope rootScope;
			Scope& rootChild_1a = rootScope.appendScope("child");
			Scope& rootChild_1b = rootScope.appendScope("child");
			Scope& rootChild_2 = rootScope.appendScope("otherChild");
			Scope& bottomChild = rootChild_1b.appendScope("child");

			// confirm parents
			Assert::IsTrue(rootChild_1a.getParent() == &rootScope);
			Assert::IsTrue(rootChild_1b.getParent() == &rootScope);
			Assert::IsTrue(rootChild_2.getParent() == &rootScope);
			Assert::IsTrue(bottomChild.getParent() == &rootChild_1b);

			// orphaning Scope sets its parent to nullptr and removes the Scope from its parent datum
			//		the datum is still in the key-value pair
			Assert::IsTrue(rootChild_1b[0].size() == 1);
			bottomChild.orphan();
			Assert::IsTrue(bottomChild.getParent() == nullptr);
			Assert::IsTrue(rootChild_1b[0].size() == 0);

			// orphans are responsible for their own destruction (unless another Scope picks them up)
			delete &bottomChild;
		}

		TEST_METHOD(ScopeToString)
		{
			Scope* brothersScope = new Scope(2);
			Scope* personScope_1 = new Scope(5);
			Scope* personScope_2 = new Scope(5);

			// test populated nested Scopes for toString
			brothersScope->adopt("older", *personScope_1);
			brothersScope->adopt("younger", *personScope_2);

			personScope_1->append("name_first") = "Justin";
			personScope_1->append("name_last") = "Moser";
			personScope_1->append("age") = 23;
			personScope_1->append("height") = 5.7f;
			personScope_1->append("weight") = 124.0f;

			personScope_2->append("name_first") = "Brandom";
			personScope_2->append("name_last") = "Moser";
			personScope_2->append("age") = 19;
			personScope_2->append("height") = 5.4f;
			personScope_2->append("weight") = 150.0f;

			// test an array of child Scopes under a single tag
			Scope& child_1 = brothersScope->appendScope("child");
			Scope& child_2 = brothersScope->appendScope("child");

			// test an empty tag, an empty child Scope, and another more nested Scope
			child_1["emptyTag"];
			child_1.appendScope("emptyChildScope");
			child_1.appendScope("childScope").append("value") = 500;

			// test an array of values
			for(uint32_t i = 0; i < 3; ++i)
			{
				child_2["ints"].pushBack((int32_t)i);
			}

			string str = brothersScope->ToString();
			str;

			delete brothersScope;
		}

		TEST_METHOD(ScopeIsAncestor)
		{
			Scope* brothersScope = new Scope(2);
			Scope* personScope_1 = new Scope(5);
			Scope* personScope_2 = new Scope(5);

			brothersScope->adopt("older", *personScope_1);
			brothersScope->adopt("younger", *personScope_2);

			personScope_1->append("name_first") = "Justin";
			personScope_1->append("name_last") = "Moser";
			personScope_1->append("age") = 23;
			personScope_1->append("height") = 5.7f;
			personScope_1->append("weight") = 124.0f;

			personScope_2->append("name_first") = "Brandom";
			personScope_2->append("name_last") = "Moser";
			personScope_2->append("age") = 19;
			personScope_2->append("height") = 5.4f;
			personScope_2->append("weight") = 150.0f;

			// scopes are ancestors of themselves
			Assert::IsTrue(brothersScope->isAncestor(*brothersScope));
			Assert::IsTrue(personScope_1->isAncestor(*personScope_1));
			Assert::IsTrue(personScope_2->isAncestor(*personScope_2));

			// verify isAncestor
			Assert::IsTrue(personScope_1->isAncestor(*brothersScope));
			Assert::IsTrue(personScope_2->isAncestor(*brothersScope));

			Assert::IsFalse(personScope_1->isAncestor(*personScope_2));
			Assert::IsFalse(personScope_2->isAncestor(*personScope_1));

			// no relation whatsoever
			Scope* notRelated = new Scope();
			Assert::IsFalse(brothersScope->isAncestor(*notRelated));
			Assert::IsFalse(personScope_1->isAncestor(*notRelated));
			Assert::IsFalse(personScope_2->isAncestor(*notRelated));
			Assert::IsFalse(notRelated->isAncestor(*brothersScope));
			Assert::IsFalse(notRelated->isAncestor(*personScope_1));
			Assert::IsFalse(notRelated->isAncestor(*personScope_2));

			// verify isAncestor with longer chain
			Scope& childScope = personScope_2->appendScope("childScope");
			Assert::IsFalse(childScope.isAncestor(*personScope_1));
			Assert::IsTrue(childScope.isAncestor(*personScope_2));
			Assert::IsTrue(childScope.isAncestor(*brothersScope));

			delete brothersScope;
			delete notRelated;
		}

		TEST_METHOD(ScopeSubscopeBrackets)
		{
			Scope* brothersScope = new Scope(2);
			Scope* personScope_1 = new Scope(5);
			Scope* personScope_2 = new Scope(5);

			brothersScope->adopt("older", *personScope_1);
			brothersScope->adopt("younger", *personScope_2);

			personScope_1->append("name_first") = "Justin";
			personScope_1->append("name_last") = "Moser";
			personScope_1->append("age") = 23;
			personScope_1->append("height") = 5.7f;
			personScope_1->append("weight") = 124.0f;

			personScope_2->append("name_first") = "Brandon";
			personScope_2->append("name_last") = "Moser";
			personScope_2->append("age") = 19;
			personScope_2->append("height") = 5.4f;
			personScope_2->append("weight") = 150.0f;

			Assert::IsTrue((*brothersScope)["older"]["name_first"] == "Justin");
			Assert::IsTrue((*brothersScope)["younger"]["name_first"] == "Brandon");

			delete brothersScope;
		}

	private:

		void TestBrackets()
		{
			Datum datum, otherDatum;
			otherDatum.pushBack(10);

			Scope scope;
			Assert::IsTrue(scope.size() == 0);

			// brackets string throws exception when given empty string
			auto scopeEmptyBrackets = [&scope]{ scope[""]; };
			Assert::ExpectException<exception>(scopeEmptyBrackets);

			// brackets string inserts a new field when the Scope doesn't have the key
			//		default datum created, size increased
			Datum& firstAppend = scope["value"];
			Assert::IsTrue(firstAppend == datum);
			Assert::IsTrue(scope.size() == 1);

			// brackets string returns the Datum already there if the key is in the Scope
			//		previous datum returned, nothing is inserted (but we can do things to the returned Datum)
			scope["value"].pushBack(10);
			Assert::IsTrue(scope["value"] == otherDatum);
			Assert::IsTrue(scope.size() == 1);

			// brackets int throws index-out-of-bounds exceptions
			Scope scope_2;
			Assert::IsTrue(scope_2.size() == 0);
			auto scopeBracketsOutOfBounds = [&scope_2]{ scope_2[1] = 10; };
			Assert::ExpectException<exception>(scopeBracketsOutOfBounds);

			// brackets int returns Datums in their insert order
			scope_2["hello"] = 0;
			scope_2["world"] = 1;
			scope_2["error"] = 2;

			Assert::IsTrue(scope_2.size() == 3);
			Assert::IsTrue(scope_2[0] == 0);
			Assert::IsTrue(scope_2[1] == 1);
			Assert::IsTrue(scope_2[2] == 2);
		}

		void TestAtMethod()
		{
			// at throws index-out-of-bounds exceptions
			Scope* scope = new Scope();
			Assert::IsTrue(scope->size() == 0);
			auto scopeBracketsOutOfBounds = [&scope]{ scope->at(1) = 10; };
			Assert::ExpectException<exception>(scopeBracketsOutOfBounds);

			// at returns Datums in their insert order
			scope->append("hello") = 0;
			scope->append("world") = 1;
			scope->append("error") = 2;

			Assert::IsTrue(scope->size() == 3);
			Assert::IsTrue(scope->at(0) == 0);
			Assert::IsTrue(scope->at(1) == 1);
			Assert::IsTrue(scope->at(2) == 2);

			delete scope;
		}

		static _CrtMemState sStartMemState;

	};

	_CrtMemState ScopeTest::sStartMemState;
}

