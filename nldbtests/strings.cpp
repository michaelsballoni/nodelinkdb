#include "pch.h"
#include "CppUnitTest.h"
#include "nldb.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace nldb
{
	TEST_CLASS(stringstests)
	{
	public:
		TEST_METHOD(TestStrings)
		{
			db db("tests.strings.db");
			setup_nldb(db);

			try 
			{
				strings::get_val(db, -1200);
				Assert::Fail();
			}
			catch (const nldberr) {}

			std::wstring str1 = L"foo";
			std::wstring str2 = L"bazr";

			auto id1 = strings::get_id(db, str1);
			Assert::AreEqual(int64_t(1), id1);

			auto id1b = strings::get_id(db, str1);
			Assert::AreEqual(int64_t(1), id1b);

			auto id1c = strings::get_id(db, str1);
			Assert::AreEqual(int64_t(1), id1c);

			auto id2 = strings::get_id(db, str2);
			Assert::AreEqual(int64_t(2), id2);

			auto id2b = strings::get_id(db, str2);
			Assert::AreEqual(int64_t(2), id2b);

			Assert::IsTrue(strings::get_val(db, id1) == str1);
			Assert::IsTrue(strings::get_val(db, id1b) == str1);
			Assert::IsTrue(strings::get_val(db, id1c) == str1);
			Assert::IsTrue(strings::get_val(db, id2) == str2);
			Assert::IsTrue(strings::get_val(db, id2b) == str2);

			strings::flush_caches();

			auto strs = strings::get_vals(db, { id1, id2 });
			Assert::IsTrue(strs[id1] == str1);
			Assert::IsTrue(strs[id2] == str2);

			auto strs2 = strings::get_vals(db, { id1, id2 });
			Assert::IsTrue(strs2[id1] == str1);
			Assert::IsTrue(strs2[id2] == str2);

			strings::flush_caches();

			auto strs3 = strings::get_vals(db, { id1, id2 });
			Assert::IsTrue(strs3[id1] == str1);
			Assert::IsTrue(strs3[id2] == str2);

			try 
			{
				strings::get_vals(db, { id1, id2, -782 });
				Assert::Fail();
			}
			catch (const nldberr& exp) 
			{
				Assert::IsTrue(std::string(exp.what()).find("not found") != std::string::npos);
			}
		}
	};
}
