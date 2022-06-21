#include "Config.h"
#include "Variables.h"

Variables Vars;

void CConfig::Setup()
{
	SetupValue(Vars.OriginalFileSize, 0, "OriginalFileSize");
	SetupValue(Vars.NewHash, "", "OriginalFileHash");
}

void CConfig::SetupValue(int& value, int def, std::string name)
{
	value = def;
	ints.push_back(new ConfigValue< int >(name, &value));
}

void CConfig::SetupValue(float& value, float def, std::string name)
{
	value = def;
	floats.push_back(new ConfigValue< float >(name, &value));
}

void CConfig::SetupValue(bool& value, bool def, std::string name)
{
	value = def;
	bools.push_back(new ConfigValue< bool >(name, &value));
}

void CConfig::SetupValue(std::string& value, std::string def, std::string name)
{
	value = def;
	strings.push_back(new ConfigValue< std::string >(name, &value));
}

void CConfig::SetupValue(bool* value, bool def, int size, std::string name)
{
	for (int c = 0; c < size; c++)
	{
		value[c] = def;

		name += std::to_string(c);

		bools.push_back(new ConfigValue< bool >(name, &value[c]));
	}
}

static char name[] = "EFTGhost";

void CConfig::Save()
{
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		folder = std::string(path) + "\\EFTGhost\\";
		file = std::string(path) + "\\EFTGhost\\cfg.ini";
	}

	CreateDirectory(folder.c_str(), NULL);

	for (auto value : ints)
		WritePrivateProfileString(name, value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

	for (auto value : floats)
		WritePrivateProfileString(name, value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

	for (auto value : bools)
		WritePrivateProfileString(name, value->name.c_str(), *value->value ? "true" : "false", file.c_str());

	for (auto value : strings)
		WritePrivateProfileString(name, value->name.c_str(), value->value->c_str(), file.c_str());
}

void CConfig::Load()
{
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		folder = std::string(path) + "\\EFTGhost\\";
		file = std::string(path) + "\\EFTGhost\\cfg.ini";
	}

	CreateDirectory(folder.c_str(), NULL);

	char value_l[50] = { '\0' };

	for (auto value : ints)
	{
		GetPrivateProfileString(name, value->name.c_str(), "", value_l, 50, file.c_str());
		*value->value = atoi(value_l);
	}

	for (auto value : floats)
	{
		GetPrivateProfileString(name, value->name.c_str(), "", value_l, 50, file.c_str());
		*value->value = atof(value_l);
	}

	for (auto value : bools)
	{
		GetPrivateProfileString(name, value->name.c_str(), "", value_l, 50, file.c_str());
		*value->value = !strcmp(value_l, "true");
	}

	for (auto value : strings)
	{
		GetPrivateProfileString(name, value->name.c_str(), "", value_l, 50, file.c_str());
		*value->value = value_l;
	}
}

CConfig* g_Config = new CConfig();