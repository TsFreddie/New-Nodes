#include <new>
#include <engine/shared/config.h>
#include <game/server/gamecontext.h>
#include <game/mapitems.h>

#include <game/server/gamemodes/nodes.hpp>

#include "building.h"
#include "laser.h"
#include "projectile.h"

#include <game/buildings_info.h>

CBuilding::CBuilding(CGameWorld *pWorld, int Type)
: CEntity(pWorld, CGameWorld::ENTTYPE_BUILDING)
{
	m_Pos = vec2(0,0);
	m_Angle = 0;
	m_Type = Type;
	
	m_Width = buildings_info[type].width;
	m_Height = buildings_info[type].height;
	
	m_Alive = false;
	m_Health = 1;
	m_Maxhealth = buildings_info[type].maxhealth;
	m_ProximityRadius = ms_PhysSize;
}

void CBuilding::Reset()
{
	Destroy();
}

bool CBuilding::Spawn(vec2 Pos, int Team)
{
	m_Alive = true;

	return true;
}

void CBuilding::Destroy()
{
	m_Alive = false;
	
	//ENTITY::destroy();
}

bool CBuilding::IsGrounded()
{
	if(GameServer()->Collision()->CheckPoint(m_Pos.x+m_ProximityRadius/2, m_Pos.y+m_ProximityRadius/2+5))
		return true;
	if(GameServer()->Collision()->CheckPoint(m_Pos.x-m_ProximityRadius/2, m_Pos.y+m_ProximityRadius/2+5))
		return true;
	return false;
}

CCharacter *CBuilding::GetNearest(int Maxdist, int t) {
	CCharacter *CloseCharacters[MAX_CLIENTS];
	int Num = GameServer()->m_World.FindEntities(m_Pos, Maxdist, (CEntity**)CloseCharacters, MAX_CLIENTS, CGameWorld::ENTTYPE_BUILDING);
	int Disti = -1;
	float Dist = 10000000.0f;
	for(int i = 0; i < num; i++) {
		if(CloseCharacters[i]->GetPlayer()->m_Team != t || GameServer()->Collision()->IntersectLine(m_Pos, CloseCharacters[i]->m_Pos, NULL, NULL)) {
			continue;
		}
		float d = distance(CloseCharacters[i]->m_Pos, m_Pos);
		if(d < Dist) {
			Disti = i;
			Dist = d;
		}
	}
	if(Disti == -1) {
		return 0;
	}
		
	return CloseCharacters[Disti];
}

bool CBuilding::Powered() {
	if(m_Type == B_SPAWN)
		return true;
	bool reactor = false;
	bool power = false;
	for(int i = 0; i < ((CGameControllerNODES*)GameServer()->m_pController)->BuildingsCount[m_Team]; i++) {
		CBuilding *b = ((CGameControllerNODES*)GameServer()->m_pController)->Buildings[m_Team][i];
		if(b->m_Type == B_REACTOR && b->m_Alive) {
			reactor = true;
		}
		if(((b->m_Type == B_REACTOR && distance(b->m_Pos, m_Pos) < 750.0f) || (b->m_Type == B_REPEATER && distance(b->m_Pos, m_Pos) < 500.0f)) && b->m_Alive) {
			power = true;
		}
	}
	return power && reactor && !((CGameControllerNODES*)GameServer()->m_pController)->m_Emp;
}

