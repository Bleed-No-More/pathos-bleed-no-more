# Particle engine

# Description
The Pathos particle engine is a heavily advanced version of what
was once in Trinity. The particle script format has been totally
updated to provide a far more descriptive and user-friendly way of
defining particle behavior.

Particle scripts are located under the path:
"pathos/scripts/particles"

The textures will be loaded from the following path:
"pathos/textures/particles"

# Scripts and their tokens:
The particle scripts contain all the information that defines how
particles behave. There's two main types of scripts used:

# Cluster scripts:
These scripts group multiple particle scripts into a single effect
that spawns all the particle systems listed. Here is an example:
 
	$clusterscript
	{
		$script acidbottle_clouds.txt
		$script acidbottle_glass_flecks1.txt
		$script acidbottle_glass_flecks2.txt
		$script acidbottle_glass_flecks3.txt
		$script acidbottle_splashes1.txt
		$script acidbottle_splashes2.txt
		$script acidbottle_droplets.txt
	}

This script will spawn all of these particle systems with one entity/call.
A cluster script cannot spawn another cluster script though, so don't
specify a cluster script inside another cluster script.

# System scripts:
These files will contain the settings that define particle behaviors. There
are a lot of options for these. Here is a simple example:

>$particlescript <br />
>{ <br />
>	$shape box <br />
>	$system_size 6.000000 <br />
>	$fade_in_time 0.500000 <br />
>	$fade_out_delay 1.000000 <br />
>	$lifetime 4.000000 <br />
>	$lifetime_variation 1.000000 <br />
>	$primary_color 255 255 255 <br />
>	$scale 16.000000 <br />
>	$scale_variation 8.000000 <br />
>	$scale_damping 8.000000 <br />
>	$max_particles -1 <br />
>	$particle_frequency 10.000000 <br />
>	$rendermode additive <br />
>	$alpha 1.000000 <br />
>	$alignment parallel <br />
>	$texture flames <br />
>	$render_flags overbright <br />
>	$frame_width 32 <br />
>	$frame_height 64 <br />
>	$num_frames 14 <br />
>	$framerate 14 <br />
>} <br />

On second thought, it's not that simple. Particle behaviors are quite
complex to define, and lacking a proper editor, I recommend using the
in-game command "efx_createparticle" for testing out systems.

# Textures:
Your particle textures can have an alpha component, or they can be
additive, where the black color is fully transparent. Texturs can also
be atlases of individual frames, defined by $num_frames, $frame_width,
$frame_height and $framerate. Frames are stored in a single texture
in order to reduce performance loss due to texture binds.
For distortion/refractive particles, I recommend using a normal map
that defines the way the particle distorts the environment behind it.
 
