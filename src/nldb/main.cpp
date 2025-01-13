#include "includes.h"
#include "strings.h"
#include "nodes.h"
#include "db.h"

#pragma comment(lib, "nldblib.lib")

#include <stdio.h>

#include <filesystem>
namespace fs = std::filesystem;

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
			printf("load \"path to directory to load into the DB\"\n");
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

			printf("nodes...");
			nodes::setup(db);

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
				throw nldberr("string-vals: Specify one or more string IDs to get the value of");

			std::vector<int64_t> ids;
			for (const std::wstring& id_str : args)
				ids.push_back(_wtoi64(id_str.c_str()));

			auto id_vals = strings::get_vals(db, ids);
			for (int64_t id : ids) {
				std::wstring val = id_vals[id];
				printf("ID: %I64d - Val: %S\n", id, val.c_str());
			}
			return 0;
		}

		if (cmd == L"load") {
			if (args.size() != 1)
				throw nldberr("load: Specify one path to a directory to load into the DB");
			std::wstring dir_path = args[0];
			printf("Directory: %S\n", dir_path.c_str());

			printf("Setting up...");
			strings::setup(db);
			nodes::setup(db);
			printf("done!\n");

			int64_t file_type_string_id = strings::get_id(db, L"file");
			int64_t dir_type_string_id = strings::get_id(db, L"dir");

			std::unordered_map<std::wstring, int64_t> string_cache;
			std::unordered_map<std::string, node> parent_node_cache;

			std::vector<std::wstring> path_parts;

			for (const auto& dir_entry : fs::recursive_directory_iterator(dir_path)) {
				bool is_file = dir_entry.is_regular_file();

    			path_parts.clear();
				for (const std::wstring& path_part : dir_entry.path())
					path_parts.push_back(path_part);

				node cur_node;
				for (size_t p = 0; p < path_parts.size(); ++p) {
					const std::wstring& path_part = path_parts[p];

					int64_t path_part_string_id;
					{
						const auto str_cache_it = string_cache.find(path_part);
						if (str_cache_it == string_cache.end()) {
							path_part_string_id = strings::get_id(db, path_part);
							string_cache.insert({ path_part, path_part_string_id });
						}
						else
							path_part_string_id = str_cache_it->second;
					}

					char parent_cache_key[100];
					sprintf(parent_cache_key, "%I64d_%I64d", cur_node.m_id, path_part_string_id);

					const auto parent_node_it = parent_node_cache.find(parent_cache_key);
					if (parent_node_it == parent_node_cache.end()) {
						auto node_in_path = nodes::get_node_in_parent(db, cur_node.m_id, path_part_string_id);
						if (!node_in_path.has_value())
							cur_node = nodes::create(db, cur_node.m_id, path_part_string_id, p == path_parts.size() - 1 && is_file ? file_type_string_id : dir_type_string_id);
						else
							cur_node = node_in_path.value();
						parent_node_cache.insert({ parent_cache_key, cur_node });
					}
					else
						cur_node = parent_node_it->second;
				}
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
