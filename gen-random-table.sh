#! /bin/sh
echo 'abcdefghijklmnoprstuvwxyz' | fold -w1 |shuf | tr -d '\n' | fold -w5
echo; echo
