//
// Copyright (C) 2015 Gregor Burger <burger.gregor@gmail.com>
//

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <array>

#include "variant.hpp"

#include <gtest/gtest.h>


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

void test_leak() {
    struct leak {
        leak(const leak &other) {
            (void)other;
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

TEST(VariantTest, StringValues)
{
    using types_t = nonstd::variant<std::string>;

    types_t str(std::string("hallo"));

    EXPECT_EQ(str.get<std::string>(), "hallo");
    str.set(std::string("gagammmmehhhl"));
    EXPECT_EQ(str.get<std::string>(), "gagammmmehhhl");
}

TEST(VariantTest, InArray)
{
    using types_t = nonstd::variant<bool, int, std::string, float, double>;
    std::array<types_t, 5> t{{true, false, std::string("hundret"), 3.142f, 3.142}};

    ASSERT_TRUE(t[0].is<bool>());
    ASSERT_TRUE(t[1].is<bool>());
    ASSERT_TRUE(t[2].is<std::string>());
    ASSERT_TRUE(t[3].is<float>());
    ASSERT_TRUE(t[4].is<double>());

    ASSERT_EQ(t[0].get<bool>(), true);
    ASSERT_EQ(t[1].get<bool>(), false);
    ASSERT_EQ(t[2].get<std::string>(), "hundret");
    ASSERT_EQ(t[3].get<float>(), 3.142f);
    ASSERT_EQ(t[4].get<double>(), 3.142);
}

TEST(VariantTest, Empty)
{
    nonstd::variant<int> v;
    ASSERT_TRUE(v.empty());
    v.set(100);
    ASSERT_FALSE(v.empty());
}

TEST(VariantTest, BaseTests)
{
    using types_t = nonstd::variant<bool, int, std::string, float, test_class>;

    types_t _int(10);
    ASSERT_EQ(_int.get<int>(), 10);
    _int.set(100);
    ASSERT_EQ(_int.get<int>(), 100);

    types_t _empty_int;
    _empty_int.set(42);
    ASSERT_EQ(_empty_int.get<int>(), 42);

    types_t _other_int = _empty_int;

    ASSERT_EQ(_other_int.get<int>(), 42);
    _other_int.set(300);
    ASSERT_EQ(_other_int.get<int>(), 300);
    ASSERT_EQ(_empty_int.get<int>(), 42);
    ASSERT_NE(_other_int.get<int>(), _empty_int.get<int>());


    types_t d(1.00001f);

    ASSERT_EQ(d.get<float>(), 1.00001f);

    types_t _g(test_class{});

    ASSERT_EQ(_g.get<test_class>().x, 100);
    ASSERT_EQ(_g.get<test_class>().z, 100.0);
    ASSERT_TRUE(_g.is<test_class>());
}


void int_cb(int &x) {
    x = 42;
}

void float_cb(float &x) {
    x = 42.42;
}

void str_cb(std::string &x) {
    x = "fortytwo";
}


TEST(VariantTest, SwitchFuncPointer)
{
    using types_t = nonstd::variant<int, float, std::string>;
    std::vector<types_t> v = {10, 10.10f, std::string("ten")};

    ASSERT_EQ(v[0].get<int>(), 10);
    ASSERT_EQ(v[1].get<float>(), 10.10f);
    ASSERT_EQ(v[2].get<std::string>(), "ten");

    for (auto & t: v) {
        t.select<int, float, std::string>(int_cb, float_cb, str_cb);
    }

    ASSERT_EQ(v[0].get<int>(), 42);
    ASSERT_EQ(v[1].get<float>(), 42.42f);
    ASSERT_EQ(v[2].get<std::string>(), "fortytwo");
}

TEST(VariantTest, SwitchLambda)
{
    using types_t = nonstd::variant<int, float, std::string>;
    std::vector<types_t> v = {10, 10.10f, std::string("ten")};

    ASSERT_EQ(v[0].get<int>(), 10);
    ASSERT_EQ(v[1].get<float>(), 10.10f);
    ASSERT_EQ(v[2].get<std::string>(), "ten");

    for (auto & t: v) {
        t.select<int, float, std::string>(
                [](int &x){x = 42;},
                [](float &x){x =  42.42f;},
                [](std::string &x){ x = "fortytwo";}
        );
    }

    ASSERT_EQ(v[0].get<int>(), 42);
    ASSERT_EQ(v[1].get<float>(), 42.42f);
    ASSERT_EQ(v[2].get<std::string>(), "fortytwo");
}

TEST(VariantTest, Select)
{
    using types_t = nonstd::variant<test_class, test_class_1, test_class_2, test_class_3>;
    std::vector<types_t> v = {test_class{}, test_class_1{}, test_class_2{}, test_class_3{}};
    ASSERT_FALSE(v[0].get<test_class>().called);
    ASSERT_FALSE(v[1].get<test_class_1>().called);
    ASSERT_FALSE(v[2].get<test_class_2>().called);
    ASSERT_FALSE(v[3].get<test_class_3>().called);

    for (auto & t: v) {
        auto expected_ret = t.is<test_class_3>() ? 0 : -10;
        auto expected_i = t.is<test_class_3>() ? 42 : -42;
        auto args = std::make_tuple(pod_parm{});
        ASSERT_EQ(std::get<0>(args).i, 42);
        ASSERT_EQ(t.select(args, &test_class::print, &test_class_1::print, &test_class_2::print), expected_ret);
        ASSERT_EQ(std::get<0>(args).i, expected_i);
    }

    ASSERT_TRUE(v[0].get<test_class>().called);
    ASSERT_TRUE(v[1].get<test_class_1>().called);
    ASSERT_TRUE(v[2].get<test_class_2>().called);
    ASSERT_FALSE(v[3].get<test_class_3>().called);
}

TEST(IsIn, IsInTests)
{
    ASSERT_FALSE((nonstd::is_in<int>()));
    ASSERT_FALSE((nonstd::is_in<int, float>()));
    ASSERT_TRUE((nonstd::is_in<int, int>()));
    ASSERT_TRUE((nonstd::is_in<int, float, double, std::string, test_class, test_class_1, test_class_2, int>()));
    ASSERT_TRUE((nonstd::is_in<test_class_2, float, double, std::string, test_class, test_class_1, test_class_2>()));
}

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}