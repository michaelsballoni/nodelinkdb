#include "pch.h"
#include "CppUnitTest.h"
#include "nldb.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace nldb
{
	TEST_CLASS(linktests)
	{
	public:
		TEST_METHOD(TestLinks)
		{
			db db("tests.nodes.db");
			setup_nldb(db);

			node from_node = nodes::create(db, 0, strings::get_id(db, L"from"), 0);
			node to_node = nodes::create(db, 0, strings::get_id(db, L"to"), 0);

			link link = links::create(db, from_node.m_id, to_node.m_id, 0);

			auto from_links = links::get_out_links(db, from_node.m_id);
			Assert::AreEqual(1U, from_links.size());
			Assert::AreEqual(link.m_id, from_links[0].m_id);

			auto to_links = links::get_in_links(db, to_node.m_id);
			Assert::AreEqual(1U, to_links.size());
			Assert::AreEqual(link.m_id, to_links[0].m_id);
		}
	};
}
