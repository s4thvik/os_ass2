# Compiler
CXX = g++
CXXFLAGS = -Wall -std=c++17 -w -lstdc++ -m64  # Add -m64 for 64-bit

# Directories
DIR1Cpus = 1Cpus
DIR2Cpus = 2Cpus

# Source files
SRC1 =  $(DIR1Cpus)/FIFO1Cpu.cpp $(DIR1Cpus)/NPSJF1Cpu.cpp $(DIR1Cpus)/PSJF1Cpu.cpp $(DIR1Cpus)/RR1Cpu.cpp $(DIR1Cpus)/parseLine1.cpp
SRC2 = $(DIR2Cpus)/FIFO2Cpu.cpp $(DIR2Cpus)/NPSJF2Cpu.cpp $(DIR2Cpus)/PSJF2Cpu.cpp $(DIR2Cpus)/RR2Cpu.cpp $(DIR2Cpus)/parseLine2.cpp

# Object files
OBJ1 = $(SRC1:.cpp=.o)
OBJ2 = $(SRC2:.cpp=.o)

# Executable
EXEC = simulator

# Default rule
all: $(EXEC)
	@echo ------------------------------------------------
	@echo Scheduling Algorithms:
	@echo 1: First In First Out (FIFO)
	@echo 2: Non-Preemptive Shortest Job First (NPSJF)
	@echo 3: Preemptive Shortest Job First (PSJF)
	@echo 4: Round Robin (RR)
	@echo "Usage : ./simulator.exe <scheduling-algorithm> <path-to-workload-description-file>"
	@echo Example usage : ./simulator.exe 1 process1.dat
	@echo ------------------------------------------------

# Link all object files
$(EXEC): main.o $(OBJ1) $(OBJ2)
	$(CXX) $(CXXFLAGS) -o $(EXEC) main.o $(OBJ1) $(OBJ2)

# Compile individual source files
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

# Compile source files in 1Cpus directory
$(DIR1Cpus)/%.o: $(DIR1Cpus)/%.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@ -m64

# Compile source files in 2Cpus directory
$(DIR2Cpus)/%.o: $(DIR2Cpus)/%.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@ -m64 

# Clean rule
clean:
	rm -f $(EXEC) main.o $(OBJ1) $(OBJ2)

# Run rule
run: all
	./$(EXEC)