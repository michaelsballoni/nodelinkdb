#include "pch.h"
#include "CppUnitTest.h"
#include "db.h"
#include "nodes.h"
#include "props.h"
#include "strings.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace nldb
{
	TEST_CLASS(propstests)
	{
	public:
		TEST_METHOD(TestProps)
		{
			db db("tests.props.db");
			strings::setup(db);
			nodes::setup(db);
			props::setup(db);

			auto node = nodes::get_node(db, 0).value();

			std::wstring prop_str;

			prop_str = props::summarize(db, props::get(db, node.m_id));
			Assert::IsTrue(prop_str.empty());

			props::set(db, node.m_id, strings::get_id(db, L"foo"), strings::get_id(db, L"bar"));
			prop_str = props::summarize(db, props::get(db, node.m_id));
			Assert::IsTrue(prop_str == L"foo bar");

			props::set(db, node.m_id, strings::get_id(db, L"blet"), strings::get_id(db, L"monkey"));
			prop_str = props::summarize(db, props::get(db, node.m_id));
			Assert::IsTrue(prop_str == L"blet monkey\nfoo bar");

			props::set(db, node.m_id, strings::get_id(db, L"foo"), -1);
			prop_str = props::summarize(db, props::get(db, node.m_id));
			Assert::IsTrue(prop_str == L"blet monkey");

			props::set(db, node.m_id, -1, -1);
			prop_str = props::summarize(db, props::get(db, node.m_id));
			Assert::IsTrue(prop_str.empty());
		}
	};
}
