CC = g++
CFLAGS =
LDFLAGS =

# Directories
ASSEMBLER_OUTPUT_DIR = assembler-output
BUILD_DIR = build
SRC_DIR = src
INC_DIR = inc

# Flex and Bison source files
LEX_SRC = misc/lexer.l
YACC_SRC = misc/parser.y
LEX_OUT = $(BUILD_DIR)/lex.yy.c
YACC_OUT = $(BUILD_DIR)/parser.tab.c
YACC_HDR = $(BUILD_DIR)/parser.tab.h

# C source file with main function
ASSEMBLER_MAIN_SRC = $(SRC_DIR)/mainFunction.cpp
LINKER_MAIN_SRC = $(SRC_DIR)/linkerMain.cpp
EMULATOR_MAIN_SRC = $(SRC_DIR)/emulatorMain.cpp

# Additional source files
ADDITIONAL_ASSEMBLER_SRCS = $(SRC_DIR)/assembler_first.cpp \
                            $(SRC_DIR)/assembler_second.cpp \
                            $(SRC_DIR)/assembler.cpp

ADDITIONAL_LINKER_SRCS = $(SRC_DIR)/linker.cpp \
                         $(SRC_DIR)/helper_testing.cpp  

ADDITIONAL_EMULATOR_SRCS = $(SRC_DIR)/emulator.cpp                      

# Additional header files
ADDITIONAL_HDRS = $(INC_DIR)/assembler_first.h \
                  $(INC_DIR)/assembler_second.h \
                  $(INC_DIR)/assembler.h
                  

LINKER_HDR = $(INC_DIR)/linker.h \

EMULATOR_HDR = $(INC_DIR)/emulator.h \

# Object files
ASSEMBLER_OBJS = $(LEX_OUT:$(BUILD_DIR)/%.c=$(BUILD_DIR)/%.o) \
                 $(YACC_OUT:$(BUILD_DIR)/%.c=$(BUILD_DIR)/%.o) \
                 $(ASSEMBLER_MAIN_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) \
                 $(ADDITIONAL_ASSEMBLER_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

LINKER_OBJS = $(LINKER_MAIN_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) \
              $(ADDITIONAL_LINKER_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

EMULATOR_OBJS = $(EMULATOR_MAIN_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) \
                $(ADDITIONAL_EMULATOR_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

              

# Executable names
ASSEMBLER_EXECUTABLE = assembler
LINKER_EXECUTABLE = linker
EMULATOR_EXECUTABLE = emulator

all: $(ASSEMBLER_EXECUTABLE) $(LINKER_EXECUTABLE) $(EMULATOR_EXECUTABLE)

$(ASSEMBLER_EXECUTABLE): $(ASSEMBLER_OBJS) | $(ASSEMBLER_OUTPUT_DIR)
	$(CC) $(CFLAGS) $(ASSEMBLER_OBJS) -o $@ $(LDFLAGS)

$(LINKER_EXECUTABLE): $(LINKER_OBJS)
	$(CC) $(CFLAGS) $(LINKER_OBJS) -o $@ $(LDFLAGS)

$(EMULATOR_EXECUTABLE): $(EMULATOR_OBJS)
	$(CC) $(CFLAGS) $(EMULATOR_OBJS) -o $@ $(LDFLAGS)

$(LEX_OUT): $(LEX_SRC) $(YACC_HDR) | $(BUILD_DIR)
	flex -o $(LEX_OUT) $(LEX_SRC)

$(YACC_OUT) $(YACC_HDR): $(YACC_SRC) | $(BUILD_DIR)
	bison -d -o $(YACC_OUT) $(YACC_SRC)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR) $(ASSEMBLER_OUTPUT_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)