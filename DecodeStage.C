#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "W.h"
#include "E.h"
#include "M.h"
#include "Stage.h"
#include "ExecuteStage.h"
#include "MemoryStage.h"
#include "DecodeStage.h"
#include "Status.h"
#include "Instructions.h"

/**
 * @brief this is the doClockLow stage.
 * 
 * @param pregs is the pipline file
 * @param stages is the Stage file
 * @return true only if true
 * @return false only if fase
 */
bool DecodeStage::doClockLow(PipeReg **pregs, Stage **stages)
{

    D *dreg = (D *)pregs[DREG];
    E *ereg = (E *)pregs[EREG];
    uint64_t stat = dreg->getstat()->getOutput();
    uint64_t icode = dreg->geticode()->getOutput();
    uint64_t ifun = dreg->getifun()->getOutput();
    uint64_t valC = dreg->getvalC()->getOutput();

    uint64_t srcA = d_srcA(icode, dreg->getrA()->getOutput());
    uint64_t srcB = d_srcB(icode, dreg->getrB()->getOutput());
    uint64_t dstE = d_dstE(icode, dreg->getrB()->getOutput());
    uint64_t dstM = d_dstM(icode, dreg->getrA()->getOutput());
    uint64_t valA = d_valA(icode, dreg->getrA()->getOutput(), pregs, stages);
    uint64_t valB = d_valB(icode, dreg->getrB()->getOutput(), pregs, stages);

    setEInput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB);
    return false;
}

/**
 * @brief this does the settting of the E input.
 * 
 * @param ereg is the e reg
 * @param stat  the instruction
 * @param icode the instruction
 * @param ifun the instruction
 * @param valC the value 
 * @param valA the value
 * @param valB the value
 * @param dstE the dest
 * @param dstM the dest
 * @param srcA the srcA
 * @param srcB the srcB
 */
void DecodeStage::setEInput(E *ereg, uint64_t stat, uint64_t icode,
                            uint64_t ifun, uint64_t valC, uint64_t valA,
                            uint64_t valB, uint64_t dstE, uint64_t dstM,
                            uint64_t srcA, uint64_t srcB)
{
    ereg->getstat()->setInput(stat);
    ereg->geticode()->setInput(icode);
    ereg->getifun()->setInput(ifun);
    ereg->getvalC()->setInput(valC);
    ereg->getvalA()->setInput(valA);
    ereg->getvalB()->setInput(valB);
    ereg->getsrcA()->setInput(srcA);
    ereg->getsrcB()->setInput(srcB);
    ereg->getdstE()->setInput(dstE);
    ereg->getdstM()->setInput(dstM);
}

/**
 * @brief this does the clock high.
 * 
 * @param pregs is from the pipeline
 */
void DecodeStage::doClockHigh(PipeReg **pregs)
{

    E *ereg = (E *)pregs[EREG];

    ereg->getstat()->normal();
    ereg->getifun()->normal();
    ereg->getvalA()->normal();
    ereg->getdstM()->normal();
    ereg->getsrcA()->normal();
    ereg->getsrcB()->normal();
    ereg->getvalB()->normal();
    ereg->getdstE()->normal();
    ereg->getvalC()->normal();
    ereg->geticode()->normal();
}

/**
 * @brief this is the d_srcA that does the src from decorde stage.
 * 
 * @param icode is from the instruction
 * @param rA is the reg
 * @return uint64_t returns  either rsp or rnone
 */
uint64_t DecodeStage::d_srcA(uint64_t icode, uint64_t rA)
{
    if (icode == IRRMOVQ || icode == IRMMOVQ || icode == IOPQ || icode == IPUSHQ)
    {
        return rA;
    }
    else if (icode == IPOPQ || icode == IRET)
    {
        return RSP;
    }
    else
    {
        return RNONE;
    }
}

/**
 * @brief thid dooes the scrB for the decodestage.
 * 
 * @param icode is from the instruction
 * @param rB  is the reg fron the instrction
 * @return uint64_t retrn rb or rsp or rnone.
 */
uint64_t DecodeStage::d_srcB(uint64_t icode, uint64_t rB)
{
    if (icode == IOPQ || icode == IMRMOVQ || icode == IRMMOVQ)
    {
        return rB;
    }
    else if (icode == IPOPQ || icode == IRET || icode == ICALL || icode == IPUSHQ)
    {
        return RSP;
    }
    else
    {
        return RNONE;
    }
}

/**
 * @brief this is the desE of the decodestage return rb rsp or rnone depdning on the icode.
 * 
 * @param icode is the instruction
 * @param rB the the rb instruction
 * @return uint64_t return rb or rsp or rnone depdning on the icode
 */
uint64_t DecodeStage::d_dstE(uint64_t icode, uint64_t rB)
{
    if (icode == IOPQ || icode == IRRMOVQ || icode == IIRMOVQ)
    {
        return rB;
    }
    else if (icode == IPUSHQ || icode == IPOPQ || icode == ICALL || icode == IRET)
    {
        return RSP;
    }
    else
    {
        return RNONE;
    }
}

/**
 * @brief this is the d_destM code. return ra or rnone.
 * 
 * @param icode is the icode of the instruction.
 * @param rA is the ra of the instruction.
 * @return uint64_t ra or rnone.
 */
