#ifndef POTATO_ECS_COMPONENT_HPP
#define POTATO_ECS_COMPONENT_HPP

#include "entity.hpp"
#include "utils.hpp"

namespace ecs {

    class icomponents {
      public:
        icomponents()          = default;
        virtual ~icomponents() = default;

        // no copy
        icomponents(const icomponents&) = delete;
        icomponents& operator=(const icomponents&) = delete;

        // allow move
        icomponents(icomponents&&)   = default;
        virtual icomponents& operator=(icomponents&&) = default;

        virtual void remove(const entity&) = 0;
    };

    template<component_type T>
    class components final : public icomponents {
      public:
        using type = T;

      private:
        using index  = uint32_t;
        using ec_map = std::unordered_map<entity::id, index>;
        using ce_map = std::unordered_map<index, entity::id>;

        ec_map         entity_component {};
        ce_map         component_entity {};
        std::vector<T> items {};

      public:
        components() {
            items.reserve(4096);
        }

        ~components() = default;

        // no copy
        components(const components&) = delete;
        components& operator=(const components&) = delete;

        // allow move
        components(components&&) = default;
        components& operator=(components&&) = default;

        template<typename... Args>
        T& add(const entity& e, Args... args) {
            // add to the vector
            // size - 1 is the index at which our inserted element is stored
            items.emplace_back(std::forward<Args>(args)...);
            auto inx = items.size() - 1;

            auto eid              = e.get_id();
            entity_component[eid] = inx;  // entity e is at index
            component_entity[inx] = eid;  // index contains entity e

            return items[inx];
        }

        T& get(const entity& e) {
            return items[entity_component[e.get_id()]];
        }

        const T& get_const(const entity& e) {
            return items.at(entity_component.at(e.get_id()));
        }

        void remove(const entity& e) override {
            // To remove an entity's component, update the vector such that the
            // last entity in the vector is moved to the hole created by the
            // removed component. While doing so, also update the moved entity's
            // mappings
            auto last_cmp   = items.size() - 1;
            auto last_eid   = component_entity[last_cmp];
            auto remove_eid = e.get_id();
            auto new_inx    = entity_component[remove_eid];

            // move the last entity's component to the hole at index
            // `entity_component[e]`
            items[new_inx] = std::move(items[last_cmp]);

            items.pop_back();

            // Update the entity<->component mapping for the `moved_entity`
            entity_component[last_eid] = new_inx;
            component_entity[new_inx]  = last_eid;

            entity_component.erase(remove_eid);
            component_entity.erase(last_cmp);
        }
    };

}  // namespace ecs

#endif
