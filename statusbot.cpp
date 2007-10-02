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

#include "statusbot.h"
#include "config.h"
#include "data.h"
#include <iostream>
#include <stdexcept>

using namespace std;

std::multimap<std::string,PresenceInfo> g_presence;
pthread_mutex_t g_presenceMut;

extern StatusBot* g_bot;

StatusBot::StatusBot()
: m_pClient(0), m_pRoster(0)
{
	JID jid(BOT_JID);
	
	g_bot = this;

	m_pClient = new Client(jid, BOT_PASSWORD);
	m_pClient->registerMessageHandler(this);
	m_pClient->registerPresenceHandler(this);
	m_pClient->setPresence(PresenceAvailable, 5);

	if(!m_pClient->connect())
		throw runtime_error("Connection error");
}

StatusBot::~StatusBot()
{
	delete m_pClient;
}

void StatusBot::addJID(string jid)
{
	if(!m_pRoster)
		m_pRoster = m_pClient->rosterManager();
	
	Roster* roster = m_pRoster->roster();
	if(roster->find(jid) != roster->end())
		return;
	
	cout << "Adding new JID: " << jid << endl;
	
	m_pRoster->subscribe(JID(jid));
	m_pRoster->synchronize();
}

void StatusBot::handleMessage(Stanza* stanza, MessageSession* session)
{
	Stanza *s = Stanza::createMessageStanza(stanza->from().full(),
			"I'm just a stupid presence bot\n"
			"Copyright 2007 Lubos Dolezel\n"
			"http://www.dolezel.info");
	
	m_pClient->send(s);
}

void StatusBot::handlePresence(Stanza* stanza)
{
	string user = stanza->from().bare();
	PresenceInfo in;
	bool removing = false;

	in.resource = stanza->from().resource();
	in.priority = stanza->priority();
	in.message = stanza->status();

	switch(stanza->presence())
	{
	case PresenceAvailable:
		in.status = "available"; break;
	case PresenceChat:
		in.status = "chat"; break;
	case PresenceAway:
		in.status = "away"; break;
	case PresenceDnd:
		in.status = "dnd"; break;
	case PresenceXa:
		in.status = "xa"; break;
	case PresenceUnavailable:
		removing = true;
		in.status = "unavailable"; break;
	default:
		removing = true;
		in.status = "unknown";
	}
	
	cout << "New presence - " << user << " is " << in.status << " (" << in.message << ")\n";

	bool found = false;

	for(multimap<string,PresenceInfo>::iterator it = g_presence.begin();
		it != g_presence.end();
		it++)
	{
		if(it->first == user && it->second.resource == in.resource)
		{
			pthread_mutex_lock(&g_presenceMut);
			if(!removing)
				it->second = in;
			else
				g_presence.erase(it);
			pthread_mutex_unlock(&g_presenceMut);
			
			found = true;
			break;
		}
	}

	if(!found && !removing)
	{
		pthread_mutex_lock(&g_presenceMut);
		g_presence.insert(pair<string,PresenceInfo>(user, in));
		pthread_mutex_unlock(&g_presenceMut);
	}
}

