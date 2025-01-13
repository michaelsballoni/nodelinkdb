#include "pch.h"
#include "CppUnitTest.h"
#include "db.h"
#include "nodes.h"
#include "strings.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace nldb
{
	TEST_CLASS(nodestests)
	{
	public:
		TEST_METHOD(TestNodes)
		{
			db db("tests.nodes.db");
			strings::setup(db);
			nodes::setup(db);

			auto null_node_opt = nodes::get_parent_node(db, 0);
			if (!null_node_opt.has_value())
				Assert::Fail();
			node null_node = null_node_opt.value();
			Assert::AreEqual(int64_t(0), null_node.m_id);
			Assert::AreEqual(int64_t(0), null_node.m_parent_id);
			Assert::AreEqual(int64_t(0), null_node.m_name_string_id);
			Assert::AreEqual(int64_t(0), null_node.m_type_string_id);

			node node1 = nodes::create(db, 0, strings::get_id(db, L"foo"), strings::get_id(db, L"bar"));
			Assert::AreNotEqual(int64_t(0), node1.m_id);
			Assert::AreEqual(int64_t(0), node1.m_parent_id);
			Assert::AreEqual(strings::get_id(db, L"foo"), node1.m_name_string_id);
			Assert::AreEqual(strings::get_id(db, L"bar"), node1.m_type_string_id);

			node node2 = nodes::create(db, 0, strings::get_id(db, L"blet"), strings::get_id(db, L"monkey"));
			Assert::AreNotEqual(int64_t(0), node2.m_id);
			Assert::AreNotEqual(node1.m_id, node2.m_id);
			Assert::AreEqual(int64_t(0), node2.m_parent_id);
			Assert::AreEqual(strings::get_id(db, L"blet"), node2.m_name_string_id);
			Assert::AreEqual(strings::get_id(db, L"monkey"), node2.m_type_string_id);

			nodes::move(db, node2.m_id, node1.m_id);

			auto node2_opt = nodes::get_node(db, node2.m_id);
			if (!node2_opt.has_value())
				Assert::Fail();
			node node2b = node2_opt.value();
			Assert::AreEqual(node2.m_id, node2b.m_id);
			Assert::AreEqual(node2.m_name_string_id, node2b.m_name_string_id);
			Assert::AreEqual(node2.m_type_string_id, node2b.m_type_string_id);
			Assert::AreEqual(node1.m_id, node2b.m_parent_id);

			std::wstring null_node_path = nodes::get_path_str(db, node());
			Assert::AreEqual(std::wstring(L"/"), null_node_path);

			std::wstring node1_path = nodes::get_path_str(db, node1);
			Assert::AreEqual(std::wstring(L"/foo"), node1_path);

			std::wstring node2_path = nodes::get_path_str(db, node2b);
			Assert::AreEqual(std::wstring(L"/foo/blet"), node2_path);

			auto null_node_path_nodes = nodes::get_path_nodes(db, L"");
			Assert::AreEqual(size_t(1), null_node_path_nodes.size());
			Assert::IsTrue(null_node == null_node_path_nodes[0]);

			auto node1_path_nodes = nodes::get_path_nodes(db, node1_path);
			Assert::AreEqual(size_t(2), node1_path_nodes.size());
			Assert::IsTrue(null_node == node1_path_nodes[0]);
			Assert::IsTrue(node1 == node1_path_nodes[1]);

			auto node2_path_nodes = nodes::get_path_nodes(db, node2_path);
			Assert::AreEqual(size_t(3), node2_path_nodes.size());
			Assert::IsTrue(null_node == node2_path_nodes[0]);
			Assert::IsTrue(node1 == node2_path_nodes[1]);
			Assert::IsTrue(node2b == node2_path_nodes[2]);
		}
	};
}
