/*
  Copyright (C) 2008 Bradley Arsenault

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

#include "BinaryStream.h"
#include "FileManager.h"
#include "NetMessage.h"
#include "Stream.h"
#include "Toolkit.h"
#include "YOGServer.h"
#include "YOGServerMapDatabank.h"

using namespace GAGCore;

YOGServerMapDatabank::YOGServerMapDatabank(YOGServer* server)
	: server(server)
{
	load();
}



void YOGServerMapDatabank::addMap(const YOGDownloadableMapInfo& map)
{
	int fileID = server->getFileDistributionManager().allocateFileDistributor();
	server->getFileDistributionManager().getDistributor(fileID)->loadFromLocally(map.getMapHeader().getFileName());
	YOGDownloadableMapInfo nmap(map);
	nmap.setFileID(fileID);
	maps.push_back(nmap);
	save();
}



YOGMapUploadRefusalReason YOGServerMapDatabank::canRecieveFromPlayer(const YOGDownloadableMapInfo& map)
{
	for(std::vector<boost::tuple<YOGDownloadableMapInfo, int> >::iterator i = uploadingMaps.begin(); i!=uploadingMaps.end(); ++i)
	{
		if(i->get<0>().getMapHeader().getMapName() == map.getMapHeader().getMapName())
			return YOGMapUploadReasonMapNameAlreadyExists;
	}
	for(std::vector<YOGDownloadableMapInfo>::iterator i = maps.begin(); i!=maps.end(); ++i)
	{
		if(i->getMapHeader().getMapName() == map.getMapHeader().getMapName())
			return YOGMapUploadReasonMapNameAlreadyExists;
	}
	return YOGMapUploadReasonUnknown;
}



Uint16 YOGServerMapDatabank::recieveMapFromPlayer(const YOGDownloadableMapInfo& map, boost::shared_ptr<YOGServerPlayer> player)
{
	int fileID = server->getFileDistributionManager().allocateFileDistributor();
	server->getFileDistributionManager().getDistributor(fileID)->loadFromPlayer(player);
	uploadingMaps.push_back(boost::make_tuple(map, fileID));
	return fileID;
}



void YOGServerMapDatabank::sendMapListToPlayer(boost::shared_ptr<YOGServerPlayer> player)
{
	boost::shared_ptr<NetDownloadableMapInfos> infos(new NetDownloadableMapInfos(maps));
	player->sendMessage(infos);
}



void YOGServerMapDatabank::update()
{
	for(std::vector<boost::tuple<YOGDownloadableMapInfo, int> >::iterator i=uploadingMaps.begin(); i!=uploadingMaps.end();)
	{
		if(server->getFileDistributionManager().getDistributor(i->get<1>())->areAllChunksLoaded())
		{
			server->getFileDistributionManager().getDistributor(i->get<1>())->saveToFile(i->get<0>().getMapHeader().getFileName());
			server->getFileDistributionManager().removeDistributor(i->get<1>());
			addMap(i->get<0>());
			std::cout<<"map: "<<i->get<0>().getMapHeader().getMapName()<<std::endl;
			Uint32 n = i - uploadingMaps.begin();
			uploadingMaps.erase(i);
			i = uploadingMaps.begin() + n;
		}
		else if(server->getFileDistributionManager().getDistributor(i->get<1>())->wasUploadingCanceled())
		{
			server->getFileDistributionManager().removeDistributor(i->get<1>());
			Uint32 n = i - uploadingMaps.begin();
			uploadingMaps.erase(i);
			i = uploadingMaps.begin() + n;
		}
		else
		{
			++i;
		}
	}
}



void YOGServerMapDatabank::load()
{
	InputStream* stream = new BinaryInputStream(Toolkit::getFileManager()->openInputStreamBackend("mapdatabank"));
	if(!stream->isEndOfStream())
	{
		Uint32 versionMinor = stream->readUint32("version");
		stream->readEnterSection("maps");
		Uint32 size = stream->readUint32("size");
		for(unsigned i=0; i<size; ++i)
		{
			stream->readEnterSection(i);
			YOGDownloadableMapInfo info;
			info.decodeData(stream, versionMinor);
			int fileID = server->getFileDistributionManager().allocateFileDistributor();
			server->getFileDistributionManager().getDistributor(fileID)->loadFromLocally(info.getMapHeader().getFileName());		
			info.setFileID(fileID);
			maps.push_back(info);
			stream->readLeaveSection();
		}
		stream->readLeaveSection();
	}
	delete stream;
}


void YOGServerMapDatabank::save()
{
	OutputStream* stream = new BinaryOutputStream(Toolkit::getFileManager()->openOutputStreamBackend("mapdatabank"));
	stream->writeUint32(VERSION_MINOR, "version");
	stream->writeEnterSection("maps");
	stream->writeUint32(maps.size(), "size");
	for(int i=0; i<maps.size(); ++i)
	{
		stream->writeEnterSection(i);
		maps[i].encodeData(stream);
		stream->writeLeaveSection();
	}
	stream->writeLeaveSection();
	delete stream;
}

