#include "pch.h"
#include "CppUnitTest.h"
#include "nldb.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace nldb
{
	TEST_CLASS(propstests)
	{
	public:
		TEST_METHOD(TestProps)
		{
			db db("tests.props.db");
			setup_nldb(db);

			int64_t item_type_id = strings::get_id(db, L"node");
			auto node = nodes::get(db, 0);
			int64_t item_id = node.m_id;

			std::wstring prop_str;

			prop_str = props::summarize(db, props::get(db, item_type_id, item_id));
			Assert::IsTrue(prop_str.empty());

			props::set(db, item_type_id, node.m_id, strings::get_id(db, L"foo"), strings::get_id(db, L"bar"));
			prop_str = props::summarize(db, props::get(db, item_type_id, item_id));
			Assert::IsTrue(prop_str == L"foo bar");

			props::set(db, item_type_id, node.m_id, strings::get_id(db, L"blet"), strings::get_id(db, L"monkey"));
			prop_str = props::summarize(db, props::get(db, item_type_id, item_id));
			Assert::IsTrue(prop_str == L"blet monkey\nfoo bar");

			props::set(db, item_type_id, node.m_id, strings::get_id(db, L"foo"), -1);
			prop_str = props::summarize(db, props::get(db, item_type_id, item_id));
			Assert::IsTrue(prop_str == L"blet monkey");

			props::set(db, item_type_id, node.m_id, -1, -1);
			prop_str = props::summarize(db, props::get(db, item_type_id, item_id));
			Assert::IsTrue(prop_str.empty());
		}
	};
}
