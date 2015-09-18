# Variant

 * Typesafe tagged union 
 * "don't call us, we'll call you" semantics
 * Variant can be empty and could possibly be retyped
 * Stack allocated

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
```C++

```
