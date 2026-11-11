/* OBJECT_TAG_DEFINITIONS.H: the object, unit and biped definition tags as the
   Xbox map lays them out (g_4e3b44[definition_index & 0xffff].bytes)

   Field names follow the Halo 2 Editing Kit's (H2EK) tag fields. The layout
   agrees with Project Cartographer's documented Halo 2 Vista layouts at every
   offset our code reads. A unit tag starts with the object's fields and a
   biped tag with the unit's, so a definition is the object part, then the
   unit part, then the biped part. The comments give each field's offset from
   the start of the tag, the form the raw reads used (definition + 0x1f8).

   String ids are longs and angles are reals, as elsewhere in this project. */

#ifndef OBJECT_TAG_DEFINITIONS_H
#define OBJECT_TAG_DEFINITIONS_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "tag_links.h"
#include <stddef.h>

/* ---- object ('obje', 0xbc bytes) ---- */

/* the object tag; every object tag starts with it. carries_objects and
   model_tag_index are the names object_list.cpp and scenery_animation.cpp
   read these fields by. */
struct s_object_definition
{
	short object_type;                                  /* 0x00 */
	union
	{
		word flags;                                     /* 0x02 */
		struct
		{
			word : 6;
			word carries_objects : 1;                   /* goes in the object list (H2EK: early mover) */
			word : 9;
		};
	};
	real bounding_radius;                               /* 0x04 */
	point3f bounding_offset;                            /* 0x08 */
	real acceleration_scale;                            /* 0x14 */
	short lightmap_shadow_mode;                         /* 0x18 */
	byte sweetener_size;                                /* 0x1a, read unsigned */
	byte pad_1b[1];
	dword runtime_flags;                                /* 0x1c */
	real dynamic_light_sphere_radius;                   /* 0x20 */
	point3f dynamic_light_sphere_offset;                /* 0x24 */
	long default_model_variant;                         /* 0x30 */
	union
	{
		s_tag_reference model;                          /* 0x34, 'hlmt' */
		struct
		{
			byte pad_34[4];
			long model_tag_index;                       /* 0x38, model.tag_index */
		};
	};
	s_tag_reference crate_object;                       /* 0x3c */
	s_tag_reference modifier_shader;                    /* 0x44 */
	s_tag_reference creation_effect;                    /* 0x4c */
	s_tag_reference material_effects;                   /* 0x54, 'foot' */
	s_tag_array ai_properties;                          /* 0x5c, 0x10-byte elements */
	s_tag_array functions;                              /* 0x64, 0x20-byte elements */
	real apply_collision_damage_scale;                  /* 0x6c */
	real_bounds game_acceleration;                      /* 0x70 */
	real_bounds game_scale;                             /* 0x78 */
	real_bounds abs_acceleration;                       /* 0x80 */
	real_bounds abs_scale;                              /* 0x88 */
	short hud_text_message_index;                       /* 0x90 */
	byte pad_92[2];
	s_tag_array attachments;                            /* 0x94, 0x18-byte elements */
	s_tag_array widgets;                                /* 0x9c, tag references */
	s_tag_array old_functions;                          /* 0xa4, 0x50-byte elements */
	s_tag_array change_colors;                          /* 0xac, 0x10-byte elements */
	s_tag_array predicted_resources;                    /* 0xb4 */
};
typedef char assert_object_definition_size[(sizeof(s_object_definition) == 0xbc) ? 1 : -1];
typedef char assert_object_definition_flags[(offsetof(s_object_definition, flags) == 0x2) ? 1 : -1];
typedef char assert_object_definition_sweetener_size[(offsetof(s_object_definition, sweetener_size) == 0x1a) ? 1 : -1];
typedef char assert_object_definition_model[(offsetof(s_object_definition, model.tag_index) == 0x38) ? 1 : -1];
typedef char assert_object_definition_model_tag_index[(offsetof(s_object_definition, model_tag_index) == 0x38) ? 1 : -1];
typedef char assert_object_definition_material_effects[(offsetof(s_object_definition, material_effects.tag_index) == 0x58) ? 1 : -1];
typedef char assert_object_definition_predicted_resources[(offsetof(s_object_definition, predicted_resources) == 0xb4) ? 1 : -1];

