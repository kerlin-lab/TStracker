#pragma once
#ifndef _TS_RECORDING_SETTING_H_
#define _TS_RECORDING_SETTING_H_

#include <string>

typedef unsigned int BreakWaitTime;
typedef unsigned int GUIFps;

class TSRecordingSetting
{
	public:
		BreakWaitTime wait_time;
		GUIFps gui_fps;

	public:
		TSRecordingSetting();

	public:
		bool loadConfig(std::string pathToConfigFile);
		bool saveConfig(std::string pathToConfigFile);


};
#endif