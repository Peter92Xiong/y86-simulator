#include <cstdint>
#include <string>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "W.h"
#include "E.h"
#include "M.h"
#include "Stage.h"
#include "Instructions.h"
#include "ConditionCodes.h"
#include "Tools.h"
#include "MemoryStage.h"
#include "ExecuteStage.h"
#include "Status.h"

/*
 * doClockLow
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool ExecuteStage::doClockLow(PipeReg **pregs, Stage **stages)
{
   E *ereg = (E *)pregs[EREG];
   M *mreg = (M *)pregs[MREG];
   W *wreg = (W *)pregs[WREG];

   uint64_t stat = ereg->getstat()->getOutput();
   uint64_t icode = ereg->geticode()->getOutput();
   uint64_t valA = ereg->getvalA()->getOutput();
   uint64_t dstM = ereg->getdstM()->getOutput();
   dstE = ereg->getdstE()->getOutput();
   uint64_t ifun = ereg->getifun()->getOutput();
   uint64_t valB = ereg->getvalB()->getOutput();
   // uint64_t valE = 0;
   uint64_t e_Cnd = 0;

   MemoryStage *m = (MemoryStage *)stages[MSTAGE];
   uint64_t m_stat = m->getm_stat();
   uint64_t W_stat = wreg->getstat()->getOutput();

   bool of = false;

   //lab8
   uint64_t valC = ereg->getvalC()->getOutput();
   valE = ALU(icode, ifun, aluA(icode, valA, valC), aluB(icode, valB), &of);

   bool error = false;
   uint64_t sign = Tools::sign(valE);
   uint64_t zf = 0;
   if (valE == 0)
   {
      zf = 1;
   }

   bool changeCC = set_cc(icode, m_stat, W_stat);

   if (changeCC)
   {
      ConditionCodes *cceditor = ConditionCodes::getInstance();
      cceditor->setConditionCode(sign, SF, error);
      cceditor->setConditionCode(zf, ZF, error);
      cceditor->setConditionCode(of, OF, error);
   }

   e_Cnd = cond(icode, ifun);
   dstE = e_dstE(icode, dstE, e_Cnd);
   setMInput(mreg, stat, icode, e_Cnd, valE, valA, dstE, dstM);

   M_bubble = calculateControlSignals(m_stat, W_stat);

   return false;
}

// /* doClockHigh
//  *
//  * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
//  */
// void ExecuteStage::doClockHigh(PipeReg **pregs)
// {
//    // M *mreg = (M *)pregs[MREG];
//    // mreg->getstat()->normal();
//    // mreg->geticode()->normal();
//    // mreg->getCnd()->normal();
//    // mreg->getdstE()->normal();
//    // mreg->getvalE()->normal();
//    // mreg->getdstM()->normal();
//    // mreg->getvalA()->normal();
//     if (!M_bubble) {
//         normalM(pregs);
//     }
//     else {
//         bubbleM(pregs);
//     }
// }

/*
bubbles the M register
*/
void ExecuteStage::bubbleM(PipeReg **pregs)
{
   M *mreg = (M *)pregs[MREG];
   mreg->getstat()->bubble(SAOK);
   mreg->geticode()->bubble(INOP);
   mreg->getCnd()->bubble();
   mreg->getvalE()->bubble();
   mreg->getvalA()->bubble();
   mreg->getdstE()->bubble(RNONE);
   mreg->getdstM()->bubble(RNONE);
}

/*
normalizes the M register
*/
void ExecuteStage::normalM(PipeReg **pregs)
{
   M *mreg = (M *)pregs[MREG];
   mreg->getstat()->normal();
   mreg->geticode()->normal();
   mreg->getCnd()->normal();
   mreg->getvalE()->normal();
   mreg->getvalA()->normal();
   mreg->getdstE()->normal();
   mreg->getdstM()->normal();
}


/* doClockHigh
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void ExecuteStage::doClockHigh(PipeReg **pregs)
{
   // M *mreg = (M *)pregs[MREG];
   // mreg->getstat()->normal();
   // mreg->geticode()->normal();
   // mreg->getCnd()->normal();
   // mreg->getdstE()->normal();
   // mreg->getvalE()->normal();
   // mreg->getdstM()->normal();
   // mreg->getvalA()->normal();
    if (!M_bubble) {
        normalM(pregs);
    }
    else {
        bubbleM(pregs);
    }
}

/*
* Sets the Memory stage input
*/
void ExecuteStage::setMInput(M *mreg, uint64_t stat, uint64_t icode, uint64_t Cnd,
                             uint64_t valE, uint64_t valA,
                             uint64_t dstE, uint64_t dstM)
{
   mreg->getstat()->setInput(stat);
   mreg->geticode()->setInput(icode);
   mreg->getCnd()->setInput(Cnd);
   mreg->getvalA()->setInput(valA);
   mreg->getvalE()->setInput(valE);
   mreg->getdstM()->setInput(dstM);
   mreg->getdstE()->setInput(dstE);
}

