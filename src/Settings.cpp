/*
  Copyright (C) 2001-2004 Stephane Magnenat & Luc-Olivier de Charrière
  for any question or comment contact us at <stephane at magnenat dot net> or <NuageBleu at gmail dot com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "Settings.h"
#include "Utilities.h"
#include <Stream.h>
#include <BinaryStream.h>
#include <stdlib.h>
#include <GAG.h>
#include <map>
#include <fstream>
#include "boost/lexical_cast.hpp"

using namespace GAGCore;

Settings::Settings()
{
	// set default values in settings or load them
	char *newUsername;

#	ifdef WIN32
		newUsername=getenv("USERNAME");
#	else // angel > case of unix and MacIntosh Systems
		newUsername=getenv("USER");		
#	endif
	if (!newUsername)
		newUsername="player";	
	username=newUsername;

	screenFlags = GraphicContext::RESIZABLE | GraphicContext::CUSTOMCURSOR;
	screenWidth = 640;
	screenHeight = 480;
	optionFlags = 0;
	defaultLanguage = 0;
	musicVolume = 255;
	mute = 0;
	rememberUnit = 1;
	restoreDefaultShortcuts();
	tempUnit = 1;
	tempUnitFuture = 1;
	//while a little ugly it is necessary to store varPrestige within settings 
	//to prevent session.cpp from overriding variable once map is chosen
	tempVarPrestige = 0;
	for(int n=0; n<IntBuildingType::NB_BUILDING; ++n)
	{
		for(int t=0; t<6; ++t)
		{
			defaultUnitsAssigned[n][t] = 0;
		}
	}
	defaultUnitsAssigned[IntBuildingType::WAR_FLAG][0] = 20;
	defaultUnitsAssigned[IntBuildingType::CLEARING_FLAG][0] = 2;
	defaultUnitsAssigned[IntBuildingType::EXPLORATION_FLAG][0] = 1;
	defaultUnitsAssigned[IntBuildingType::SWARM_BUILDING][0] = 7;
	defaultUnitsAssigned[IntBuildingType::SWARM_BUILDING][1] = 4;
	defaultUnitsAssigned[IntBuildingType::FOOD_BUILDING][0] = 3;
	defaultUnitsAssigned[IntBuildingType::FOOD_BUILDING][1] = 2;
	defaultUnitsAssigned[IntBuildingType::FOOD_BUILDING][2] = 5;
	defaultUnitsAssigned[IntBuildingType::FOOD_BUILDING][3] = 3;
	defaultUnitsAssigned[IntBuildingType::FOOD_BUILDING][4] = 15;
	defaultUnitsAssigned[IntBuildingType::FOOD_BUILDING][5] = 8;
	defaultUnitsAssigned[IntBuildingType::HEAL_BUILDING][0] = 2;
	defaultUnitsAssigned[IntBuildingType::HEAL_BUILDING][2] = 4;
	defaultUnitsAssigned[IntBuildingType::HEAL_BUILDING][4] = 6;
	defaultUnitsAssigned[IntBuildingType::WALKSPEED_BUILDING][0] = 3;
	defaultUnitsAssigned[IntBuildingType::WALKSPEED_BUILDING][2] = 7;
	defaultUnitsAssigned[IntBuildingType::WALKSPEED_BUILDING][4] = 12;
	defaultUnitsAssigned[IntBuildingType::SWIMSPEED_BUILDING][0] = 2;
	defaultUnitsAssigned[IntBuildingType::SWIMSPEED_BUILDING][2] = 5;
	defaultUnitsAssigned[IntBuildingType::SWIMSPEED_BUILDING][4] = 12;
	defaultUnitsAssigned[IntBuildingType::ATTACK_BUILDING][0] = 3;
	defaultUnitsAssigned[IntBuildingType::ATTACK_BUILDING][2] = 6;
	defaultUnitsAssigned[IntBuildingType::ATTACK_BUILDING][4] = 9;
	defaultUnitsAssigned[IntBuildingType::SCIENCE_BUILDING][0] = 5;
	defaultUnitsAssigned[IntBuildingType::SCIENCE_BUILDING][2] = 10;
	defaultUnitsAssigned[IntBuildingType::SCIENCE_BUILDING][4] = 20;
	defaultUnitsAssigned[IntBuildingType::DEFENSE_BUILDING][0] = 3;
	defaultUnitsAssigned[IntBuildingType::DEFENSE_BUILDING][1] = 2;
	defaultUnitsAssigned[IntBuildingType::DEFENSE_BUILDING][2] = 5;
	defaultUnitsAssigned[IntBuildingType::DEFENSE_BUILDING][3] = 2;
	defaultUnitsAssigned[IntBuildingType::DEFENSE_BUILDING][4] = 8;
	defaultUnitsAssigned[IntBuildingType::DEFENSE_BUILDING][5] = 2;
	defaultUnitsAssigned[IntBuildingType::STONE_WALL][0] = 1;
	defaultUnitsAssigned[IntBuildingType::MARKET_BUILDING][0] = 3;

	cloudPatchSize=16;//the bigger the faster the uglier
	cloudMaxAlpha=120;//the higher the nicer the clouds the harder the units are visible
	cloudMaxSpeed=3;
	cloudWindStability=3550;//how much will the wind change
	cloudStability=1300;//how much will the clouds change shape
	cloudSize=300;//the bigger the better they look with big Patches. The smaller the better they look with smaller patches
	cloudHeight=150;//(cloud - ground) / (eyes - ground) * 100 (to get an int value)
}



void Settings::restoreDefaultShortcuts()
{
	keyboard_shortcuts["akey"]="toggle draw accessibility aids";
	keyboard_shortcuts["bkey"]="";
	keyboard_shortcuts["ckey"]="";
	keyboard_shortcuts["dkey"]="destroy building";
	keyboard_shortcuts["ekey"]="";
	keyboard_shortcuts["fkey"]="";
	keyboard_shortcuts["gkey"]="";
	keyboard_shortcuts["hkey"]="";
	keyboard_shortcuts["ikey"]="toggle draw information";
	keyboard_shortcuts["jkey"]="";
	keyboard_shortcuts["kkey"]="";
	keyboard_shortcuts["lkey"]="";
	keyboard_shortcuts["mkey"]="mark map";
	keyboard_shortcuts["nkey"]="";
	keyboard_shortcuts["okey"]="";
	keyboard_shortcuts["pkey"]="pause game";
	keyboard_shortcuts["qkey"]="";
	keyboard_shortcuts["rkey"]="repair building";
	keyboard_shortcuts["skey"]="";
	keyboard_shortcuts["tkey"]="toggle draw unit paths";
	keyboard_shortcuts["ukey"]="upgrade building";
	keyboard_shortcuts["vkey"]="record voice";
	keyboard_shortcuts["wkey"]="";
	keyboard_shortcuts["xkey"]="";
	keyboard_shortcuts["ykey"]="";
	keyboard_shortcuts["zkey"]="";

	editor_keyboard_shortcuts["akey"]="";
	editor_keyboard_shortcuts["bkey"]="";
	editor_keyboard_shortcuts["ckey"]="";
	editor_keyboard_shortcuts["dkey"]="";
	editor_keyboard_shortcuts["ekey"]="";
	editor_keyboard_shortcuts["fkey"]="";
	editor_keyboard_shortcuts["gkey"]="";
	editor_keyboard_shortcuts["hkey"]="";
	editor_keyboard_shortcuts["ikey"]="";
	editor_keyboard_shortcuts["jkey"]="";
	editor_keyboard_shortcuts["kkey"]="";
	editor_keyboard_shortcuts["lkey"]="";
	editor_keyboard_shortcuts["mkey"]="";
	editor_keyboard_shortcuts["nkey"]="";
	editor_keyboard_shortcuts["okey"]="";
	editor_keyboard_shortcuts["pkey"]="";
	editor_keyboard_shortcuts["qkey"]="";
	editor_keyboard_shortcuts["rkey"]="";
	editor_keyboard_shortcuts["skey"]="";
	editor_keyboard_shortcuts["tkey"]="";
	editor_keyboard_shortcuts["ukey"]="";
	editor_keyboard_shortcuts["vkey"]="";
	editor_keyboard_shortcuts["wkey"]="";
	editor_keyboard_shortcuts["xkey"]="";
	editor_keyboard_shortcuts["ykey"]="";
	editor_keyboard_shortcuts["zkey"]="";
}




#define READ_PARSED_STRING(var) \
{ \
	if (parsed.find(#var) != parsed.end()) \
		var = parsed[#var]; \
}

#define READ_PARSED_INT(var) \
{ \
	if (parsed.find(#var) != parsed.end()) \
		var = atoi(parsed[#var].c_str()); \
}

void Settings::load(const char *filename)
{
	std::map<std::string, std::string> parsed;

	InputStream *stream = new BinaryInputStream(Toolkit::getFileManager()->openInputStreamBackend(filename));
	if (stream->isEndOfStream())
	{
		std::cerr << "Settings::load(\"" << filename << "\") : error, can't open file." << std::endl;
	}
	else
	{
		// load and parse file
		char *dest, *varname, *token;
		char buffer[256];
		while ((dest = Utilities::gets(buffer, 256, stream))!=NULL)
		{
			token = strtok(dest,"\t\n\r=;");
			if ((!token) || (strcmp(token,"//")==0))
				continue;
			varname = token;
			token = strtok(NULL,"\t\n\r=");
			if (token)
				parsed[varname] = token;
		}

		// read values
		READ_PARSED_STRING(username);
		READ_PARSED_STRING(password);
		READ_PARSED_INT(screenWidth);
		READ_PARSED_INT(screenHeight);
		READ_PARSED_INT(screenFlags);
		READ_PARSED_INT(optionFlags);
		READ_PARSED_INT(defaultLanguage);
		READ_PARSED_INT(musicVolume);		
		READ_PARSED_INT(mute);
		READ_PARSED_INT(rememberUnit);

		for(int n=0; n<IntBuildingType::NB_BUILDING; ++n)
		{
			for(int t=0; t<6; ++t)
			{
				std::string keyname="defaultUnitsAssigned["+boost::lexical_cast<std::string>(n)+"]["+boost::lexical_cast<std::string>(t)+"]";
				if(parsed.find(keyname)!=parsed.end())
					defaultUnitsAssigned[n][t] = boost::lexical_cast<int>(parsed[keyname]);
			}
		}

		READ_PARSED_INT(cloudPatchSize);
		READ_PARSED_INT(cloudMaxAlpha);
		READ_PARSED_INT(cloudMaxSpeed);
		READ_PARSED_INT(cloudWindStability);
		READ_PARSED_INT(cloudStability);
		READ_PARSED_INT(cloudSize);
		READ_PARSED_INT(cloudHeight);

		for(std::map<std::string, std::string>::iterator i=keyboard_shortcuts.begin(); i!=keyboard_shortcuts.end(); ++i)
		{
			if(parsed.find("game_"+i->first)!=parsed.end())
				i->second=parsed["game_"+i->first];
		}
		for(std::map<std::string, std::string>::iterator i=editor_keyboard_shortcuts.begin(); i!=editor_keyboard_shortcuts.end(); ++i)
		{
			if(parsed.find("editor_"+i->first)!=parsed.end())
				i->second=parsed["editor_"+i->first];
		}
	}
	delete stream;
}

void Settings::save(const char *filename)
{
	OutputStream *stream = new BinaryOutputStream(Toolkit::getFileManager()->openOutputStreamBackend(filename));
//	std::fstream f(filename);
	if (stream->isEndOfStream())
//	if (!f.is_open())
	{
		std::cerr << "Settings::save(\"" << filename << "\") : error, can't open file." << std::endl;
	}
	else
	{
		Utilities::streamprintf(stream, "username=%s\n", username.c_str());
		Utilities::streamprintf(stream, "password=%s\n", password.c_str());
		Utilities::streamprintf(stream, "screenWidth=%d\n", screenWidth);
		Utilities::streamprintf(stream, "screenHeight=%d\n", screenHeight);
		Utilities::streamprintf(stream, "screenFlags=%d\n", screenFlags);
		Utilities::streamprintf(stream, "optionFlags=%d\n", optionFlags);
		Utilities::streamprintf(stream, "defaultLanguage=%d\n", defaultLanguage);
		Utilities::streamprintf(stream, "musicVolume=%d\n", musicVolume);
		Utilities::streamprintf(stream, "mute=%d\n", mute);
		Utilities::streamprintf(stream, "rememberUnit=%d\n", rememberUnit);

		for(int n=0; n<IntBuildingType::NB_BUILDING; ++n)
		{
			for(int t=0; t<6; ++t)
			{
				std::string keyname="defaultUnitsAssigned["+boost::lexical_cast<std::string>(n)+"]["+boost::lexical_cast<std::string>(t)+"]";
				Utilities::streamprintf(stream, "%s=%i\n", keyname.c_str(), defaultUnitsAssigned[n][t]);
			}
		}

		Utilities::streamprintf(stream, "cloudPatchSize=%d\n",	cloudPatchSize);
		Utilities::streamprintf(stream, "cloudMaxAlpha=%d\n",	cloudMaxAlpha);
		Utilities::streamprintf(stream, "cloudMaxSpeed=%d\n",	cloudMaxSpeed);
		Utilities::streamprintf(stream, "cloudWindStability=%d\n",	cloudWindStability);
		Utilities::streamprintf(stream, "cloudStability=%d\n",	cloudStability);
		Utilities::streamprintf(stream, "cloudSize=%d\n",	cloudSize);
		Utilities::streamprintf(stream, "cloudHeight=%d\n",	cloudHeight);
		
		for(std::map<std::string, std::string>::iterator i=keyboard_shortcuts.begin(); i!=keyboard_shortcuts.end(); ++i)
		{
			Utilities::streamprintf(stream, "game_%s=%s\n", i->first.c_str(), i->second.c_str());
		}
		for(std::map<std::string, std::string>::iterator i=editor_keyboard_shortcuts.begin(); i!=editor_keyboard_shortcuts.end(); ++i)
		{
			Utilities::streamprintf(stream, "editor_%s=%s\n", i->first.c_str(), i->second.c_str());
		}
	}
	delete stream;
}
