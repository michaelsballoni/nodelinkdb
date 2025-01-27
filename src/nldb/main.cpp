#include "pch.h"

using namespace nldb;

int wmain(int argc, wchar_t* argv[]) 
{
	try 
	{
		if (argc < 2) {
			printf("Usage: db-path command arguments\n");
			printf("\n");
			printf("Commands:\n");
			printf("init\n");
			printf("string-id \"one string to get the ID of\"\n");
			printf("string-val \"string ID to get the value for\"\n");
			printf("string-vals \"one string ID to get the value of\" \"another string ID to get the value of\"\n");
			printf("load \"path to directory to load into the DB\"\n");
			printf("cmd\n");
			return 0;
		}

		std::string db_path = toNarrowStr(argv[1]);
		printf("DB File: %s\n", db_path.c_str());
		db db(db_path);

		std::wstring cmd = argv[2];
		std::vector<std::wstring> args;
		for (int a = 3; a < argc; ++a)
			args.push_back(argv[a]);

		if (cmd == L"init") 
		{
			printf("Setting up...");
			setup_nldb(db);
			printf("\nAll done.\n");
			return 0;
		}

		if (cmd == L"string-id") 
		{
			if (args.size() != 1)
				throw nldberr("string-id: Specify one string value to get the ID of");
			int64_t id = strings::get_id(db, args[0]);
			printf("ID: %I64d\n", id);
			return 0;
		}

		if (cmd == L"string-val") 
		{
			if (args.size() != 1)
				throw nldberr("string-val: Specify one string IDs to get the value of");
			int64_t id = _wtoi64(args[0].c_str());
			std::wstring val = strings::get_val(db, id);
			printf("Val: %S\n", val.c_str());
			return 0;
		}

		if (cmd == L"string-vals") 
		{
			if (args.empty())
				throw nldberr("string-vals: Specify one or more string IDs to get the value of");

			std::vector<int64_t> ids;
			for (const std::wstring& id_str : args)
				ids.push_back(_wtoi64(id_str.c_str()));

			auto id_vals = strings::get_vals(db, ids);
			for (int64_t id : ids) 
			{
				std::wstring val = id_vals[id];
				printf("ID: %I64d - Val: %S\n", id, val.c_str());
			}
			return 0;
		}

		if (cmd == L"load") 
		{
			if (args.size() != 1)
				throw nldberr("load: Specify one path to a directory to load into the DB");
			std::wstring dir_path = args[0];
			printf("Directory: %S\n", dir_path.c_str());

			loader loader(db);
			stopwatch sw("load");
			loader.load_directory(dir_path);
			sw.record();
			sw.print();

			return 0;
		}

		if (cmd == L"cmd")
		{
			nldb::cmd cmd_obj(db);
			std::wstring cur_path;
			std::wstring cmd_str;
			while (true)
			{
				cur_path = cmd_obj.get_cur_path();
				printf("%S> ", cur_path.c_str());

				std::getline(std::wcin, cmd_str);
				if (cmd_str.empty())
					continue;
				else if (cmd_str == L"quit" || cmd_str == L"exit")
					break;

				try
				{
					std::wstring after_party;
					size_t space = cmd_str.find(' ');
					if (space != std::wstring::npos)
					{
						after_party = cmd_str.substr(space + 1);
						cmd_str = cmd_str.substr(0, space);
					}

					stopwatch sw("cmd");
					if (cmd_str == L"cd")
						cmd_obj.cd(after_party);
					else if (cmd_str == L"load")
						cmd_obj.load(after_party);
					else if (cmd_str == L"dir")
						printf("%S\n", join(cmd_obj.dir(), L"\n").c_str());
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
			
			return 0;
		}

		printf("ERROR: Unknown command: %S\n", cmd.c_str());
		return 1;
	}
	catch (const std::exception& exp) 
	{
		printf("EXCEPTION: %s\n", exp.what());
		return 1;
	}
}
