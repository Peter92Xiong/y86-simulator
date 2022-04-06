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
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "Memory.h"

/*
 * doClockLow
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool MemoryStage::doClockLow(PipeReg **pregs, Stage **stages)
{
    M *mreg = (M *)pregs[MREG];
    W *wreg = (W *)pregs[WREG];

    stat = mreg->getstat()->getOutput();
    uint64_t icode = mreg->geticode()->getOutput();
    uint64_t dstM = mreg->getdstM()->getOutput();
    uint64_t dstE = mreg->getdstE()->getOutput();
    uint64_t valE = mreg->getvalE()->getOutput();

    uint64_t valA = mreg->getvalA()->getOutput();

    //@@@@@@@@@@@@lab10
    bool error;
    Memory *m = Memory::getInstance();
    uint64_t addressOfMem = mem_addr(icode, valA, valE);

    valM = 0;
    if (mem_read(icode))
    {
        valM = m->getLong(addressOfMem, error);
    }

    if (mem_write(icode))
    {
        m->putLong(valA, addressOfMem, error);
    }

    //@@@@@@@@@@@@@lab10 ends

    //@@@@@@@lab111

    if (error)
    {
        stat = SADR;
    }
    // else
    // {
    //     stat = stat;
    // }
    //lab11 ends

    setWInput(wreg, stat, icode, valE, valA, dstE, dstM, valM);

    return false;
}

/* doClockHigh
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void MemoryStage::doClockHigh(PipeReg **pregs)
{
    W *wreg = (W *)pregs[WREG];

    wreg->getdstM()->normal();
    wreg->getstat()->normal();
    wreg->geticode()->normal();
    wreg->getvalE()->normal();
    wreg->getvalM()->normal();
    wreg->getdstE()->normal();
}

/*
* Sets the Writeback stage input
*/
void MemoryStage::setWInput(W *wreg, uint64_t stat, uint64_t icode, uint64_t valE,
                            uint64_t valA, uint64_t dstE, uint64_t dstM, uint64_t valM)
{
    wreg->getstat()->setInput(stat);
    wreg->geticode()->setInput(icode);
    wreg->getvalE()->setInput(valE);
    wreg->getdstE()->setInput(dstE);
    wreg->getdstM()->setInput(dstM);
    wreg->getvalM()->setInput(valM);
}

/*
* getter for memory stages dstM
*/
uint64_t MemoryStage::get_dstM()
{
    return dstM;
}

/*
* getter for memory stages valM
*/
uint64_t MemoryStage::get_valM()
{
    return valM;
}

//%%%%%%%%%%%%%%%%%^@@@@@@@@@@@%%lab 10 starts here
/**
 * @brief this tells us where to get the mem address.
 * 
 * @param icode is the instruction
 * @param valA is the valA
 * @param valE is the valE
 * @return uint64_t it returns 0 or the valA or valE
 */
uint64_t MemoryStage::mem_addr(uint64_t icode, uint64_t valA, uint64_t valE)
{

    if (icode == IRMMOVQ || icode == IPUSHQ || icode == ICALL || icode == IMRMOVQ)
    {
        return valE;
    }
    else if (icode == IPOPQ || icode == IRET)
    {
        return valA;
    }
    return 0;
}

/**
 * @brief this is the method that tells if its true or not.
 * 
 * @param icode is part of the instruciton
 * @return true is it is icode is == to imrmoveQ or Ipop or Iret than return true
 * @return false else return false.
 */
bool MemoryStage::mem_read(uint64_t icode)
{
    if (icode == IMRMOVQ || icode == IPOPQ || icode == IRET)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool MemoryStage::mem_write(uint64_t icode)
{
    if (icode == IRMMOVQ || icode == IPUSHQ || icode == ICALL)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
* getter for memory stages stat
*/
uint64_t MemoryStage::getm_stat()
{
    return stat;
}