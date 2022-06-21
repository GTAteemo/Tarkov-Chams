#pragma once

class Variables
{
public:
	int OriginalFileSize = 0;
	std::string NewHash = "";
	std::string CurrentHash = "";
};

extern Variables Vars;