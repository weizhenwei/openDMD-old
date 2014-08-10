#!/bin/sh

# clean gcov info
rm -f gcov.info

#clean result dir
rm -rf code-coverage

#clean temprorary files
find . -name \*.gcno | xargs rm -f
find . -name \*.gcda | xargs rm -f



