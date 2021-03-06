# Variant

[![Build Status](https://travis-ci.org/gregorburger/variant.svg)](https://travis-ci.org/gregorburger/variant)

 * Typesafe tagged union (variant)
 * Small and simple (below 250 lines C++ code)
 * C++14 required (decltype(auto))
 * visitation for member functions, lambdas and function pointers
 * visitation using perfect forwarded arguments
 * Variant can be empty and could possibly be retyped
 * Stack allocated
 * No dependencies (gtest for tests)
 * Works on clang (3.5), gcc (4.9), MSVC (14)
 * No MACRO voodoo
 * Supports pure value semantics
 
# Basic Features:
```C++
using types_t = nonstd::variant<int, float, std::string>;
types_t _int(42);

std::cout << _int.get<int>() << "\n"; //prints 42
_int.set<int>(100);

double d = _int.get<double>(); //won't compile
bool is_double = _int.is<double>(); //is_double = true

types_t _empty;
bool is_empty = _empty.empty(); //is_empty = true
_empty.set<std::string>(std::string("Hello, World!");
```

# Advanced Features:

## Select on Callback:
This is like a switch case statement
```C++
using types_t = nonstd::variant<int, float, std::string>;
std::vector<types_t> v = {10, 10.10f, std::string("ten")};

for (auto & t: v) {
    t.select<int, float, std::string>(
            [](int &x){x = 42;},
            [](float &x){x = 42.42f;},
            [](std::string &x){x = "fortytwo";}
    );
}
```

## Select on member function:
This is like inheritance without inheritance
```C++
//test classes defined below

using types_t = nonstd::variant<test_class, test_class_1, test_class_2, test_class_3>;
std::vector<types_t> v = {test_class{}, test_class_1{}, test_class_2{}, test_class_3{}};

//call member functions
for (auto & t: v) {
    auto args = std::make_tuple(pod_parm{});
    t.select(args, &test_class::print, &test_class_1::print, &test_class_2::print);
}

//visitation using perfect forwarding
for (auto & t: v) {
    pod_parm arg;
    t.visit(&test_class::print, &test_class_1::print, &test_class_2::print)(arg);
}

//test classes
struct pod_parm {
    int i = 42;
};

struct test_class {
    test_class() : x(100), z(100.0f) {}
    int x;
    float z;
    int print(pod_parm &_p) {
        called = true;
        _p.i = -_p.i;
        return -10;
    }
    bool called = false;
};

struct test_class_1 {
    int print(pod_parm &_p) {
        called = true;
        _p.i = -_p.i;
        return -10;
    }
    bool called = false;
};

struct test_class_2 {
    int print(pod_parm &_p) {
        called = true;
        _p.i = -_p.i;
        return -10;
    }
    bool called = false;
};

struct test_class_3 {
    int print(pod_parm &_p) {
        called = true;
        _p.i = -_p.i;
        return -10;
    }
    bool called = false;
};
```
