#include <core/engine.h>

#include "register_types.h"
#include "voronoi.h"

void register_voronoi_types()
{
	ClassDB::register_class<Voronoi>();
	Engine::get_singleton()->add_singleton(
			Engine::Singleton("Voronoi",
				Voronoi::get_singleton()));
}
void unregister_voronoi_types()
{
}

