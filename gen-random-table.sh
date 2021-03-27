#! /bin/sh
echo ' !"#$%&'\''()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~' | fold -w1 | shuf | tr -d '\n' | fold -w12
echo; echo
