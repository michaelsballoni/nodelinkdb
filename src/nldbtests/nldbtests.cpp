#include "pch.h"
#include "CppUnitTest.h"
#include "db.h"
#include "strings.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace nldb
{
	TEST_CLASS(stringstests)
	{
	public:
		TEST_METHOD(TestStrings)
		{
			db db("tests.strings.db");
			strings::init(db);

			try {
				strings::get_val(db, -1200);
				Assert::Fail();
			}
			catch (const nldberr) {}

			std::wstring str1 = L"foo";
			auto id1 = strings::get_id(db, str1);
			Assert::AreEqual(int64_t(1), id1);

			std::wstring str2 = L"bazr";
			auto id2 = strings::get_id(db, str2);
			Assert::AreEqual(int64_t(2), id2);

			Assert::IsTrue(strings::get_val(db, id1) == str1);
			Assert::IsTrue(strings::get_val(db, id2) == str2);

			auto strs = strings::get_vals(db, { id1, id2 });
			Assert::IsTrue(strs[id1] == str1);
			Assert::IsTrue(strs[id2] == str2);

			try {
				strings::get_vals(db, { id1, id2, -782 });
				Assert::Fail();
			}
			catch (const nldberr) {}
		}
	};
}
