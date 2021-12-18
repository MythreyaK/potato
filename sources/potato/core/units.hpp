#ifndef POTATO_CORE_UNITS_HPP
#define POTATO_CORE_UNITS_HPP

#include <concepts>
#include <ratio>

namespace {
    template<typename T>
    requires std::integral<T>
    constexpr size_t pow2(T a) {
        return 1ull << a;
    }
}  // namespace

namespace units {

    // comment
    namespace data {
        template<typename rep, typename ratio = std::ratio<1, 1>>
        class size {
          private:
            rep m_size { 0 };

          public:
            constexpr size() = default;

            explicit constexpr size(rep n)
              : m_size { (n * ratio::num) / ratio::den } {}

            constexpr rep count() const {
                return m_size;
            }

            // allow for implicit conversion from size to rep
            constexpr operator rep() {
                return m_size;
            }
        };

        using bit  = std::ratio<1, 1>;
        using byte = std::ratio<pow2(3), 1>;
        using kilo = std::ratio<pow2(10), 1>;
        using mega = std::ratio<pow2(20), 1>;
        using giga = std::ratio<pow2(30), 1>;
        using peta = std::ratio<pow2(40), 1>;

    }  // namespace data
}  // namespace units

namespace units::literals {

    // Kilobyte
    constexpr data::size<size_t, data::kilo>
    operator"" _kb(unsigned long long m) {
        return data::size<size_t, data::kilo> { m };
    }

    constexpr data::size<size_t, data::mega>
    operator"" _mb(unsigned long long m) {
        return data::size<size_t, data::mega> { m };
    }

    // Gigabyte
    constexpr data::size<size_t, data::giga>
    operator"" _gb(unsigned long long m) {
        return data::size<size_t, data::giga> { m };
    }
}  // namespace units::literals

#endif
