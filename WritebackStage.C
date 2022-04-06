#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "M.h"
#include "W.h"
#include "E.h"
#include "Stage.h"
#include "MemoryStage.h"
#include "WritebackStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"

/*
 * doClockLow
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool WritebackStage::doClockLow(PipeReg **pregs, Stage **stages)
{

    W *wreg = (W *)pregs[WREG];
    //uint64_t icode = wreg->geticode()->getOutput();

    //@@@@@@@@@@@@@@@@@@@@@@@lab10
    // M *mreg = (M *)pregs[WREG];
    // uint64_t valM = mreg->getvalA()->getOutput();

    // wreg->getdstM()->setInput(valM);
    // MemoryStage *m = (MemoryStage *)stages[ESTAGE];
    //  uint64_t valM = m->get_valM();
    //  wreg->getdstM()->setInput(valM);
    // uint64_t dstM = valA;

    //M *mreg = (M *)pregs[MREG];
    uint64_t valM = wreg->getvalM()->getOutput();
    //wreg->getdstM()->setInput(valM);
    //wreg->getvalM()->setInput(valM);
    uint64_t dstE = wreg->getdstE()->getOutput();
    //uint64_t dstM = wreg->getdstM()->getOutput();

    RegisterFile *regInstance = RegisterFile::getInstance();
    bool error;

    regInstance->writeRegister(valM, dstE, error);

    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@lab10ends

    // if (icode == IHALT)
    // {
    //     return true;
    // }
    // else
    // {
    //     return false;
    // }

    //lab 11@@@@@@@@@@@@@@@@@@
    uint64_t stat = wreg->getstat()->getOutput();
    if (stat != SAOK)
    {
        return true;
    }

    return false;

}

/* doClockHigh
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void WritebackStage::doClockHigh(PipeReg **pregs)
{
    W *wreg = (W *)pregs[WREG];
    RegisterFile *regInstance = RegisterFile::getInstance();
    bool error;
    regInstance->writeRegister(wreg->getvalE()->getOutput(), wreg->getdstE()->getOutput(), error);
    regInstance->writeRegister(wreg->getvalM()->getOutput(), wreg->getdstM()->getOutput(), error);
}