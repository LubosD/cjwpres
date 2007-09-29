/***************************************************************************
 *   Copyright (C) 2007 by Luboš Doležel                                   *
 *   http://www.dolezel.info                                               *
 *   lubos at dolezel info                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include "statusbot.h"
#include "config.h"
#include "data.h"

using namespace std;

extern StatusBot* g_bot;
string getStatus(const char* jid);
string escapeString(string in);

void runServer()
{
	int fd;
	sockaddr_in addr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		throw runtime_error("socket() failed");
	
	memset(&addr, 0, sizeof addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(BOT_PORT);
	addr.sin_addr.s_addr = htonl(0x7f000001);
	
	if(bind(fd, (sockaddr*) &addr, sizeof addr) < 0)
		throw runtime_error("bind() failed");
	
	while(true)
	{
		ssize_t bytes;
		char jid[512];
		sockaddr_in addr2;
		socklen_t len;
		string response;

		bytes = recvfrom(fd, jid, sizeof(jid)-1, 0, (sockaddr*) &addr2, &len);
		
		if(!bytes)
			continue;

		jid[bytes] = 0;
		response = getStatus(jid);

		sendto(fd, response.c_str(), response.size(), 0, (sockaddr*) &addr2, sizeof addr2);
	}
}

string getStatus(const char* jid)
{
	list<PresenceInfo> pres;

	cout << "Getting status for: " << jid << endl;

	for(multimap<string,PresenceInfo>::const_iterator it = g_presence.begin();
		it != g_presence.end();
		it++)
	{
		if(it->first == jid)
			pres.push_back(it->second);
	}

	if(pres.empty())
	{
		g_bot->addJID(jid);
		return string();
	}
	else
	{
		string result;
		
		pres.sort();
		
		for(list<PresenceInfo>::const_iterator it = pres.begin();
			it != pres.end();
			it++)
		{
			char* line = new char[it->message.size()+1024];
			string message = escapeString(it->message);
			sprintf(line, "%s\t%d\t%s\t%s\n",
				it->resource.c_str(),
				it->priority,
				it->status.c_str(),
				message.c_str());
	
			result += line;
			delete [] line;
		}
		
		cout << "Result: " << result;
	
		return result;
	}
}

string escapeString(string in)
{
	size_t pos;
	while((pos = in.find('\n')) != string::npos)
		in.replace(pos, 1, "\\n");
	while((pos = in.find('\t')) != string::npos)
		in.replace(pos, 1, "\\t");
	return in;
}