/* ---- unit ('unit', 0x1ec bytes) ---- */

/* a unit's (or a seat's) camera */
struct s_unit_definition_camera
{
	long camera_marker_name;                            /* +0x00 */
	long camera_submerged_marker_name;                  /* +0x04 */
	real pitch_auto_level;                              /* +0x08 */
	real_bounds pitch_range;                            /* +0x0c */
	s_tag_array camera_tracks;                          /* +0x14, tag references */
};
typedef char assert_unit_definition_camera_size[(sizeof(s_unit_definition_camera) == 0x1c) ? 1 : -1];

/* how much acceleration a unit (or a seat's rider) takes before its actions
   fail */
struct s_unit_seat_acceleration
{
	vector3f acceleration_range;                        /* +0x00 */
	real acceleration_action_scale;                     /* +0x0c */
	real acceleration_attach_scale;                     /* +0x10 */
};
typedef char assert_unit_seat_acceleration_size[(sizeof(s_unit_seat_acceleration) == 0x14) ? 1 : -1];

/* the damage effects of boarding and melee */
struct s_unit_boarding_melee
{
	s_tag_reference boarding_melee_damage;              /* +0x00 */
	s_tag_reference boarding_melee_response;            /* +0x08 */
	s_tag_reference landing_melee_damage;               /* +0x10 */
	s_tag_reference flurry_melee_damage;                /* +0x18 */
	s_tag_reference obstacle_smash_damage;              /* +0x20 */
};
typedef char assert_unit_boarding_melee_size[(sizeof(s_unit_boarding_melee) == 0x28) ? 1 : -1];

struct s_unit_boost
{
	real boost_peak_power;                              /* +0x00 */
	real boost_rise_power;                              /* +0x04 */
	real boost_peak_time;                               /* +0x08 */
	real boost_fall_power;                              /* +0x0c */
	real dead_time;                                     /* +0x10 */
};
typedef char assert_unit_boost_size[(sizeof(s_unit_boost) == 0x14) ? 1 : -1];

struct s_unit_definition_lipsync
{
	real attack_weight;                                 /* +0x00 */
	real decay_weight;                                  /* +0x04 */
};
typedef char assert_unit_definition_lipsync_size[(sizeof(s_unit_definition_lipsync) == 0x8) ? 1 : -1];

