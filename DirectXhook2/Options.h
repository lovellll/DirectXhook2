#pragma once

#include <string>

#define OPTION(type, var, val) type var = val

class Config
{
public:
	OPTION(bool, text_enabled, false);
};

extern Config g_Options;
extern bool   g_Unload;
