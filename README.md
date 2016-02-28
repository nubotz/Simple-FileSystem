# Simple-FileSystem

Hello there.

1) Download the HD file and put it in the same directory as the source code.

2) Compile the code:
gcc mkfs.c -o mkfs
gcc tshell.c -o tshell

3) Run the mkfs first to make superblock and root directory:
./mkfs

4) Run the tshell:
./tshell

5) try those commands:
a) ls_t
b) mkdir_t /abc
c) cd_t /abc
d) external_cp ./outsidefile.txt /text.txt
e) cp_t /text.txt /abc/text2.txt

Remarks:
Bascially all the commands are fulfilled.
open_t, read_t, write_t functions are in the "sfs.h" to be included in tshell.c and mkfs.c
Fork is used in tshell.
The time displayed is UNIX time in ls_t, those human readable format is unneccessary.

I have to admit that i_size of directory might have some problems and there are some more bugs existed.

Thanks for reading. Wish you have a nice day.
