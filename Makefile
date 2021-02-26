DETOURS_SRC=Detours/src

all: setDate.dll setDate.exe

setDate.dll:  hooks.o Detours.o disasm.o modules.o
	g++ -shared hooks.o Detours.o disasm.o modules.o -lntdll     -o setDate.dll

setDate.exe: setDate.o creatwth.o Detours.o disasm.o modules.o
	g++        setDate.o creatwth.o Detours.o disasm.o modules.o -o setDate.exe

hooks.o : hooks.c setDate.h
	gcc -I$(DETOURS_SRC) -c hooks.c

Detours.o : $(DETOURS_SRC)/Detours.cpp
	g++ -I$(DETOURS_SRC) -c $(DETOURS_SRC)/Detours.cpp

disasm.o : $(DETOURS_SRC)/disasm.cpp
	g++ -I$(DETOURS_SRC) -c $(DETOURS_SRC)/disasm.cpp

modules.o : $(DETOURS_SRC)/modules.cpp
	g++ -I$(DETOURS_SRC) -c $(DETOURS_SRC)/modules.cpp

creatwth.o : $(DETOURS_SRC)/creatwth.cpp
	g++ -I$(DETOURS_SRC) -c $(DETOURS_SRC)/creatwth.cpp

setDate.o: setDate.c setDate.h
	gcc -I$(DETOURS_SRC) -c setDate.c

# test-target.exe: test-target.o
# 	gcc target.o -o test-target.exe
