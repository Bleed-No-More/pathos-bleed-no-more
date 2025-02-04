# Useful cvars and commands

I've included several useful console commands and cvars that help 
with debugging issues with the engine. In this file I will list a 
detailed list and description of them.

The console variables:
 - dev_npcdebug - I use this to precache all npc entities, so I
				  can spawn them with the "give" command.
 - dev_triggers - This will cause a verbose spam of info into the
				  console that will tell you what entities are being 
				  triggered by which other entities. You need to set 
				  "developer" to highler level for this to work.
 - con_historybox - This will draw a red reticle on your screen that 
					will show the most recent console prints for a 
					limited amount of time.
 - developer - Allows you to set the developer print level. 0 will
			   disable all developer prints, 1 will enable developer 
			   mode, and 3 will set it to verbose.
 - host_timescale - This is a multiplier for the frametime, which
					by default is 1. You can use this to slow down 
					the game, for time-sensitive things.
 - r_draworigins - This will draw the origins of all entities in
				   the level with an axial marker.
 - r_particles_debug - Toggle printing of particle debug info.
 - r_portals_debug - Toggle printing of portal rendering debug info.
 - r_show_bogus_decals - This toggles rendering of markers where
						 decals have failed to spawn for some reason.
 - r_stats - This is essentially the same as r_speeds for Half-Life,
			 and will show the renderer statistics for polygons 
			 rendered.
 - r_timegraph - This will draw a graph on the bottom of the screen
				 that tracks the FPS over time.
 - r_vbmdecal_vcache_size - This controls the size of the decal
							vertex cache for VBM models. Setting this 
							higher will reduce the overturn of decals 
							and thus the removal of older decals.
 - r_wadchecks - If this is enabled, Pathos will generate PMF files
				 for any textures in WAD files, or those embedded in 
				 the BSP. I don't recommend setting this on by default, 
				 as it will slow down level load times.
 - r_water_debug - Enable printing of water rendering statistics for
				   each water pass.
 - r_wireframe - A setting of 1 will cause everything to be rendered
				 with a wireframe overlay. White lines mark single-pass 
				 rendered polygons, while red ones are multi-pass.
 - s_debug - This will enable rendering of debug visual information
			 for sounds being played in-game. This will render two points, 
			 the top one indicating whether the sound is playing, and the 
			 bottom one indicating whether the sound is on a valid visleaf.
 - sun_debug_pitch and sun_debug_roll - If you have one or multiple 
										env_sun entities in your level and 
										you set these cvars to a non-zero
										value, those sunflares will take up 
										the values from these cvars. This is 
										useful for helping you get the proper 
										axis values to set inside the env_sun 
										entity.
 - sv_netdebug - This'll enable net-related debug console prints, but as said 
			     in the other files, networking is not really supported.
 - clear - Cleas console history
 - cmdlist - Dump all console commands.
 - cvarlist - Dump all console variables.
 - condump - Dump the contents of the console into a condump file inthe mod
			 directory.
 - connect - Used to connect to a remote server if sv_maxplayers is above 1.
 - disconnet - Shuts down the active game, or disconnects from a server.
 - dumpcheats - Dumps all cheat commands.
 - dumpmodelcache - Dump the contents of the loaded models' cache.
 - exec - Execute a text file containing a list of commands.
 - god - Enable godmode, needs sv_cheats to be set to 1.
 - heartbeat - Checks the response speed/status of the server.
 - loadmodel - Loads a model into the cache.
 - map - Load up a level.
 - messagemode - Enables chat functionality, can be useful for noting down
				 issues in levels via text displayed on-screen, then 
				 screenshotting the issue.
 - noclip - Disables player gravity and collisions, but triggers will still
		    be hit by the player.
 - notarget - Makes the player inaudible/invisible to NPCs.
 - pastedecal - Paste a random decal from a specified decal group in front 
				of the view.
 - playmusic - Play a music file, relative to the "sound" folder.
 - r_bsp2smd_lm - This will export the map currently loaded to an SMD file
                  in the mod folder, and will export the lightmap texture
				  along with it.
 - r_bsp2smd_tx - Similar to "r_bsp2smd_lm", but with texture coordinates
				  as UV coordinates, instead of lightmap UVs.
 - r_buildcubemaps - If your map has env_cubemap entities in it, this will
					 trigger the engine to generate cubemaps for the level
					 and save them into a .ecd file, which will be read
					 back upon level load from then on.
 - r_detail_auto - This will generate automatic detail texture info for the
				   textures in the level, based on the contents of the file
				   "detailtextures.txt" under "pathos/scripts/legacy".
 - restart - Restars the game with the current level.
 - snapshot - Creates a screenshot.
 - stopmusic - Stops all playing music tracks.
 - timerefresh - Performs a panorama render of the entire 360 degrees of the
				 view and reports the FPS.
 