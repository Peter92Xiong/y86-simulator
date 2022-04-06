#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "M.h"
#include "W.h"
#include "D.h"
#include "E.h"
#include "Stage.h"
#include "FetchStage.h"
#include "Status.h"
#include "ExecuteStage.h"
#include "Instructions.h"
#include "Memory.h"
#include "Tools.h"
#include "DecodeStage.h"
#include <iomanip>
#include <iostream>
#include "Debug.h"


/**
 * @brief this is the dolocklow does does the input for this file.
 * 
 * @param pregs from the pipereg.
 * @param stages from the stage file.
 * @return true if true.
 * @return false if false.
 */
bool FetchStage::doClockLow(PipeReg **pregs, Stage **stages)
{

   F *freg = (F *)pregs[FREG];
   D *dreg = (D *)pregs[DREG];
   M *mreg = (M *)pregs[MREG];
   W *wreg = (W *)pregs[WREG];
   uint64_t f_pc = 0, icode = 0, ifun = 0, valC = 0, valP = 0;
   uint64_t rA = RNONE, rB = RNONE, stat = SAOK;

   f_pc = FetchStage::selectPC(freg, mreg, wreg);

   Memory *mem = Memory::getInstance();
   bool mem_error;
   uint8_t i_byte = mem->getByte(f_pc, mem_error);
   ifun = Tools::getBits(i_byte, 0, 3);
   icode = Tools::getBits(i_byte, 4, 7);

   valP = PCincrement(f_pc, need_regids(icode), need_valC(icode));
   uint64_t predictedPC = predictPC(icode, valC, valP);
   freg->getpredPC()->setInput(predictedPC);

   bool needRegIds = FetchStage::need_regids(icode);
   bool needValC = FetchStage::need_valC(icode);

   if (needRegIds == true)
   {
      getRegIds(rA, rB, f_pc);
      if (needValC == true)
      {
         valC = buildValC(f_pc, needRegIds);
      }
      else
      {
         valC = 0;
      }
   }
   else
   {
      if (needValC == true)
      {
         valC = buildValC(f_pc, needRegIds);
      }
      else
      {
         valC = 0;
      }
      rA = RNONE;
      rB = RNONE;
   }

   // Lab11 @@@@@@@@@@@@@@@
   bool instrValid = instr_valid(icode);
   stat = f_stat(icode, mem_error, instrValid);
   // if (mem_error) {
   //    icode = INOP;
   // }

   // if (mem_error) {
   //    ifun = FNONE;
   // }

   //@@@@@@@@@@note to self try this later
   // if (mem_error) {
   //    icode = INOP;
   //    ifun = FNONE;
   // }
   if(icode){
      mem_error = INOP;
   }

   if(ifun){
      mem_error = FNONE;
   }

   setDInput(dreg, stat, icode, ifun, rA, rB, valC, valP);

   //lab9
   // if (debug) std::cout << std::hex << icode << std::endl;  //print icode in hex



   return false;
}


/**
 * @brief this is the dolockhigh not much code.
 * 
 * @param pregs from the pipereg file.
 */
void FetchStage::doClockHigh(PipeReg **pregs)
{
   F *freg = (F *)pregs[FREG];
   D *dreg = (D *)pregs[DREG];

   dreg->getstat()->normal();
   dreg->geticode()->normal();
   freg->getpredPC()->normal();
   dreg->getifun()->normal();
   dreg->getrB()->normal();
   dreg->getvalC()->normal();
   dreg->getvalP()->normal();
   dreg->getrA()->normal();
}

/**
 * @brief this is the selectPC method that does the selction of the pc.
 * 
 * @param freg this is from the f file.
 * @param mreg this is from the m file.
 * @param wreg this is from the w file.
 * @return uint64_t return  wreg or freg wreg.
 */
uint64_t FetchStage::selectPC(F *freg, M *mreg, W *wreg)
{
   uint64_t m_icode = mreg->geticode()->getOutput();
   uint64_t w_icode = wreg->geticode()->getOutput();
   uint64_t m_Cnd = mreg->getCnd()->getOutput();

   if (m_icode == IJXX && !m_Cnd)
   {
      return mreg->getvalA()->getOutput();
   }
   if (w_icode == IRET)
   {
      return wreg->getvalM()->getOutput();
   }
   return freg->getpredPC()->getOutput();
}


/**
 * @brief this is the setDinput that does the setting of the d input.
 * 
 * @param dreg is the deg.
 * @param stat is the stage.
 * @param icode is form the instruion.
 * @param ifun is from the instruction.
 * @param rA is from the ra if it has one.
 * @param rB is from the rb if it has one.
 * @param valC is the valc of the instruction.
 * @param valP is the valcP of then instruction.
 */
