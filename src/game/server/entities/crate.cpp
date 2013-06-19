#include <new>
#include <engine/e_server_interface.h>
#include <engine/e_config.h>
#include <game/server/gamecontext.hpp>
#include <game/mapitems.hpp>

#include <game/server/gamemodes/nodes.hpp>
#include "crate.hpp"

#include <game/buildings_info.hpp>

//NODESTODO: START TO PORT THIS!!

//enum {CRATE_EXPLOSION = 0, CRATE_ILLNESS, CRATE_NOGRAV, CRATE_FREEZE, CRATE_STRIP, CRATE_HEALTH, CRATE_ARMOR, CRATE_AMMO, CRATE_WEAPONS };


CRATE::CRATE(int type, vec2 pos)
: ENTITY(NETOBJTYPE_CRATE)
{
	this->type = type;
	this->pos = pos;
	alive = true;
	proximity_radius = phys_size;
	creation_tick = server_tick();
	game.world.insert_entity(this);
	//dbg_msg("nodes", "new CRATE");
}

void CRATE::reset()
{
	destroy();
}

void CRATE::destroy()
{
	alive = false;
	ENTITY::destroy();
}

bool CRATE::is_grounded()
{
	if(col_check_point((int)(pos.x+phys_size/2), (int)(pos.y+phys_size/2+5)))
		return true;
	if(col_check_point((int)(pos.x-phys_size/2), (int)(pos.y+phys_size/2+5)))
		return true;
	return false;
}

CHARACTER *CRATE::get_nearest(int maxdist) {
	CHARACTER *close_characters[MAX_CLIENTS];
	int num = game.world.find_entities(pos, maxdist, (ENTITY**)close_characters, MAX_CLIENTS, NETOBJTYPE_CHARACTER);
	int disti = -1;
	float dist = 10000000.0f;
	for(int i = 0; i < num; i++) {
		if(col_intersect_line(pos, close_characters[i]->pos, 0x0, 0x0)) {
			continue;
		}
		float d = distance(close_characters[i]->pos, pos);
		if(d < dist) {
			disti = i;
			dist = d;
		}
	}
	if(disti == -1) {
		return 0;
	}
		
	return close_characters[disti];
}

