ASSEMBLER=./assembler
LINKER=./linker
EMULATOR=./emulator



${ASSEMBLER} -o main.o ./tests/nivo-a/main.s
${ASSEMBLER} -o math.o ./tests/nivo-a/math.s
${ASSEMBLER} -o handler.o ./tests/nivo-a/handler.s
${ASSEMBLER} -o isr_timer.o ./tests/nivo-a/isr_timer.s
${ASSEMBLER} -o isr_terminal.o ./tests/nivo-a/isr_terminal.s
${ASSEMBLER} -o isr_software.o ./tests/nivo-a/isr_software.s
${LINKER} -hex \
  -place=my_code@0x40000000 -place=math@0xF0000000 \
  -o program.hex \
  ./assembler-output/handler.o ./assembler-output/math.o ./assembler-output/main.o ./assembler-output/isr_terminal.o ./assembler-output/isr_timer.o ./assembler-output/isr_software.o
${EMULATOR} program.hex 

#${EMULATOR} program.hex > debug_emulator.txt 2>&1