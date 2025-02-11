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

			link link = links::create(db, from_node.id, to_node.id, 0, L"pay-load");

			auto from_links = links::get_out_links(db, from_node.id);
			Assert::AreEqual(size_t(1), from_links.size());
			Assert::AreEqual(link.id, from_links[0].id);

			auto to_links = links::get_in_links(db, to_node.id);
			Assert::AreEqual(size_t(1), to_links.size());
			Assert::AreEqual(link.id, to_links[0].id);

			auto from_link = links::get_link(db, from_links[0].id);
			Assert::AreEqual(std::wstring(L"pay-load"), from_link.value().payload.value());

			Assert::IsTrue(links::remove(db, from_node.id, to_node.id, 0));
			Assert::IsTrue(!links::remove(db, from_node.id, to_node.id, 0));

			auto from_links2 = links::get_out_links(db, from_node.id);
			Assert::IsTrue(from_links2.empty());

			auto to_links2 = links::get_in_links(db, to_node.id);
			Assert::IsTrue(to_links2.empty());
		}
	};
}
