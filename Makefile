CXX = g++
INCLUDES = -I./stack/include -I./asm/include -I./spu/include -I./include
CXXFLAGS = -Wall -g -Wextra -Wshadow $(INCLUDES)

ASM_DIR    = asm/src
DISASM_DIR = asm/disasm
SPU_DIR    = spu/src
STACK_DIR  = stack/src

ASM_SRCS    = $(ASM_DIR)/assem_tex.cpp $(ASM_DIR)/assem.cpp $(ASM_DIR)/onegin.cpp 
SPU_SRCS    = $(SPU_DIR)/spu_tex.cpp $(SPU_DIR)/spu.cpp $(SPU_DIR)/funk.cpp $(STACK_DIR)/stack.cpp
DISASM_SRCS = $(DISASM_DIR)/disassm.cpp

ASM_OBJS    = $(notdir $(ASM_SRCS:.cpp=.o))
SPU_OBJS    = $(notdir $(SPU_SRCS:.cpp=.o))
DISASM_OBJS = $(notdir $(DISASM_SRCS:.cpp=.o))

all: asm_bin spu_bin disasm_bin

asm_bin: $(ASM_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@.out

spu_bin: $(SPU_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@.out

disasm_bin: $(DISASM_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@.out

vpath %.cpp $(ASM_DIR) $(DISASM_DIR) $(SPU_DIR) $(STACK_DIR)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o asm_bin.out spu_bin.out disasm_bin.out asm.out 

remake: clean all

.PHONY: all clean remake