/*
@copyright Louis Dionne 2014
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#include <boost/hana/list/list.hpp>

#include <boost/hana/benchmark/measure.hpp>

<%= setup %>

template <int i>
struct x { };

int main() {
    using L = <%= datatype %>;

    boost::hana::benchmark::measure([] {
        boost::hana::make<L>(
            <%= (1..n).to_a.map { |i| "x<#{i}>{}" }.join(', ') %>
        );
    });
}