Below are the options that can be specified in these scripts:

 - $shape - Defines the shape of the system. The options are:
   - point - The system is a single point that will emit particles.
   - default - Same as "point"
   - box - The system is a box, defined by $system_size
   - playerplane - A plane above the player that follows them
                   around. Used for precipitation effects like
				   snow or rain. See $system_size, and
				   $playerplane_max_height for more info.
				   Particle systems of this type should be
				   positioned under a sky brush to help gauge
				   the optimal spawn position of precipitation
				   particles.
				   
 - $flags - Defines some generic properties as bitflags for the
            system. Options:
   - random_direction - Direction vector is ignored, and the particles
                        will go off in completely random directions.
   - globs - Particles will be spawned in groups called "globs". See
             $glob_size and $num_glob_particles for more info.
   - softoff - The system will fade out instead of terminating
               instantly. See $soft_turnoff_duration for more info.
			   
 - $primary_color - The primary color of the particles spawned.
 - $secondary_color - The secondary color of the particles. Particles
                      Can transition from a primary color to their
					  secondary color. See $color_transition_delay,
					  $color_transition_duration and 
					  $color_transition_variance for more info.
 
 - $lighting - Specifies the lighting attributes for the particles.
               These are applied as bitflags. The flags are:
   - normal - Lighting is taken from the surface below and applied
              to the particles.
   - mix_primary_color - The primary color specified will be mixed
                         with the lighting.
   - use_intensity - Lighting will be applied as a brightness value
                     instead of as color information.
   - only_once - Only take light values upon spawn, and no other
                 after. Useful for performance optimization.
   - none - No lighting will be applied.
   - nodynlights - Dynamic lights will not affect particles.
   
 - $collision - Defines collision behaviors. The options are:
   - none/default : No collision occurs.
   - die - The particles will disappear upon impacting anything.
   - bounce - Particles will bounce off surfaces.
   - decal - The particle will spawn a decal specified in $create.
   - stuck - The particle will get stuck, then fade out. See the
             option $stuck_death_time for more info.
   - create_system - The particle will spawn a new particle system
                     where it has impacted. See $create for more.
					 
 - $collision_flags - Defines the specifics of collision behaviors.
                      The flags are:
   - precise - A full traceline is used for collision detection, this
               will provide the best possible results, but might be a
			   bit slower than the default behavior, which just tests
			   if the particle is inside a solid at it's position each
			   frame.
   - collide_brushmodels - Particles will collide not only with the world,
                           but also with brush models. This could become
						   quite slow with too many particles.
   - collide_water - The particle will collide with func_water entities.
                     See the option $create_on_water_impact for more.
   
 - $rendermode - Defines the various rendermodes for particles. These are:
   - additive/default - The particles will be rendered with additive 
                        rendering mode.
   - alphablend - Particles will be rendered with alpha blending.
   - refractive - Particles will refract the scene behind them instead.
   - alphatest - Particles will be rendered with alpha testing instead 
                 of blending.
				 
 - $alignment - Defines how the particles are aligned on-screen. The 
                options are:
   - tiled/default - The particles will render flat on the screen.
   - parallel - Particles will render parallel to the view.
   - to_normal - Aligned to a surface normal, similar to a decal.
   - tracer - Particles will be aligned based on their trajectory.
              Good for things like sparks and rain particles.
			  
 - $render_flags - Specified rendering properties as bitflags.
   - overbright - Particles will have overbright applied.
   - skybox - Particles are only rendered in a skybox setting.
   - nocull - Do not cull back faces.
   - nofog - Do not apply fog.
 - $wind_type Specifies how wind affects the particles:
   - none - Default behavior, no wind is applied.
   - linear - Wind is applied in a linear fashion.
   - sine - Wind is applied in a sinus-wave fashion.
 - $attachment_flags - Defines how a system attaches to an entity.
                       This is only usable in very specific situations
					   right now and is not possible to apply via just
					   the env_particle_system entity.
 
 - $min_velocity - Minimum velocity a particle can spawn with.
 - $max_velocity - Maximum velocity a particle can spawn with.
 - $max_offset - The max angle offset from the spawn direction
                 specified.
 - $fade_in_time - Time it takes for the particle to fade in.
 - $fade_out_delay - Time until particle begins fading out.
 - $alpha - Defines the main alpha value of the particle.
 - $velocity_damping - Defines how much velocity is damped each frame.
 - $velocity_damping_delay - Delay until velocity starts being dampened.
 - $lifetime - Maximum base lifetime of the particle.
 - $lifetime_variation - Max variation in both negative and positive
                         for particle lifetimes. Best not to set a value
						 that would be too close to the fadeout delay.
 - $color_transition_delay - Time until the color begins to transition
                             to the secondary color.
 - $color_transition_duration - Duration of the color transition.
 - $color_transition_variance - Min/max variance in color transition duration.
 - $scale - Base scale of the particle.
 - $scale_variation - Min/max variation in particle scales.
 - $scale_damping_delay - Time until scale begins decreasing if set.
 - $scale_damping - Factor by which the scale begins decreasing. Use a negative
                    value to make the particle grow instead.
 - $gravity - Defines gravity strength on particles. 0 will disable it. This is
              dependent upon sv_gravity.
 - $system_size - Size of the system in all directions. Used for non-standard
                  system shapes.
 - $max_particles - Maximum amount of particles spawned until the system
                    exhausts itself. A setting of -1 will make particles spawn
					indefinitely.
 - $particle_frequency - Amount of particles spawned per second. Can be a float
                         value for very low spawn rates.
 - $particles_on_spawn - Number of particles to create instantly upon spawn. This
                         does not add to $max_particles.
 - $particle_frequency_variation - Min/Max variation in particle frequency.
 - $impact_velocity_dampening - Defines how much velocity is dampened if the
                                particle impacts a surface with the "bounce"
								collision setting.
 - $rotate_x_variation - Variation in rotation speed on the X axis.
 - $rotate_x_speed - Speed of rotation on X axis.
 - $rotate_x_dampening - Defines how much rotation is dampened on the X axis.
                         Use a negative value to make rotation speed increase.
 - $rotate_x_dampening_delay - Delay until dampening is applied to X axis
                               rotation.
 - $rotate_y_variation - Variation in rotation speed on the Y axis.
 - $rotate_y_speed - Speed of rotation on Y axis.
 - $rotate_y_dampening - Defines how much rotation is dampened on the Y axis.
                         Use a negative value to make rotation speed increase.
 - $rotate_y_dampening_delay - Delay until dampening is applied to Y axis
                               rotation.
 - $rotate_z_variation - Variation in rotation speed on the Z axis.
 - $rotate_z_speed - Speed of rotation on Z axis.
 - $rotate_z_dampening - Defines how much rotation is dampened on the Z axis.
                         Use a negative value to make rotation speed increase.
 - $rotate_z_dampening_delay - Delay until dampening is applied to z axis
                               rotation.
 - $create - Defines what will be spawned when the particle impacts something.
             If collision is set to "decal", then the decal group specified will
			 be picked. If set to "create_system" is set, the specified particle
			 system script will be used to spawn a new particle system.
			 This can also specify particles that will be spawned when $tracer_distance
			 is used. In this case, one particle is spawned for each time the parent
			 particle travels the distance defined by $tracer_distance.
 - $create_on_death - Defines a particle system that can be created if the particle
                      dies due to exhausting it's life.
 - $create_on_water_impact - Defines the particle system that will be spawned at
                             the point where the particle has impacted a water
							 entity. This will ignore "$create"
 - $wind_x_velocity - Defines wind speed on the X axis.
 - $wind_y_velocity - Defines wind speed on the Y axis.
 - $wind_velocity_variance - Min/max variation on the wind speed
 - $wind_sine_variance_speed_multiplier - Used to scale the sine wave wind value
                                          for faster/slower variation in wind.
 - $wind_sine_min_variance - Min and max variance on sine wave wind.
 - $wind_sine_variance - Wind variance for sine wave wind.
 - $playerplane_max_height - Maximum height above player at which precipitation
                             particles can spawn.
 - $tracer_distance - Defines the distance intervals at which daughter particles 
                      specified in $create are spawned.
 - $fade_near_distance - Near distance to view, at which particles will fade out
                         completely.
 - $fade_far_distance - Distance to view at which particles will begin fading out.
 - $num_frames - Number of frames stored in the particle texture.
 - $frame_width - Width of a single particle frame in the particle texture.
 - $frame_height - Height of a single particle frame in the particle texture.
 - $framerate - Framerate of particles.
 - $chance_to_create - If set, particles spawned via $create-type settings will
                       spawn one out of n chance specified. Useful for dense
					   precipitation but without overwhelming performance with
					   the ground particles spawned.
 - $min_light_value - Minimum light value of particles in the 0-1 range.
 - $max_light_value - Maximum light value of particles in the 0-1 range.
 - $glob_size - The area size around a particle which will spawn daughter particles.
 - $num_glob_particles - Number of daughter particles spawned around the parent.
 - $soft_turnoff_duration - If set, the particle system's particle frequency will
                            fade out over this duration until reaching zero.
 - $decal_lifetime - Lifetime of decal spawned by particle if collision is set to 
                     spawn decals.
 - $decal_fade_time - Duration of decal fade-out.	
 - $decal_growth_time - Over this time, the spawned decal with grow from 1/10th of
                        it's size to it's actual size. Useful for things like pools
						of blood and such.
 - $texture - Specifies the texture used by the particles.