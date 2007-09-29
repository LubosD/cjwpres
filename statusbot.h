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
#include <string>
#include <map>

using namespace gloox;
using namespace std;

class StatusBot : public MessageHandler, public PresenceHandler
{
public:
	StatusBot();
	~StatusBot();
	virtual void handleMessage(Stanza* stanza, MessageSession* session = 0);
	virtual void handlePresence(Stanza* stanza);
	void addJID(string jid);
private:
	Client* m_pClient;
};

