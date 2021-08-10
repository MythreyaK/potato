#ifndef POTATO_ECS_UTILS_HPP
#define POTATO_ECS_UTILS_HPP

#include <concepts>

namespace ecs {
    // clang-format off
    template<uint64_t a>
    concept power_of_2 = std::same_as<
        std::integral_constant<bool, (a & a - 1) == 0 && (a != 0)>,
        std::true_type
    >;

    template<typename T>
    concept component_type = power_of_2<T::signature>;

    template <typename T>
    concept component_store =
      std::is_member_function_pointer<decltype(&T::add)> &&
      std::is_member_function_pointer<decltype(&T::remove)>;
    // clang-format on

    // forward declares just cause

    class entity;
    class context;
    class icomponents;

    template<component_type T>
    class components;

}  // namespace ecs

#endif
