#!/bin/sh
set -eu

test_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
c_dir=$(CDPATH= cd -- "$test_dir/.." && pwd)
build_dir=$(mktemp -d "${TMPDIR:-/tmp}/mkvautosubset-c-tests.XXXXXX")
trap 'rm -rf -- "$build_dir"' EXIT HUP INT TERM

: "${CC:=cc}"

"$CC" ${CPPFLAGS:-} ${CFLAGS:-} \
    -std=c11 \
    -I"$c_dir" \
    -I"$c_dir/ass2bdnxml" \
    "$test_dir/ass2pgs_crop_test.c" \
    "$c_dir/ass2pgs.c" \
    "$c_dir/s.c" \
    -o "$build_dir/ass2pgs_crop_test" \
    $(pkg-config --cflags --libs libass libpng) \
    ${LDFLAGS:-}

"$build_dir/ass2pgs_crop_test"

"$CC" ${CPPFLAGS:-} ${CFLAGS:-} \
    -std=c11 \
    -I"$c_dir" \
    -I"$c_dir/ass2bdnxml" \
    "$test_dir/image_buffer_sizes_test.c" \
    "$c_dir/ass2pgs.c" \
    "$c_dir/s.c" \
    -o "$build_dir/image_buffer_sizes_test" \
    $(pkg-config --cflags --libs libass libpng) \
    ${LDFLAGS:-}

"$build_dir/image_buffer_sizes_test"

"$CC" ${CPPFLAGS:-} ${CFLAGS:-} \
    -std=c11 \
    -I"$c_dir" \
    -I"$c_dir/ass2bdnxml" \
    "$test_dir/ass2pgs_invalid_resolution_test.c" \
    "$c_dir/ass2pgs.c" \
    "$c_dir/s.c" \
    -o "$build_dir/ass2pgs_invalid_resolution_test" \
    $(pkg-config --cflags --libs libass libpng) \
    ${LDFLAGS:-}

"$build_dir/ass2pgs_invalid_resolution_test"

"$CC" ${CPPFLAGS:-} ${CFLAGS:-} \
    -std=c11 \
    -I"$c_dir" \
    -I"$c_dir/ass2bdnxml" \
    "$test_dir/zero_transparent_test.c" \
    "$c_dir/ass2pgs.c" \
    "$c_dir/s.c" \
    -o "$build_dir/zero_transparent_test" \
    $(pkg-config --cflags --libs libass libpng) \
    ${LDFLAGS:-}

"$build_dir/zero_transparent_test"

"$CC" ${CPPFLAGS:-} ${CFLAGS:-} \
    -std=c11 \
    -I"$c_dir" \
    -I"$c_dir/ass2bdnxml" \
    "$test_dir/palletize_crops_test.c" \
    "$c_dir/ass2bdnxml/palletize.c" \
    -o "$build_dir/palletize_crops_test" \
    ${LDFLAGS:-}

"$build_dir/palletize_crops_test"