void CBuilding::Tick()
{
	// handle death-tiles and leaving gamelayer
	if(GameServer()->Collision()->GetCollisionAt(m_Pos.x+m_ProximityRadius/2.f, m_Pos.y-m_ProximityRadius/2.f)&CCollision::COLFLAG_DEATH ||
		GameServer()->Collision()->GetCollisionAt(m_Pos.x+m_ProximityRadius/2.f, m_Pos.y+m_ProximityRadius/2.f)&CCollision::COLFLAG_DEATH ||
		GameServer()->Collision()->GetCollisionAt(m_Pos.x-m_ProximityRadius/2.f, m_Pos.y-m_ProximityRadius/2.f)&CCollision::COLFLAG_DEATH ||
		GameServer()->Collision()->GetCollisionAt(m_Pos.x-m_ProximityRadius/2.f, m_Pos.y+m_ProximityRadius/2.f)&CCollision::COLFLAG_DEATH ||
		GameLayerClipped(m_Pos))
	{
		Die(-1, WEAPON_WORLD);
		return;
	}
	
	if(!IsGrounded()) {
		Die(-1, WEAPON_WORLD);
		return;
	}
	
	
	
	if(Server()->Tick() % 5 == 0 && m_Decon) {
		if( ((CGameControllerNODES*)GameServer()->m_pController)->Spawns[m_Team] <= 0)
		{
			m_Decon = false;
		} else {
			TakeDamage(vec2(0,0), 1, -1, WEAPON_SELF);
		}
	}
		
	if(m_Decay > 0)
		m_Decay--;
		
	if(m_AnimDecay > 0)
		m_AnimDecay--;
		
	if(m_SpawnanimDecay > 0)
		m_SpawnanimDecay--;

	m_Power = Powered();

	if(!m_Power && (m_Type == B_REPEATER || m_Type == B_TELEPORT))
		m_Anim = 0;
	
	if(!m_Power)
		m_Hit = 0;
		
	
		
	if(!m_Power && m_Type == B_TELEPORT)
		m_Anim = 0;

	if(!m_Alive || (!m_Power && m_Type != B_SHIELD))
		return;

	if(Server()->Tick() % 100 == 0 && m_Health < m_Maxhealth/4) {
		GameServer()->CreateExplosion(pos, -1, -1, true);
		
	}	
	
	switch(m_Type) {
		case B_REACTOR: {
			if(m_AnimDecay == 0) {
				m_Anim++;
				if(m_Anim > 3) {
					m_Anim = 0;
					m_AnimDecay = 100;
				} else
					m_AnimDecay = 10;
			}
		} break;
		
		case B_REPEATER: {
			if(!m_Power) {
				m_Anim = 0;
			} else {
				if(m_AnimDecay == 0) {
					m_Anim++;
					if(m_Anim > 1) {
						m_Anim = 0;
						m_AnimDecay = 100;
					} else
						m_AnimDecay = 10;
				}
			}
		} break;
		
		case B_SPAWN: {
			m_Anim = m_Anim & 0xF;
			if(m_AnimDecay == 0) {
				m_Anim++;
				if(m_Anim > 6)
					m_Anim = 0;
				m_AnimDecay = 35;
			}
			if(m_Spawnanim > 0 && m_SpawnanimDecay == 0) {
				m_Spawnanim--;
				m_SpawnanimDecay = 4;
			}
			m_Anim = (m_Spawnanim << 4) | m_Anim;
		} break;
		case B_AMMO1: case B_AMMO2: case B_AMMO3: {
			CCharacter *c = GetNearest(ms_PhysSize, m_Team);
			if(c) {
				if(m_Anim < 3 && m_AnimDecay == 0) {
					m_Anim++;
					m_AnimDecay = 4;
				}
				if(m_Decay == 0 && m_Anim == 3) {
					m_Decay = 50;
					int weapon = -1;
					if(m_Type == B_AMMO1)
						weapon = WEAPON_SHOTGUN;
					else if(m_Type == B_AMMO2) 
						weapon = WEAPON_GRENADE;
					else if(m_Type == B_AMMO3)
						weapon = WEAPON_RIFLE;
					if(!c->m_Weapons[weapon].m_Got) {
						c->m_Weapons[weapon].m_Got = 1;	
						c->m_ActiveWeapon = weapon;
						GameServer()->CreateSound(m_Pos, SOUND_PICKUP_SHOTGUN);
					} else {
						GameServer()->CreateSound(m_Pos, SOUND_PICKUP_GRENADE);
					}
					c->m_Weapons[weapon].m_Ammo = 10;
				}
			}
			else {
				if(m_Anim > 0 && m_AnimDecay == 0) {
					m_Anim--;
					m_AnimDecay = 4;
				}
			}
		} break;
		
		case B_TURRET1: {
			vec2 realpos = vec2(m_Pos.x, m_Pos.y-74);
			vec2 tmppos = m_Pos;
			m_Pos = realpos;
			CCharacter *c = GetNearest(700, m_Team^1);
			m_Pos = tmppos;
			if(c) {
				m_Angle = GetAngle(normalize(c->m_Pos - realpos));
				if(m_Decay == 0) {
					m_Decay = 12;
						float a = m_Angle;
						a+=frandom()*0.20f -0.10f;
						CProjectile *pProj = new CProjectile(WEAPON_GUN,
							-1,
							realpos,
							vec2(cosf(a), sinf(a)),
							(int)(Server()->TickSpeed()*GameServer()->Tuning()->m_GunLifetime),
							1, 0, 0, -1, WEAPON_GUN, false);
					GameServer()->CreateSound(realpos, SOUND_GUN_FIRE);
				}
			}
		} break;
		
		case B_TURRET2: {
			vec2 realpos = vec2(m_Pos.x, m_Pos.y-66);
			vec2 tmppos = m_Pos;
			m_Pos = realpos;
			CCharacter *c = GetNearest(500, m_Team^1);
			m_Pos = tmppos;
			if(c) {
				m_Angle = GetAngle(normalize(c->m_Pos-realpos));
				if(m_Decay == 0) {
					m_Decay = 30;
					int shotspread = 2;
					
					for(int i = -shotspread; i <= shotspread; i++)
					{
						float spreading[] = {-0.185f, -0.070f, 0, 0.070f, 0.185f};
						float a = m_Angle;
						a += spreading[i+2];
						float v = 1-(abs(i)/(float)shotspread);
						float speed = mix((float)tuning.shotgun_speeddiff, 1.0f, v);
						CProjectile *pProj = new CProjectile(WEAPON_SHOTGUN,
							-2, 
							realpos,
							vec2(cosf(a), sinf(a))*speed,
							(int)(Server()->TickSpeed()*GameServer()->Tuning()->m_ShotgunLifetime),
							1, 0, 0, -1, WEAPON_SHOTGUN, false);
					}
					game.create_sound(realpos, SOUND_SHOTGUN_FIRE);
				}
			}
		} break;
		
		case B_HEALTH: case B_ARMOR: {
			CCharacter *c = GetNearest(ms_PhysSize, m_Team);
			if(c) {
				if(m_Decay == 0) { //wait for open first
					if(m_AnimDecay == 0 && m_Decay == 0 && m_Anim < 4) {
						m_AnimDecay = 3;
						m_Anim++;
					}
					else if(m_AnimDecay == 0) {
						if(m_Type == B_HEALTH && c->m_Health < 10) {
							m_Decay = 150;
							c->IncreaseHealth(3);
							GameServer()->CreateSound(m_Pos, SOUND_PICKUP_HEALTH);
							c->m_Ill = false;
						} else if(m_Type == B_HEALTH) {
							c->m_Ill = false;
						} else if(m_Type == B_ARMOR && c->m_Armor < 10) {
							m_Decay = 150;
							c->IncreaseArmor(3);
							GameServer()->CreateSound(m_Pos, SOUND_PICKUP_ARMOR);
						}
					}
				}
			}
			else { 
				if(m_AnimDecay == 0 && m_Decay == 0 && m_Anim > 0) {
					m_AnimDecay = 3;
					m_Anim--;
				}
			}
		} break;
		
		case B_SHIELD: {
			if(Server()->Tick() % 50 == 0 && m_Power) {
				if(m_Armor < 30)
					m_Armor++;				
				//game.create_damageind(pos, 3.141592, armor > 0 ? (int)(10.0f*armor/30) : 0);
			}
			
			if(Server()->Tick() % 5 == 0) {
				if(m_Hit > 0)
					m_Hit--;
				if(m_Power && m_Armor > 0) {
					CProjectile *pProj[128];
					int num = GameServer()->m_World.FindEntities(m_Pos, 250.0f, (CEntity**)pProj, 128, CGameWorld::ENTTYPE_PROJECTILE);
					for(int i = 0; i < num && m_Armor > 0; i++) {
						if(pProj[i]->m_Bounce)
							continue;
						if((pProj[i]->m_Owner >= 0) && GameServer()->m_apPlayers[pProj[i]->m_Owner] && GameServer()->m_apPlayers[pProj[i]->m_Owner]->m_Team == m_Team)
							continue;
						if(pProj[i]->m_Owner < 0)
							continue;
						
						pProj[i]->m_Initpos = pProj[i]->m_Pos;
						pProj[i]->m_StartTick = Server()->Tick();
						pProj[i]->m_Direction = normalize(pProj[i]->m_Pos-m_Pos);
						pProj[i]->m_Bounce = 1;
						m_Hit = 3;
						
						//game.create_shieldhit(m_Pos, 0);
						
						m_Armor = clamp(m_Armor-(pProj[i]->m_Weapon == WEAPON_GRENADE ? 4 : 2), 0, 30);
						if(m_Armor == 0) {
							m_Armor = -4;
						}
					}
				}
			}
			m_Anim = (m_Armor > 0 ? (int)(9.0f*m_Armor/30.0f) : 0) | (m_Hit << 4);
		} break;
		
		//NODESTODO: COUNTINUE!
		case B_TELEPORT: {
			if(m_Decay == 0) {
				//find other teleport... D=
				CBuilding *other = 0;
				for(int i = 0; i < ((CGameControllerNODES*)GameServer()->m_pController)->BuildingsCount[m_Team]; i++) {
					CBuilding *b = ((CGameControllerNODES*)GameServer()->m_pController)->Buildings[m_Team][i];
					if( b != this && b->m_Type == B_TELEPORT && b->m_Alive && b->m_Power) {
						other = b;
					}
				}
				if(other) {
					bool is_enemy = false;
					CCharacter *c = GetNearest(ms_PhysSize-4.0f, m_Team); 
					if(!c && m_Anim == 8) {//once opened, port enemies as well
						c = GetNearest(ms_PhysSize-4.0f, m_Team^1);
						is_enemy = true;
					}
					if(c) {
						if(m_Anim < 8 && m_AnimDecay == 0 && !is_enemy) {
							m_Anim++;
							m_AnimDecay = 2;
							other->m_Anim = m_Anim;
							other->m_AnimDecay = m_AnimDecay+1;
							other->m_Decay =m_AnimDecay+1;
						}
						else if(anim == 8 && m_AnimDecay == 0) {
							if(!c->m_pPlayer->m_Ported) {
								//dbg_msg("nodes", "yay");
								float ill_prob = min((distance(other->m_Pos, m_Pos))/8000.0f * 0.2f, 0.2f);
								for(int i = 0; i < MAX_CLIENTS; i++) {
									if(game.players[i] && game.players[i]->get_character() && (game.players[i]->get_character()->core.hooked_player == c->player->client_id || c->core.hooked_player == game.players[i]->client_id)) {
										CCharacter *hc = game.players[i]->get_character();
										hc->m_Pos = vec2(other->m_Pos.x, other->m_Pos.y-1);
										hc->core.m_Pos = vec2(other->m_Pos.x, other->m_Pos.y-1);
										hc->player->ported = true;
										hc->core.hook_state = -1;
										hc->core.hooked_player = -1;
										if(frandom() <= ill_prob) {
											if(!hc->ill) {
												hc->ill = true;
												hc->infecter = -1;
											}
										}
									}
								}
								//dbg_msg("nodes", "porting %d from (%d,%d) to (%d,%d)", c->player->client_id, (int)pos.x, (int)pos.y, (int)other->pos.x, (int)other->pos.y);
								c->pos = vec2(other->pos.x, other->pos.y-1);
								c->core.pos = vec2(other->pos.x, other->pos.y-1);
								c->player->ported = true;
								c->core.hook_state = -1;
								c->core.hooked_player = -1;
								if(frandom() <= ill_prob) {
									if(!c->ill) {
										c->ill = true;
										c->infecter = -1;
									}
								}
								decay = 50;
								other->decay = 50;
								anim = 1;
								other->anim = 1;
								game.create_playerspawn(other->pos);
								game.create_sound(pos, SOUND_TELEPORT);
								game.create_death(pos, c->player->client_id);
								game.create_sound(other->pos, SOUND_TELEPORT);
							} else {
								anim = 0;
							}
						}
					}
					else {
						if(anim > 0 && anim_decay == 0) {
							anim++;
							if(anim == 9)
								anim = 0;
							anim_decay = 2;
							other->anim = anim;
							other->anim_decay = anim_decay+1;
							other->decay =anim_decay+1;
						}
					}
				}
			} else {
				if(anim_decay == 0 && anim > 0) {
					anim_decay = 2;
					anim++;
					if(anim == 9)
						anim = 0;
				}
			}
		} break;
	}

	return;
}

