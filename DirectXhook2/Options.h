#pragma once

#include <string>

#define OPTION(type, var, val) type var = val

class Config
{
public:
	//example: OPTION(bool, esp_enabled, false);
	OPTION(bool, text_enabled, false);
	OPTION(bool, light_enabled, false);
	OPTION(bool, texture_enabled, false);
};

extern Config g_Options;
extern bool   g_Unload;