/* the unit tag's own fields, after the object's */
struct s_unit_struct_definition
{
	dword flags;                                        /* 0xbc */
	short default_team;                                 /* 0xc0 */
	short constant_sound_volume;                        /* 0xc2 */
	s_tag_reference integrated_light_toggle;            /* 0xc4 */
	real camera_field_of_view;                          /* 0xcc */
	real camera_stiffness;                              /* 0xd0 */
	s_unit_definition_camera unit_camera;                          /* 0xd4 */
	s_unit_seat_acceleration acceleration;              /* 0xf0 */
	real soft_ping_threshold;                           /* 0x104 */
	real soft_ping_interrupt_time;                      /* 0x108 */
	real hard_ping_threshold;                           /* 0x10c */
	real hard_ping_interrupt_time;                      /* 0x110 */
	real hard_death_threshold;                          /* 0x114 */
	real feign_death_threshold;                         /* 0x118 */
	real feign_death_time;                              /* 0x11c */
	real distance_of_evade_anim;                        /* 0x120 */
	real distance_of_dive_anim;                         /* 0x124 */
	real stunned_movement_threshold;                    /* 0x128 */
	real feign_death_chance;                            /* 0x12c */
	real feign_repeat_chance;                           /* 0x130 */
	s_tag_reference spawned_turret_character;           /* 0x134 */
	short spawned_actor_count[2];                       /* 0x13c, lower and upper */
	real spawned_velocity;                              /* 0x140 */
	real aiming_velocity_maximum;                       /* 0x144 */
	real aiming_acceleration_maximum;                   /* 0x148 */
	real casual_aiming_modifier;                        /* 0x14c */
	real looking_velocity_maximum;                      /* 0x150 */
	real looking_acceleration_maximum;                  /* 0x154 */
	long right_hand_node;                               /* 0x158 */
	long left_hand_node;                                /* 0x15c */
	long preferred_gun_node;                            /* 0x160 */
	s_tag_reference melee_damage;                       /* 0x164 */
	s_unit_boarding_melee boarding_melee;               /* 0x16c */
	short motion_sensor_blip_size;                      /* 0x194 */
	byte pad_196[2];
	s_tag_array postures;                               /* 0x198 */
	s_tag_array new_hud_interfaces;                     /* 0x1a0, tag references */
	s_tag_array dialogue_variants;                      /* 0x1a8 */
	real grenade_velocity;                              /* 0x1b0 */
	short grenade_type;                                 /* 0x1b4 */
	short grenade_count;                                /* 0x1b6 */
	s_tag_array powered_seats;                          /* 0x1b8 */
	s_tag_array weapons;                                /* 0x1c0, tag references */
	s_tag_array seats;                                  /* 0x1c8, s_unit_seat_definition (units.h) */
	s_unit_boost boost;                                 /* 0x1d0 */
	s_unit_definition_lipsync lipsync;                      /* 0x1e4 */
};

/* the unit tag ('unit'); vehicle and biped tags start with it */
struct s_unit_definition
{
	s_object_definition object;                         /* 0x00 */
	s_unit_struct_definition unit;                      /* 0xbc */
};
typedef char assert_unit_struct_definition_size[(sizeof(s_unit_struct_definition) == 0x1ec - 0xbc) ? 1 : -1];
typedef char assert_unit_definition_size[(sizeof(s_unit_definition) == 0x1ec) ? 1 : -1];

/* ---- biped ('bipd', 0x314 bytes) ---- */

struct s_biped_definition_lock_on
{
	dword flags;                                        /* +0x00 */
	real lock_on_distance;                              /* +0x04 */
};
typedef char assert_biped_definition_lock_on_size[(sizeof(s_biped_definition_lock_on) == 0x8) ? 1 : -1];

/* how a character walks: the slopes it can climb */
struct s_biped_movement_ground
{
	real maximum_slope_angle;                           /* +0x00 */
	real downhill_falloff_angle;                        /* +0x04 */
	real downhill_cutoff_angle;                         /* +0x08 */
	real uphill_falloff_angle;                          /* +0x0c */
	real uphill_cutoff_angle;                           /* +0x10 */
	real downhill_velocity_scale;                       /* +0x14 */
	real uphill_velocity_scale;                         /* +0x18 */
	real cosine_maximum_slope_angle;                    /* +0x1c */
	real negative_sine_downhill_falloff_angle;          /* +0x20 */
	real negative_sine_downhill_cutoff_angle;           /* +0x24 */
	real sine_uphill_falloff_angle;                     /* +0x28 */
	real sine_uphill_cutoff_angle;                      /* +0x2c */
};
typedef char assert_biped_movement_ground_size[(sizeof(s_biped_movement_ground) == 0x30) ? 1 : -1];

/* how a character flies */
struct s_biped_movement_flying
{
	real bank_angle;                                    /* +0x00 */
	real bank_apply_time;                               /* +0x04 */
	real bank_decay_time;                               /* +0x08 */
	real pitch_ratio;                                   /* +0x0c */
	real max_velocity;                                  /* +0x10 */
	real max_sidestep_velocity;                         /* +0x14 */
	real acceleration;                                  /* +0x18 */
	real deceleration;                                  /* +0x1c */
	real angular_velocity_maximum;                      /* +0x20 */
	real angular_acceleration_maximum;                  /* +0x24 */
	real crouch_velocity_modifier;                      /* +0x28 */
};
typedef char assert_biped_movement_flying_size[(sizeof(s_biped_movement_flying) == 0x2c) ? 1 : -1];