bool BUILDING::increase_health(int amount)
{
	if(health >= maxhealth)
		return false;
		
	int gain = 2;
	if(health_tick+25 > server_tick())
		gain = 1;
	else
		health_tick = server_tick();
		
	decon = false;
	
	health = clamp(health+gain, 0, maxhealth);
	if(health == maxhealth && !alive) {
		alive = true;
		if(type == B_REACTOR) {
			game.create_sound(pos, SOUND_REACTOR);
		} 
		((GAMECONTROLLER_NODES*)game.controller)->do_team_sound(SOUND_CTF_GRAB_PL, team);
		if(type == B_REACTOR) {
			game.create_sound(pos, SOUND_REACTOR);
		} 
	}
	
	return true;
}

void BUILDING::die(int killer, int weapon)
{
	bool was_alive = alive;
	alive = false;
	if(killer != -5) {
		game.create_sound(pos, SOUND_GRENADE_EXPLODE);
	
		if(type == B_REACTOR && was_alive && !decon) {
			game.create_effect(pos, EFFECT_FALLOUT);
			((GAMECONTROLLER_NODES*)game.controller)->fallout[((GAMECONTROLLER_NODES*)game.controller)->fallout_count++] = pos;
			((GAMECONTROLLER_NODES*)game.controller)->fallout_times[((GAMECONTROLLER_NODES*)game.controller)->fallout_count-1] = server_tick();
		}
		game.create_death(pos, -1);
		if(killer != -4) {
			game.create_explosion(pos, -4, WEAPON_WORLD, false);
		}
	}

	game.world.destroy_entity(this);

	if(killer >= 0 && game.players[killer] && game.players[killer]->team != team) {
		NETMSG_SV_KILLMSG msg;
		msg.killer = killer;
		
		msg.victim = -(type+1);
		msg.weapon = weapon;
		msg.mode_special = 0;
		msg.pack(MSGFLAG_VITAL);
		server_send_msg(-1);
	}
	((GAMECONTROLLER_NODES*)game.controller)->destroy_building(this, (killer == -5?true:false));

}

