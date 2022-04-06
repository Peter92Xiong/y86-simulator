class FetchStage : public Stage
{
private:
   void setDInput(D *dreg, uint64_t stat, uint64_t icode, uint64_t ifun,
                  uint64_t rA, uint64_t rB,
                  uint64_t valC, uint64_t valP);
   uint64_t predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP);
   uint64_t PCincrement(int32_t f_pc, bool need_regids, bool need_valC);
   void getRegIds(uint64_t &rA, uint64_t &rB, uint64_t regId);
   uint64_t buildValC(uint64_t valC, bool needRegIds);
   uint64_t selectPC(F *freg, M *mreg, W *wreg);
   bool need_regids(uint64_t icode);
   bool need_valC(uint64_t icode);
   bool instr_valid(uint64_t icode);
   uint64_t f_stat(uint64_t icode, bool mem_error, bool instr_valid);

public:
   bool doClockLow(PipeReg **pregs, Stage **stages);
   void doClockHigh(PipeReg **pregs);
};