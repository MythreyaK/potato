#include "entity.hpp"

namespace ecs {

    entity::id entity::next_eid = { 1 };
    entity::id entity::count    = { 0 };
    // std::mutex   entity::mut   = {};

    entity::entity(context& c)
      : entity_id { next_eid++ }
      , entity_context { &c } {
        ++count;
    }

    entity::id entity::get_id() const {
        return entity_id;
    }

    entity::~entity() {
        if ( entity_id != 0 ) {
            --count;
            entity_context->remove_entity(*this);
        }
        // else entity was moved, deleting this is not needed
    }

    // // allow move
    entity::entity(entity&& other) {
        // when moving entity, do a normal move, but make entity_id 0
        entity_id       = other.entity_id;
        entity_context  = other.entity_context;
        other.entity_id = 0;
    }

    entity& entity::operator=(entity&& other) {
        // when moving entity, do a normal move, but make entity_id 0
        entity_id       = other.entity_id;
        entity_context  = other.entity_context;
        other.entity_id = 0;
        return *this;
    }

}  // namespace ecs
