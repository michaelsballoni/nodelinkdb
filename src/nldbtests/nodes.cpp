#include "pch.h"
#include "CppUnitTest.h"
#include "nldb.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

bool has(const std::vector<nldb::node>& vec, int64_t id)
{
	for (size_t i = 0; i < vec.size(); ++i)
		if (vec[i].m_id == id)
			return true;
	return false;
}

namespace nldb
{
	TEST_CLASS(nodetests)
	{
	public:
		TEST_METHOD(TestNodes)
		{
			db db("tests.nodes.db");
			setup_nldb(db);

			auto null_node_opt = nodes::get_parent(db, 0);
			Assert::IsTrue(!null_node_opt.has_value());
			node null_node = nodes::get_node(db, 0).value();
			Assert::AreEqual(int64_t(0), null_node.m_id);
			Assert::AreEqual(int64_t(0), null_node.m_parentId);
			Assert::AreEqual(int64_t(0), null_node.m_nameStringId);
			Assert::AreEqual(int64_t(0), null_node.m_typeStringId);
			Assert::IsTrue(nodes::get_children(db, 0).empty());
			Assert::IsTrue(nodes::get_parents(db, 0).empty());

			node node1 = nodes::create(db, 0, strings::get_id(db, L"foo"), strings::get_id(db, L"bar"));
			Assert::AreNotEqual(int64_t(0), node1.m_id);
			Assert::AreEqual(int64_t(0), node1.m_parentId);
			Assert::AreEqual(strings::get_id(db, L"foo"), node1.m_nameStringId);
			Assert::AreEqual(strings::get_id(db, L"bar"), node1.m_typeStringId);
			Assert::IsTrue(nodes::get_children(db, node1.m_id).empty());
			Assert::IsTrue(nodes::get_parents(db, node1.m_id).empty());
			Assert::AreEqual(1U, nodes::get_children(db, 0).size());
			Assert::AreEqual(node1.m_id, nodes::get_children(db, 0)[0].m_id);

			node node2 = nodes::create(db, 0, strings::get_id(db, L"blet"), strings::get_id(db, L"monkey"));
			Assert::AreNotEqual(int64_t(0), node2.m_id);
			Assert::AreNotEqual(node1.m_id, node2.m_id);
			Assert::AreEqual(int64_t(0), node2.m_parentId);
			Assert::AreEqual(strings::get_id(db, L"blet"), node2.m_nameStringId);
			Assert::AreEqual(strings::get_id(db, L"monkey"), node2.m_typeStringId);
			Assert::IsTrue(nodes::get_children(db, node2.m_id).empty());
			Assert::IsTrue(nodes::get_parents(db, node2.m_id).empty());
			Assert::AreEqual(2U, nodes::get_children(db, 0).size());
			Assert::IsTrue(has(nodes::get_children(db, 0), node1.m_id));
			Assert::IsTrue(has(nodes::get_children(db, 0), node2.m_id));

			nodes::move(db, node2.m_id, node1.m_id);
			Assert::AreEqual(1U, nodes::get_children(db, node1.m_id).size());
			Assert::AreEqual(node2.m_id, nodes::get_children(db, node1.m_id)[0].m_id);
			Assert::IsTrue(nodes::get_children(db, node2.m_id).empty());

			auto node2_opt = nodes::get_node(db, node2.m_id);
			if (!node2_opt.has_value())
				Assert::Fail();
			node node2b = node2_opt.value();
			Assert::AreEqual(node2.m_id, node2b.m_id);
			Assert::AreEqual(node2.m_nameStringId, node2b.m_nameStringId);
			Assert::AreEqual(node2.m_typeStringId, node2b.m_typeStringId);
			Assert::AreEqual(node1.m_id, node2b.m_parentId);

			Assert::IsTrue(nodes::get_path_str(db, node()).empty());

			std::wstring node1_path = nodes::get_path_str(db, node1);
			Assert::AreEqual(std::wstring(L"/foo"), node1_path);

			std::wstring node2_path = nodes::get_path_str(db, node2b);
			Assert::AreEqual(std::wstring(L"/foo/blet"), node2_path);

			Assert::IsTrue(nodes::get_path_nodes(db, L"").empty());

			auto node1_path_nodes = nodes::get_path_nodes(db, node1_path);
			Assert::AreEqual(size_t(1), node1_path_nodes.size());
			Assert::IsTrue(node1 == node1_path_nodes[0]);

			auto node2_path_nodes = nodes::get_path_nodes(db, node2_path);
			Assert::AreEqual(size_t(2), node2_path_nodes.size());
			Assert::IsTrue(node1 == node2_path_nodes[0]);
			Assert::IsTrue(node2b == node2_path_nodes[1]);

			nodes::remove(db, node1.m_id);
			Assert::IsTrue(!nodes::get_node(db, node2.m_id).has_value());
			Assert::IsTrue(!nodes::get_node(db, node1.m_id).has_value());
		}
	};
}
