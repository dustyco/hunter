

#pragma once
#include "common.h"
using namespace std;


// Null id is 0
// Null name is ""
struct PlayerInfo
{
	// Persistent
	PlayerID      id;
	string        name;
	
	// Temporary
	bool          online;
	PilotControls pilot_controls;
	
	PlayerInfo () : id(0), online(false) {}
	PlayerInfo (const PlayerID& id_, const string& name_) : id(id_), name(name_), online(false) {}
};

// Global - reference using getSingleton()
struct PlayerDB : private NonCopyable
{
	struct Internal;
	
	Internal* internal;
	
	                 PlayerDB     ();
	                ~PlayerDB     ();
	static PlayerDB& getSingleton ();
	
	PlayerID    issueID ();
	PlayerInfo& add     (const PlayerInfo& player);  // Does not check if it exists or is valid
	bool        has     (const PlayerID& id);
	bool        has     (const string& name);
	void        remove  (const PlayerID& id);
	void        remove  (const string& name);
	PlayerInfo& get     (const PlayerID& id);
	PlayerInfo& get     (const string& name);
	bool        load    ();
	bool        save    ();
	bool        load    (const string& path);
	bool        save    (const string& path);
	void        clear   ();
};
