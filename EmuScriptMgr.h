#ifndef EMUSCRIPTMGR_H
#define EMUSCRIPTMGR_H

#include <vector>
#include "sol/sol.hpp"
#include "sol/assert.hpp"
#include <cassert>

class EmuScriptMgr
{
public:
	EmuScriptMgr();
	~EmuScriptMgr();

	void LoadScripts();
	void ConvertToBytecode(const std::string& script);
	std::vector<std::string> FindAllFilePaths();
	std::vector<std::string> KeepScriptPathsWithExt(std::string);
private:
	std::vector<std::string> s_scripts;
	std::vector<sol::bytecode> c_scripts;
};

#endif // EMUSCRIPTMGR_H