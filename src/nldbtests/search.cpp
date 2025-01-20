#include "pch.h"
#include "CppUnitTest.h"
#include "nldb.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace nldb
{
	TEST_CLASS(searchtests)
	{
	public:
		TEST_METHOD(TestSearch)
		{
			db db("tests.search.db");
			setup_nldb(db);

			auto results0 = search::find_nodes(db, search_query());
			Assert::IsTrue(results0.empty());

			int64_t item_type_id = strings::get_id(db, L"node");
			
			auto node0 = nodes::get_node(db, 0).value();
			int64_t item_id0 = node0.m_id;

			props::set(db, item_type_id, item_id0, strings::get_id(db, L"foo"), strings::get_id(db, L"bar"));
			auto results1 = search::find_nodes(db, search_query({ search_criteria(strings::get_id(db, L"foo"), L"not it") }));
			Assert::IsTrue(results1.empty());

			auto results2 = search::find_nodes(db, search_query({ search_criteria(strings::get_id(db, L"foo"), L"bar") }));
			Assert::AreEqual(1U, results2.size());
			Assert::AreEqual(item_id0, results2[0].m_id);

			props::set(db, item_type_id, item_id0, strings::get_id(db, L"blet"), strings::get_id(db, L"monkey"));
			auto results3 = 
				search::find_nodes
				(
					db, 
					search_query
					(
						{ 
							search_criteria(strings::get_id(db, L"foo"), L"bar"), 
							search_criteria(strings::get_id(db, L"blet"), L"not it") 
						}
					)
				);
			Assert::IsTrue(results3.empty());

			auto results4 =
				search::find_nodes
				(
					db,
					search_query
					(
						{
							search_criteria(strings::get_id(db, L"foo"), L"bar"),
							search_criteria(strings::get_id(db, L"blet"), L"monkey")
						}
					)
				);
			Assert::AreEqual(1U, results4.size());
			Assert::AreEqual(item_id0, results4[0].m_id);
		}
	};
}
