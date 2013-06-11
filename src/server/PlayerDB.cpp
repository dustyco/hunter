

#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/foreach.hpp>
#include "PlayerDB.h"


typedef map<PlayerID,PlayerInfo*> IDTree;
typedef map<string,PlayerInfo*> StringTree;


struct PlayerDB::Data
{
	IDTree     by_id;
	StringTree by_name;
	PlayerInfo blank;
	PlayerID   next_id;
};


PlayerDB::PlayerDB ()
{
	data = new Data;
	data->next_id = 1;
}

PlayerDB::~PlayerDB ()
{
	clear();
	delete data;
}

PlayerID PlayerDB::issueID ()
{
	return data->next_id++;
}

PlayerInfo& PlayerDB::add (const PlayerInfo& player)
{
	PlayerInfo* ptr = new PlayerInfo(player);
	data->by_id[player.id] = ptr;
	data->by_name[player.name] = ptr;
	return *ptr;
}

bool PlayerDB::has (const PlayerID& id)
{
	IDTree::iterator ptr = data->by_id.find(id);
	return (ptr!=data->by_id.end());
}

bool PlayerDB::has (const string& name)
{
	StringTree::iterator ptr = data->by_name.find(name);
	return (ptr!=data->by_name.end());
}

void PlayerDB::remove (const PlayerID& id)
{
	IDTree::iterator ptr = data->by_id.find(id);
	if (ptr!=data->by_id.end()) {
		data->by_name.erase(ptr->second->name);
		delete ptr->second;
	} else return;
}

void PlayerDB::remove (const string& name)
{
	StringTree::iterator ptr = data->by_name.find(name);
	if (ptr!=data->by_name.end()) {
		data->by_id.erase(ptr->second->id);
		delete ptr->second;
	} else return;
}
	
PlayerInfo& PlayerDB::get (const PlayerID& id)
{
	IDTree::iterator ptr = data->by_id.find(id);
	if (ptr!=data->by_id.end()) return *(ptr->second);
	else return data->blank;
}

PlayerInfo& PlayerDB::get (const string& name)
{
	StringTree::iterator ptr = data->by_name.find(name);
	if (ptr!=data->by_name.end()) return *(ptr->second);
	else return data->blank;
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
		
		// Check this first before adding any player data
		data->next_id = pt_db.get<PlayerID>("next_id");
		
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
		BOOST_FOREACH(IDTree::value_type& v, data->by_id) {
			PlayerInfo& info = *v.second;
			ptree pt_info;
			pt_info.put("id", info.id);
			pt_info.put("name", info.name);
			pt_db.add_child("PlayerInfo", pt_info);
		}
		pt_db.put("next_id", data->next_id);
		
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
	for (IDTree::iterator it=data->by_id.begin(); it!=data->by_id.end(); ++it)
		delete it->second;
	data->by_id.clear();
	data->by_name.clear();
	data->next_id = 1;
}


