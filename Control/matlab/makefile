CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=matlab_rtwutil.cpp matlab_initialize.cpp matlab_terminate.cpp arclengthcurve.cpp diff.cpp power.cpp rdivide.cpp cubic_spline3.cpp cubic_spline.cpp mldivide.cpp colon.cpp adaptivegaus.cpp buildbfcurvature.cpp builddetailedbf.cpp parevalspline.cpp histc.cpp localize.cpp matlab_emxutil.cpp matlab_emxAPI.cpp rt_nonfinite.cpp rtGetNaN.cpp rtGetInf.cpp
OBJECTS=$(SOURCES:.cpp=.o)
OUT=libmatlab.a
	
$(OUT): $(OBJECTS) 
	ar rcsv $(OUT) $(OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
