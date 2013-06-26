

#pragma once
#include "common.h"
using namespace std;


// Null id is 0
// Null name is ""
struct PlayerInfo
{
	PlayerID id;
	string   name;
	
	PlayerInfo () : id(0) {}
	PlayerInfo (const PlayerID& id_, const string& name_) : id(id_), name(name_) {}
};
	
struct PlayerDB : private NonCopyable
{
	struct Data;
	Data* data;
	
	            PlayerDB ();
	           ~PlayerDB ();
	PlayerID    issueID  ();
	PlayerInfo& add      (const PlayerInfo& player);  // Does not check if it exists or is valid
	bool        has      (const PlayerID& id);
	bool        has      (const string& name);
	void        remove   (const PlayerID& id);
	void        remove   (const string& name);
	PlayerInfo& get      (const PlayerID& id);
	PlayerInfo& get      (const string& name);
	bool        load     ();
	bool        save     ();
	bool        load     (const string& path);
	bool        save     (const string& path);
	void        clear    ();
};
