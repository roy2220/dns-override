#!/usr/bin/env bash

set -o errexit -o nounset -o pipefail

pushd subhook
cmake -DSUBHOOK_STATIC=1 -DCMAKE_C_FLAGS=-fPIC
make
popd

cc -Isubhook -O2 -shared -fPIC -o libdns-override.so dns-override.c subhook/libsubhook.a
