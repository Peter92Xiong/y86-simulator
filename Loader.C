/**
 * Names: Brayton and Peter
 * Team: java
*/
#include <iostream>
#include <fstream>
#include <string.h>
#include <ctype.h>
#include <sstream>

using namespace std;

#include "Loader.h"
#include "Memory.h"

//first column in file is assumed to be 0
#define ADDRBEGIN 2 //starting column of 3 digit hex address
#define ADDREND 4   //ending column of 3 digit hext address
#define DATABEGIN 7 //starting column of data bytes
#define COMMENT 28  //location of the '|' character
#define SPACEBAR 6 //space bar location in the .yo file
#define COLON 5 //the colon location in the .yo file

/**
 * Loader constructor
 * Opens the .yo file named in the command line arguments, reads the contents of the file
 * line by line and loads the program into memory.  If no file is given or the file doesn't
 * exist or the file doesn't end with a .yo extension or the .yo file contains errors then
 * loaded is set to false.  Otherwise loaded is set to true.
 *
 * @param argc is the number of command line arguments passed to the main; should
 *        be 2
 * @param argv[0] is the name of the executable
 *        argv[1] is the name of the .yo file
 */
Loader::Loader(int argc, char *argv[])
{
    loaded = false;

    if (argc < 2 || argc > 2) //argc must be 2 if not return
    {
        return;
    }
    if (!checkFile(argv[1]))
    {
        return;
    }

    loadFile();
}

/**
 * @brief this method checks 2 other methods the isYoFile and the doesFileExist
 * In order for the Y86 to work the file must be a .yo and that it does exist
 * 
 * @param fileName is the name of the file that is passed in
 * @return true if the file does exist and it is a .yo file else
 * @return false if the file is not a .yo file OR the file does not exist
 */
bool Loader::checkFile(char *fileName)
{
    return isYoFile(fileName) && doesFileExist(fileName);
}

/**
 * @brief all this method should do is to check if the file does exist
 * it should open the file and read the file.
 * 
 * @param fileName is the name of the file that is passed in
 * @return true if the file is good
 * @return false if the file is no good
 */
bool Loader::doesFileExist(char *fileName)
{
    inf.open(fileName);
    return inf.good();
}

/**
 * @brief this method check the last 3 char of the file name
 * the last 3 char should be ".yo" if it is return true
 * passing in the fileName pointer
 * @param fileName is the name of the file but using a char pointer bc we want to check the last 3 char
 * @return true if the file is a .yo
 * @return false  if the file is NOT a .yo
 */
bool Loader::isYoFile(char *fileName)
{
    uint32_t offset = 0;
    uint32_t strLength = strlen(fileName);
    if (strlen(fileName) > 3)
    {
        offset = strLength - 3;
    }
    char *p = fileName;
    p += offset;
    char match[] = ".yo\n";
    return strcmp(p, match);
}

/**
 * @brief this method will get the line and load the line
 * 
 */
void Loader::loadFile()
{
    string line;
    int linenumber = 1;
    int initialMem = -1;
    bool error;
    lastMemLocation = &initialMem;

    while (getline(inf, line))
    {
        loadLine(line, &error);

        if (error)
        {
            printf("Error on line %d: %s\n", linenumber, line.c_str());
            return;
        }

        linenumber++;
    }

    loaded = true;
}

/**
 * @brief this method load the line and check if there is an error the line
 * 
 * @param line load the line of the file
 * @param error set error to true if there is an error
 */
void Loader::loadLine(string line, bool *error)
{
    bool isComment = true;

    //this if statment check for the comment
    if (line[COMMENT] != '|')
    {                  //"|" this is where the comment start the 28th char
        *error = true; //if theres no comment error is true and just return
        return;
    }
    //ADDRBEGIN "2" starting column of 3 digit hex address
    //Forloop from 2-28
    for (int i = ADDRBEGIN; i < COMMENT; i++)
    {
        if (line[i] != ' ')
        {                      //if the character is not blank
            isComment = false; //iscomment is false
            break;             //if false than break out of the foorloop
        }
    }

    if (isComment)
    {
        return;
    }

    if (line[1] != 'x' || line[0] != '0')
    { //check for x and 0
        *error = true;
        return;
    }

    string address = " ";
    //forloop for 2-28
    for (int i = ADDRBEGIN; i <= ADDREND; i++)
    {
        if (!isxdigit(line[i]))
        {
            *error = true;
            return;
        }

        address = address + line[i];
    }

    if (line[COLON] != ':')
    { //if there is no ":" than error
        *error = true;
        return;
    }

    int32_t baseAddress = stringAddrToHex(address);
    int numOfBits = 0;
    bool readingFinished = false;

    if (line[DATABEGIN] == ' ')
    {
        readingFinished = true;
    }

    for (int i = DATABEGIN; i < COMMENT; i++)
    {
        if ((line[i] != ' ') != readingFinished)
        {
            if (readingFinished)
            {
                continue;
            }

            if (!isxdigit(line[i]))
            {
                *error = true;
                return;
            }

            numOfBits++;

            if (line[i + 1] == ' ')
            {
                readingFinished = true;
            }
        }
        else
        {
            *error = true;
            return;
        }
    }

    if (numOfBits % 2 != 0)
    {
        *error = true;
        return;
    }

    int32_t byteCount = numOfBits / 2;

    if (byteCount > 0 && line[SPACEBAR] != ' ')
    {
        *error = true;
        return;
    }

    if (baseAddress <= *lastMemLocation)
    {
        *error = true;
        return;
    }

    if (baseAddress + byteCount > MEMSIZE)
    {
        *error = true;
        return;
    }

    Memory *mem = Memory::getInstance();

    for (int i = 0; i < byteCount; i++)
    {
        string byte = "";
        byte += line[DATABEGIN + (i * 2)];
        byte += line[DATABEGIN + ((i * 2) + 1)];
        int byteVal = stringAddrToHex(byte);
        mem->putByte(byteVal, baseAddress + i, *error);
        *lastMemLocation = baseAddress + i;
    }
}

/**
 * @brief turn the address to hex value
 * 
 * @param address is what is passed in to turn into hex
 * @return int32_t return the hex value I think that works
 */
int32_t Loader::stringAddrToHex(string address)
{
    istringstream iss(address);
    int32_t addressNum;
    iss >> hex >> addressNum; //base offset
    //iss.str(addressNum);
    return addressNum;
}

//Start by writing a method that opens the file (checks whether it ends
//with a .yo and whether the file successfully opens; if not, return without
//loading)

//The file handle is declared in Loader.h.  You should use that and
//not declare another one in this file.

//Next write a simple loop that reads the file line by line and prints it out

//Next, add a method that will write the data in the line to memory
//(call that from within your loop)

//Finally, add code to check for errors in the input line.
//When your code finds an error, you need to print an error message and return.
//Since your output has to be identical to your instructor's, use this cout to print the
//error message.  Change the variable names if you use different ones.
//  std::cout << "Error on line " << std::dec << lineNumber
//       << ": " << line << std::endl;

//If control reaches here then no error was found and the program
//was loaded into memory.

/**
 * isLoaded
 * returns the value of the loaded data member; loaded is set by the constructor
 *
 * @return value of loaded (true or false)
 */
bool Loader::isLoaded()
{
    return loaded;
}

//You'll need to add more helper methods to this file.  Don't put all of your code in the
//Loader constructor.  When you add a method here, add the prototype to Loader.h in the private
//section.
