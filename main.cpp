#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <array>

#include "variant.hpp"


struct ref_parm {

};

struct gaga {
    void print(ref_parm) {
        int i = 0;
        std::cout << "i'm gaga " << i << std::endl;
    }
    int x = 100;
    float z = 100;
};

struct gaga1 {

    void print(ref_parm) {
        int i = 0;
        std::cout << "i'm gaga1 " << i << std::endl;
    }
    int x = 100;
    float z = 100;
    float *d;
};

struct gaga2 {
    void print(ref_parm) {
        int i = 0;
        std::cout << "i'm gaga2 " << i << std::endl;
    }
    int x = 100;
    float z = 100;
};

void test_call() {
    using types_t = nonstd::variant<gaga, gaga1, gaga2>;
    std::vector<types_t> v = {gaga{}, gaga1{}, gaga2{}};

    for (auto t: v) {
        t.select(std::make_tuple(ref_parm{}), &gaga1::print, &gaga::print, &gaga2::print);
    }
}

void base_test_variant() {
    static_assert(nonstd::is_in<int, float, int, double>(), "not in typelist");

    using types_t = nonstd::variant<bool, int, std::string, float, gaga>;

    types_t _int(10);
    std::cout << _int.get<int>() << std::endl;
    _int.set(100);
    std::cout << _int.get<int>() << std::endl;

    types_t _empty_int;
    _empty_int.set(42);

    std::cout << _empty_int.get<int>() << std::endl;

    //_empty_int.set(std::string("asdf"));

    types_t _other_int = _empty_int;

    std::cout << _other_int.get<int>() << std::endl;
    _other_int.set(300);
    std::cout << _other_int.get<int>() << std::endl;
    assert(_empty_int.get<int>() != _other_int.get<int>());
    std::cout << _empty_int.get<int>() << std::endl;


    types_t d(1.00001f);

    std::cout << d.get<float>() << std::endl;

    types_t _g(gaga{});

    assert(_g.get<gaga>().x == 100);
    assert(_g.get<gaga>().z == 100.0);
    assert(_g.is<gaga>());
}

void test_string() {
    using types_t = nonstd::variant<bool, int, std::string, float, gaga>;

    std::string h("hallo");

    types_t str(h);

    std::cout << str.get<std::string>() << std::endl;

    str.set(std::string("gagammmmehhhl"));

    std::cout << str.get<std::string>() << std::endl;
}

void test_in_array() {
    using types_t = nonstd::variant<bool, int, std::string, float>;
    std::array<types_t, 4> t{{true, false, std::string("hundat"), 3.142f}};

    std::cout << t[0].get<bool>() << std::endl;
    std::cout << t[1].get<bool>() << std::endl;
    std::cout << t[2].get<std::string>() << std::endl;
    std::cout << t[3].get<float>() << std::endl;
}

void test_leak() {
    struct leak {
        leak(const leak &other) {
            std::cout << "copy cons\n";
        }
        leak() {
            array = new int[1000];
            std::cout << "cons\n";
        }
        ~leak() {
            std::cout << "des\n";
            delete[] array;
        }

        leak &operator=(const leak &other) {
            std::cout << "assign\n";
            if (this != &other) {
                delete[] array;
                array = new int[1000];
            }
            return *this;
        }

        int *array = 0;
    };
    leak l;
    nonstd::variant<leak, int, std::string> v;
    v.set<leak>(l);
}

void string_crash() {
    constexpr auto str_size = sizeof(std::string);
    char ptr[str_size];// = {0};
    std::string *str_ptr = (std::string *)ptr; //&empty;
    std::string value("sehr");

    *str_ptr = value;

    std::cout << *str_ptr << std::endl;
}

int main() {
    //string_crash();
    test_leak();
    test_call();
    test_in_array();
    base_test_variant();
    test_string();
    std::cout << "through" << std::endl;
    return 0;
}