bool BUILDING::take_damage(vec2 force, int dmg, int from, int weapon)
{	
	if(from >= 0 && game.players[from] && game.players[from]->team == team && !config.sv_buildings_ff) //No friendly fire D=
		return false;
	
	if(from >= 0 && game.players[from] && game.players[from]->get_character() && game.players[from]->get_character()->damage_buff) {
		dmg += 1;
	}
	
	damage_taken++;

	// create healthmod indicator
	if(server_tick() < damage_taken_tick+25 && !decon)
	{
		// make sure that the damage indicators doesn't group together
		game.create_damageind(pos, damage_taken*0.25f, dmg);
	}
	else if(!decon)
	{
		damage_taken = 0;
		game.create_damageind(pos, 0, dmg);
	}

	if(dmg)
	{	
		health -= dmg;
	}

	damage_taken_tick = server_tick();
	if(damage_tick+150 < server_tick()) {
		damage_tick = server_tick();
		
		if(health < maxhealth / 4) {
			if(type == B_REACTOR) {
				int target = GAMECONTEXT::CHAT_RED;
				if(team == 1)
					target = GAMECONTEXT::CHAT_BLUE;
				game.send_chat(-1, target, "The reactor is badly damaged!");
			}	
		} else {
			if(type == B_REACTOR) {
				int target = GAMECONTEXT::CHAT_RED;
				if(team == 1)
					target = GAMECONTEXT::CHAT_BLUE;
				game.send_chat(-1, target, "The reactor is under attack!");
			}
		}
	}

	// do damage hit sound
	if(from >= 0 && game.players[from]) {
		game.create_sound(game.players[from]->view_pos, SOUND_HIT, cmask_one(from));
		if(game.players[from]->team != team)
			game.players[from]->rage = min(100, game.players[from]->rage+dmg*config.sv_rage_modifier);
		else
			game.players[from]->rage = max(0, game.players[from]->rage-dmg*config.sv_rage_modifier);
	}

	// check for death
	if(health <= 0)
	{
		die(from, weapon);
		
		// set attacker's face to happy (taunt!)
		if (from >= 0 && game.players[from])
		{
			CHARACTER *chr = game.players[from]->get_character();
			if (chr && game.players[from]->team != team)
			{
				chr->emote_type = EMOTE_HAPPY;
				chr->emote_stop = server_tick() + server_tickspeed();
			}
		}
	
		return false;
	}

	return true;
}

void BUILDING::snap(int snapping_client)
{
	if(networkclipped(snapping_client) || (!game.players[snapping_client]->get_character() && game.players[snapping_client]->selectspawn && (type != B_SPAWN || team != game.players[snapping_client]->team)))
		return;
	
	NETOBJ_BUILDING *building = (NETOBJ_BUILDING *)snap_new_item(NETOBJTYPE_BUILDING, id, sizeof(NETOBJ_BUILDING));

	building->x = (int)pos.x;
	building->y = (int)pos.y;
	
	if(!game.players[snapping_client]->get_character() && game.players[snapping_client]->selectspawn)
		building->health = 0;
	else
		building->health = 80*health/maxhealth;

	building->team = team;
	building->anim = anim & 0xF;
	building->anim2 = anim >> 4;
	building->type = type;
	building->power = power;
	building->angle = (int)((angle+pi/2)/(2*pi)*MAX_INT);
	building->alive = alive;
}
