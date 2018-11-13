Tweeners++ is a tweener library written in C++.

- [Building](#building)
- [Usage](#usage)
- [Testing](#testing)
- [Benchmark](#benchmark)

# Building

The library is header-only so you can just copy the `include` folder
where you need it. For a more typical install you may want to run
`cmake <source root>/cmake` and `make install`.

The following arguments can be passed to `cmake` 

- `-DTWEENERS_CMAKE_PACKAGE_ENABLED=ON/OFF` controls whether the CMake
  script allowing to import the library in another project must be
  installed or not. Default is `ON`.
- `-DTWEENERS_TESTING_ENABLED=ON/OFF` controls the build of the unit
  tests. Default value is `ON`. You will need
  [Google Test](https://github.com/google/googletest) for this.
- `-DTWEENERS_DEMO_ENABLED=ON/OFF` controls the build of a sample
  program. Default value is `OFF`. You will need
  [SDL 2](https://www.libsdl.org/index.php) for this.
- `-DTWEENERS_BENCHMARKING_ENABLED=ON/OFF` controls the build of a
  benchmark program to compare the library with other C++ tweeners
  libraries. It will download
  [cpptweener](https://github.com/devmario/CppTweener.git) and, if
  [Boost](https://www.boost.org) is available, it will also download
  [Claw](https://github.com/j-jorge/libclaw.git) and compare to
  them. Default is `OFF`.
- `-DTWEENERS_DEBUG=ON/OFF` controls the activation of debugging code
  in the library. It is a developer feature, you probably won't want
  it. Default is `OFF`.

# Usage

For a minimal working program, instantiate a `tweeners::system` to
handle your tweeners then use a `tweeners::builder` to create the
tweeners. Finally call `tweeners::system::update( time_step )` to
update your tweeners.

```c++
#include <tweeners/builder.hpp>
#include <tweeners/easing.hpp>
#include <tweeners/system.hpp>

#include <iostream>

int main()
{
  int x;
  constexpr int from( 0 );
  constexpr int to( 100 );
  constexpr float duration( 10 );

  tweeners::system system;

  tweeners::builder()
    .range_transform( from, to, duration, x, &tweeners::easing::sine )
    .build( system );
    
  for( int i( 0 ); i != 10; ++i )
    {
      system.update( 1.f );
      std::cout << i << " -> " << x << '\n';
    }

  return 0;
}
```

See the code for details about these types.

# Customization points

`tweeners::system` is actually an alias for `tweeners::system_base< Config >`
where `Config` carries some of the key types used by `tweeners::base_system`.
The same goes for `tweeners::builder` and `tweeners::builder_base`.

`Config` must define four types as listed below, see
[the comments in source for their meaning](include/tweeners/config.hpp):

```c++
struct custom_config
{
  using duration_type = /* ... */;
  using id_type = /* ... */;
  using float_type = /* ... */;
  using function_type = /* ... */;
};
```

# Testing

Once the unit tests are built, run `<build dir>/products/tests/tweeners-test`.

# Benchmark

The benchmark compares the library with other C++ tweener libraries,
namely Claw and CppTweener for tweener update performance.

First N tweeners of various durations are created. Half of them are
followed by another tweener, then half of these ones are also followed
by another one, and so on.

The time needed to update all the tweeners until they are all
completed is measured and reported.

The number of tweeners, their duration and the time step can be
configured on the command line. Run
`<build dir>/products/benchmarks/benchmarks --help` for details.

Performance comparison relatively to Tweeners++ is shown in the table
below. N is the number of tweeners in the first iteration. The
durations are chosen in `{5, 20, 10, 0, 30}`, the update step
is 3. The fastest of ten runs is kept for the comparison.

|      N | cpptweener |   claw  |
| ------ | ---------- | ------- |
|     10 |    0.61    |    1.13 |
|    100 |    0.98    |   10.98 |
|   1000 |    5.22    |  145    |
|  10000 |   50.43    | 1892    |
| 100000 |  679       | N.A.    |
