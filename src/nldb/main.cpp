#include "pch.h"

using namespace nldb;

std::wstring getOneCmd(const std::vector<std::wstring>& cmds)
{
	if (cmds.size() != 2)
		throw nldberr("ERROR: This command takes one command");
	else
		return cmds[1]; // right after the command itself
}

bool verify()
{
	printf("Are you sure?  Enter Y or N: ");
	std::wstring cmd_str;
	std::getline(std::wcin, cmd_str);
	if (cmd_str.empty())
		return false;
	else if (cmd_str[0] != 'Y' && cmd_str[0] != 'y')
		return false;
	else
		return true;
}

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
				
				if (cmd_str == L"help")
				{
					printf
					(
						"Here are the available commands.\n"
						"Parameters passed into commands can have \"\" around them, not <>\n"
						"init - Sets up or reinitializes the data in the database file\n"
						"mount <file system directory path to add to DB>\n"
						"cd <path of node to change to>\n"
						"dir - lists children of the current node\n"
						"mknode <node name> - add a node to the DB\n"
						"copy <new parent node> - Make a copy the current node in a new parent node\n"
						"move <new parent node> - Move the current node to a different parent node\n"
						"rename <node name> - Rename the current node\n"
						"remove - Removes the current node from the database\n"
						"string-id <string to get ID of>\n"
						"string-val <string ID to get value of>\n"
					);
					continue;
				}

				auto cmds = cmd::parseCommands(cmd_str);
				if (cmds.empty())
					continue;
				std::wstring cmd_str = cmds[0];

				stopwatch sw("cmd");
				if (cmd_str == L"init")
				{
					if (verify())
						setup_nldb(db);
				}
				else if (cmd_str == L"string-id")
					printf("ID: %I64d\n", strings::get_id(db, getOneCmd(cmds)));
				else if (cmd_str == L"string-val")
					printf("Val: %S\n", strings::get_val(db, _wtoi64(getOneCmd(cmds).c_str())).c_str());
				else if (cmd_str == L"cd")
					cmd_obj.cd(getOneCmd(cmds));
				else if (cmd_str == L"mount")
					cmd_obj.mount(getOneCmd(cmds));
				else if (cmd_str == L"dir")
					printf("%S\n", join(cmd_obj.dir(), L"\n").c_str());
				else if (cmd_str == L"mknode")
					cmd_obj.mknode(getOneCmd(cmds));
				else if (cmd_str == L"copy")
					cmd_obj.copy(getOneCmd(cmds));
				else if (cmd_str == L"move")
					cmd_obj.move(getOneCmd(cmds));
				else if (cmd_str == L"remove")
				{
					if (verify())
						cmd_obj.remove();
				}
				else if (cmd_str == L"rename")
					cmd_obj.rename(getOneCmd(cmds));
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
