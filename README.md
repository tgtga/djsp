# djsp (Draco's Juggler Sequence Program)

## Description

djsp is a program created to efficiently and easily compute values of the [juggler sequence](https://en.wikipedia.org/wiki/Juggler_sequence). djsp also calculates the juggler sequence in bases other than 2, based on the [extension given by Owen Elton at matheminutes](https://matheminutes.blogspot.com/2019/08/extending-juggler-sequences.html).

## Installation

Compilation is done with `make` and installation is done with `sudo make install`.

## Usage

The main executable is `./djsp.rb`. By default, it will start running infinitely over the base-2 sequence -- currently, `SIGINT` does not kill the program! You will have to send `SIGKILL` or similar.

Help can be accessed with the `-h`/`--help` flag.

## Versioning

Program versions are described by the [Semantic Versioning 2.0.0 protocol](https://semver.org).

## C Interface

`.h` header files are located in `include/` and the corresponding `.c` source files are located, with the same name before the extension, in `src/`.

The functions in `logging.c`, `intmath.c`, and `step.c` are not intended to be used directly. Their interfaces may or may not be documented.

### oneshot.c

Functions in this file are used to calculate the length of sequences starting at a certain "seed" value.

### sequence.c

Functions in this file are used to iterate over all the seed values in a range, calling a provided callback function when the high-water marks of the range are found.

Individual function documentation coming soon...
