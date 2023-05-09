#!/usr/bin/env sh

TEST_FILES_DIR=test_files

mkdir -p "${TEST_FILES_DIR}"

# sticky bit
mkdir \
    "${TEST_FILES_DIR}/00_sticky_bit_dir_perm" \
    "${TEST_FILES_DIR}/01_sticky_bit_dir_no_perm"

chmod +t \
    "${TEST_FILES_DIR}/00_sticky_bit_dir_perm" \
    "${TEST_FILES_DIR}/01_sticky_bit_dir_no_perm"

chmod -x "${TEST_FILES_DIR}/01_sticky_bit_dir_no_perm"

touch \
    "${TEST_FILES_DIR}/02_sticky_bit_file_perm" \
    "${TEST_FILES_DIR}/03_sticky_bit_file_no_perm"

chmod +t \
    "${TEST_FILES_DIR}/02_sticky_bit_file_perm" \
    "${TEST_FILES_DIR}/03_sticky_bit_file_no_perm"

chmod +x "${TEST_FILES_DIR}/02_sticky_bit_file_perm"
chmod -x "${TEST_FILES_DIR}/03_sticky_bit_file_no_perm"

# setuid
# setgid

# TODO
