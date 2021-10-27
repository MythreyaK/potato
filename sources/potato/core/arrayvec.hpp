#ifndef POTATO_CORE_DS_ARRAYVEC_HPP
#define POTATO_CORE_DS_ARRAYVEC_HPP

#include <array>
#include <compare>
#include <iterator>

namespace potato::core {

    // std::array if fine, but sometimes we want to iterate only
    // over the elements that are "valid", and not default-constructed
    // This class helps with this usecase, so that user doesn't need to keep
    // track of number of "valid" elements in the array

    template<typename T, size_t N>
    class arrayvec_const_iterator {
      public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = const T;
        using pointer           = const T*;
        using reference         = const T&;

      private:
        pointer m_ptr {};

      public:
        explicit arrayvec_const_iterator(pointer ptr)
          : m_ptr { ptr } {}

        constexpr arrayvec_const_iterator() noexcept
          : m_ptr() {}

        [[nodiscard]] constexpr reference operator*() const noexcept {
            return *m_ptr;
        }

        [[nodiscard]] constexpr pointer operator->() const noexcept {
            return m_ptr;
        }

        constexpr arrayvec_const_iterator& operator++() noexcept {
            ++m_ptr;
            return *this;
        }

        constexpr arrayvec_const_iterator operator++(int) noexcept {
            arrayvec_const_iterator tmp = *this;
            ++m_ptr;
            return tmp;
        }

        constexpr arrayvec_const_iterator& operator--() noexcept {
            --m_ptr;
            return *this;
        }

        constexpr arrayvec_const_iterator operator--(int) noexcept {
            arrayvec_const_iterator tmp = *this;
            --m_ptr;
            return tmp;
        }

        constexpr arrayvec_const_iterator&
        operator+=(const difference_type offset) noexcept {
            m_ptr += offset;
            return *this;
        }

        constexpr arrayvec_const_iterator&
        operator-=(const difference_type offset) noexcept {
            m_ptr -= offset;
            return *this;
        }

        [[nodiscard]] constexpr arrayvec_const_iterator
        operator+(const difference_type offset) const noexcept {
            arrayvec_const_iterator tmp = *this;
            tmp += offset;
            return *tmp;
        }

        [[nodiscard]] constexpr arrayvec_const_iterator
        operator-(const difference_type offset) const noexcept {
            arrayvec_const_iterator tmp = *this;
            tmp -= offset;
            return *tmp;
        }

        [[nodiscard]] constexpr difference_type
        operator-(const arrayvec_const_iterator& other) const noexcept {
            assert(m_ptr >= other.m_ptr);
            return m_ptr - other.m_ptr;
        }

        [[nodiscard]] constexpr reference
        operator[](const difference_type offset) const noexcept {
            return m_ptr[offset];
        }

        // TODO: Why does removing this and the next break <=> ?
        bool operator!=(const arrayvec_const_iterator& other) const noexcept {
            return m_ptr != other.m_ptr;
        }

        bool operator==(const arrayvec_const_iterator& other) const noexcept {
            return m_ptr == other.m_ptr;
        }

        [[nodiscard]] std::strong_ordering
        operator<=>(const arrayvec_const_iterator& other) const noexcept {
            return m_ptr <=> other.m_ptr;
        }
    };

    template<typename T, size_t N>
    class arrayvec_iterator final : public arrayvec_const_iterator<T, N> {
      public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

      private:
        using base = arrayvec_const_iterator<T, N>;

      public:
        explicit arrayvec_iterator(pointer ptr)
          : base { ptr } {}

        constexpr reference operator*() {
            return const_cast<reference>(base::operator*());
        }

        constexpr pointer operator->() {
            return const_cast<pointer>(base::operator->());
        }

        constexpr reference operator[](const difference_type d) {
            return const_cast<reference>(base::operator[](d));
        }

        constexpr arrayvec_iterator& operator++() {
            base::operator++();
            return *this;
        }

        constexpr arrayvec_iterator operator++(int) {
            arrayvec_iterator tmp { *this };
            base::            operator++();
            return *tmp;
        }

        constexpr arrayvec_iterator& operator--() {
            base::operator--();
            return *this;
        }

        constexpr arrayvec_iterator operator--(int) {
            arrayvec_iterator tmp { *this };
            base::            operator--();
            return *tmp;
        }

        constexpr arrayvec_iterator& operator+=(const difference_type d) {
            base::operator+=(d);
            return *this;
        }

        constexpr arrayvec_iterator& operator-=(const difference_type d) {
            base::operator-=(d);
            return *this;
        }

        [[nodiscard]] constexpr arrayvec_iterator
        operator-(const ptrdiff_t offset) const noexcept {
            arrayvec_iterator tmp { *this };
            return tmp += offset;
        }

        [[nodiscard]] constexpr arrayvec_iterator
        operator+(const difference_type offset) const noexcept {
            arrayvec_iterator tmp { *this };
            return tmp += offset;
        }
    };

    // Like a vector, but constant compile-time size
    template<typename T, size_t N>
    class arrayvec {
      private:
        T elems[N] {};
        using iterator               = arrayvec_iterator<T, N>;
        using const_iterator         = arrayvec_const_iterator<T, N>;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        size_t max_size { N };
        size_t curr_size { 0 };

      public:
        [[nodiscard]] size_t size() const noexcept {
            return curr_size;
        }

        [[nodiscard]] constexpr size_t capacity() const noexcept {
            return max_size;
        }

        [[nodiscard]] constexpr T& front() noexcept {
            return elems[0];
        }

        [[nodiscard]] constexpr const T& front() const noexcept {
            return elems[0];
        }

