CXX=g++

CPPFLAGS=-D__GXX_EXPERIMENTAL_CXX0X__ -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1 -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2 -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8

CXXFLAGS=-std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -pthread $(CPPFLAGS)

LDFLAGS=-lpthread

RM=rm -f

SOURCES=./ADC/Adc.cpp ./BlackGPIO/BlackGPIO.cpp ./BlackADC/BlackADC.cpp ./BlackDirectory/BlackDirectory.cpp ./BlackI2C/BlackI2C.cpp BlackMutex/BlackMutex.cpp BlackPWM/BlackPWM.cpp BlackSPI/BlackSPI.cpp BlackThread/BlackThread.cpp BlackTime/BlackTime.cpp BlackUART/BlackUART.cpp ./BlackCore.cpp ./main.cpp

OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=main


all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)

