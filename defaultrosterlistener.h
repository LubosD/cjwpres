#ifndef DEFAULTROSTERLISTENER_H
#define DEFAULTROSTERLISTENER_H
#include <gloox/rosterlistener.h>

// Because the RosterListener's virtual methods are pure, we're forced to implement them all.
//
// But that would put a lot of unnecessary crap into our real handler(s), where only a few
// events need to be handled. Therefore I've created a default listener that just ignores all events.

class DefaultRosterListener : public gloox::RosterListener
{
public:
	virtual void handleNonrosterPresence (gloox::Stanza *stanza) {}
	virtual void handleRosterError (gloox::Stanza *stanza) {}
	virtual void handleItemAdded (const gloox::JID &jid) {}
	virtual void handleItemSubscribed (const gloox::JID &jid) {}
	virtual void handleItemRemoved (const gloox::JID &jid) {}
	virtual void handleItemUpdated (const gloox::JID &jid) {}
	virtual void handleItemUnsubscribed (const gloox::JID &jid) {}
	virtual void handleRoster (const gloox::Roster &roster) {}
	virtual bool handleSubscriptionRequest(const gloox::JID& jid, const std::string& msg) { return true; }
	virtual bool handleUnsubscriptionRequest(const gloox::JID& jid, const std::string& msg) { return true; }
	virtual void handleRosterPresence(const gloox::RosterItem & item, const std::string& resource, gloox::Presence presence, const std::string& msg) {}
	virtual void handleSelfPresence(const gloox::RosterItem & item, const std::string& resource, gloox::Presence presence, const std::string& msg) {}
};

#endif
