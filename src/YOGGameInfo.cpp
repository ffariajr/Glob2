/*
  Copyright (C) 2007 Bradley Arsenault

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "YOGGameInfo.h"
#include "SDL_net.h"
#include <iostream>

YOGGameInfo::YOGGameInfo()
{
	gameID=0;
	gameState = GameOpen;
}



YOGGameInfo::YOGGameInfo(const std::string& gameName, Uint16 gameID)
	: gameID(gameID), gameName(gameName), gameState(GameOpen)
{
}



void YOGGameInfo::setGameName(const std::string& newGameName)
{
	gameName = newGameName;
}


	
std::string YOGGameInfo::getGameName() const
{
	return gameName;
}



void YOGGameInfo::setGameID(Uint16 id)
{
	gameID=id;
}



Uint16 YOGGameInfo::getGameID() const
{
	return gameID;
}



YOGGameInfo::GameState YOGGameInfo::getGameState() const
{
	return gameState;
}


	
void YOGGameInfo::setGameState(const YOGGameInfo::GameState& state)
{
	gameState = state;
}


void YOGGameInfo::encodeData(GAGCore::OutputStream* stream) const
{
	stream->writeEnterSection("YOGGameInfo");
	stream->writeUint16(gameID, "gameID");
	stream->writeText(gameName, "gameName");
	stream->writeUint8(static_cast<Uint8>(gameState), "gameState");
	stream->writeLeaveSection();
}



void YOGGameInfo::decodeData(GAGCore::InputStream* stream)
{
	stream->readEnterSection("YOGGameInfo");
	gameID=stream->readUint16("gameID");
	gameName=stream->readText("gameName");
	gameState=static_cast<GameState>(stream->readUint8("gameState"));
	stream->readLeaveSection();
}


	
bool YOGGameInfo::operator==(const YOGGameInfo& rhs) const
{
	if(gameName == rhs.gameName && gameID == rhs.gameID && gameState == rhs.gameState)
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

	
	
bool YOGGameInfo::operator!=(const YOGGameInfo& rhs) const
{
	if(gameName != rhs.gameName || gameID != rhs.gameID || gameState!=rhs.gameState)
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

