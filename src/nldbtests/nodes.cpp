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

			node null_node = nodes::get_parent_node(db, 0);
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
		}
	};
}
