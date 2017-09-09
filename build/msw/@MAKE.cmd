rem mingw32-make -j4 -f makefile.gcc SHARED=1 UNICODE=1 BUILD=release clean
rem mingw32-make -j4 -f makefile.gcc SHARED=1 UNICODE=1 BUILD=debug clean

mingw32-make -j4 -f makefile.gcc SHARED=1 UNICODE=1 BUILD=release
mingw32-make -j4 -f makefile.gcc SHARED=1 UNICODE=1 BUILD=debug

pause
