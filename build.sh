#!/usr/bin/env sh

set -xe

gcc -Wall -Wextra -pedantic -O3 -o dlx dlx.c
