#! /bin/sh
echo ' !"#$%&'\''()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[\\]^_' | fold -w1 | shuf | tr -d '\n' | fold -w8
echo; echo
