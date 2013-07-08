

#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/foreach.hpp>
#include "PlayerDB.h"


typedef map<PlayerID,PlayerInfo*> IDTree;
typedef map<string,PlayerInfo*> StringTree;

struct PlayerDB::Internal
{
	IDTree     by_id;
	StringTree by_name;
	PlayerInfo blank;
	PlayerID   next_id;
};


PlayerDB::PlayerDB ()
{
	internal = new Internal;
	internal->next_id = 1;
}

PlayerDB::~PlayerDB ()
{
	clear();
	delete internal;
}

PlayerDB& PlayerDB::getSingleton ()
{
	// Guaranteed to be destroyed.
	// Instantiated on first use.
	static PlayerDB instance;
	return instance;
}

PlayerID PlayerDB::issueID ()
{
	return internal->next_id++;
}

PlayerInfo& PlayerDB::add (const PlayerInfo& player)
{
	PlayerInfo* ptr = new PlayerInfo(player);
	internal->by_id[player.id] = ptr;
	internal->by_name[player.name] = ptr;
	return *ptr;
}

bool PlayerDB::has (const PlayerID& id)
{
	IDTree::iterator ptr = internal->by_id.find(id);
	return (ptr!=internal->by_id.end());
}

bool PlayerDB::has (const string& name)
{
	StringTree::iterator ptr = internal->by_name.find(name);
	return (ptr!=internal->by_name.end());
}

void PlayerDB::remove (const PlayerID& id)
{
	IDTree::iterator ptr = internal->by_id.find(id);
	if (ptr!=internal->by_id.end()) {
		internal->by_name.erase(ptr->second->name);
		delete ptr->second;
	} else return;
}

void PlayerDB::remove (const string& name)
{
	StringTree::iterator ptr = internal->by_name.find(name);
	if (ptr!=internal->by_name.end()) {
		internal->by_id.erase(ptr->second->id);
		delete ptr->second;
	} else return;
}
	
PlayerInfo& PlayerDB::get (const PlayerID& id)
{
	IDTree::iterator ptr = internal->by_id.find(id);
	if (ptr!=internal->by_id.end()) return *(ptr->second);
	else return internal->blank;
}

PlayerInfo& PlayerDB::get (const string& name)
{
	StringTree::iterator ptr = internal->by_name.find(name);
	if (ptr!=internal->by_name.end()) return *(ptr->second);
	else return internal->blank;
}

bool PlayerDB::load ()
{
	return false;
}

bool PlayerDB::save ()
{
	return false;
}

bool PlayerDB::load (const string& path)
{
	clear();
	using namespace boost::property_tree;
	try {
		ptree pt;
		info_parser::read_info(path, pt);
		ptree pt_db = pt.get_child("PlayerDB");
		
		// Check this first before adding any player internal
		internal->next_id = pt_db.get<PlayerID>("next_id");
		
		// Add PlayerInfo entries
		BOOST_FOREACH(ptree::value_type& v, pt_db) {
			if (v.first.compare("PlayerInfo")==0) {
				ptree& pt_info = v.second;
				PlayerInfo info(pt_info.get<PlayerID>("id", 0), pt_info.get<string>("name", ""));
				// TODO Get any additional properties (remember to use default values to avoid throwing ptree_error
				if (info.id!=0 && !info.name.empty() && !has(info.id) && !has(info.name)) add(info);
			}
		}
	} catch (ptree_error& e) {
		cout << "Warning: Failed to load PlayerDB: " << e.what() << endl;
		clear();
		return false;
	}
	
	return true;
}

bool PlayerDB::save (const string& path)
{
	using namespace boost::property_tree;
	try {
		ptree pt_db;
		BOOST_FOREACH(IDTree::value_type& v, internal->by_id) {
			PlayerInfo& info = *v.second;
			ptree pt_info;
			pt_info.put("id", info.id);
			pt_info.put("name", info.name);
			pt_db.add_child("PlayerInfo", pt_info);
		}
		pt_db.put("next_id", internal->next_id);
		
		ptree pt;
		// Read the file if it exists and replace PlayerDB
		try { info_parser::read_info(path, pt); }
		catch (ptree_error& e) { pt.clear(); }
		pt.put_child("PlayerDB", pt_db);
		info_parser::write_info(path, pt);
	} catch (ptree_error& e) {
		cout << "Warning: Failed to save PlayerDB: " << e.what() << endl;
		return false;
	}
	
	return false;
}

void PlayerDB::clear ()
{
	for (IDTree::iterator it=internal->by_id.begin(); it!=internal->by_id.end(); ++it)
		delete it->second;
	internal->by_id.clear();
	internal->by_name.clear();
	internal->next_id = 1;
}