/* a character's shape and movement (the biped tag's at 0x264) */
struct s_biped_movement
{
	dword flags;                                        /* +0x00 */
	real height_standing;                               /* +0x04 */
	real height_crouching;                              /* +0x08 */
	real radius;                                        /* +0x0c */
	real mass;                                          /* +0x10 */
	long living_material_name;                          /* +0x14 */
	long dead_material_name;                            /* +0x18 */
	short living_global_material_index;                 /* +0x1c */
	short dead_global_material_index;                   /* +0x1e */
	s_tag_array dead_sphere_shapes;                     /* +0x20, 0x70-byte elements */
	s_tag_array pill_shapes;                            /* +0x28, 0x50-byte elements */
	s_tag_array sphere_shapes;                          /* +0x30, 0x70-byte elements */
	s_biped_movement_ground ground_physics;  /* +0x38 */
	s_biped_movement_flying flying_physics;  /* +0x68 */
};
typedef char assert_biped_movement_size[(sizeof(s_biped_movement) == 0x94) ? 1 : -1];

/* the biped tag's own fields, after the unit's */
struct s_biped_struct_definition
{
	real moving_turning_speed;                          /* 0x1ec */
	dword flags;                                        /* 0x1f0 */
	real stationary_turning_threshold;                  /* 0x1f4 */
	real jump_velocity;                                 /* 0x1f8 */
	real maximum_soft_landing_time;                     /* 0x1fc */
	real maximum_hard_landing_time;                     /* 0x200 */
	real minimum_soft_landing_velocity;                 /* 0x204 */
	real minimum_hard_landing_velocity;                 /* 0x208 */
	real maximum_hard_landing_velocity;                 /* 0x20c */
	real death_hard_landing_velocity;                   /* 0x210 */
	real stun_duration;                                 /* 0x214 */
	real standing_camera_height;                        /* 0x218 */
	real crouching_camera_height;                       /* 0x21c */
	real crouch_transition_time;                        /* 0x220 */
	real camera_interpolation_start;                    /* 0x224 */
	real camera_interpolation_end;                      /* 0x228 */
	real camera_forward_movement_scale;                 /* 0x22c */
	real camera_side_movement_scale;                    /* 0x230 */
	real camera_vertical_movement_scale;                /* 0x234 */
	real camera_exclusion_distance;                     /* 0x238 */
	real autoaim_width;                                 /* 0x23c */
	s_biped_definition_lock_on lock_on_data;                  /* 0x240 */
	short physics_control_node_index;                   /* 0x248 */
	byte pad_24a[2];
	real cosine_stationary_turning_threshold;           /* 0x24c */
	real crouch_transition_velocity;                    /* 0x250 */
	short pelvis_node_index;                            /* 0x254 */
	short head_node_index;                              /* 0x256 */
	real headshot_acceleration_scale;                   /* 0x258 */
	s_tag_reference area_damage_effect;                 /* 0x25c */
	s_biped_movement physics;             /* 0x264 */
	s_tag_array contact_points;                         /* 0x2f8, marker names (longs) */
	s_tag_reference reanimation_character;              /* 0x300 */
	s_tag_reference death_spawn_character;              /* 0x308 */
	short death_spawn_count;                            /* 0x310 */
	byte pad_312[2];
};

/* the biped tag ('bipd') */
struct s_biped_definition
{
	s_object_definition object;                         /* 0x00 */
	s_unit_struct_definition unit;                      /* 0xbc */
	s_biped_struct_definition biped;                    /* 0x1ec */
};
typedef char assert_biped_struct_definition_size[(sizeof(s_biped_struct_definition) == 0x314 - 0x1ec) ? 1 : -1];
typedef char assert_biped_definition_size[(sizeof(s_biped_definition) == 0x314) ? 1 : -1];

