CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=matlab_rtwutil.cpp matlab_initialize.cpp matlab_terminate.cpp arclengthcurve.cpp rdivide.cpp cubic_spline3.cpp cubic_spline.cpp mldivide.cpp colon.cpp adaptivegaus.cpp builddetailedbf.cpp parevalspline.cpp matlab_emxutil.cpp matlab_emxAPI.cpp rt_nonfinite.cpp rtGetNaN.cpp rtGetInf.cpp
OBJECTS=$(SOURCES:.cpp=.o)
OUT=libmatlab.a
	
$(OUT): $(OBJECTS) 
	ar rcs $(OUT) $(OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
