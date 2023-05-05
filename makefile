all: assembler
assembler: assembler.c assembler.h preProcessor.c preProcessor.h firstPass.c firstPass.h secondPass.c secondPass.h labelFunc.c labelFunc.h helpFunc.c coding.c coding.h utils.h
	gcc -Wall -ansi -pedantic assembler.c assembler.h preProcessor.c preProcessor.h firstPass.c firstPass.h secondPass.c secondPass.h labelFunc.c labelFunc.h helpFunc.c coding.c coding.h utils.h -o assembler
