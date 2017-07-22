CC = g++
CC_FLAGS = -O3 -Wall -c
LD_FLAGS = -O3 -Wall
LIBS = -lgsl -lgslcblas -lm
CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
MAINS = obj/main.o obj/main_singlehost.o obj/main_parametersweep.o 

all: epidemic singlehost parametersweep
 
clean:
	rm -f obj/*.o obj/*.d epidemic singlehost parametersweep
 
epidemic: $(filter-out $(MAINS),$(OBJ_FILES)) obj/main.o
	$(CC) $(LD_FLAGS) $^ $(LIBS) -o $@

singlehost: $(filter-out $(MAINS),$(OBJ_FILES)) obj/main_singlehost.o
	$(CC) $(LD_FLAGS) $^ $(LIBS) -o $@

parametersweep: $(filter-out $(MAINS),$(OBJ_FILES)) obj/main_parametersweep.o
	$(CC) $(LD_FLAGS) $^ $(LIBS) -o $@
 
obj/%.o: src/%.cpp $^
	$(CC) $(CC_FLAGS) $< -o $@

$(OBJ_FILES): | obj

obj:
	mkdir -p obj

CC_FLAGS += -MMD
-include $(OBJ_FILES:.o=.d)
