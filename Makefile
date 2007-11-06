MAKEALIB = 1
CC   = g++
CLEAR= @clear
ECHO = @echo
RM   = @rm -f
STTY = @stty
TPUT = @tput

INTERFACES   = display.h rayTracer.h rtObjects.h Maths/math3D.h Maths/Matrix4.h
REALISATIONS = $(INTERFACES:.h=.cpp) main.cpp
OBJECTS       = $(REALISATIONS:.cpp=.o)

CFLAG        = -g
LDFLAG = -lSDLmain -lSDL
EXECUTABLE = rayTracer
INCLUDE = -I Maths

%.o : %.cpp
	$(ECHO) "Compiling $< -> $(<:.cpp=.o)"
	$(CC) $(INCLUDE) $(CFLAG) -c $< -o $(<:.cpp=.o)

$(EXECUTABLE) : $(OBJECTS)
	$(ECHO) "Linking"
	$(CC) $(LDFLAG) $(OBJECTS) -o $(EXECUTABLE) $(LIB)

all : $(EXECUTABLE)

clr :
	$(ECHO) "Cleaning..."
	$(RM) core
	$(RM) $(OBJECTS)
	$(RM) $(EXECUTABLE)
	$(ECHO) "Cleaning over"

clean : clr
