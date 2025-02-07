#include "pch.h"
#include "CppUnitTest.h"
#include "nldb.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace nldb
{
	TEST_CLASS(cmdtests)
	{
	private:
		void ValidateCmd(std::wstring cmd, std::vector<std::wstring> matches)
		{
			auto gotten = cmd::parse_cmds(cmd);
			Assert::AreEqual(matches.size(), gotten.size());
			for (size_t m = 0; m < matches.size(); ++m)
				Assert::AreEqual(matches[m], gotten[m]);
		}

	public:
		TEST_METHOD(TestParseCommands)
		{
			ValidateCmd(L"", {});
			ValidateCmd(L" ", {});
			ValidateCmd(L"  ", {});
			ValidateCmd(L"   ", {});

			ValidateCmd(L"foo", { L"foo" });
			ValidateCmd(L"foo ", { L"foo" });
			ValidateCmd(L" foo", { L"foo" });
			ValidateCmd(L" foo ", { L"foo" });

			ValidateCmd(L"foo bar", { L"foo", L"bar" });
			ValidateCmd(L"foo  bar", { L"foo", L"bar" });
			ValidateCmd(L"foo   bar", { L"foo", L"bar" });

			ValidateCmd(L"foo bar blet", { L"foo", L"bar", L"blet" });
			ValidateCmd(L"foo  bar  blet", { L"foo", L"bar", L"blet" });

			ValidateCmd(L"\"\"", { L"" });
			ValidateCmd(L"\" \"", { L" " });
			ValidateCmd(L"\"fred\"", { L"fred" });

			ValidateCmd(L"\"fred some\"", { L"fred some" });
			ValidateCmd(L"\"fred some\" blet", { L"fred some", L"blet" });
			ValidateCmd(L"\"fred some\" \"blet\"", { L"fred some", L"blet" });
			ValidateCmd(L"\"fred some\" \"blet\" \"some monkey\"", { L"fred some", L"blet", L"some monkey" });
		}
	};
}
