#ifndef GAME_SERVER_ENTITY_BUILDING_H
#define GAME_SERVER_ENTITY_BUILDING_H

#include <game/server/entity.h>
#include <game/generated/server_data.h>
#include <game/generated/protocol.h>

#include <game/gamecore.h>

class CBuilding : public CEntity
{
public:
	CBuilding(CGameWorld *pWorld, int Type);
	
	virtual void Reset();
	virtual void Destroy();
	bool IsGrounded();
	bool Powered();


	CCharacter *GetNearest(int Maxdist, int t);

	void Die(int Killer, int Weapon);
	bool TakeDamage(vec2 Force, int Dmg, int From, int Weapon);
	bool Spawn(vec2 Pos, int Team);
	//bool init_tryspawn(int team);
	bool Remove();

	static const int ms_PhysSize = 28;

	virtual void Tick();
	virtual void Snap(int SnappingClient);
	
	bool IncreaseHealth(int Amount);
	
private:
	/*static int buildings;
	static int next;*/
	float m_Width, m_Height;
	
	bool m_Alive;
	
	int m_HealthTick;
	int m_DamageTick;
	
	int m_Type;
	
	int m_Armor;

	/*
	char m_SkinName[64];
	int m_UseCustomColor;
	int colorbody;
	int color_feet;
	*/
	
	int m_DamageTaken;
	int m_DamageTakenTick;
	
	int m_Health;
	int m_Maxhealth;

	int m_Team;
	int m_Decay;
	int m_AnimDecay;
	
	int m_Anim;
	
	int m_Spawnanim;
	int m_SpawnanimDecay;
	
	float m_Angle;
	
	int m_Hit;
	
	bool m_Decon;
	
	bool m_Power;
	
	bool m_Free;

};

#endif
