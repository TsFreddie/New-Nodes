/* copyright (c) 2007 magnus auvinen, see licence.txt for more info */
#include <base/system.h>
#include <base/math.h>
#include <base/vmath.h>

#include <game/buildings_info.h>

buildings_info_t buildings_info[] = {
	{
		"Reactor",
		"Most important building, needed\n\
		to build other things.\n\
		Supplies your base with energy.\n\
		There can be only one reactor\n\
		at the same time.",
		0,
		200,
		86.0f,
		110.0f
	},
	
	{
		"Spawn",
		"Allows your teammates to spawn\n\
		at this building. If there are\n\
		no spawns and no players left,\n\
		your team has lost. So handle\n\
		these carefully!",
		10,
		70,
		78.0f,
		125.0f
	},
	
	{
		"Ammo Dispenser I",
		"Supplies your team with the\n\
		shotgun and ammunition.",
		8,
		60,
		62.0f,
		62.0f
	},
	
	{
		"Health Station",
		"Refreshs your health in case of\n\
		medical emergencies.",
		10,
		50,
		55.0f,
		55.0f
	},
	
	{
		"Repeater",
		"Mini Reactor. Allows you to build\n\
		at any other spot on the map, as\n\
		long as there is a working reactor.",
		12,
		50,
		64.0f,
		70.0f
	},
	
	{
		"Turret I",
		"Shoots at enemies automatically.\n\
		High range and fire rate, but low\n\
		damage. Be careful: Might hurt\n\
		teammates as well!",
		10,
		40,
		40.0f,
		93.0f
	},
	
	{
		"Shield Generator",
		"Protects friendly players and\n\
		buildings from projectiles\n\
		by tossing them back to their senders.",
		12,
		40,
		80.0f,
		72.0f
	},
	
	{
		"Ammo Dispenser II",
		"Supplies your team with the\n\
		grenade launcher and ammunition.",
		8,
		60,
		62.0f,
		62.0f
	},
	
	{
		"Teeleporter",
		"Makes faster-than-light\n\
		travelling between two of those\n\
		finally possible.",
		10,
		50,
		96.0f,
		25.0f
	},
	
	{
		"Armor Station",
		"Gives you protective armor\n\
		against weapons of all kinds.",
		10,
		50,
		55.0f,
		55.0f
	},
	
	{
		"Ammo Dispenser III",
		"Supplies your team with\n\
		laser weapons and ammunition.\n\
		Not affected by shields!",
		8,
		60,
		62.0f,
		62.0f
	},
	
	{
		"Turret II",
		"Shoots at enemies automatically\n\
		using a shotgun. High damage,\n\
		but low range. Be careful: Might\n\
		hurt teammates as well!",
		12,
		60,
		40.0f,
		93.0f
	}
};

ents_info_t ents_info[] = {
	{
		"Infection Mine",
		"Infects players on contact.",
		100,
		1
	},
	
	{
		"Gravity Mine",
		"Attracts players on contact.",
		200,
		2
	},
	
	{
		"Explosion Mine",
		"Hurts players on contact and\nslows them down.",
		300,
		3
	},
	
	{
		"N.N",
		"N.N",
		13373,
		4
	},
	
	{
		"Speed Flag",
		"Increases your and your\nteammates speed.",
		1,
		1
	},
	
	{
		"Reload Flag",
		"Decreases the reload time for you and your teammates.",
		1,
		2
	},
	
	{
		"Damage Flag",
		"Increases the damage you and your teammates make",
		1,
		3
	},
	
	{
		"N.N",
		"N.N",
		13373,
		4
	},
	
	{
		"N.N",
		"N.N",
		13373,
		4
	},
	
	{
		"N.N",
		"N.N",
		13373,
		4
	},
	
	{
		"N.N",
		"N.N",
		13373,
		4
	},
	
	{
		"N.N",
		"N.N",
		13373,
		4
	},
};

