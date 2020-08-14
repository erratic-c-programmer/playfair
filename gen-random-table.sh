#! /bin/sh
echo '789:;<=>?@abcdefghijklmnopqrstuvwxyz' | fold -w1 | shuf | tr -d '\n' | fold -w6
echo; echo
