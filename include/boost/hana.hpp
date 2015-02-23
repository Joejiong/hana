/*!
@file
Includes all the library components except the adapters for external
libraries.


@todo
- To consider: is it possible/desirable to eliminate partial functions?
  For example, removing `head` and `tail`, which can fail, and replace
  them by a function which returns a `Maybe`.
- Document how to write common Boost.Fusion and Boost.MPL idioms with
  Boost.Hana.
- Setup a BJam build system.

@copyright Louis Dionne 2014
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_HPP
#define BOOST_HANA_HPP

// Concepts
#include <boost/hana/applicative.hpp>
#include <boost/hana/comparable.hpp>
#include <boost/hana/constant.hpp>
#include <boost/hana/enumerable.hpp>
#include <boost/hana/foldable.hpp>
#include <boost/hana/functor.hpp>
#include <boost/hana/group.hpp>
#include <boost/hana/integral_domain.hpp>
#include <boost/hana/iterable.hpp>
#include <boost/hana/logical.hpp>
#include <boost/hana/monad.hpp>
#include <boost/hana/monad_plus.hpp>
#include <boost/hana/monoid.hpp>
#include <boost/hana/orderable.hpp>
#include <boost/hana/product.hpp>
#include <boost/hana/record.hpp>
#include <boost/hana/ring.hpp>
#include <boost/hana/searchable.hpp>
#include <boost/hana/sequence.hpp>
#include <boost/hana/traversable.hpp>

// Data types
#include <boost/hana/bool.hpp>
#include <boost/hana/either.hpp>
#include <boost/hana/integral_constant.hpp>
#include <boost/hana/lazy.hpp>
#include <boost/hana/map.hpp>
#include <boost/hana/maybe.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/range.hpp>
#include <boost/hana/set.hpp>
#include <boost/hana/string.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/type.hpp>

// Misc
#include <boost/hana/assert.hpp>
#include <boost/hana/config.hpp>
#include <boost/hana/core.hpp>
#include <boost/hana/functional.hpp>


// This is a bit stupid, but putting the documentation in the `boost::hana`
// namespace makes Doxygen resolve references like `Iterable` properly.
// Otherwise, we would need to write `boost::hana::Iterable` everytime.
namespace boost { namespace hana {

//! @defgroup group-concepts Concepts
//! Concepts provided by the library.

//! @defgroup group-datatypes Data types
//! General purpose data types provided by the library.

//! @defgroup group-functional Functional
//! General purpose function objects.
//!
//! In the documentation of these utilities, a simplified implementation is
//! often provided in pseudo-code using lambdas. The reason is that the actual
//! implementation is often contrived because of limitations with lambdas in
//! the current language. Here is an explanation of some terms that appear in
//! the pseudo-code:
//!
//! ### `forwarded(x)`
//! Means that the object is forwarded optimally. This means that if `x` is a
//! parameter, it is `std::forward`ed, and if it is a captured variable, it is
//! moved from whenever the lambda is an rvalue.
//!
//! Also note that when `x` can be moved from, the statement
//! `return forwarded(x);` in a function with `decltype(auto)` does not mean
//! that an rvalue reference to `x` will be returned, which would create a
//! dangling reference. Rather, it means that `x` is returned by value, the
//! value being constructed with the `std::forward`ed `x`.
//!
//! ### `perfect-capture`
//! Means that the captured variables are initialized using perfect
//! forwarding, as if `[x(forwarded(x))...]() { }` had been used.

//! @defgroup group-core Core
//! Core utilities of the library.

//! @defgroup group-config Configuration options
//! Configurable options to tweak the global behavior of the library.

//! @defgroup group-assertions Assertions
//! Macros to perform different kinds of assertions.

//! @defgroup group-details Details
//! Implementation details.

/*!
@mainpage User Manual

@tableofcontents


@section tutorial-preface Preface

------------------------------------------------------------------------------
The seed that became this library was planted in late 2012, when I first
started to reimplement the [Boost.MPL][] using C++11 in a project named
[MPL11][]. In spring 2014, I applied to [Google Summer of Code][GSoC] with
that project for the Boost organization and got in. The goal was to polish
the MPL11 and get it in Boost by the end of the summer. In May, before GSoC
was started full steam, I presented the project at [C++Now][] and had
insightful conversations with several attendees. The idea that it was
possible to unify the [Boost.Fusion][] and the [Boost.MPL][] libraries
made its way and I became convinced of it after writing the first prototype
for what is now Boost.Hana. After working on Hana and polishing many rough
edges during several months, Hana will soon go through informal and then
formal reviews with the goal of being part of Boost.

Let the fun begin.


@section tutorial-introduction Introduction

------------------------------------------------------------------------------
Hana is a small (~ 13 KLOC), header-only replacement for Boost.MPL and
Boost.Fusion, with more features as well. It boasts faster compile time
performance than Boost.MPL or Boost.Fusion, and runtime performance on par
or better than Boost.Fusion. It provides a unified way of manipulating types
and values for a smoother metaprogramming experience. It is easy to extend in
a ad-hoc manner and it provides out-of-the-box inter-operation with Boost.Fusion,
Boost.MPL and the standard library. Hana is written against the C++14 standard.


__Motivation__\n
For users that want to do extensive metaprogramming, Boost.MPL is clearly a
very valuable tool. Its greatest weakness is that its liberal use leads to
long compile times. For users that want to compute on sequences of
heterogeneously-typed values, Boost.Fusion is also a very valuable tool.
It also suffers from long compile times when used extensively.

The C++11 and C++14 standards have made many metaprogramming tasks a lot
easier and more straightforward to accomplish. Hana thus provides the
functionality of Boost.MPL and Boost.Fusion with more straightforward
(and thus more compile-time efficient) implementations.


__Warning: functional programming ahead__\n
Programming with heterogeneous objects is inherently functional -- since it is
impossible to modify the type of an object, a new object must be introduced
instead, which rules out mutation. Unlike previous metaprogramming libraries
like Boost.MPL and Boost.Fusion, I have decided to embrace this and as a
result, Hana uses an almost purely functional style of programming. However,
this style of programming influences not only the internal implementation; it
also leaks into the interface. Hence, if you continue beyond this point, be
prepared to see quite a bit of functional programming along the way.

In particular, unlike previous metaprogramming libraries, the design of Hana
is not based on that of the STL. Instead, it is inspired by several standard
and non standard modules written for the Haskell programming language. Through
experience, I have found this to be much more expressive, flexible and easy
to use while not sacrificing any performance given the heterogeneously typed
setting. However, as a result, many concepts used in Hana will be unfamiliar
to C++ programmers without a knowledge of FP, to whom I say: be assured that
the reward is totally worth it.


@section tutorial-quickstart Quick start

------------------------------------------------------------------------------
This tutorial assumes the reader is already familiar with basic metaprogramming
and the [C++14 standard][Wikipedia.C++14]. First, let's include the library:

@snippet example/tutorial/quickstart.cpp includes

Unless specified otherwise, the documentation assumes that the above lines
are present before examples and code snippets. Also note that finer grained
headers are provided and will be explained in the [Header organization]
(@ref tutorial-header_organization) section. If you are reading this
documentation, chances are you already know `std::tuple` and `std::make_tuple`.
Hana provides its own `tuple` and `make_tuple` implementations:

@snippet example/tutorial/quickstart.cpp tuple_auto

`make<Tuple>` is the usual way of creating a tuple in Hana. Simply put, `make`
is used all around Hana to create different types of objects, thus generalizing
the `std::make_xxx` family of functions. You have probably observed how the
`auto` keyword is used when defining `xs`; it is often useful to let the
compiler deduce the type of a tuple, but sometimes it is necessary to specify
it. This is of course also possible:

@snippet example/tutorial/quickstart.cpp tuple_type

Hana also provides several basic operations and higher level algorithms that
can be performed on heterogeneous sequences. For example, here are a couple
of basic operations; the higher level algorithms will be explained later:

@snippet example/tutorial/quickstart.cpp operations

An interesting observation in the previous example is that `is_empty` and
`length` both return a constant expression, even though the tuple they were
called on is not a constant expression (it couldn't be one because it
contains a `std::string`). Indeed, the size of the sequence is known at
compile-time regardless of its contents, so it only makes sense that Hana
does not throw away this information. If that seems surprising, think about
`std::tuple`:

@snippet example/tutorial/quickstart.cpp std_tuple_size

Since the size of the tuple is encoded in it's type, it's always available
at compile-time regardless of whether the tuple is `constexpr` or not. How
this works in Hana will be explained in detail later, but the trick is that
`is_empty` returns something like a `std::integral_constant`, which can
always be converted to an integral value at compile-time. Hana also provides
high level algorithms to manipulate tuples and other heterogeneous containers.
One example is `transform` (analogous to `std::transform`), which takes a
sequence and a function, maps the function over each element of the sequence
and returns the result in a new sequence:

@snippet example/tutorial/quickstart.cpp transform

Unlike with Boost.Fusion, Hana algorithms always return a new sequence
containing the result. Instead, Boost.Fusion returned views which held
the original sequence by reference. This could lead to subtle lifetime
issues, which can be avoided by using value semantics. Hana uses perfect
forwarding heavily to ensure the best runtime performance possible. Other
useful algorithms worth mentionning are:

@snippet example/tutorial/quickstart.cpp algorithms

There are many more operations and algorithms that can be performed on
sequences; they are documented in their respective concept (Foldable,
Iterable, Searchable, Sequence, etc...).


@subsection tutorial-quickstart-cheatsheet Cheatsheet

For quick reference, here's a cheatsheet of the most useful functions and
algorithms. Always keep in mind that the algorithms return their result as
a new sequence and no in-place mutation is ever performed.

function                                     |  concept   | description
:------------------------------------------: | :--------: | :----------:
`make<Tuple>(x1, ..., xN)`                   | None       | Returns a tuple containing the given elements.
`transform(sequence, f)`                     | Functor    | Apply a function to each element of a sequence and return the result.
`adjust(sequence, predicate, f)`             | Functor    | Apply a function to each element of a sequence satisfying some predicate and return the result.
`replace(sequence, predicate, value)`        | Functor    | Replace the elements of a sequence that satisfy some predicate by some value.
`fill(sequence, value)`                      | Functor    | Replace all the elements of a sequence with some value.
`foldl(sequence, state, f)`                  | Foldable   | Accumulates the elements of a sequence from the left. Equivalent to `f(...f(f(state, x1), x2), ..., xN)`.
`foldr(sequence, state, f)`                  | Foldable   | Accumulates the elements of a sequence from the right. Equivalent to `f(x1, f(x2, ..., f(xN, state))...)`.
`for_each(sequence, f)`                      | Foldable   | Call a function on each element of a sequence. Returns `void`.
`length(sequence)`                           | Foldable   | Returns the length of a sequence as an `integral_constant`.
`{minimum, maximum}_by(predicate, sequence)` | Foldable   | Returns the smallest/greatest element of a sequence w.r.t. a predicate.
`{minimum, maximum}_by(predicate, sequence)` | Foldable   | Returns the smallest/greatest element of a sequence. The elements must be Orderable.
`count(sequence, predicate)`                 | Foldable   | Returns the number of elements that satisfy the predicate.
`unpack(sequence, f)`                        | Foldable   | Calls a function with the contents of a sequence. Equivalent to `f(x1, ..., xN)`.
`head(sequence)`                             | Iterable   | Returns the first element of a sequence.
`tail(sequence)`                             | Iterable   | Returns all the elements except the first one. Analogous to `pop_front`.
`is_empty(sequence)`                         | Iterable   | Returns whether a sequence is empty as an `integral_constant`.
`at(index, sequence)`                        | Iterable   | Returns the n-th element of a sequence. The index must be an `integral_constant`.
`last(sequence)`                             | Iterable   | Returns the last element of a sequence.
`drop(number, sequence)`                     | Iterable   | Drops the n first elements from a sequence and returns the rest. `n` must be an `integral_constant`.
`drop_{while,until}(sequence, predicate)`    | Iterable   | Drops elements from a sequence while/until a predicate is satisfied. The predicate must return an `integral_constant`.
`flatten(sequence)`                          | Monad      | Flatten a sequence of sequences, a bit like `std::tuple_cat`.
`prepend(value, sequence)`                   | MonadPlus  | Prepend an element to a sequence.
`append(sequence, value)`                    | MonadPlus  | Append an element to a sequence.
`concat(sequence1, sequence2)`               | MonadPlus  | Concatenate two sequences.
`filter(sequence, predicate)`                | MonadPlus  | Remove all the elements that do not satisfy a predicate. The predicate must return an `integral_constant`.
`{any,none,all}(sequence, predicate)`        | Searchable | Returns whether any/none/all of the elements of the sequence satisfy some predicate.
`{any,none,all}_of(sequence)`                | Searchable | Returns whether any/non/all of the elements of a sequence are true-valued.
`elem(sequence, value)`                      | Searchable | Returns whether an object is in a sequence.
`find(sequence, predicate)`                  | Searchable | Find the first element of a sequence satisfying the predicate and return `just` it, or return `nothing`. See Maybe.
`lookup(sequence, value)`                    | Searchable | Find the first element of a sequence which is equal to some value and return `just` it, or return nothing. See Maybe.
`group_by(predicate, sequence)`              | Sequence   | %Group the adjacent elements of a sequence which all satisfy (or all do not satisfy) some predicate.
`group(sequence)`                            | Sequence   | %Group adjacent elements of a sequence that compare equal. The elements must be Comparable.
`init(sequence)`                             | Sequence   | Returns all the elements of a sequence, except the last one. Analogous to `pop_back`.
`partition(sequence, predicate)`             | Sequence   | Partition a sequence into a pair of elements that satisfy some predicate, and elements that do not satisfy it.
`remove_at(index, sequence)`                 | Sequence   | Remove the element at the given index. The index must be an `integral_constant`.
`reverse(sequence)`                          | Sequence   | Reverse the order of the elements in a sequence.
`slice(sequence, from, to)`                  | Sequence   | Returns the elements of a sequence at indices contained in `[from, to)`.
`sort_by(predicate, sequence)`               | Sequence   | Sort the elements of a sequence according to some predicate. The sort is stable.
`sort(predicate)`                            | Sequence   | Sort the elements of a sequence, which must be Orderable. The sort is stable.
`take(number, sequence)`                     | Sequence   | Take the first n elements of a sequence. n must be an `integral_constant`.
`take_{while,until}(sequence, predicate)`    | Sequence   | Take elements of a sequence while/until some predicate is satisfied, and return that.
`zip(sequence1, ..., sequenceN)`             | Sequence   | Zip `N` sequences into a sequence of tuples.
`zip.with(f, sequence1, ..., sequenceN)`     | Sequence   | Zip `N` sequences with a `N`-ary function.


@section tutorial-heterogeneity Heterogeneity and generalized types

------------------------------------------------------------------------------
The purpose of Hana is to manipulate heterogeneous objects. Before we dive any
deeper into the library, let's ask a fundamental question: does it even make
sense to manipulate heterogeneous objects?

For the sake of the explanation, let me make the following claim: a function
template that compiles with an argument of every possible type must have a
trivial implementation, in the sense that it must do nothing with its argument
except perhaps return it. Hence, for a function template to do something
interesting, it must fail to compile for some set of arguments. While I won't
try to prove that claim formally -- it might be false in some corner cases --,
think about it for a moment. Let's say I want to apply a function to each
element of an heterogeneous sequence:

@code
    for_each([x, y, z], f)
@endcode

The first observation is that `f` must have a templated call operator because
`x`, `y` and `z` have different types. The second observation is that without
knowing anything specific about the types of `x`, `y` and `z`, it is impossible
for `f` to do anything meaningful. For example, could it print its argument?
Of course not, since it does not know whether `std::cout << x` is well-formed!
In order to do something meaningful, the function has to put constraints on
its arguments, it has to define a domain which is more specific that the set
of all types. In other words, it can't be _fully_ polymorphic, at least not
conceptually.

So while we're manipulating types that are technically heterogeneous, they
still conceptually need something in common, or it wouldn't be possible to
do anything meaningful with them. We'll still say that we're manipulating
heterogeneous objects, but always keep in mind that the objects we manipulate
must have something in common, and hence be homogeneous in _some regard_.

Pushing this to the extreme, some type families represent exactly the same
entity, except they must have a different C++ type because the language
requires them to. For example, this is the case of `_tuple<...>`. In our
context, we would like to see `_tuple<int, int>` and `_tuple<int, char, float>`
as different representations for the same data structure (a "tuple"), but the
C++ language requires us to give them different types. In Hana, we associate
what we call a _generalized type_ (we also say _data type_) to each type family.
A generalized type is simply a tag (like in MPL or Fusion) which is associated
to all the types in a family through the `datatype` metafunction. For
`_tuple<...>`, this generalized type is `Tuple`; other constructs in Hana
also follow this convention of naming their generalized type with a capital
letter. This tag is useful for several purposes, for example creating a tuple
with `make<Tuple>`. Another important role of generalized types is to customize
algorithms; see the section on [tag-dispatching](@ref tutorial-extending-tag_dispatching)
for more information. Finally, you can also consult the reference of the
[datatype](@ref datatype) metafunction for details on how to specify the
generalized type of a family.


@section tutorial-type_computations Type computations

------------------------------------------------------------------------------
At this point, if you are interested in doing Boost.MPL-like computations on
types, you might be wondering how is Hana going to help you. Do not despair.
Hana provides a way to perform type-level computations with a great deal of
expressiveness by representing types as values. This is a completely new way
of metaprogramming, and you should try to set your MPL habits aside for a bit
if you want to become proficient with Hana. Basically, Hana provides a way of
representing a type `T` as an object, and it also provides a way of applying
type transformations to those objects as-if they were functions, by wrapping
them properly. Concretely;

@snippet example/tutorial/type_computations.cpp type

@note
The `type<int>` expression is _not_ a type! It is an object, more precisely a
[variable template][Wikipedia.variable_template] defined roughly as
@code
    template <typename T>
    constexpr the-type-of-the-object type{};
@endcode

Now, since `type<...>` is just an object, we can store it in a heterogeneous
sequence like a tuple. This also means that all the algorithms that apply to
usual heterogeneous sequences are available to us, which is nice:

@snippet example/tutorial/type_computations.cpp type_sequence

Also, since typing `type<...>` can be annoying at the end of the day, Hana
provides a variable template called `tuple_t`, which creates a tuple of
`type<...>`s:

@snippet example/tutorial/type_computations.cpp tuple_t

I won't say much more about `type` and `metafunction` (see the [reference]
(@ref Type)), but the last essential thing to know is that `decltype(type<T>)`
is a MPL nullary metafunction. In other words, `decltype(type<T>)::%type` is
an alias to `T`:

@snippet example/tutorial/type_computations.cpp type_as_nullary_metafunction

This way, you can recover the result of a type computation by unwrapping it
with `decltype(...)::%type`. Hence, doing type-level metaprogramming with Hana
is usually a three step process:
1. Wrap the types with `type<...>` so they become values
2. Apply whatever type transformation `F` by using `metafunction<F>`
3. Unwrap the result with `decltype(...)::%type`

At this point, you must be thinking this is incredibly cumbersome. Why on
earth would you want to write

@snippet example/tutorial/type_computations.cpp type_three_step_cumbersome

instead of simply writing

@snippet example/tutorial/type_computations.cpp type_three_step_alternative

The answer is that of course you don't! For simple type computations such as
this one, where you know the type transformation and the type itself, just
use the most straightforward way of doing it. However, for more complex
type computations, the syntactic noise of this three step process becomes
negligible in light of the expressiveness gain of working with values instead
of types inside that computation. Indeed, since we're working inside real
functions instead of clunky structs, we can use variables, lambdas and a
reasonable syntax. It also means that we don't need `typename` all around
the place, which is nice (but can be avoided in MPL world with aliases).
But this syntactic unification is not only some sugar for our eyes; it also
means that some of our "metafunctions" will actually also work on normal
values. Hence, any piece of generic enough code will work with both types
and values out-of-the-box, without any extra work on our side. With previous
approaches to type-level static metaprogramming, we had to reimplement type
sequences and basically everything else from the ground up. For example,
consider the simple problem of applying a transformation to each element in
a sequence of sequences. For some metafunction `f` and sequence

@code
    [
        [x1, x2, ..., xN],
        [y1, y2, ..., yM],
        ...
        [z1, z2, ..., zK]
    ]
@endcode

we want to produce a sequence

@code
    [
        [f<x1>::type, f<x2>::type, ..., f<xN>::type],
        [f<y1>::type, f<y2>::type, ..., f<yM>::type],
        ...
        [f<z1>::type, f<z2>::type, ..., f<zK>::type]
    ]
@endcode

With Hana, this is very straightforward, and in fact the resulting algorithm
will even work when given a regular function and values instead of a
metafunction and types:

@snippet example/tutorial/type_computations.cpp apply_to_all

> `BOOST_HANA_CONSTANT_CHECK` is essentially equivalent to `static_assert`;
> see the [reference](@ref BOOST_HANA_CONSTANT_ASSERT) for more information.

However, expressing the same algorithm using Boost.MPL requires using lambda
expressions, which are much more limited than plain lambdas. It also requires
being at class or global scope, which means that you can't create this
algorithm on the fly (e.g. inside a function), and it will only work on types!

@snippet example/tutorial/type_computations.cpp apply_to_all_mpl


@todo
Write a cheatsheet for common MPL idioms. Same for Fusion idioms, but where
should it go? %Maybe in some Appendix?


@section tutorial-constant Constants, or setting constexpr straight

------------------------------------------------------------------------------
In C++, the border between compile-time and runtime is hazy, a fact that is
even more true with the introduction of [generalized constant expressions]
[Wikipedia.generalized_constexpr] in C++11. However, being able to manipulate
heterogeneous objects is all about understanding that border and then crossing
it at one's will. The goal of this section is to set things straight with
`constexpr`; to understand which problems it can solve and which ones it
can't. Let's start off with a [GOTW][] style question: do you think the
following code should compile, and why?

@code
    template <typename T>
    constexpr T assert_positive(T i) {
        static_assert(i > 0, "");
        return i;
    }

    int main() {
        constexpr int i = 2;
        assert_positive(i);
    }
@endcode

The right answer is "no". The error given by Clang goes like

@code
    error: static_assert expression is not an integral constant expression
        static_assert(i > 0, "");
                      ^~~~~
@endcode

The problem is that when you are in a function (`constexpr` or otherwise), you
can't use an argument as a constant expression, even if the argument turns out
to be a constant expression when you call the function. If you are surprised,
consider the following code; the function is not a template anymore, which
doesn't change anything with respect to the `constexpr`-ness of the argument
if you think about it. Yet, the answer is now obvious:

@code
    constexpr int assert_positive(int i) {
        static_assert(i > 0, "");
        return i;
    }

    int main() {
        constexpr int i = 2;
        assert_positive(i);
    }
@endcode

Actually, since the body of `assert_positive` is not dependent anymore, Clang
does not even reach `main` before giving the same error as before. Now, this
is not a big problem for static assertions because we have [another way]
[constexpr_throw] of reporting errors inside `constexpr` functions. However,
it also means that we can't use an argument as a non-type template parameter,
since that requires a constant expression. In other words, we can't create
types that are dependent on the _value_ of an argument in C++, which is
nothing new if you think about it:

@code
    template <int i>
    struct foo { };

    void f(int i) {
        foo<i> x; // obviously won't work
    }
@endcode

In particular, this means that the return type of a function can't depend on
the value of its arguments; it may only depend on their type, and `constexpr`
can't change this fact. This is of utmost importance to us, because we're
interested in manipulating heterogeneous objects and eventually returning them
from functions. Some of these functions might want to return an object of type
`T` in one case and an object of type `U` in the other; from our little
analysis, we now know that these "cases" will have to depend on information
encoded in the _types_ of the arguments, not in their _values_.

To represent this fact, Hana defines the concept of a `Constant`, which is an
object from which a constant expression may always be obtained, regardless of
the `constexpr`-ness of the object. `Constant`s provide a way to obtain that
constant expression through the use of the `value` function. Specifically, for
any `Constant` `c`, the following program must be valid:

@code
    template <typename X>
    void f(X x) {
        constexpr auto y = value(x);
    }

    int main() {
        f(c);
    }
@endcode

This law that must be respected by `Constant`s expresses the minimal
requirement that we're able to retrieve a constant expression from an object,
even if that object isn't a constant expression. There is no restriction on
what the type of the constant expression might be, but it should be documented.
Hana provides a model of this concept called an `IntegralConstant`; it encodes
a compile-time value of an integral type, and you can think of it as a
`std::integral_constant`. Before going on to the next section, you probably
want to take a look at the [reference documentation](@ref IntegralConstant)
for `IntegralConstant`, which explains how to create these objects and what
you can expect from them.


@subsection tutorial-constant-side_effects Side effects


@note
Before reading this section, you should be familiar with the Constant concept.
Also note that this section contains somewhat advanced material, and it can
safely be skipped during a first read.


Let me ask a tricky question. Is the following code valid?

@code
    template <typename X>
    auto identity(X x) { return x; }

    static_assert(value(identity(bool_<true>)), "");
@endcode

The answer is "no", but the reason might not be obvious at first. Even more
puzzling is that the following code is perfectly valid:

@snippet example/tutorial/constant_side_effects.cpp pure

To understand why the compiler can't possibly evaluate the first assertion
at compile-time, notice that `identity` was not marked `constexpr` and
consider the following alternative (but valid) definition for `identity`:

@snippet example/tutorial/constant_side_effects.cpp impure_identity

The signature of the function did not change; the function could even have
been defined in a separate source file. However, it is now obvious that the
compiler can't evaluate that expression at compile-time. On the other hand,
when we write

@snippet example/tutorial/constant_side_effects.cpp impure

we're telling the compiler to perform those potential side effects during the
dynamic initialization phase! Then, we use `value` to return the compile-time
value associated to its argument. Also note that `value` takes a `const&` to
its argument; if it tried taking it by value, we would be reading from a
non-`constexpr` variable to do the copying, and that could hide side-effects.


@section tutorial-external_libraries Integration with external libraries

------------------------------------------------------------------------------

@subsection tutorial-external_libraries-stl The standard library

@subsection tutorial-external_libraries-fusion Boost.Fusion

@subsection tutorial-external_libraries-mpl Boost.MPL


@section tutorial-performance_considerations Performance considerations

------------------------------------------------------------------------------

@subsection tutorial-performance_considerations-compile_time Compile-time performance

@subsection tutorial-performance_considerations-run_time Runtime performance



@section tutorial-extending Extending the library

------------------------------------------------------------------------------
Because of its modular design, Hana can be extended in a ad-hoc manner very
easily. Actually, all the functionality of the library is provided through
this ad-hoc customization mechanism.


@subsection tutorial-extending-tag_dispatching Tag dispatching

Tag dispatching is a generic programming technique for picking the right
implementation of a function depending on the type of the arguments passed
to the function. The usual mechanism for overriding a function's behavior
is overloading. Unfortunately, this mechanism is not always convenient when
dealing with families of related types having different C++ types, or with
objects of unspecified types as is often the case in Hana. For example,
consider trying to overload a function for all Boost.Fusion vectors:

@code
    template <typename ...T>
    void function(boost::fusion::vector<T...> v) {
        // whatever
    }
@endcode

If you know Boost.Fusion, then you probably know that it won't work. This is
because Boost.Fusion vectors are not necessarily specializations of the
`boost::fusion::vector` template. Fusion vectors also exist in numbered
forms, which are all of different types:

@code
    boost::fusion::vector1<T>
    boost::fusion::vector2<T, U>
    boost::fusion::vector3<T, U, V>
    ...
@endcode

This is an implementation detail required by the lack of variadic templates in
C++03 that leaks into the interface. This is unfortunate, but we need a way to
work around it. To do so, we use an infrastructure with three distinct
components:

1. A metafunction associating a single tag to every type in a family of
   related types. In Hana, we use the generalized type of an object,
   which is accessible through the `datatype` metafunction.

2. A function belonging to the public interface of the library, for which
   we'd like to be able to provide a customized implementation.

3. An implementation for the function, parameterized with the data type(s)
   of the argument(s) passed to the function.

When the public interface function is called, it will use the metafunction
on its argument(s) (or a subset thereof) to obtain their data type(s) and
redirect to the implementation associated to those data type(s). For example,
a basic setup for tag dispatching of a function that prints its argument to a
stream would look like:

@snippet tutorial/tag_dispatching.cpp setup

Then, if you want to customize the behavior of the `print` function for some
user defined family of types, you only need to specialize the `print_impl`
template for the tag representing the whole family of types:

@snippet tutorial/tag_dispatching.cpp customize


@subsection tutorial-extending-creating_concepts Creating new concepts


@section tutorial-header_organization Header organization

------------------------------------------------------------------------------
The library is designed to be modular while keeping the number of headers that
must be included to get basic functionality reasonably low. The structure of
the library was also intentionally kept simple, because we all love simplicity.

- `boost/hana.hpp`\n
  This is the master header of the library. It includes the whole public
  interface of the library except adapters for external libraries, which
  must be included separately.

- `boost/hana/`\n
  This is the main directory of the library containing the definitions of
  concepts and data types. A file of the form `boost/hana/[XXX].hpp` contains
  the definition for the concept or data type named `XXX`.

  - `boost/hana/core/`\n
    This subdirectory contains the machinery for tag-dispatching and other
    related utilities like `make` and `to`.

  - `boost/hana/fwd/`\n
    This subdirectory contains the forward declaration of every concept
    and data type in the library. Basically, `boost/hana/fwd/[XXX].hpp`
    is the forward declaration for the concept or data type named `XXX`.

  - `boost/hana/functional/`\n
    This subdirectory contains various function objects that are often useful,
    but that do not necessarily belong to a concept.

  - `boost/hana/ext/`\n
    This directory contains adapters for external libraries. This is the only
    part of the public interface which is not included by the master header,
    because that would make the master header dependent on those external
    libraries. Note that only the strict minimum required to adapt the
    external components is included in these headers (e.g. a forward
    declaration). This means that the definition of the external component
    should still be included when one wants to use it. For example:

    @snippet example/tutorial/include_ext.cpp main

  - `boost/hana/sandbox/`\n
    This directory contains experimental code on which no guarantee whatsoever
    is made. It might not even compile and it will definitely not be stable.

  - `boost/hana/detail/`\n
    This directory contains utilities required internally. Nothing in `detail/`
    is guaranteed to be stable, so you should not use it.


@section tutorial-using_the_reference Using the reference

------------------------------------------------------------------------------
You now have everything you need to start using the library. From here on,
mastering the library is only a matter of understanding and knowing how to
use the general purpose concepts and data types provided with it, which
is best done by looking at the reference documentation. At some point, you
will probably also want to create your own concepts and data types that fit
your needs better; go ahead, the library was intended to be used that way.

The structure of the reference (available in the menu to the left) goes as
follow:
  - @ref group-core\n
    Documentation for the core module, which contains everything needed to
    create concepts, data types and related utilities. This is relevant
    if you need to extend the library, but otherwise you can probably
    ignore this.

  - @ref group-functional\n
    General purpose function objects that are generally useful in a purely
    functional setting. These are currently not tied to any concept or data
    type.

  - @ref group-concepts\n
    Documentation for all the concepts provided with the library. Each concept:
    - Documents laws that are internal to this concept
    - Documents the concept(s) it is derived from, if any. In the documentation,
      we usually call those base concepts _superclasses_. Sometimes, a concept
      is powerful enough to provide a model of its superclass, or at least the
      implementation for some of its methods. When this is the case, the
      concept will document which superclass methods it provides, and how
      it does so. Also, it is sometimes possible that the model for a
      superclass is unique, in which case it can be provided automatically.
      When this happens, it will be documented but you don't have to do
      anything special to get that model.
    - Documents which methods must be implemented absolutely in order to
      model that concept.

  - @ref group-datatypes\n
    Documentation for all the data types provided with the library. Each
    data type documents the concept(s) it models, and how it does so. It
    also documents the methods tied to that data type but not to any concept,
    for example `make<Tuple>`.

  - @ref group-config\n
    Macros that can be used to tweak the global behavior of the library.

  - @ref group-assertions\n
    Macros to perform various types of assertions.

  - @ref group-details\n
    Implementation details. Don't go there.

I hope you enjoy using the library, and please consider contributing since
there is still a lot of work to do!

-- Louis


<!-- Links -->
[Boost.Fusion]: http://www.boost.org/doc/libs/release/libs/fusion/doc/html/index.html
[Boost.MPL]: http://www.boost.org/doc/libs/release/libs/mpl/doc/index.html
[C++Now]: http://cppnow.org
[GSoC]: http://www.google-melange.com/gsoc/homepage/google/gsoc2014
[MPL11]: http://github.com/ldionne/mpl11
[constexpr_throw]: http://stackoverflow.com/a/8626450/627587
[GOTW]: http://www.gotw.ca/gotw/index.htm

[Wikipedia.C++14]: http://en.wikipedia.org/wiki/C%2B%2B14
[Wikipedia.CXX14_udl]: http://en.wikipedia.org/wiki/C%2B%2B11#User-defined_literals
[Wikipedia.generic_lambda]: http://en.wikipedia.org/wiki/C%2B%2B14#Generic_lambdas
[Wikipedia.variable_template]: http://en.wikipedia.org/wiki/C%2B%2B14#Variable_templates
[Wikipedia.generalized_constexpr]: http://en.wikipedia.org/wiki/C%2B%2B11#constexpr_.E2.80.93_Generalized_constant_expressions

 */

}} // end namespace boost::hana

#endif // !BOOST_HANA_HPP
