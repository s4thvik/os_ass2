# Compiler
CXX = g++
CXXFLAGS = -Wall -Werror -std=c++11

# Directories
ONE_CPU_DIR = 1CPUs
TWO_CPU_DIR = 2CPUs

# Source files
ONE_CPU_SRC = $(ONE_CPU_DIR)/FIFO1Cpu.cpp \
              $(ONE_CPU_DIR)/NPSJF1Cpu.cpp \
              $(ONE_CPU_DIR)/PSJF1Cpu.cpp \
              $(ONE_CPU_DIR)/RR1Cpu.cpp \
              $(ONE_CPU_DIR)/parseLine1.cpp

TWO_CPU_SRC = $(TWO_CPU_DIR)/FIFO2Cpu.cpp \
              $(TWO_CPU_DIR)/NPSJF2Cpu.cpp \
              $(TWO_CPU_DIR)/PSJF2Cpu.cpp \
              $(TWO_CPU_DIR)/RR2Cpu.cpp \
              $(TWO_CPU_DIR)/parseLine2.cpp

# Main file
MAIN_SRC = main.cpp

# Object files
ONE_CPU_OBJ = $(ONE_CPU_SRC:.cpp=.o)
TWO_CPU_OBJ = $(TWO_CPU_SRC:.cpp=.o)
MAIN_OBJ = $(MAIN_SRC:.cpp=.o)

# Output executable
OUTPUT = simulator.exe

# Targets
all: $(OUTPUT)

$(OUTPUT): $(MAIN_OBJ) $(ONE_CPU_OBJ) $(TWO_CPU_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile .cpp files to .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target to remove object files and executable
clean:
	rm -f $(ONE_CPU_OBJ) $(TWO_CPU_OBJ) $(MAIN_OBJ) $(OUTPUT)

# Run target to print messages
run: $(OUTPUT)
	@echo "Scheduling Algorithms:"
	@echo "First In First Out (FIFO)"
	@echo "Non-Preemptive Shortest Job First (NPSJF)"
	@echo "Preemptive Shortest Job First (PSJF)"
	@echo "Round Robin (RR)"
	@echo "\"Usage : ./simulator.exe <scheduling-algorithm> <path-to-workload-description-file>\""
	@echo "Example usage : ./simulator.exe 1 process1.dat"

.PHONY: all clean run
