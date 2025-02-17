#include "pch.h"
#include "CppUnitTest.h"
#include "nldb.h"
#include "testutils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace nldb
{
	TEST_CLASS(cloudtests)
	{
	public:
		TEST_METHOD(TestClouds)
		{
			db db("tests.cloud.db");
			setup_nldb(db);

			auto node1 = nodes::create(db, 0, strings::get_id(db, L"node1"), 0);
			auto node2 = nodes::create(db, 0, strings::get_id(db, L"node2"), 0);
			auto node3 = nodes::create(db, 0, strings::get_id(db, L"node3"), 0);
			auto node4 = nodes::create(db, 0, strings::get_id(db, L"node4"), 0);

			auto link1 = links::create(db, node1.id, node2.id);
			auto link2 = links::create(db, node2.id, node3.id);
			auto link3 = links::create(db, node3.id, node4.id);
			auto link4 = links::create(db, node4.id, node1.id);

			cloud cloud(db, node1.id);
			cloud.init();
			Assert::AreEqual(int64_t(2), cloud.seed());
			Assert::AreEqual(0, cloud.max_generation());
			auto links = cloud.links(0);
			Assert::AreEqual(size_t(2), links.size());
			Assert::IsTrue((links, link1.id));
			Assert::IsTrue(hasLink(links, link4.id));

			Assert::AreEqual(int64_t(2), cloud.expand());
			Assert::AreEqual(1, cloud.max_generation());
			links = cloud.links(cloud.max_generation());
			Assert::AreEqual(size_t(2), links.size());
			Assert::IsTrue(hasLink(links, link2.id));
			Assert::IsTrue(hasLink(links, link3.id));

			links = cloud.links(0);
			Assert::AreEqual(size_t(4), links.size());
			Assert::IsTrue(hasLink(links, link1.id));
			Assert::IsTrue(hasLink(links, link2.id));
			Assert::IsTrue(hasLink(links, link3.id));
			Assert::IsTrue(hasLink(links, link4.id));

			Assert::AreEqual(int64_t(0), cloud.expand());
			Assert::AreEqual(1, cloud.max_generation());
			links = cloud.links(cloud.max_generation());
			Assert::AreEqual(size_t(2), links.size());
			Assert::IsTrue(hasLink(links, link2.id));
			Assert::IsTrue(hasLink(links, link3.id));

			links = cloud.links(0);
			Assert::AreEqual(size_t(4), links.size());
			Assert::IsTrue(hasLink(links, link1.id));
			Assert::IsTrue(hasLink(links, link2.id));
			Assert::IsTrue(hasLink(links, link3.id));
			Assert::IsTrue(hasLink(links, link4.id));
		}
	};
}
