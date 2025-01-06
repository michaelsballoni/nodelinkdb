#include "includes.h"
#include "strings.h"
#include "db.h"

#pragma comment(lib, "nldblib.lib")

#include <stdio.h>

using namespace nldb;

int wmain(int argc, wchar_t* argv[]) {
	try {
		if (argc < 2) {
			printf("Usage: db-path command arguments\n");
			printf("\n");
			printf("Commands:\n");
			printf("init\n");
			printf("string-id \"one string to get the ID of\"\n");
			printf("string-val \"string ID to get the value for\"\n");
			printf("string-vals \"one string ID to get the value of\" \"another string ID to get the value of\"\n");
			return 0;
		}

		std::string db_path = toNarrowStr(argv[1]);
		printf("DB File: %s\n", db_path.c_str());
		db db(db_path);

		std::wstring cmd = argv[2];
		std::vector<std::wstring> args;
		for (int a = 3; a < argc; ++a)
			args.push_back(argv[a]);

		if (cmd == L"init") {
			printf("Setting up...");

			printf("strings...");
			strings::setup(db);

			printf("\nAll done.\n");
			return 0;
		}

		if (cmd == L"string-id") {
			if (args.size() != 1)
				throw nldberr("string-id: Specify one string value to get the ID of");
			int64_t id = strings::get_id(db, args[0]);
			printf("ID: %I64d\n", id);
			return 0;
		}

		if (cmd == L"string-val") {
			if (args.size() != 1)
				throw nldberr("string-val: Specify one string IDs to get the value of");
			int64_t id = _wtoi64(args[0].c_str());
			std::wstring val = strings::get_val(db, id);
			printf("Val: %S\n", val.c_str());
			return 0;
		}

		if (cmd == L"string-vals") {
			if (args.empty())
				throw nldberr("string-vals: Specify one or mroe string IDs to get the value of");

			std::vector<int64_t> ids;
			for (auto id_str : args)
				ids.push_back(_wtoi64(id_str.c_str()));

			auto id_vals = strings::get_vals(db, ids);

			for (auto id : ids) {
				std::wstring val = id_vals[id];
				printf("ID: %I64d - Val: %S\n", id, val.c_str());
			}
			return 0;
		}

		printf("ERROR: Unknown command: %S\n", cmd.c_str());
		return 1;
	}
	catch (const std::exception& exp) {
		printf("EXCEPTION: %s\n", exp.what());
		return 1;
	}
}
