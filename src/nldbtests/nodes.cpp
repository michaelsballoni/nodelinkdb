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
			node null_node = nodes::get(db, 0).value();
			Assert::AreEqual(int64_t(0), null_node.m_id);
			Assert::AreEqual(int64_t(0), null_node.m_parentId);
			Assert::AreEqual(int64_t(0), null_node.m_nameStringId);
			Assert::AreEqual(int64_t(0), null_node.m_typeStringId);
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
			Assert::AreNotEqual(int64_t(0), node1.m_id);
			Assert::AreEqual(int64_t(0), node1.m_parentId);
			Assert::AreEqual(strings::get_id(db, L"foo"), node1.m_nameStringId);
			Assert::AreEqual(strings::get_id(db, L"bar"), node1.m_typeStringId);
			Assert::IsTrue(nodes::get_children(db, node1.m_id).empty());
			Assert::IsTrue(nodes::get_all_children(db, node1.m_id).empty());
			Assert::IsTrue(nodes::get_parents(db, node1.m_id).empty());
			Assert::AreEqual(1U, nodes::get_children(db, 0).size());
			Assert::AreEqual(node1.m_id, nodes::get_children(db, 0)[0].m_id);
			Assert::AreEqual(1U, nodes::get_all_children(db, 0).size());
			Assert::AreEqual(node1.m_id, nodes::get_all_children(db, 0)[0].m_id);

			// create our second node
			node node2 = nodes::create(db, 0, strings::get_id(db, L"blet"), strings::get_id(db, L"monkey"));
			Assert::AreNotEqual(int64_t(0), node2.m_id);
			Assert::AreNotEqual(node1.m_id, node2.m_id);
			Assert::AreEqual(int64_t(0), node2.m_parentId);
			Assert::AreEqual(strings::get_id(db, L"blet"), node2.m_nameStringId);
			Assert::AreEqual(strings::get_id(db, L"monkey"), node2.m_typeStringId);
			Assert::IsTrue(nodes::get_children(db, node2.m_id).empty());
			Assert::IsTrue(nodes::get_all_children(db, node2.m_id).empty());
			Assert::IsTrue(nodes::get_parents(db, node2.m_id).empty());
			Assert::AreEqual(2U, nodes::get_children(db, 0).size());
			Assert::IsTrue(has(nodes::get_children(db, 0), node1.m_id));
			Assert::IsTrue(has(nodes::get_children(db, 0), node2.m_id));
			Assert::AreEqual(2U, nodes::get_all_children(db, 0).size());
			Assert::IsTrue(has(nodes::get_all_children(db, 0), node1.m_id));
			Assert::IsTrue(has(nodes::get_all_children(db, 0), node2.m_id));

			// move the second into the first
			nodes::move(db, node2.m_id, node1.m_id);
			Assert::AreEqual(1U, nodes::get_children(db, node1.m_id).size());
			Assert::AreEqual(1U, nodes::get_all_children(db, node1.m_id).size());
			Assert::AreEqual(node2.m_id, nodes::get_children(db, node1.m_id)[0].m_id);
			Assert::AreEqual(node2.m_id, nodes::get_all_children(db, node1.m_id)[0].m_id);
			Assert::IsTrue(nodes::get_children(db, node2.m_id).empty());
			Assert::IsTrue(nodes::get_all_children(db, node2.m_id).empty());

			// get the second again
			node node2b = nodes::get(db, node2.m_id).value();
			Assert::AreEqual(node2.m_id, node2b.m_id);
			Assert::AreEqual(node2.m_nameStringId, node2b.m_nameStringId);
			Assert::AreEqual(node2.m_typeStringId, node2b.m_typeStringId);
			Assert::AreEqual(node1.m_id, node2b.m_parentId);

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
			nodes::set_payload(db, node2.m_id, L"foobletbarmonkey");
			Assert::AreEqual(std::wstring(L"foobletbarmonkey"), nodes::get(db, node2.m_id).value().m_payload.value());

			nodes::set_payload(db, node2.m_id, L"");
			Assert::AreEqual(std::wstring(L""), nodes::get(db, node2.m_id).value().m_payload.value());

			// rename the second node
			int64_t new_name_string_id = strings::get_id(db, L"new name hot hot hot");
			nodes::rename(db, node2b.m_id, new_name_string_id);
			Assert::AreEqual(node2b.m_id, nodes::get_node_in_parent(db, node2b.m_parentId, new_name_string_id).value().m_id);

			// delete our root
			nodes::remove(db, node1.m_id);
			Assert::IsTrue(!nodes::get(db, node2.m_id).has_value());
			Assert::IsTrue(!nodes::get(db, node1.m_id).has_value());
		}
	};
}
