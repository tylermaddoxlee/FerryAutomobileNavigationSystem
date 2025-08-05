CXX       := g++
CXXFLAGS  := -std=c++17 -Wall -Wextra -g

# Source files for the main application
SRCS      := MenuUI.cpp \
             ReservationASM.cpp ReservationCommandProcessor.cpp \
             SailingASM.cpp SailingCommandProcessor.cpp \
             Utilities.cpp \
             VehicleASM.cpp VesselASM.cpp VesselCommandProcessor.cpp \
             main.cpp

# Object files for the main application
OBJS      := $(SRCS:.cpp=.o)

# Test source files
TEST_SRCS := testFileOps.cpp testSailingReport.cpp
# Object files for tests
TEST_OBJS := $(TEST_SRCS:.cpp=.o)

# Executable names
TARGET    := myprogram
TEST1     := testFileOps
TEST2     := testSailingReport

# Default target builds application and tests
all: $(TARGET) $(TEST1) $(TEST2)

# Link the main application
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Link testFileOps (exclude main.o)
$(TEST1): testFileOps.o $(filter-out main.o,$(OBJS))
	$(CXX) $(CXXFLAGS) -o $@ $^

# Link testSailingReport (exclude main.o)
$(TEST2): testSailingReport.o $(filter-out main.o,$(OBJS))
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile each .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TARGET) $(TEST1) $(TEST2)

# Deep clean removes all build artifacts *and* data files
deepclean: clean
	rm -f *.dat

.PHONY: all clean deepclean
