About
=====

This is a static recompilation tool for Android applications with native
components. It may be usefull if you have Android device with CPU of
different architecture (MIPS for example), don't have sources of app,
but still want it running. Utility helps to convert ARM binaries to MIPS ones.
Or any other arch, since it creates C code, which is then compiled with ndk-build.

It is not complete utility that one could just use as a black box. It's
more like framework (without usual framework properties like code quality :-) ).
If you dare to use it, you will have to dig into it. And of course you
have to understand how Android NDK works.


Copying
=======

You may use this code under LGPLv3 terms.
