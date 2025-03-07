# djsp

## DESCRIPTION

djsp is a program created to efficiently and easily compute values of the [juggler sequence](https://en.wikipedia.org/wiki/Juggler_sequence). djsp also calculates the juggler sequence in bases other than 2, based on the [extension given by matheminutes](https://matheminutes.blogspot.com/2019/08/extending-juggler-sequences.html).

It is written in C and Ruby, exposing interfaces through both languages.

## INSTALLATION

Compilation can be performed by running `make`, automatically running tests, and installation can be performed by running `sudo make install`.

## USAGE

`./djsp.rb` is the main executable; running it will provide a list of command-line options and short help for each. All options have both a short and long option supported; long options are able to be provided with both the form `--long option` and `--long=option`, while short options are only able to be provided with `-s option`.

#### `-l`, `--log`

This flag, when provided with a filename, will 