void FetchStage::setDInput(D *dreg, uint64_t stat, uint64_t icode,
                           uint64_t ifun, uint64_t rA, uint64_t rB,
                           uint64_t valC, uint64_t valP)
{
   dreg->getstat()->setInput(stat);
   dreg->geticode()->setInput(icode);
   dreg->getifun()->setInput(ifun);
   dreg->getrA()->setInput(rA);
   dreg->getrB()->setInput(rB);
   dreg->getvalP()->setInput(valP);
   dreg->getvalC()->setInput(valC);
}

/**
 * @brief thisdoes the pc increment code that increment basiced of the need regi.
 * 
 * @param f_pc is the pc.
 * @param need_regids is the need regid.
 * @param need_valC the valC.
 * @return uint64_t will return 1 or + 8 always + 1 or defult.
 */
uint64_t FetchStage::PCincrement(int32_t f_pc, bool need_regids, bool need_valC)
{
   if (need_regids)
   {
      f_pc += 1;
   }

   if (need_valC)
   {
      f_pc += 8;
   }

   f_pc += 1;

   return f_pc;
}

/**
 * @brief this does the need valc code that input the instruction in.
 * 
 * @param icode is from the instruction.
 * @return true only if the insturciton is true.
 * @return false only if its not true.
 */
bool FetchStage::need_valC(uint64_t icode)
{
   return (icode == IRMMOVQ || icode == IIRMOVQ || icode == ICALL || icode == IMRMOVQ || icode == IJXX);
}


/**
 * @brief this does the prediction of the pc. only for jump and call
 * 
 * @param f_icode look for the icode if its jump or call.
 * @param f_valC will teturn this if its jump or call.
 * @param f_valP will reutn this else otherwise.
 * @return uint64_t will return fvalc or valp.
 */
uint64_t FetchStage::predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP)
{

   if (f_icode == IJXX || f_icode == ICALL)
   {
      return f_valC;
   }
   else
   {
      return f_valP;
   }
}


/**
 * @brief this does the regid neededd for the pc.
 * 
 * @param icode from the code.
 * @return true only if icode is == to the following.
 * @return false else its false.
 */
bool FetchStage::need_regids(uint64_t icode)
{
   return (icode == IPOPQ || icode == IIRMOVQ || icode == IPUSHQ || icode == IRMMOVQ || icode == IMRMOVQ || icode == IRRMOVQ || icode == IOPQ);
}

/**
 * @brief this gets the regIDs.
 * 
 * @param rA from the code instrction.
 * @param rB from the code instrction.
 * @param f_pc is the ftue pc.
 */
void FetchStage::getRegIds(uint64_t &rA, uint64_t &rB, uint64_t f_pc)
{
   Memory *memInstance = Memory::getInstance();
   bool error = false;

   rA = memInstance->getByte(f_pc + 1, error);
   rA = rA >> 4;

   rB = memInstance->getByte(f_pc + 1, error);
   rB = rB & 0x0F;
}

/**
 * @brief this does the building of the valc.
 * 
 * @param f_pc from the fpc.
 * @param needRegIds will need this.
 * @return uint64_t will return and shift if needed.
 */
uint64_t FetchStage::buildValC(uint64_t f_pc, bool needRegIds)
{
   Memory *memInstance = Memory::getInstance();
   uint64_t word = 0;
   bool error;

   if (needRegIds)
   {
      for (unsigned int i = 9; i >= 2; i--)
      {
         word += memInstance->getByte(f_pc + i, error);
         if (i != 2)
         {
            word = word << 8;
         }
      }
   }
   else
   {
      for (unsigned int i = 8; i >= 1; i--)
      {
         word += memInstance->getByte(f_pc + i, error);
         if (i != 1)
         {
            word = word << 8;
         }
      }
   }
   return word;
}

//lab11 @@@@@@@@@@@@@@@@@@@@@@@@@@@

/*
checks if icode is a valid istruction
*/
bool FetchStage::instr_valid(uint64_t icode) {
   if (icode == INOP || icode == IHALT || icode == IRRMOVQ 
    || icode == IIRMOVQ || icode == IRMMOVQ || icode == IMRMOVQ 
    || icode == IOPQ || icode == IJXX || icode == ICALL 
    || icode == IRET || icode == IPUSHQ || icode == IPOPQ) {
       return true;
    }
    return false;
}


/*
determines the fetch stat
*/
uint64_t FetchStage::f_stat(uint64_t icode, bool mem_error, bool instr_valid) {
   if (mem_error) {
      return SADR;
   }
   if (!instr_valid) {
      return SINS;
   }
   if (icode == IHALT) {
      return SHLT;
   }
   return SAOK;
}


