A 0.6 Port Version of Nodes!!

Everyone can help me to port this mod by fork this Repo and Modify, then Pull~
The 0.5 original Nodes(1.0.1) source: http://adf.ly/Pto4C	

Glad to help me!

Progress[Only list the file need to port]:

datasrc [DONE]
	content.py [DONE]
	network.py [DONE]
src
	engine
		e_config_variables.h -> shared\config_variables.h [DONE]
		e_engine.c -> engine.cpp [DONE]
		e_if_client.h -> [DELETED] [?]
		e_if_other.h -> [DELETED] [?]
		e_network.c -> network.cpp [DONE]
		e_network_internal.h -> shared\protocol.h [DONE] [?]
		client
			ec_client.c -> client.cpp [DONE]
			ec_gfx.c -> backend_sdl.cpp [?]
			ec_srvbrowse.c -> serverbrowser.cpp [DONE]
		server
			es_server.c -> server.cpp [DONE]
	game
		building_info.*  [COPYED]
		collision.cpp [DONE]
		crate_info.hpp -> crate_info.h [COPYED]
		gamecore.* [DONE]
		variables.hpp -> variables.h [DONE]
		version.hpp -> version.h [DONE]
		client
			gameclient.* [DONE]
			render.* [TODO:RenderTee]
			components
				binds.cpp [DONE] [?]
				broadcast.* [0%]
				buildmenu.* [DONE]
				buymenu.* [DELETED]
				controls.cpp [0%]
				effects.* [DONE]
				emoticon.cpp [DONE]
				hud.cpp [DONE]
				items.* [0%]
				killmessages.cpp [0%]
				maplayers.cpp [0%]
				menus.* [0%]
				menus_*.* [0%]
				nameplates.cpp [0%]
				particles.* [DONE]
				players.cpp [30%]
				scoreboard.* [0%]
		server
			eventhandler.cpp [DONE]
			gamecontext.* [80%]
			gamecontroller.* [DONE]
			gameworld.* [50%]
			hooks.cpp [?]
			player.* [0%]
			entities
				building.* [50%]
				character.* [0%]
				crate.* [COPYED 0%]
				laser.* [0%]
				projectile.* [0%]
			gamemodes
				nodes.* [0%]



Copyright (c) 2012 Magnus Auvinen


This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.


Please visit http://www.teeworlds.com for up-to-date information about 
the game, including new versions, custom maps and much more.
