#pragma once

#include "base.h"
#include "sys.h"
#include "tile_map.h"

#define DT (1.0f / 60.0f)

struct world;

enum entity_flag : u16 {
	EF_DESTROYED = 0x01,
	EF_PLAYER    = 0x02,
	EF_BULLET    = 0x04,
	EF_PLANET    = 0x08,
	EF_TETHER    = 0x10,
	EF_HOOK      = 0x20
};

enum entity_type : u16 {
	ET_PLAYER,
	ET_BULLET,
	ET_PLANET,
	ET_TETHER,
	ET_HOOK
};

struct entity_handle {
	u16 index;

	explicit entity_handle(u16 index_ = 0xFFFF) : index(index_) { }
};

struct entity {
	entity(entity_type type);
	virtual ~entity();

	virtual void init();
	virtual void tick();
	virtual void draw(draw_context* dc);

	u16           _flags;
	entity_type   _type;
	entity_handle _handle_internal;

	vec2 _pos;
	vec2 _old_pos;
	vec2 _vel;
	f32  _rot;
	f32  _radius;
	rgba _colour;
};

struct player : entity {
	player();

	virtual void init();
	virtual void tick();
	virtual void draw(draw_context* dc);

	bool _firing;
	int _reload_time;
	float _tether_shot_visible;
	int _tether_reload;
};

struct bullet : entity {
	bullet();

	virtual void init();
	virtual void tick();
	virtual void draw(draw_context* dc);

	int _time;
	vec2 _acc;
	float _damp;
};

struct planet : entity {
	planet();

	virtual void init();
	virtual void tick();

	void draw_bg(draw_context* dc);
	void draw_mg(draw_context* dc);
	void draw_mg2(draw_context* dc);
	void draw_fg(draw_context* dc);

	vec2 get_exit_point(vec2 start, vec2 end, float point_radius);

	bool _captured;
	float _pulse;
	float _pulse_t;

	array<entity_handle> _tethered_to;
};

float planet_radius(planet* self, planet* other);

struct tether : entity {
	tether();

	virtual void init();
	virtual void tick();
	virtual void draw(draw_context* dc);

	entity_handle _from;
	entity_handle _to;
};

bool has_tether(entity* a, entity* b);

struct hook : entity {
	hook();

	virtual void init();
	virtual void tick();
	virtual void draw(draw_context* dc);

	int _time;
	entity_handle _from;
};

struct world {
	random r;

	list<entity> entities;
	array<entity*> handles;

	vec2 camera_pos;
	vec2 camera_vel;
	vec2 camera_target;
	vec2 camera_old_target;

	vec2 view_size;
	mat44 view_proj;

	world();
};

extern world g_world;

entity* spawn_entity(entity* e, vec2 initial_pos);
void destroy_entity(entity* e);

entity* get_entity(entity_handle h);
entity_handle get_entity_handle(entity* e);

int entity_move_slide(entity* e);

planet* get_nearest_planet(vec2 pos);
planet* get_nearest_planet_unique(vec2 pos);

void world_tick();
void world_draw(draw_context* dc);

template<typename T> T* spawn_entity(T* e, vec2 initial_pos) {
	return (T*)spawn_entity(static_cast<entity*>(e), initial_pos);
}

template<typename F> void for_all(F&& f) {
	auto& entities = g_world.entities;

	for(int i = 0; i < entities.size(); i++) {
		entity* e = entities[i];

		if (!(e->_flags & EF_DESTROYED))
			f(e);
	}
}

void init_stars();
void update_stars(vec2 delta);
void render_stars(draw_context* dc);