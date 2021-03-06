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
using namespace gloox;

StatusBot::StatusBot()
: m_pClient(0), m_pRoster(0)
{
	pthread_mutex_init(&m_presenceMut, 0);
}

StatusBot::~StatusBot()
{
	pthread_mutex_destroy(&m_presenceMut);
	delete m_pClient;
}

void StatusBot::start()
{
	JID jid(BOT_JID);
	
	m_pClient = new Client(jid, BOT_PASSWORD);
	m_pClient->registerMessageHandler(this);
	m_pClient->registerConnectionListener(this);
	m_pClient->setPresence(PresenceAvailable, 5);

	m_pRoster = m_pClient->rosterManager();
	m_pRoster->registerRosterListener(this);
	
	m_pClient->connect(); // this function returns only in case of failure
	
	delete m_pClient;
	m_pClient = 0;
	
	throw runtime_error("Connect failed");
}

void StatusBot::stop()
{
	m_pClient->disconnect();
}

void StatusBot::addJID(string jid)
{	
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

void StatusBot::getPresence(std::string jid, std::list<PresenceInfo>& out)
{
	out.clear();
	pthread_mutex_lock(&m_presenceMut);
	
	for(multimap<string,PresenceInfo>::const_iterator it = m_presence.begin();
		it != m_presence.end();
		it++)
	{
		if(it->first == jid)
			out.push_back(it->second);
	}
	
	pthread_mutex_unlock(&m_presenceMut);
}

void StatusBot::handleRosterPresence(const RosterItem& item, const std::string& resource, Presence presence, const std::string& msg)
{
	string user = item.jid();
	PresenceInfo in;
	bool removing = false;
	
	if(const Resource* res = item.resource(resource))
		in.priority = res->priority();
	else
		in.priority = 0;
	
	in.resource = resource;
	in.message = msg;

	switch(presence)
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

	for(multimap<string,PresenceInfo>::iterator it = m_presence.begin();
		it != m_presence.end();
		it++)
	{
		if(it->first == user && it->second.resource == in.resource)
		{
			pthread_mutex_lock(&m_presenceMut);
			if(!removing)
				it->second = in;
			else
				m_presence.erase(it);
			pthread_mutex_unlock(&m_presenceMut);
			
			found = true;
			break;
		}
	}

	if(!found && !removing)
	{
		pthread_mutex_lock(&m_presenceMut);
		m_presence.insert(pair<string,PresenceInfo>(user, in));
		pthread_mutex_unlock(&m_presenceMut);
	}
}

void StatusBot::onConnect()
{
	cout << "Successfully connected to the Jabber server\n";
}

void StatusBot::onDisconnect(ConnectionError e)
{
	if(e != ConnNoError)
	{
		if(e == ConnAuthenticationFailed)
			throw runtime_error("ConnAuthenticationFailed");
		else // to lazy to write other descriptions
			throw runtime_error("Connection error");
	}
}

void StatusBot::onResourceBindError(ResourceBindError error)
{
	throw runtime_error("ResourceBindError");
}

void StatusBot::onSessionCreateError(SessionCreateError error)
{
	throw runtime_error("SessionCreateError");
}
