CC = g++
CFLAGS = -g -c -Wall -std=c++11 -O0
OBJ = Tools.o RegisterFile.o Loader.o\
ConditionCodes.o Memory.o Simulate.o yess.o W.o M.o F.o D.o E.o\
PipeReg.o FetchStage.o DecodeStage.o ExecuteStage.o MemoryStage.o WritebackStage.o PipeRegField.o

.C.o:
	$(CC) $(CFLAGS) $< -o $@

yess: $(OBJ)
	$(CC) Tools.o Memory.o Loader.o ConditionCodes.o PipeReg.o RegisterFile.o F.o D.o E.o M.o W.o FetchStage.o DecodeStage.o ExecuteStage.o MemoryStage.o WritebackStage.o Simulate.o PipeRegField.o yess.o -o yess

yess.o: Simulate.h PipeReg.h F.h D.h E.h M.h W.h FetchStage.h ExecuteStage.h MemoryStage.h WritebackStage.h Memory.h RegisterFile.h ConditionCodes.h PipeRegField.h

Loader.o: Loader.C Loader.h Memory.h

Memory.o: Memory.h Tools.h

Tools.o: Tools.h 

RegisterFile.o: RegisterFile.h Tools.h

ConditionCodes.o: ConditionCodes.h Tools.h

PipeReg.o: PipeReg.h

F.o: Stage.h F.h PipeReg.h RegisterFile.h Instructions.h PipeRegField.h
D.o: Stage.h D.h PipeReg.h RegisterFile.h Instructions.h PipeRegField.h
E.o: Stage.h E.h PipeReg.h RegisterFile.h Instructions.h PipeRegField.h
M.o: Stage.h M.h PipeReg.h RegisterFile.h Instructions.h PipeRegField.h
W.o: Stage.h W.h PipeReg.h RegisterFile.h Instructions.h PipeRegField.h

FetchStage.o: FetchStage.h Stage.h RegisterFile.h PipeReg.h Stage.h F.h D.h M.h W.h E.h Memory.h Tools.h PipeRegField.h Debug.h
DecodeStage.o: DecodeStage.h Stage.h RegisterFile.h PipeReg.h Stage.h F.h D.h M.h W.h E.h Memory.h Tools.h ExecuteStage.h Instructions.h MemoryStage.h
ExecuteStage.o: ExecuteStage.h Stage.h RegisterFile.h PipeReg.h F.h D.h M.h W.h E.h Tools.h Instructions.h ConditionCodes.h
MemoryStage.o: Stage.h MemoryStage.h ExecuteStage.h RegisterFile.h PipeReg.h F.h D.h M.h W.h E.h
WritebackStage.o: Stage.h MemoryStage.h WritebackStage.h RegisterFile.h Instructions.h PipeReg.h F.h D.h M.h W.h E.h
PipeRegField.o: PipeRegField.h

Simulate.o: Simulate.h PipeReg.h Stage.h F.h D.h E.h M.h W.h FetchStage.h DecodeStage.h ExecuteStage.h MemoryStage.h WritebackStage.h Memory.h RegisterFile.h ConditionCodes.h 

clean:
	rm $(OBJ) yess

run:
	make clean
	make yess
	./run.sh

