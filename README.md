ddmd
====

DDMD (Dual Domain Multiscale Denoising) is a complete denoising algorithms
combining Non-Local Bayes, DA3D and the Multiscale Framework.

Website
-------

The support website for this algorithm can be found
[here](http://dev.ipol.im/~pierazzo/CombinedDenoising/).

Building
--------

To compile, use

    $ mkdir build
    $ cd build
    $ cmake .. [-DCMAKE_CXX_COMPILER=/path/of/c++/compiler -DCMAKE_C_COMPILER=/path/of/c/compiler] [-DCMAKE_BUILD_TYPE=Debug]
    $ make

To rebuild, e.g. when the code is modified, use

    $ cd build
    $ make

Using
-----

The building creates three binaries.

* `nl_bayes` is the standard single-scale Non-Local Bayes denoising algorithm.
* `da3d` is the second-step denoising DA3D, that can be applied to any other algorithm.
* `ddmd` is the complete algorithm.

To run, use

    ddmd sigma [input [output]] [-c recomposition factor] [-n scales] [-single file] [-ms file] [-da3d file] [-v]

The options `-single`, `-ms` and `-da3d` can be use to save the partial results. `-c` and `-n` control the parameters of the Multiscale Framework.
