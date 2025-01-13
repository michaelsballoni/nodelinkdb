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
		}
	};
}
