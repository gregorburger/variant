//
// Copyright (C) 2015 Gregor Burger <burger.gregor@gmail.com>
//

#ifndef VARIANT_VARIANT_H
#define VARIANT_VARIANT_H

#include <typeindex>
#include <tuple>

namespace nonstd {

    template <typename A>
    constexpr size_t max_size()  {
        return sizeof(A);
    }

    template <typename A, typename B, typename ... Args>
    constexpr size_t max_size() {
        return max_size<A>() > max_size<B, Args...>() ? max_size<A>() : max_size<B, Args...>();
    }

    template <typename Check>
    constexpr bool is_in() {
        return false;
    };

    template <typename Check, typename Head>
    constexpr bool is_in() {
        return std::is_same<Check, Head>::value;
    };

    template <typename Check, typename Head, typename Next, typename... Types>
    constexpr bool is_in() {
        return is_in<Check, Head>() ? true : is_in<Check, Next, Types...>();
    };

    template <class> struct holder_type;//undefined

    template <typename Ret, typename Class, typename... Args>
    struct holder_type<Ret(Class::*)(Args...)> {
        typedef Class type;
    };

    template <class> struct return_type;//undefined

    template <typename Ret, typename Class, typename... Args>
    struct return_type<Ret(Class::*)(Args...)> {
        typedef Ret type;
    };

    template<typename... Types>
    struct variant {

        variant() : info(typeid(empty_type)) {}

        template<typename T>
        variant(const T &t) : info(typeid(T)) {
            static_assert(is_in<T, Types...>(), "not in types");
            new (b) T(t);
        }

        ~variant() {
            _delete<Types...>();
        }

        template <typename T>
        T &get() const {
            static_assert(is_in<T, Types...>(), "not in types");
            assert(is<T>());
            return *(T*) b;
        }

        template <typename T>
        void set(const T &other) {
            static_assert(is_in<T, Types...>(), "not in types");
            if (empty()) {
                info = typeid(T);
                new (b) T();
            } 
            _delete<Types...>();
            assert(is<T>());

            T *_t = (T*)b;
            *_t = other;
        }

        bool empty() const {
            return info == typeid(empty_type);
        }

        template <typename T>
        bool is() const {
            static_assert(is_in<T, Types...>(), "not in types");
            return std::type_index(typeid(T)) == info;
        }

        template<typename htype, typename Func, typename Tup, std::size_t... index>
        decltype(auto) invoke_helper(htype *ptr, Func&& func, Tup&& tup, std::index_sequence<index...>) {
            return (ptr->*func)(std::get<index>(std::forward<Tup>(tup))...);
        }

        template <typename Tup, typename Callback>
        decltype(auto) select(Tup &&args, Callback &&cb) {
            using htype = typename holder_type<Callback>::type;
            using ret_type = typename return_type<Callback>::type;

            if (std::type_index(typeid(htype)) != info) {
                return ret_type(); //throw here?
            }

            constexpr auto size = std::tuple_size<typename std::decay<decltype(args)>::type>::value;
            htype *ptr = (htype *)b;
            return invoke_helper(ptr, cb, std::forward<Tup>(args), std::make_index_sequence<size>{});
        }

        template <typename Tup, typename Callback, typename... TailCallbacks>
        decltype(auto) select(Tup &&args, Callback&& cb, TailCallbacks&& ...cbs) {
            using htype = typename holder_type<Callback>::type;
            if (is<htype>()) {
                return select(std::forward<Tup>(args), std::forward<Callback>(cb));
            } else {
                return select(std::forward<Tup>(args), std::forward<TailCallbacks>(cbs)...);
            }
        }

        template <typename T, typename CallbackT>
        void select(CallbackT &&f) {
            static_assert(is_in<T, Types...>(), "type not in variant");
            if (is<T>()) {
                f(get<T>());
            }
        }

        template <typename T, typename... Ts, typename CallbackT, typename... CallbackTs>
        void select(CallbackT&& cb, CallbackTs&& ...cbs) {
            static_assert(sizeof...(Ts) + 1 == sizeof...(CallbackTs) + 1, "not the same size");
            select<T>(std::forward<CallbackT>(cb));
            select<Ts...>(std::forward<CallbackTs>(cbs)...);
        }


    private:
        template <typename T>
        bool _delete() {
            if (empty())
                return true;
            if (!is<T>())
                return false; 

            ((T*)b)->~T();
            return true;
        }

        template <typename T, typename Thead, typename... Ts>
        void _delete() {
            if (empty()) 
                return;
            if (_delete<T>())
                return;
            _delete<Thead, Ts...>();
        }
        struct empty_type {};

        char b[max_size<Types...>()] = {0};
        std::type_index info = typeid(empty_type);
    };
}


#endif //VARIANT_VARIANT_H