/* the offsets the converted reads in bipeds.cpp depend on */
typedef char assert_unit_definition_flags[(offsetof(s_unit_definition, unit.flags) == 0xbc) ? 1 : -1];
typedef char assert_unit_definition_default_team[(offsetof(s_unit_definition, unit.default_team) == 0xc0) ? 1 : -1];
typedef char assert_unit_definition_seats[(offsetof(s_unit_definition, unit.seats.address) == 0x1cc) ? 1 : -1];
typedef char assert_unit_definition_boost[(offsetof(s_unit_definition, unit.boost.boost_peak_power) == 0x1d0) ? 1 : -1];
typedef char assert_biped_definition_moving_turning_speed[(offsetof(s_biped_definition, biped.moving_turning_speed) == 0x1ec) ? 1 : -1];
typedef char assert_biped_definition_flags[(offsetof(s_biped_definition, biped.flags) == 0x1f0) ? 1 : -1];
typedef char assert_biped_definition_jump_velocity[(offsetof(s_biped_definition, biped.jump_velocity) == 0x1f8) ? 1 : -1];
typedef char assert_biped_definition_soft_landing_velocity[(offsetof(s_biped_definition, biped.minimum_soft_landing_velocity) == 0x204) ? 1 : -1];
typedef char assert_biped_definition_hard_landing_velocity[(offsetof(s_biped_definition, biped.maximum_hard_landing_velocity) == 0x20c) ? 1 : -1];
typedef char assert_biped_definition_stun_duration[(offsetof(s_biped_definition, biped.stun_duration) == 0x214) ? 1 : -1];
typedef char assert_biped_definition_standing_camera_height[(offsetof(s_biped_definition, biped.standing_camera_height) == 0x218) ? 1 : -1];
typedef char assert_biped_definition_crouching_camera_height[(offsetof(s_biped_definition, biped.crouching_camera_height) == 0x21c) ? 1 : -1];
typedef char assert_biped_definition_cosine_turning[(offsetof(s_biped_definition, biped.cosine_stationary_turning_threshold) == 0x24c) ? 1 : -1];
typedef char assert_biped_definition_headshot_acceleration_scale[(offsetof(s_biped_definition, biped.headshot_acceleration_scale) == 0x258) ? 1 : -1];
typedef char assert_biped_definition_physics_flags[(offsetof(s_biped_definition, biped.physics.flags) == 0x264) ? 1 : -1];
typedef char assert_biped_definition_height_standing[(offsetof(s_biped_definition, biped.physics.height_standing) == 0x268) ? 1 : -1];
typedef char assert_biped_definition_height_crouching[(offsetof(s_biped_definition, biped.physics.height_crouching) == 0x26c) ? 1 : -1];
typedef char assert_biped_definition_radius[(offsetof(s_biped_definition, biped.physics.radius) == 0x270) ? 1 : -1];
typedef char assert_biped_definition_pill_shapes[(offsetof(s_biped_definition, biped.physics.pill_shapes.count) == 0x28c) ? 1 : -1];
typedef char assert_biped_definition_pill_shapes_address[(offsetof(s_biped_definition, biped.physics.pill_shapes.address) == 0x290) ? 1 : -1];
typedef char assert_biped_definition_sphere_shapes_address[(offsetof(s_biped_definition, biped.physics.sphere_shapes.address) == 0x298) ? 1 : -1];
typedef char assert_biped_definition_flying_physics[(offsetof(s_biped_definition, biped.physics.flying_physics) == 0x2cc) ? 1 : -1];
typedef char assert_biped_definition_contact_points[(offsetof(s_biped_definition, biped.contact_points.count) == 0x2f8) ? 1 : -1];
typedef char assert_biped_definition_contact_points_address[(offsetof(s_biped_definition, biped.contact_points.address) == 0x2fc) ? 1 : -1];

#endif
