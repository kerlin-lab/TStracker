#include "TSRecordingSetting.h"

#include <iostream>
#include <fstream>
#include <stdio.h>

#define SYNTAX_COUNT 2

const char* syntax[] = {
	"<BreakTime>%d</BreakTime>",
	"<GUIFPS>%d</GUIFPS>"
};

TSRecordingSetting::TSRecordingSetting()
{
	// Deafult values
	this->gui_fps = 30;
	this->wait_time = 500;
}


bool TSRecordingSetting::loadConfig(std::string pathToConfigFile)
{
	std::ifstream configFile;
	std::string line;
	int vl;
	configFile.open(pathToConfigFile);
	if (!configFile.is_open())
	{
		return false;
	}
	else
	{
		while (!configFile.eof())
		{
			std::getline(configFile, line);
			for (unsigned i = 0; i < SYNTAX_COUNT; i++)
			{
				if (sscanf(line.c_str(), syntax[i], &vl) == 1)
				{
					switch (i)
					{
					case 0:
						this->wait_time = vl;
						break;
					case 1:
						this->gui_fps = vl;
						break;
					}
				}
			}
		}
	}
	configFile.close();
	return true;
}

bool TSRecordingSetting::saveConfig(std::string pathToConfigFile)
{
	std::ofstream configFile;
	char buff[100];
	configFile.open(pathToConfigFile, std::ofstream::out | std::ofstream::trunc);
	if (!configFile.is_open())
	{
		return false;
	}
	else
	{
		for (unsigned i = 0; i < SYNTAX_COUNT; i++)
		{
			switch (i)
			{
			case 0:
				sprintf(buff, syntax[i], this->wait_time);
				break;
			case 1:
				sprintf(buff, syntax[i], this->gui_fps);
				break;
			}
			configFile << buff << std::endl;
		}
	}
	configFile.close();
	return true;
}