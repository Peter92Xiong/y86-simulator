//class to perform the combinational logic of
//the decode stage
class WritebackStage : public Stage
{
public:
    bool doClockLow(PipeReg **pregs, Stage **stages);
    void doClockHigh(PipeReg **pregs);
};