        [[nodiscard]] constexpr T& back() noexcept {
            return elems[curr_size];
        }

        [[nodiscard]] const T& back() const noexcept {
            return elems[curr_size];
        }

        [[nodiscard]] constexpr T* data() noexcept {
            return elems;
        }

#pragma region ITER_BASE
        [[nodiscard]] constexpr iterator begin() noexcept {
            return iterator { elems };
        }

        [[nodiscard]] constexpr iterator end() noexcept {
            return iterator { elems + curr_size };
        }

        [[nodiscard]] constexpr const_iterator begin() const noexcept {
            return const_iterator { elems };
        }

        [[nodiscard]] constexpr const_iterator end() const noexcept {
            return const_iterator { elems + curr_size };
        }
#pragma endregion ITER_BASE
#pragma region    ITER_BASE_REVERSE

        [[nodiscard]] constexpr reverse_iterator rbegin() noexcept {
            return reverse_iterator { end() };
        }

        [[nodiscard]] constexpr reverse_iterator rend() noexcept {
            return reverse_iterator { begin() };
        }

        [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator { end() };
        }

        [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept {
            return const_reverse_iterator { begin() };
        }
#pragma endregion ITER_BASE_REVERSE
#pragma region    ITER_BASE_CONST_REVERSE

        [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
            return const_iterator { elems };
        }

        [[nodiscard]] constexpr const_iterator cend() const noexcept {
            return const_iterator { end() };
        }

        [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept {
            return rbegin();
        }

        [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept {
            return rend();
        }
#pragma endregion ITER_BASE_CONST_REVERSE

        [[nodiscard]] constexpr const T* data() const noexcept {
            return elems;
        }

        [[nodiscard]] T& operator[](size_t inx) noexcept {
            return elems[inx];
        }

        [[nodiscard]] const T& operator[](size_t inx) const noexcept {
            assert(inx < curr_size);
            return elems[inx];
        }

        [[nodiscard]] T& at(size_t inx) noexcept {
            assert(inx < curr_size);
            return elems[inx];
        }

        [[nodiscard]] const T& at(size_t inx) const {
            assert(inx < curr_size);
            return elems[inx];
        }

        void push(const T& o) {
            assert(curr_size + 1 < max_size);
            elems[curr_size++] = o;
        }

        T pop() {
            return elems[curr_size--];
        }

        bool emplace_back(T&& o) noexcept {
            if ( curr_size == max_size ) return false;
            elems[curr_size++] = std::move(o);
        }
    };

    // template<class _Ty, size_t _Size>
    // class arrayvec_iterator : public arrayvec_const_iterator<_Ty, _Size> {
    //   public:
    //     using _Mybase = arrayvec_const_iterator<_Ty, _Size>;
    //     using iterator_concept = contiguous_iterator_tag;

    //     using iterator_category = random_access_iterator_tag;
    //     using value_type        = _Ty;
    //     using difference_type   = difference_type;
    //     using pointer           = _Ty*;
    //     using reference         = _Ty&;

    //     enum { _EEN_SIZE = _Size };  // helper for expression evaluator

    //     constexpr arrayvec_iterator() noexcept {}

    //     constexpr explicit arrayvec_iterator(pointer _Parg,
    //                                           size_t  offset = 0) noexcept
    //       : _Mybase(_Parg, offset) {}

    //     [[nodiscard]] constexpr reference operator*() const noexcept {
    //         return const_cast<reference>(_Mybase::operator*());
    //     }

    //     [[nodiscard]] constexpr pointer operator->() const noexcept {
    //         return const_cast<pointer>(_Mybase::operator->());
    //     }

    //     constexpr arrayvec_iterator& operator++() noexcept {
    //         _Mybase::operator++();
    //         return *this;
    //     }

    //     constexpr arrayvec_iterator operator++(int) noexcept {
    //         arrayvec_iterator tmp = *this;
    //         _Mybase::       operator++();
    //         return tmp;
    //     }

    //     constexpr arrayvec_iterator& operator--() noexcept {
    //         _Mybase::operator--();
    //         return *this;
    //     }

    //     constexpr arrayvec_iterator operator--(int) noexcept {
    //         arrayvec_iterator tmp = *this;
    //         _Mybase::       operator--();
    //         return tmp;
    //     }

    //     constexpr arrayvec_iterator& operator+=(const difference_type offset)
    //     noexcept {
    //         _Mybase::operator+=(offset);
    //         return *this;
    //     }

    //     [[nodiscard]] constexpr arrayvec_iterator
    //     operator+(const difference_type offset) const noexcept {
    //         arrayvec_iterator tmp = *this;
    //         return tmp += offset;
    //     }

    //     constexpr arrayvec_iterator& operator-=(const difference_type offset)
    //     noexcept {
    //         _Mybase::operator-=(offset);
    //         return *this;
    //     }

    //     using _Mybase::operator-;

    //     [[nodiscard]] constexpr arrayvec_iterator
    //     operator-(const difference_type offset) const noexcept {
    //         arrayvec_iterator tmp = *this;
    //         return tmp -= offset;
    //     }

    //     [[nodiscard]] constexpr reference
    //     operator[](const difference_type offset) const noexcept {
    //         return const_cast<reference>(_Mybase::operator[](offset));
    //     }

    //     using _Prevent_inheriting_unwrap = arrayvec_iterator;

    //     [[nodiscard]] constexpr pointer _Unwrapped() const noexcept {
    //         return const_cast<pointer>(_Mybase::_Unwrapped());
    //     }
    // };

}  // namespace potato::core

#endif