uint64_t DecodeStage::d_dstM(uint64_t icode, uint64_t rA)
{
    if (icode == IMRMOVQ || icode == IPOPQ)
    {
        return rA;
    }
    else
    {
        return RNONE;
    }
}

// uint64_t DecodeStage::d_valA(uint64_t icode, uint64_t rA, PipeReg ** pregs, Stage ** stages) {
//    RegisterFile * regInstance = RegisterFile::getInstance();

//    bool error;
//    uint64_t d_srcA1 = d_srcA(icode, rA);

//     return regInstance->readRegister(d_srcA1, error);
// }

// uint64_t DecodeStage::d_valB(uint64_t icode, uint64_t rB, PipeReg ** pregs, Stage ** stages) {
//     RegisterFile * regInstance = RegisterFile::getInstance();

//    bool error;
//    uint64_t d_srcB1 = d_srcB(icode, rB);

//     return regInstance->readRegister(d_srcB1, error);
// }

/*
calculates the d_valA
*/

/**
 * @brief This is the d_valA for this part of the code.
 * 
 * @param icode is part of the instrution.
 * @param rA is part of the instruciton.
 * @param pregs the from the pipereg.
 * @param stages is from the pipestatge.
 * @return uint64_t it returns.
 */
uint64_t DecodeStage::d_valA(uint64_t icode, uint64_t rA, PipeReg **pregs, Stage **stages)
{
    MemoryStage *mstage = (MemoryStage *)stages[MSTAGE];
    RegisterFile *regInstance = RegisterFile::getInstance();
    ExecuteStage *estage = (ExecuteStage *)stages[ESTAGE];
    D *dreg = (D *)pregs[DREG];
    W *wreg = (W *)pregs[WREG];
    M *mreg = (M *)pregs[MREG];

    uint64_t d_srcA1 = d_srcA(icode, rA);
    uint64_t M_valE = mreg->getvalE()->getOutput();
    uint64_t W_dstE = wreg->getdstE()->getOutput();
    uint64_t e_valE = estage->gete_valE();
    uint64_t W_valE = wreg->getvalE()->getOutput();
    uint64_t W_dstM = wreg->getdstM()->getOutput();
    uint64_t M_dstE = mreg->getdstE()->getOutput();
    uint64_t e_dstE = estage->gete_dstE();
    uint64_t M_dstM = mreg->getdstM()->getOutput();
    uint64_t W_valM = wreg->getvalM()->getOutput();
    uint64_t M_valM = mstage->get_valM();
     bool error;

    if (icode == ICALL || icode == IJXX)
    {
        return dreg->getvalP()->getOutput();
    }
    if (d_srcA1 == RNONE)
    {
        return 0;
    }
    if (d_srcA1 == e_dstE)
    {
        return e_valE;
    }
    if (d_srcA1 == M_dstM)
    {
        return M_valM;
    }

    if (d_srcA1 == M_dstE)
    {
        return M_valE;
    }

    if (d_srcA1 == W_dstM)
    {
        return W_valM;
    }

    if (d_srcA1 == W_dstE)
    {
        return W_valE;
    }

    return regInstance->readRegister(d_srcA1, error);
}

/**
 * @brief this does the d_valB of the descode stage.
 * 
 * @param icode is from the instruction.
 * @param rB is from rB of the instruciton.
 * @param pregs is the pregs of the pipestage.
 * @param stages is form the stage file.
 * @return uint64_t reture does the calc for dvalB.
 */
uint64_t DecodeStage::d_valB(uint64_t icode, uint64_t rB, PipeReg **pregs, Stage **stages)
{
    RegisterFile *regInstance = RegisterFile::getInstance();
    MemoryStage *mstage = (MemoryStage *)stages[MSTAGE];
    ExecuteStage *estage = (ExecuteStage *)stages[ESTAGE];
    W *wreg = (W *)pregs[WREG];
    M *mreg = (M *)pregs[MREG];

    uint64_t e_dstE = estage->gete_dstE();
    uint64_t e_valE = estage->gete_valE();
    uint64_t W_dstE = wreg->getdstE()->getOutput();
    uint64_t M_valE = mreg->getvalE()->getOutput();
    uint64_t M_dstM = mreg->getdstM()->getOutput();
    uint64_t W_valE = wreg->getvalE()->getOutput();
    uint64_t W_valM = wreg->getvalM()->getOutput();
    uint64_t M_dstE = mreg->getdstE()->getOutput();
    uint64_t W_dstM = wreg->getdstM()->getOutput();
    uint64_t M_valM = mstage->get_valM();
    uint64_t d_srcB1 = d_srcB(icode, rB);
    bool error;

    if (d_srcB1 == RNONE)
    {
        return 0;
    }
    if (d_srcB1 == e_dstE)
    {
        return e_valE;
    }

    if (d_srcB1 == M_dstM)
    {
        return M_valM;
    }

    if (d_srcB1 == M_dstE)
    {
        return M_valE;
    }

    if (d_srcB1 == W_dstM)
    {
        return W_valM;
    }

    if (d_srcB1 == W_dstE)
    {
        return W_valE;
    }

    return regInstance->readRegister(d_srcB1, error);
}
