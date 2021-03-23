#include <core/engine.h>

#include "register_types.h"
#include "meshops.h"
#include "town.h"
#include "queue.h"
#include "town_queue.h"

void register_meshops_types()
{
	ClassDB::register_class<DelayedQueue>();
	ClassDB::register_class<TownQueue>();
	ClassDB::register_class<GenExteriorSet>();
	ClassDB::register_class<GenInteriorSet>();
	ClassDB::register_class<GenBuildingSet>();
	ClassDB::register_class<GenRoadSet>();
	ClassDB::register_class<GenCitySet>();
	ClassDB::register_class<MeshItemList>();
	ClassDB::register_class<MeshOps>();
	Engine::get_singleton()->add_singleton(
			Engine::Singleton("MeshOps",
				MeshOps::get_singleton()));
}
void unregister_meshops_types()
{
}

