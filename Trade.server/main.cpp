#include "stdafx.h"

int _tmain(int argc, char* argv[])
{
	HANDLE hToken;
	if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken)) {
		sk::Helper::SetPrivilege(hToken, SE_DEBUG_NAME, TRUE); 
	}
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(268);
#endif
	sk::Dump __Dump;


	local::Global::CoreGet()->Open();

	do {
		sk::stringa cin_;
		char c;
		while (std::cin >> std::noskipws >> c)
		{
			if ('\n' == c) break;
			cin_.push_back(c);
		}
		std::cin >> std::skipws;
		sk::StringConvert::ToLowerA(cin_);
		std::vector<sk::stringa> cmds;
		sk::Helper::OneUnknownParseA(cin_, ' ', cmds);
		if (cmds.empty())
			continue;

		sk::stringa cmd = cmds[0];
		sk::stringa param;
		if (cmds.size() > 1)
		{
			param = cmds[1];
		}	
		if (cmd == "q" || cmd == "/q" || cmd == "/quit")
		{
			local::Global::CoreGet()->Close();
			break;
		}
		else
		{

		}
	} while (1);

	return 0;
}