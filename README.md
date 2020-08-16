# Extended playfair implementation in C
## Features:
 - Adjustable table width and height
 - Table can include any number of characters, but they must be sequential (see `man ascii`). One advantage of this is that non-alphabetical characters are not only preserveed, but also encoded. Case is also preserved.
 - Includes table generator. Use like this: `./gen-random-table.sh > <tablefile>`. Requires shuf, tr and fold to work
 - Adds 'XX' to end of encypted message if the last letter is a space
 - Adds 'X' to the end of a message if it has an odd length
 - Allows double letters
 - Dynamic message length
 - Likely faster as it does not loop through the table for every letter

Compile by running `make`.
For help, run `./playfair -h` after compiling.
