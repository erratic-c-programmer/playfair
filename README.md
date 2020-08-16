# Extended playfair implementation in C
## Features:
 - Adjustable table width and height
 - Table can include any number of characters, but they must be sequential (see `man ascii`).
 - Includes table generator. Use like this: `./gen-random-table.sh > <tablefile>`. Requires shuf, tr and fold to work.

Compile by running `make`.
For help, run `./playfair -h` after compiling.
