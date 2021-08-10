#include "entity.hpp"

namespace ecs {

    entity::id entity::next_eid = { 0 };
    entity::id entity::count    = { 0 };
    // std::mutex   entity::mut   = {};

    entity::entity(context& c)
      : entity_id { next_eid++ }
      , entity_context { c } {
        ++count;
    }

    entity::id entity::get_id() const {
        return entity_id;
    }

    entity::~entity() {
        --count;
        entity_context.remove_entity(*this);
    }

}  // namespace ecs
