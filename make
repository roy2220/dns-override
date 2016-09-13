#!/usr/bin/sh

set -e

pushd subhook
cmake -DSUBHOOK_STATIC=1 -DCMAKE_C_FLAGS=-fPIC
make
popd

cc -Isubhook -O2 -shared -fPIC -o libdns-override.so dns_override.c subhook/libsubhook.a
