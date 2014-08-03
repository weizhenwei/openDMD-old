#!/bin/sh

# This file originates from
# https://github.com/cisco/openh264/blob/master/code-coverage.sh

# lcov -b . -d . -c -o tmp.info
# lcov -e tmp.info \*/codec/\* -o gcov.info
# mkdir -p code-coverage
# genhtml gcov.info -o ./code-coverage
# rm -f tmp.info gcov.info

lcov -b . -d . -c -o gcov.info
mkdir -p code-coverage
genhtml gcov.info -o ./code-coverage
