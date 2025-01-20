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

			int64_t node_item_type_id = strings::get_id(db, L"node");
			int64_t link_item_type_id = strings::get_id(db, L"link");

			auto node0 = nodes::get_node(db, 0).value();
			int64_t item_id0 = node0.m_id;

			auto node1 = nodes::create(db, item_id0, strings::get_id(db, L"show"), 0);
			int64_t item_id1 = node1.m_id;

			auto link1 = links::create(db, item_id0, item_id1, 0);
			int64_t item_id2 = link1.m_id;

			//
			// NODES
			//
			{
				auto results0 = search::find_nodes(db, search_query());
				Assert::IsTrue(results0.empty());

				props::set(db, node_item_type_id, item_id0, strings::get_id(db, L"foo"), strings::get_id(db, L"bar"));
				auto results1 = search::find_nodes(db, search_query({ search_criteria(strings::get_id(db, L"foo"), L"not it") }));
				Assert::IsTrue(results1.empty());

				auto results2 = search::find_nodes(db, search_query({ search_criteria(strings::get_id(db, L"foo"), L"bar") }));
				Assert::AreEqual(1U, results2.size());
				Assert::AreEqual(item_id0, results2[0].m_id);

				props::set(db, node_item_type_id, item_id0, strings::get_id(db, L"blet"), strings::get_id(db, L"monkey"));
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

				auto results5 =
					search::find_nodes
					(
						db,
						search_query
						(
							{
								search_criteria(strings::get_id(db, L"foo"), L"bar"),
								search_criteria(strings::get_id(db, L"blet"), L"monk%", true)
							}
						)
					);
				Assert::AreEqual(1U, results5.size());
				Assert::AreEqual(item_id0, results5[0].m_id);

				props::set(db, node_item_type_id, item_id1, strings::get_id(db, L"flint"), strings::get_id(db, L"stone"));
				auto results6 = search::find_nodes(db, search_query({ search_criteria(strings::get_id(db, L"flint"), L"not it") }));
				Assert::IsTrue(results6.empty());

				auto results7 = search::find_nodes(db, search_query({ search_criteria(strings::get_id(db, L"flint"), L"stone") }));
				Assert::AreEqual(1U, results7.size());
				Assert::AreEqual(item_id1, results7[0].m_id);
			}

			//
			// LINKS
			//
			{
				props::set(db, link_item_type_id, item_id2, strings::get_id(db, L"link"), strings::get_id(db, L"sink"));
				auto results8 = search::find_links(db, search_query({ search_criteria(strings::get_id(db, L"link"), L"not it") }));
				Assert::IsTrue(results8.empty());

				auto results9 = search::find_links(db, search_query({ search_criteria(strings::get_id(db, L"link"), L"sink") }));
				Assert::AreEqual(1U, results9.size());
				Assert::AreEqual(item_id2, results9[0].m_id);
			}

			//
			// ORDER BY / LIMIT
			//
			{
				props::set(db, node_item_type_id, item_id0, strings::get_id(db, L"some"), strings::get_id(db, L"one"));
				props::set(db, node_item_type_id, item_id1, strings::get_id(db, L"some"), strings::get_id(db, L"two"));

				auto results10 =
					search::find_nodes
					(
						db,
						search_query({ search_criteria(strings::get_id(db, L"some"), L"%", true) }, L"some", true)
					);
				Assert::AreEqual(2U, results10.size());
				Assert::AreEqual(item_id0, results10[0].m_id);
				Assert::AreEqual(item_id1, results10[1].m_id);

				auto results11 =
					search::find_nodes
					(
						db,
						search_query({ search_criteria(strings::get_id(db, L"some"), L"%", true) }, L"some", false)
					);
				Assert::AreEqual(2U, results11.size());
				Assert::AreEqual(item_id1, results11[0].m_id);
				Assert::AreEqual(item_id0, results11[1].m_id);

				auto results12 =
					search::find_nodes
					(
						db,
						search_query({ search_criteria(strings::get_id(db, L"some"), L"%", true) }, L"some", false, 1)
					);
				Assert::AreEqual(1U, results12.size());
				Assert::AreEqual(item_id1, results12[0].m_id);
			}
		}
	};
}
