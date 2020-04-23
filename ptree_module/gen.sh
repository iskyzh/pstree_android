#!/bin/bash

set -e

var=$(cat $KID/System.map | grep sys_call_table)
vars=( $var )

cat > gen.h <<EOF
#define ${vars[2]}_addr 0x${vars[0]}
EOF