void CRATE::tick()
{
	if(col_get((int)(pos.x+phys_size/2), (int)(pos.y-phys_size/2))&COLFLAG_DEATH ||
			col_get((int)(pos.x+phys_size/2), (int)(pos.y+phys_size/2))&COLFLAG_DEATH ||
			col_get((int)(pos.x-phys_size/2), (int)(pos.y-phys_size/2))&COLFLAG_DEATH ||
			col_get((int)(pos.x-phys_size/2), (int)(pos.y+phys_size/2))&COLFLAG_DEATH)
	{
		die();
		return;
	}
	
	grounded = is_grounded();
	
	bool was_grounded = grounded;
	
	if(!grounded) {
		//die();
		int k = 3;
		while(!grounded && k-- > 0) {
			pos.y += 1;
			grounded = is_grounded();
		}
	}
	
	if(grounded && !was_grounded)
		game.create_sound(pos, SOUND_CRATE_IMPACT);
	
	was_grounded = grounded;
	
	if(!alive)
		return;

	CHARACTER *c = get_nearest(28);
	GAMECONTROLLER_NODES* gc = (GAMECONTROLLER_NODES*)game.controller;
	
	if(c) {
		dbg_msg("nodes", "activated a crate of type %d", type);
		char buf[128];
		str_format(buf, sizeof(buf), "%s", BUF_NAMES[type]);
		game.send_broadcast(buf, c->player->client_id);
		
		switch(type) {
			//c->crate = type;
			//c->crate_tick = server_tick();
			
			case CRATE_HEALTH: {
				c->health = 10;
				game.create_sound(pos, SOUND_PICKUP_HEALTH);
			} break;
			
			case CRATE_ARMOR: {
				c->armor = 10;
				game.create_sound(pos, SOUND_PICKUP_ARMOR);
			} break;
			
			case CRATE_CONSTRUCTION: {
				
				
				c->crate = CRATE_CONSTRUCTION;
				c->crate_tick = server_tick();
				c->core.frozen = false;
				/*if(gc->buildings_buffpoints[1-c->team] > 0) {
					gc->buildings_points[1-c->team] -= gc->buildings_buffpoints[1-c->team];
					gc->buildings_buffpoints[1-c->team] = 0;
				}
				
				if(gc->buildings_buffpoints[c->team] == 0) {
					gc->buildings_points[c->team] += config.sv_buffpoints;
					gc->buildings_buffpoints[c->team] = config.sv_buffpoints;
				}*/
			} break;
			
			case CRATE_BASEBALL: {
				c->crate = CRATE_BASEBALL;
				c->crate_tick = server_tick();
				c->core.frozen = false;
				c->active_weapon = WEAPON_HAMMER;
			} break;
			
			case CRATE_FREEZE: {
				c->crate = 0; //undo crate thingy, only enemies will get this "buff"
				c->core.frozen = false;
				
				CHARACTER *close_characters[MAX_CLIENTS];
				int num = game.world.find_entities(c->pos, config.sv_freeze_radius, (ENTITY**)close_characters, MAX_CLIENTS, NETOBJTYPE_CHARACTER);
				
				for(int i = 0; i < num; i++) {
					if(close_characters[i]->player->team != c->player->team) {
						close_characters[i]->core.frozen = true;
						close_characters[i]->crate = CRATE_FREEZE;
						close_characters[i]->crate_tick = server_tick();
						close_characters[i]->crate_misc = (int)(frandom() *2);
						game.create_sound(close_characters[i]->pos, SOUND_CRATE_ICE);
					}
				}
				game.create_sound(pos, SOUND_CRATE_ICE);
				game.create_effect(pos, 3);
			} break;
			
			case CRATE_EMP: {
				gc->emp = true;
				gc->emp_tick = server_tick();
				game.create_effect(pos, 2);
				game.create_sound_global(SOUND_CRATE_EMP, -1);
			} break;
			
			case CRATE_DUCK: {
				c->core.frozen = true;
				//c->ill = true;
				c->crate = CRATE_FREEZE;
				c->crate_tick = server_tick();
				c->crate_misc = (int)(frandom() *2);
				c->core.hook_state = -1;
				c->core.hooked_player = -1;
				game.create_sound(pos, SOUND_CRATE_ICE);
			} break;
			
			case CRATE_BOOM: {
				game.create_explosion(pos, -4, WEAPON_WORLD, false);
				game.create_explosion(pos, -4, WEAPON_WORLD, false);
				game.create_explosion(pos, -4, WEAPON_WORLD, false);
				game.create_sound(pos, SOUND_GRENADE_EXPLODE);
			}
			
			/*case CRATE_EXPLOSION: {
				game.create_explosion(pos, -1, WEAPON_WORLD, false);
				game.create_explosion(pos, -1, WEAPON_WORLD, false);
				game.create_explosion(pos, -1, WEAPON_WORLD, false);
			} break;
			case CRATE_ILLNESS: {
				c->ill = true;
				c->infecter = -1;
			} break;
			case CRATE_NOGRAV: {
				c->core.nograv = true;
			} break;
			case CRATE_FREEZE: {
				c->core.frozen = true;
			} break;
			case CRATE_STRIP: {
				c->weapons[WEAPON_GUN].got = 0;
				c->weapons[WEAPON_GRENADE].got = 0;
				c->weapons[WEAPON_RIFLE].got = 0;
			}
			
			case CRATE_HEALTH: {
				c->health = 10;
			} break;
			case CRATE_ARMOR: {
				c->armor = 10;
			}
			case CRATE_AMMO: {
				c->weapons[WEAPON_GUN].ammo = 10;
				c->weapons[WEAPON_GRENADE].ammo = 10;
				c->weapons[WEAPON_RIFLE].ammo = 10;
			}
			case CRATE_WEAPONS: {
				c->weapons[WEAPON_GUN].got = 1;
				c->weapons[WEAPON_GRENADE].got = 1;
				c->weapons[WEAPON_RIFLE].got = 1;
			}*/
			
		}
		
		die();
		return;
	}

	if(server_tick() > creation_tick + 20*50) {
		game.create_explosion(pos, -1, WEAPON_WORLD, true);
		game.create_sound(pos, SOUND_GRENADE_EXPLODE);
		die();
	}
	
	return;
}

void CRATE::die()
{
	alive = false;
	game.world.destroy_entity(this);	
}

void CRATE::snap(int snapping_client)
{
	if(networkclipped(snapping_client))
		return;
	
	NETOBJ_CRATE *flag = (NETOBJ_CRATE *)snap_new_item(NETOBJTYPE_CRATE, id, sizeof(NETOBJ_CRATE));
	flag->x = (int)pos.x;
	flag->y = (int)pos.y;
	flag->type = type;
	flag->grounded = grounded;
}
