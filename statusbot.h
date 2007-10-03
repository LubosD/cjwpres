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

#include <gloox/client.h>
#include <gloox/messagehandler.h>
#include <gloox/presencehandler.h>
#include <gloox/rostermanager.h>
#include <gloox/connectionlistener.h>
#include <string>
#include <map>
#include <list>

#include "defaultrosterlistener.h"
#include "data.h"

class StatusBot : public gloox::MessageHandler, public gloox::ConnectionListener, public DefaultRosterListener
{
public:
	StatusBot();
	~StatusBot();
	
	void start();
	void stop();
	
	void addJID(std::string jid);
	void getPresence(std::string jid, std::list<PresenceInfo>& out);
	
	virtual void handleMessage(gloox::Stanza* stanza, gloox::MessageSession* session = 0);
	virtual void handleRosterPresence(const gloox::RosterItem & item, const std::string& resource, gloox::Presence presence, const std::string& msg);
	
	virtual void onConnect();
	virtual void onDisconnect (gloox::ConnectionError e);
	virtual void onResourceBindError(gloox::ResourceBindError error);
	virtual void onSessionCreateError(gloox::SessionCreateError error);
	virtual bool onTLSConnect(const gloox::CertInfo &info) { return true; }
	virtual void onStreamEvent(gloox::StreamEvent event) {}
private:
	gloox::Client* m_pClient;
	gloox::RosterManager* m_pRoster;
	
	std::multimap<std::string,PresenceInfo> m_presence;
	pthread_mutex_t m_presenceMut;
};

