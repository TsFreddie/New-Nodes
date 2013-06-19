#ifndef GAME_SERVER_ENTITY_CRATE_H
#define GAME_SERVER_ENTITY_CRATE_H

#include <game/server/entity.hpp>
#include <game/generated/gs_data.hpp>
#include <game/generated/g_protocol.hpp>

#include <game/gamecore.hpp>
#include <game/crate_info.hpp>

//NODESTODO: START TO PORT THIS!!

static const char* BUF_NAMES[9] = {"", "Health", "Armor", "Construction", "Baseball", "Freeze", "EMP", "Duck", "Boom"};

class CRATE : public ENTITY
{
private:

public:
	int type;
	bool alive;

	int creation_tick;
	bool grounded;
	
	CRATE(int type, vec2 pos);
	
	virtual void reset();
	virtual void destroy();
	bool is_grounded();

	CHARACTER *get_nearest(int maxdist);
	
	void die();
	bool remove();

	static const int phys_size = 28;

	virtual void tick();
	virtual void snap(int snapping_client);
};

#endif
