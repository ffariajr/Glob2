/*
  Copyright (C) 2007 Bradley Arsenault

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

#include "MultiplayerGameEvent.h"
#include <sstream>


MGPlayerListChangedEvent::MGPlayerListChangedEvent()
{
}



Uint8 MGPlayerListChangedEvent::getEventType() const
{
	return MGEPlayerListChanged;
}



std::string MGPlayerListChangedEvent::format() const
{
	std::ostringstream s;
	s<<"MGPlayerListChangedEvent()";
	return s.str();
}



bool MGPlayerListChangedEvent::operator==(const MultiplayerGameEvent& rhs) const
{
	if(typeid(rhs)==typeid(MGPlayerListChangedEvent))
	{
		//const MGPlayerListChangedEvent& r = dynamic_cast<const MGPlayerListChangedEvent&>(rhs);
		return true;
	}
	return false;
}


MGReadyToStartEvent::MGReadyToStartEvent()
{
}



Uint8 MGReadyToStartEvent::getEventType() const
{
	return MGEReadyToStart;
}



std::string MGReadyToStartEvent::format() const
{
	std::ostringstream s;
	s<<"MGReadyToStartEvent()";
	return s.str();
}



bool MGReadyToStartEvent::operator==(const MultiplayerGameEvent& rhs) const
{
	if(typeid(rhs)==typeid(MGReadyToStartEvent))
	{
		//const MGReadyToStartEvent& r = dynamic_cast<const MGReadyToStartEvent&>(rhs);
		return true;
	}
	return false;
}


MGNotReadyToStartEvent::MGNotReadyToStartEvent()
{
}



Uint8 MGNotReadyToStartEvent::getEventType() const
{
	return MGENotReadyToStart;
}



std::string MGNotReadyToStartEvent::format() const
{
	std::ostringstream s;
	s<<"MGNotReadyToStartEvent()";
	return s.str();
}



bool MGNotReadyToStartEvent::operator==(const MultiplayerGameEvent& rhs) const
{
	if(typeid(rhs)==typeid(MGNotReadyToStartEvent))
	{
		//const MGNotReadyToStartEvent& r = dynamic_cast<const MGNotReadyToStartEvent&>(rhs);
		return true;
	}
	return false;
}


MGPlayerLostEvent::MGPlayerLostEvent(Uint8 playerNum)
	: playerNum(playerNum)
{
}



Uint8 MGPlayerLostEvent::getEventType() const
{
	return MGEPlayerLost;
}



std::string MGPlayerLostEvent::format() const
{
	std::ostringstream s;
	s<<"MGPlayerLostEvent("<<"playerNum="<<playerNum<<"; "<<")";
	return s.str();
}



bool MGPlayerLostEvent::operator==(const MultiplayerGameEvent& rhs) const
{
	if(typeid(rhs)==typeid(MGPlayerLostEvent))
	{
		const MGPlayerLostEvent& r = dynamic_cast<const MGPlayerLostEvent&>(rhs);
		if(r.playerNum == playerNum)
			return true;
	}
	return false;
}


Uint8 MGPlayerLostEvent::getPlayerNum() const
{
	return playerNum;
}



//code_append_marker
