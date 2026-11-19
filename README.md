# boxc

boxc is a C-like language transpiled into C.

# Development

> [!IMPORTANT]
> This language is in active development and has no current releases, below are some ideas / dev logs and current
progress. Below code examples, you will see checklists on what is implemented and what is not.
>
> Although it may not look like I have done much in the development of this language, I have made a couple of other
unfinished languages that I have learned from. See,
> - [quark-programming/quark](https://github.com/quark-programming/quark)
> - [ephf/gamma](https://github.com/ephf/gamma)

## Tuples

The main idea for this first round of development is focusing on the type system, generics, and overloading. In order to
do this, I am trying to build out a `tuple` type, similar to C++.

```c++
struct tuple<...>;
```

> - [ ] body-less structures (only on generic types)
> - [ ] spread operator
>   - [x] *partial, using `~` as a placeholder*
> - [ ] nameless unbounded generics

This code declares a tuple structure without a body, it cannot be used in this state to define a variable,

```c++
tuple<> x; // error: tuple<> has no body
```

> - [ ] variable declarations

The tuple struct will have two main overloads that will create a linked-list structure in compile time.

```c++
struct tuple<<First, Items...>> {
    First value;
    tuple<Items> next;
}

struct tuple<> {}
```

> [!NOTE]
> Defining a tuple using `<<>>` instead of `<>` is a shorthand which will create an overload. Normally overloading
requires type arguments and a generics declaration, like the following:
> ```c++
> struct tuple<First, Items><First, Items...>;
> //            |      |      |      |
> //       type arguments    generics declarations
> ```
> This can also be used to mix and match type arguments and generics,
> ```c++
> struct tuple<i32, i32, Items><Items...>;
> // this will overload a tuple that starts with <i32, i32>
> ```

> - [ ] generic overloading
>   - [x] *partial*
> - [ ] generic overloading with `<<>>`

Below is an example of what the compiled code would look like,

```c++
tuple<i32, char, usize> x;
```

```c++
struct tuple {};

struct tuple0 {
    size_t value;
    struct tuple next;
};

struct tuple1 {
    char value;
    struct tuple0 next;
};

struct tuple2 {
    int32_t value;
    struct tuple1 next;
};

struct tuple2 x;
```

You could implement a function similar to `std::get<size_t>` in C++, however there are a couple of things I wanted to
implement that would make this easier.

Here I have defined a trait for the `field_access` operation or `.`

```c++
namespace std::operator {
    trait field_access<Accessor> {
        Field field_access<Field>(self, Accessor);
        Field field_access<Field, Accessor accessor>(self);
    }
}
```

> - [ ] traits
> - [ ] namespaces
>   - [x] *partial, through structs*
> - [x] bounded generics / generics predicates
> - [ ] expression generics
>   - [x] *partial*
> - [ ] function overloading
> - [ ] differentiating function overloading from generic overloading
> - [ ] `self` argument keyword
> - [ ] nameless arguments

The `field_access` trait has two functions that can be overridden. The first one accepts any expression, and the second
one, which takes precedence, takes any compiler constant expression.

We can implement this trait in the tuple type to make it easier to select specific tuple values.

```c++
struct tuple<First, Items><Index : usize, First, Items...> 
    : std::operator::field_access<Index> 
{
    First value;
    tuple<Items> next;
    
    inline auto field_access<First, index><Index index>(self)
        index ? next.field_access<auto, index - 1>() : value;
}
```

> - [x] autos
> - [ ] compiler constant evaluation
>   - [x] *partial, simple literal expressions*
> - [ ] function expanding using `inline`
> - [ ] ternary operator returning tagged union
> - [ ] tagged union flattening in constant evaluation

This will allow us to use the `.` operator to get a specific element in our tuples at compile-time,

```c++
tuple<i32, u32> x;

auto y = x.0;
auto z = z.1;

assert typeof y == i32;
assert typeof z == u32;
```

> - [ ] `typeof` operator
> - [ ] reflections
>   - [x] *partial*
> - [ ] assertions

Overall, I think one of the main things I want in this language is for a lot of the language features to be
implementable from within the language itself instead of relying on syntactic sugar.

## Optionals

Similar to JavaScript's `?.` operator, I wanted to create an easy way to interface with nullable values and optionals.
Below is a definition for an operator override using `?` as an operator.

```c++
namespace std {
    trait nullable_coalesce<NonNull> {
        std::tuple<bool, NonNull> nullable_coalesce(self);
    }
}
```

If we have a value that implements this trait, we can use the `?` operator to conditionally search through a hierarchy
of values.

> [!NOTE]
> You can differentiate the `?` nullish coalescing operator from the `?` ternary operator because the nullish coalescing
operator needs to be followed by a [postfix operator](include/expression/right.hpp) like `.` or `()`.

```c++
struct MyStruct {
    int field;
}

int? get_field<T : std::nullable_coalesce<MyStruct>>(T maybe) {
    return maybe?.field;
}

MyStruct* not_null = &{5};  // referencing an rvalue will stack allocate its value
auto x = get_field(not_null);
auto y = get_field(null);   // null is a keyword similar to nullptr, but it can also be used as a type

assert *x == 5;
assert !y;
```

> - [ ] optional types using `?`
> - [ ] nullish coalescing using `?` followed by an operation
> - [ ] stack allocated rvalues

`int?` is syntactic sugar for `std::option<int>`. Before we write `option`, there are a couple of traits it needs to
override,

```c++
namespace std::operator {
    trait dereference<Type> {
        Type dereference(self);
    }
}

namespace std::trans {
    trait from {
        Self from<From>(From);
    }
    
    trait into {
        Into into<Into>(self);
    }
}
```

> - [ ] `Self` type

Now we can write `option`,

```c++
namespace std {
    struct option<Type> 
        : operator::dereference<Type>, trans::from, trans::into
    {
        Type || null data;
        
        inline Type dereference(self) 
            self.data.(Type);
        
        inline Self from<Type>(Type data)
            Self { data };
        
        inline Self from<null>(null)
            Self { null };
        
        inline bool into(self)
            self.data.is<Type>();
    };
}
```

> - [ ] `||` operator for tagged unions (`|` for untagged)

Overall, even for syntactic sugar, there will be definitions inside of `std` that are being referenced.
