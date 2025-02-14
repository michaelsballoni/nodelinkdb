#include "pch.h"
#include "CppUnitTest.h"
#include "nldb.h"
#include "testutils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace nldb
{
	TEST_CLASS(nodetests)
	{
	public:
		TEST_METHOD(TestNodes)
		{
			// set up shop from scratch
			db db("tests.nodes.db");
			setup_nldb(db);

			// check out the null node
			node null_node = nodes::get(db, 0);
			Assert::AreEqual(int64_t(0), null_node.id);
			Assert::AreEqual(int64_t(0), null_node.parentId);
			Assert::AreEqual(int64_t(0), null_node.nameStringId);
			Assert::AreEqual(int64_t(0), null_node.typeStringId);
			Assert::IsTrue(nodes::get_children(db, 0).empty());
			Assert::IsTrue(nodes::get_all_children(db, 0).empty());
			Assert::IsTrue(nodes::get_parents(db, 0).empty());

			try
			{
				node node_bad = nodes::create(db, 0, strings::get_id(db, L"foo / bar")); // "/" not allowed in paths
				Assert::Fail();
			}
			catch (const nldberr&) {}

			// create our first node
			node node1 = nodes::create(db, 0, strings::get_id(db, L"foo"), strings::get_id(db, L"bar"));
			Assert::AreNotEqual(int64_t(0), node1.id);
			Assert::AreEqual(int64_t(0), node1.parentId);
			Assert::AreEqual(strings::get_id(db, L"foo"), node1.nameStringId);
			Assert::AreEqual(strings::get_id(db, L"bar"), node1.typeStringId);
			Assert::IsTrue(nodes::get_children(db, node1.id).empty());
			Assert::IsTrue(nodes::get_all_children(db, node1.id).empty());
			Assert::IsTrue(nodes::get_parents(db, node1.id).empty());
			Assert::AreEqual(size_t(1), nodes::get_children(db, 0).size());
			Assert::AreEqual(node1.id, nodes::get_children(db, 0)[0].id);
			Assert::AreEqual(size_t(1), nodes::get_all_children(db, 0).size());
			Assert::AreEqual(node1.id, nodes::get_all_children(db, 0)[0].id);

			// create our second node
			node node2 = nodes::create(db, 0, strings::get_id(db, L"blet"), strings::get_id(db, L"monkey"));
			Assert::AreNotEqual(int64_t(0), node2.id);
			Assert::AreNotEqual(node1.id, node2.id);
			Assert::AreEqual(int64_t(0), node2.parentId);
			Assert::AreEqual(strings::get_id(db, L"blet"), node2.nameStringId);
			Assert::AreEqual(strings::get_id(db, L"monkey"), node2.typeStringId);
			Assert::IsTrue(nodes::get_children(db, node2.id).empty());
			Assert::IsTrue(nodes::get_all_children(db, node2.id).empty());
			Assert::IsTrue(nodes::get_parents(db, node2.id).empty());
			Assert::AreEqual(size_t(2), nodes::get_children(db, 0).size());
			Assert::IsTrue(hasNode(nodes::get_children(db, 0), node1.id));
			Assert::IsTrue(hasNode(nodes::get_children(db, 0), node2.id));
			Assert::AreEqual(size_t(2), nodes::get_all_children(db, 0).size());
			Assert::IsTrue(hasNode(nodes::get_all_children(db, 0), node1.id));
			Assert::IsTrue(hasNode(nodes::get_all_children(db, 0), node2.id));

			// move the second into the first
			nodes::move(db, node2.id, node1.id);
			Assert::AreEqual(size_t(1), nodes::get_children(db, node1.id).size());
			Assert::AreEqual(size_t(1), nodes::get_all_children(db, node1.id).size());
			Assert::AreEqual(node2.id, nodes::get_children(db, node1.id)[0].id);
			Assert::AreEqual(node2.id, nodes::get_all_children(db, node1.id)[0].id);
			Assert::IsTrue(nodes::get_children(db, node2.id).empty());
			Assert::IsTrue(nodes::get_all_children(db, node2.id).empty());

			// get the second again
			node node2b = nodes::get(db, node2.id);
			Assert::AreEqual(node2.id, node2b.id);
			Assert::AreEqual(node2.nameStringId, node2b.nameStringId);
			Assert::AreEqual(node2.typeStringId, node2b.typeStringId);
			Assert::AreEqual(node1.id, node2b.parentId);

			// work with paths
			Assert::IsTrue(nodes::get_path_str(db, node()).empty());

			std::wstring node1_path = nodes::get_path_str(db, node1);
			Assert::AreEqual(std::wstring(L"/foo"), node1_path);

			std::wstring node2_path = nodes::get_path_str(db, node2b);
			Assert::AreEqual(std::wstring(L"/foo/blet"), node2_path);

			Assert::IsTrue(!nodes::get_path_nodes(db, L"").has_value());

			auto node1_path_nodes = nodes::get_path_nodes(db, node1_path);
			Assert::AreEqual(size_t(1), node1_path_nodes.value().size());
			Assert::IsTrue(node1 == node1_path_nodes.value()[0]);

			auto node2_path_nodes = nodes::get_path_nodes(db, node2_path);
			Assert::AreEqual(size_t(2), node2_path_nodes.value().size());
			Assert::IsTrue(node1 == node2_path_nodes.value()[0]);
			Assert::IsTrue(node2b == node2_path_nodes.value()[1]);

			// set a payload
			nodes::set_payload(db, node2.id, L"foobletbarmonkey");
			Assert::AreEqual(std::wstring(L"foobletbarmonkey"), nodes::get_payload(db, node2.id));

			nodes::set_payload(db, node2.id, L"");
			Assert::AreEqual(std::wstring(L""), nodes::get_payload(db, node2.id));

			// rename the second node
			try
			{
				nodes::rename(db, node2b.id, strings::get_id(db, L"/blet monkey"));
				Assert::Fail();
			}
			catch (const nldberr&) {}

			int64_t new_name_string_id = strings::get_id(db, L"new name hot hot hot");
			nodes::rename(db, node2b.id, new_name_string_id);
			Assert::AreEqual(node2b.id, nodes::get_node_in_parent(db, node2b.parentId, new_name_string_id).value().id);

			// delete our root
			nodes::remove(db, node1.id);
			try
			{
				nodes::get(db, node1.id);
				Assert::Fail();
			}
			catch (const nldberr&) {}
			try
			{
				nodes::get(db, node2.id);
				Assert::Fail();
			}
			catch (const nldberr&) {}
		}

		TEST_METHOD(TestCopy)
		{
			// set up shop from scratch
			db db("tests.node-copy.db");
			setup_nldb(db);

			node node1 = nodes::create(db, 0, strings::get_id(db, L"foo"), strings::get_id(db, L"bar"));
			node node2 = nodes::create(db, node1.id, strings::get_id(db, L"blet"), strings::get_id(db, L"monkey"));
			node node3 = nodes::create(db, node1.id, strings::get_id(db, L"yet"), strings::get_id(db, L"another"));
			node node4 = nodes::create(db, node3.id, strings::get_id(db, L"deeper"), strings::get_id(db, L"still"));
			
			node dest_node = nodes::create(db, 0, strings::get_id(db, L"dest"), strings::get_id(db, L"ination"));
			
			nodes::copy(db, node1.id, dest_node.id);

			Assert::AreEqual(node1.nameStringId, nodes::get_path_nodes(db, L"/dest/foo/").value().back().nameStringId);
			Assert::AreEqual(node2.nameStringId, nodes::get_path_nodes(db, L"/dest/foo/blet/").value().back().nameStringId);
			Assert::AreEqual(node3.nameStringId, nodes::get_path_nodes(db, L"/dest/foo/yet/").value().back().nameStringId);
			Assert::AreEqual(node4.nameStringId, nodes::get_path_nodes(db, L"/dest/foo/yet/deeper/").value().back().nameStringId);
		}
	};
}
