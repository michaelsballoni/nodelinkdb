#include "pch.h"
#include "CppUnitTest.h"
#include "nldb.h"
#include "testutils.h"

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

			auto node0 = nodes::get(db, 0);
			int64_t item_id0 = node0.id;

			auto node1 = nodes::create(db, item_id0, strings::get_id(db, L"show"), 0);
			int64_t item_id1 = node1.id;

			auto link1 = links::create(db, item_id0, item_id1);
			int64_t item_id2 = link1.id;

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
				Assert::AreEqual(size_t(1), results2.size());
				Assert::AreEqual(item_id0, results2[0].id);

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
				Assert::AreEqual(size_t(1), results4.size());
				Assert::AreEqual(item_id0, results4[0].id);

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
				Assert::AreEqual(size_t(1), results5.size());
				Assert::AreEqual(item_id0, results5[0].id);

				props::set(db, node_item_type_id, item_id1, strings::get_id(db, L"flint"), strings::get_id(db, L"stone"));
				auto results6 = search::find_nodes(db, search_query({ search_criteria(strings::get_id(db, L"flint"), L"not it") }));
				Assert::IsTrue(results6.empty());

				auto results7 = search::find_nodes(db, search_query({ search_criteria(strings::get_id(db, L"flint"), L"stone") }));
				Assert::AreEqual(size_t(1), results7.size());
				Assert::AreEqual(item_id1, results7[0].id);
			}

			//
			// LINKS
			//
			{
				props::set(db, link_item_type_id, item_id2, strings::get_id(db, L"link"), strings::get_id(db, L"sink"));
				auto results8 = search::find_links(db, search_query({ search_criteria(strings::get_id(db, L"link"), L"not it") }));
				Assert::IsTrue(results8.empty());

				auto results9 = search::find_links(db, search_query({ search_criteria(strings::get_id(db, L"link"), L"sink") }));
				Assert::AreEqual(size_t(1), results9.size());
				Assert::AreEqual(item_id2, results9[0].id);
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
				Assert::AreEqual(size_t(2), results10.size());
				Assert::AreEqual(item_id0, results10[0].id);
				Assert::AreEqual(item_id1, results10[1].id);

				auto results11 =
					search::find_nodes
					(
						db,
						search_query({ search_criteria(strings::get_id(db, L"some"), L"%", true) }, L"some", false)
					);
				Assert::AreEqual(size_t(2), results11.size());
				Assert::AreEqual(item_id1, results11[0].id);
				Assert::AreEqual(item_id0, results11[1].id);

				auto results12 =
					search::find_nodes
					(
						db,
						search_query({ search_criteria(strings::get_id(db, L"some"), L"%", true) }, L"some", false, 1)
					);
				Assert::AreEqual(size_t(1), results12.size());
				Assert::AreEqual(item_id1, results12[0].id);
			}

			//
			// SEARCH BY PAYLOAD
			//
			{
				nodes::set_payload(db, item_id1, L"some payload");
				search_query search1({ search_criteria(strings::get_id(db, L"payload"), L"not that payload") });
				auto no_payload_results = search::find_nodes(db, search1);
				Assert::IsTrue(no_payload_results.empty());

				search_query search2({ search_criteria(strings::get_id(db, L"payload"), L"some payload") });
				auto with_payload_results = search::find_nodes(db, search2);
				Assert::AreEqual(size_t(1), with_payload_results.size());
				Assert::IsTrue(!with_payload_results[0].payload.has_value());

				search_query search3({ search_criteria(strings::get_id(db, L"payload"), L"some payload") });
				search3.m_includePayload = true;
				auto with_payload_results2 = search::find_nodes(db, search3);
				Assert::AreEqual(size_t(1), with_payload_results2.size());
				Assert::AreEqual(std::wstring(L"some payload"), with_payload_results2[0].payload.value());
			}

			//
			// SEARCH BY NAME
			//
			{
				search_query search1({ search_criteria(strings::get_id(db, L"name"), L"slow poke") });
				auto no_results = search::find_nodes(db, search1);
				Assert::IsTrue(no_results.empty());

				search_query search2({ search_criteria(strings::get_id(db, L"name"), L"show") });
				auto with_results = search::find_nodes(db, search2);
				Assert::AreEqual(size_t(1), with_results.size());
				Assert::IsTrue(with_results[0] == node1);
			}

			//
			// SEARCH BY PATH
			//
			auto node2 = nodes::create(db, node1.id, strings::get_id(db, L"leafy"), 0);
			{
				search_query search1({ search_criteria(strings::get_id(db, L"path"), L"/fred/nothing/ha ha") });
				auto no_results = search::find_nodes(db, search1);
				Assert::IsTrue(no_results.empty());

				search_query search2({ search_criteria(strings::get_id(db, L"path"), L"/show") });
				auto with_results = search::find_nodes(db, search2);
				Assert::AreEqual(size_t(1), with_results.size());
				Assert::IsTrue(with_results[0] == node2);
			}

			//
			// SEARCH BY PARENT
			//
			{
				auto node3 = nodes::create(db, node1.id, strings::get_id(db, L"leaf"), 0);
				auto node4 = nodes::create(db, node3.id, strings::get_id(db, L"leafier"), 0);

				search_query search2({ search_criteria(strings::get_id(db, L"path"), L"/show") });
				auto with_results = search::find_nodes(db, search2);
				Assert::AreEqual(size_t(3), with_results.size());
				Assert::IsTrue(hasNode(with_results, node2.id));
				Assert::IsTrue(hasNode(with_results, node3.id));
				Assert::IsTrue(hasNode(with_results, node4.id));

				search_query search1({ search_criteria(strings::get_id(db, L"parent"), L"/fred/nothing/ha ha") });
				auto no_results = search::find_nodes(db, search1);
				Assert::IsTrue(no_results.empty());

				search_query search3({ search_criteria(strings::get_id(db, L"parent"), L"/show") });
				auto with_results2 = search::find_nodes(db, search3);
				Assert::AreEqual(size_t(2), with_results2.size());
				Assert::IsTrue(hasNode(with_results2, node2.id));
				Assert::IsTrue(hasNode(with_results2, node3.id));
			}

			//
			// SEARCH BY TYPE
			//
			{
				auto node3 = nodes::create(db, node1.id, strings::get_id(db, L"leaf2"), strings::get_id(db, L"type1"));
				auto node4 = nodes::create(db, node3.id, strings::get_id(db, L"leafier2"), strings::get_id(db, L"type2"));

				search_query search2({ search_criteria(strings::get_id(db, L"type"), L"type1") });
				auto with_results = search::find_nodes(db, search2);
				Assert::AreEqual(size_t(1), with_results.size());
				Assert::IsTrue(hasNode(with_results, node3.id));

				search_query search3({ search_criteria(strings::get_id(db, L"type"), L"type2") });
				auto with_results2 = search::find_nodes(db, search3);
				Assert::AreEqual(size_t(1), with_results2.size());
				Assert::IsTrue(hasNode(with_results2, node4.id));

				search_query search4({ search_criteria(strings::get_id(db, L"type"), L"fred") });
				auto with_results3 = search::find_nodes(db, search4);
				Assert::AreEqual(size_t(0), with_results3.size());
			}
		}
	};
}
