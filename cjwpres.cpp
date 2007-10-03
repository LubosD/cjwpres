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

#include <iostream>
#include <pthread.h>
#include <stdexcept>
#include <errno.h>
#include "statusbot.h"
#include "udpserver.h"

using namespace std;

StatusBot* g_bot = 0;

void* statusBot(void*);
void handleError(const exception& e);

int main()
{
	pthread_t id;
	
	try
	{
		pthread_create(&id, 0, statusBot, 0);
		runServer();
	}
	catch(const exception& e)
	{
		g_bot->stop();
		
		handleError(e);
	}
	
	return 1;
}

void* statusBot(void*)
{
	try
	{
		g_bot = new StatusBot;
		g_bot->start();
	}
	catch(const exception& e)
	{
		delete g_bot;
		g_bot = 0;
		
		handleError(e);
	}
}

void handleError(const exception& e)
{
	cerr << "Error: " << e.what() << endl;
	
	if(errno != 0)
		cerr << "Errno value: " << strerror(errno) << endl;
	
	exit(1);
}