/*
* Calculates the alu A component used in the ALU
*/
uint64_t ExecuteStage::aluA(uint64_t icode, uint64_t valA, uint64_t valC)
{
   if (icode == IRRMOVQ || icode == IOPQ)
   {
      return valA;
   }
   if (icode == IIRMOVQ || icode == IRMMOVQ || icode == IMRMOVQ)
   {
      return valC;
   }
   if (icode == ICALL || icode == IPUSHQ)
   {
      return -8;
   }
   if (icode == IRET || icode == IPOPQ)
   {
      return 8;
   }
   else
   {
      return 0;
   }
}

/*
* Calculates the alu B component used in the ALU
*/
uint64_t ExecuteStage::aluB(uint64_t icode, uint64_t valB)
{
   if (icode == IRMMOVQ || icode == IMRMOVQ || icode == IOPQ || icode == ICALL || icode == IPUSHQ || icode == IRET || icode == IPOPQ)
   {
      return valB;
   }
   //comment
   else
   {
      return 0;
   }

   //return 0;
}

/*
* Check if IOPQ
*/
uint64_t ExecuteStage::alufun(uint64_t icode, uint64_t ifun)
{
   if (icode == IOPQ)
   {
      return ifun;
   }
   else
   {
      return ADDQ;
   }
}

/*
* Informs if Condition codes need to be changed for IOPQ instruction
*/
bool ExecuteStage::set_cc(uint64_t icode, uint64_t m_stat, uint64_t W_stat)
{
   if ((icode == IOPQ) && (m_stat != SADR && m_stat != SINS && m_stat != SHLT) && (W_stat != SADR && W_stat != SINS && W_stat != SHLT))
   {
      return true;
   }
   else
   {
      return false;
   }
}

/*
* Calculates the execute stage for dstE
*/
uint64_t ExecuteStage::e_dstE(uint64_t icode, uint64_t dstE, uint64_t e_Cnd)
{
   if (icode == IRRMOVQ && !e_Cnd)
   {
      return RNONE;
   }
   else
   {
      return dstE;
   }
}

/*
* Sets the condition codes 
*/
void ExecuteStage::CC(uint64_t icode, uint64_t ifun, uint64_t op1, uint64_t op2, uint64_t m_stat, uint64_t W_stat)
{
   bool error;
   ConditionCodes *codeInstance = ConditionCodes::getInstance();
   if (set_cc(icode, m_stat, W_stat))
   {

      codeInstance->setConditionCode(1, ZF, error);
   }
   else
   {
      codeInstance->setConditionCode(0, ZF, error);
   }
}

/*
* Arithmetic Logic Unit (ALU)
* Does the arithmetic operations
*/
uint64_t ExecuteStage::ALU(uint64_t icode, uint64_t ifun, uint64_t aluA, uint64_t aluB, bool *of)
{
   uint64_t alufun1 = alufun(icode, ifun);
   if (alufun1 == ADDQ)
   {
      *of = Tools::addOverflow(aluA, aluB);
      return aluA + aluB;
   }
   else if (alufun1 == SUBQ)
   {
      *of = Tools::subOverflow(aluA, aluB);
      return aluB - aluA;
   }
   else if (alufun1 == XORQ)
   {
      return aluA ^ aluB;
   }
   else
   {
      return aluA & aluB;
   }
}

/*
* Sets Cnd if we take a jump or not
*/
uint64_t ExecuteStage::cond(uint64_t icode, uint64_t ifun)
{
   ConditionCodes *codeInstance = ConditionCodes::getInstance();
   bool error;
   bool sf = codeInstance->getConditionCode(SF, error);
   bool of = codeInstance->getConditionCode(OF, error);
   bool zf = codeInstance->getConditionCode(ZF, error);
   if (icode == IJXX || icode == ICMOVXX)
   {
      if (ifun == UNCOND)
      {
         return 1;
      }
      else if (ifun == LESSEQ)
      {
         return ((sf ^ of) | zf);
      }
      else if (ifun == LESS)
      {
         return (sf ^ of);
      }
      else if (ifun == EQUAL)
      {
         return (zf);
      }
      else if (ifun == NOTEQUAL)
      {
         return (!zf);
      }
      else if (ifun == GREATER)
      {
         return (!(sf ^ of) & (!zf));
      }
      else if (ifun == GREATEREQ)
      {
         return (!(sf ^ of));
      }
   }
   return 0;
}

/*
* getter for execute stage dstE
*/
uint64_t ExecuteStage::gete_dstE()
{
   return dstE;
}

/*
* getter for execute stage valE
*/
uint64_t ExecuteStage::gete_valE()
{
   return valE;
}

/*
* returns true if the stat is valid for the current instruction
*/
bool ExecuteStage::calculateControlSignals(uint64_t m_stat, uint64_t W_stat)
{
   if ((m_stat == SADR || m_stat == SINS || m_stat == SHLT) || (W_stat == SADR || W_stat == SINS || W_stat == SHLT))
   {
      return true;
   }
   return false;
}
