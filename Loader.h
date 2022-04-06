using namespace std;

class Loader
{
private:
   bool loaded;  //set to true if a file is successfully loaded into memory
   ifstream inf; //input file handle

   bool isYoFile(char *fileName);
   bool checkFile(char *fileName);
   bool doesFileExist(char *fileName);
   void loadFile();
   void loadLine(string line, bool *error);
   int32_t stringAddrToHex(string num);
   int32_t *lastMemLocation;

public:
   Loader(int argc, char *argv[]);
   bool isLoaded();
};
