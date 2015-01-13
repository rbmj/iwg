CC=gcc
CFLAGS=-fno-stack-protector -z execstack -Wno-format-security -g 
SRC=$(wildcard *.c)
BIN=$(SRC:.c=)

AS=nasm
ASFLAGS=-g -f elf
LD=ld

AS_SRC=$(wildcard *.asm)
AS_OBJ=$(AS_SRC:.asm=.o)
AS_BIN=$(AS_SRC:.asm=)


all: $(BIN) $(AS_BIN)

$(AS_BIN): $(AS_OBJ)
	$(LD) $(LDFLAGS) -o $@ $(@:=.o)

$(AS_OBJ): $(AS_SRC)
	$(AS) $(ASFLAGS) -o $@ $(@:.o=.asm)

clean:
	rm -f *.o $(BIN) $(AS_OBJ) $(AS_BIN) *~
