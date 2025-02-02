#include "pch.h"

using namespace nldb;

int wmain(int argc, wchar_t* argv[]) 
{
	try 
	{
		if (argc != 2) 
		{
			printf("Usage: db-path\n");
			return 0;
		}

		std::string db_path = toNarrowStr(argv[1]);
		printf("DB File: %s\n", db_path.c_str());
		db db(db_path);

		nldb::cmd cmd_obj(db);
		std::wstring cur_path;
		std::wstring cmd_str;
		while (true)
		{
			try
			{
				cur_path = cmd_obj.get_cur_path();
				printf("\n%S> ", cur_path.c_str());

				std::getline(std::wcin, cmd_str);
				if (cmd_str.empty())
					continue;
				else if (cmd_str == L"quit" || cmd_str == L"exit")
					break;

				std::wstring after_party;
				size_t space = cmd_str.find(' ');
				if (space != std::wstring::npos)
				{
					after_party = cmd_str.substr(space + 1);
					cmd_str = cmd_str.substr(0, space);
				}

				stopwatch sw("cmd");
				if (cmd_str == L"string-id")
				{
					int64_t id = strings::get_id(db, after_party);
					printf("ID: %I64d\n", id);
				}
				else if (cmd_str == L"string-val")
				{
					if (after_party.empty())
						throw nldberr("string-val: Specify one string IDs to get the value of");
					int64_t id = _wtoi64(after_party.c_str());
					std::wstring val = strings::get_val(db, id);
					printf("Val: %S\n", val.c_str());
				}
				else if (cmd_str == L"cd")
					cmd_obj.cd(after_party);
				else if (cmd_str == L"mount")
					cmd_obj.mount(after_party);
				else if (cmd_str == L"dir")
					printf("%S\n", join(cmd_obj.dir(), L"\n").c_str());
				else if (cmd_str == L"mknode")
					cmd_obj.mknode(after_party);
				else if (cmd_str == L"remove")
					cmd_obj.remove();
				else if (cmd_str == L"rename")
					cmd_obj.rename(after_party);
				else
					throw nldberr("Unknown command: " + toNarrowStr(cmd_str));
				sw.record();
				sw.print();
			}
			catch (const std::exception& cmdExp)
			{
				printf("ERROR: %s\n", cmdExp.what());
			}
		}
	}
	catch (const std::exception& exp) 
	{
		printf("EXCEPTION: %s\n", exp.what());
		return 1;
	}
	return 0;
}
