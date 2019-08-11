# ARSE (ARSEs Representing Strings Editorially)

ARSE is a GPL3+ text editor library to be utilized by any kind of frontend.

The text editor uses a piece chain structure at its core to represent the initial string and the changes made during a session.

## Cool Features

- Slave-editors: By making a part of the buffer being represented by yet another ARSE instance, you can link multiple identical strings to the same ARSE-instance and have changes reflect upon both of them at the same time. Using some finaggling with the internal structures you can theoretically build the structure to represent recursive acronyms in this library (Such as its name), however it will never be able to build the string to return for you for obvious reasons. (Implemented partially)
- Line-by-line undo/redo: Each line is running its own editor and thus keeps track of their history separately. This can be (ab)used, how useful it is is a different question however.

## Dependencies
None as of yet.

## Building & Installing
Just run `make release` to create the library. Run `make install` if you want it installed into /usr/lib/ as well as arse.h put into /usr/include/. Edit the make file recipe if you want different paths or install it manually.
