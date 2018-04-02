//
//  main.cpp
//  COMPORG_PROJECT2
//
//  Created by Mahmud Ahmad on 12/12/17.
//  Copyright © 2017 Mahmud Ahmad. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <unistd.h>
#include <sstream>

using namespace std;

bool goToNextState();
string readMemory(int address, int size);
string writeMemory(int address, int size, string data);
int isInt(char chr);
string reverseString(string toReverse);
string intToBinary(int _int, int length);
uint64_t hexToInt(string hexString);
string hexToBinary(string hexString, int desiredBinLength);
int binToInt(string binString, bool isUnsigned);
ofstream writeToFile(string fileName, string lineToWrite, bool closeFile);
void readFromFile(string fileName);
void loadProgramCode(string filePath);
void pritnSignalsAndBusses(string state);
string getOpCode(string instruction);
void initialPopulateMemory();
int itypeGetImm();
string getRegisterFromIR();
void fetchInstruction();
void updatePCandLoadAB();
int getALUfromRtype();
void rtypeWriteBack();
void rtypeMemory();
void rtypeExecute(int _aluop);
void rtypeDataPath();
void loadMemoryDataPathWriteBack();
void loadMemoryDataPathMemory();
void loadMemoryDataPathExecute();
void branchDataPathMemory();
void branchDataPathExecute();
void branchDataPathWriteBack();
void storeMemoryDataPathExecute();
void storeMemoryDataPathMemory();
void storeMemoryDataPathWriteBack();
void aluidataPathWriteBack();
void aluIdataPathMemory();
void aluIdataPathExecute();
void itypeDataPath();
int jtypeGetImmediate();
void jtypeDataPath();
void pickDataPath();


int numberOfInstructions = 0;
int lastInstructionAddress = 0;
int instructionsRan = 0;

int mdr, mar, pc, s1, s2, addr, a, b, c, dest = 0;
string databus;
int s2op, aluop, cload, regload, aload, aoe, bload, boe, regselect, 
         irload, iroes1, iroes2, reset, pcload, pcoes1, pcmarselect, marload, 
         memread, memwrite, memop, mdrload, mdroes2, zflag = 0;

string ir = "00000000000000000000000000000000";
string memory[5000] = {};
int memorysize = 5000;
int registerValuesArray[32] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31
};

string registerNameArray[32] = {
    "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7",
    "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",
    "R16", "R17", "R18", "R19", "R20", "R21", "R22", "R23",
    "R24", "R25", "R26", "R27", "R28", "R29", "R30", "R31"
};

bool goToNextState() {
    cout << "Enter any key to continue \n";
    cin.clear();
    fflush(stdin);
    if(cin.get()) {
        cout << "True\n";
        return true;
    } else {
        cout << "False\n";
        goToNextState();
    }
    return true;
}

string readMemory(int address, int size) {
    //size: 0 = word,
    //      1 = half-word
    //      2 = byte
    //      other = return error
    string stringReturned = "";
    if(address > memorysize || address < 0) {
        return "Error, invalid address";
    }
    if(size == 0) {
        stringReturned = memory[address];
        stringReturned += memory[address + 1];
        stringReturned += memory[address + 2];
        stringReturned += memory[address + 3];
        return stringReturned;
    } else if(size == 1) {
        stringReturned = memory[address];
        stringReturned += memory[address + 1];
        return stringReturned;
    } else if(size == 2) {
        stringReturned = memory[address];
        return stringReturned;
    } else {
        return "Error, not a valid size";
    }
}

string writeMemory(int address, int size, string data) {
    //size: 0 = word,
    //      1 = half-word
    //      2 = byte
    //      other = return error
    if(address > memorysize || address < 0) {
        return "Error, invalid address";
    }
    if(size == 0) {
        string intermediate1 = "";
        string intermediate2 = "";
        string intermediate3 = "";
        string intermediate4 = "";
        for(int i = 0; i < 8; i++) {
            intermediate1 += data[i];
            intermediate2 += data[i + 8];
            intermediate3 += data[i + 16];
            intermediate4 += data[i + 24];
        }
        memory[address] = intermediate1;
        memory[address + 1] = intermediate2;
        memory[address + 2] = intermediate3;
        memory[address + 3] = intermediate4;
        return "Success";
    } else if(size == 1){
        string intermediate1 = "";
        string intermediate2 = "";
        for(int i = 0; i < 8; i++) {
            intermediate1 += data[i];
            intermediate2 += data[i + 8];
        }
        memory[address] = intermediate1;
        memory[address + 1] = intermediate2;
        return "Success";
    } else if(size == 2) {
        memory[address] = data;
        return "Success";
    } else {
        return "Error, not a valid size";
    }
}

int isInt(char chr) {
    /*
     this function checks if a character
     falls between 0 - 9. returns 1 if
     true, 0 if false
    */
    if(chr >= '0' && chr <= '9') {
        return 1;
    } else {
        return 0;
    }
}

string reverseString(string toReverse) {
    /*
        this function reverses the order
        of a string
    */
    string reversed = "";
    for(int i = 0; i < toReverse.length(); i++) {
        char x = toReverse[toReverse.length() - (i+1)];
        //cout << x << "\n";
        reversed += x;
    }
    return reversed;
}

string intToBinary(int _int, int length) {
    /*
        this method convers an integer to
        a binary string
    */
    uint64_t x;
    string appendage;
    if(_int < 0) {
        x = -1 * _int;
        x = ~x;
        x++;
        appendage = "1";
    } else {
        x = _int;
        appendage = "0";
    }
    string binaryString = "";
    while(x >= 1) {
        binaryString += to_string(x % 2);
        x = x - (x % 2);
        x = x / 2;
    }
    binaryString = reverseString(binaryString);
    if(binaryString.length() < length) {
        for(int i = 0; i < binaryString.length() - length; i++) {
            binaryString.insert(0, appendage);
        }
    } else if(binaryString.length() > length) {
        string intermediate;
        for(int i = 0; i < length; i ++) {
            intermediate += binaryString[binaryString.length() - (i + 1)];
        }
        binaryString = reverseString(intermediate);
    }
    return binaryString;
}

string intToBinaryUnsigned(uint64_t _int, int length) {
    string binaryString = "";
    while(_int >= 1) {
        binaryString += to_string(_int % 2);
        _int = _int - (_int % 2);
        _int = _int / 2;
    }
    binaryString = reverseString(binaryString);
    if(binaryString.length() < length) {
        for(int i = 0; i < binaryString.length() - length; i++) {
            binaryString.insert(0, "0");
        }
    } else if(binaryString.length() > length) {
        string intermediate;
        for(int i = 0; i < length; i ++) {
            intermediate += binaryString[binaryString.length() - (i + 1)];
        }
        binaryString = reverseString(intermediate);
    }
    
    
    return binaryString;
}

uint64_t hexToInt(string hexString) {
    /*
        this function converts a hex string
        to an integer
    */
    uint64_t returnedNumber = 0;
    uint64_t powerOfHex = hexString.length() - 1;
    for(int i = 0; i < hexString.length(); i++) {
        if(isInt(hexString[i]) == 1) {
            returnedNumber += (hexString[i] - 48) * (pow(16, powerOfHex - i));
        } else {
            returnedNumber += (hexString[i] - 55) * (pow(16, powerOfHex - i));
        }
    }
    return returnedNumber;
}

string hexToBinary(string hexString, int desiredBinLength) {
    /*
        this function returns a binary string
        from a hex stirng
    */
    if(desiredBinLength < hexString.length()/4) {
        return "error";
    } else {
        return intToBinaryUnsigned(hexToInt(hexString), desiredBinLength);
    }
}

int binToInt(string binString, bool isUnsigned) {
    uint64_t powerOfTwo = binString.length() - 1;
    int intToReturn = 0;
    if(binString[0] == '1' && isUnsigned == 0) {
            intToReturn += -1 * pow(2, powerOfTwo);
        for(int i = 1; i < binString.length(); i++) {
            intToReturn += (binString[i] - 48) * pow(2, powerOfTwo -i);
        }
    } else {
        for(int i = 0; i < binString.length(); i++) {
            intToReturn += (binString[i] - 48) * (pow(2, powerOfTwo - i));
        }
    }
    return intToReturn;
}

ofstream writeToFile(string fileName, string lineToWrite, bool closeFile){
    ofstream file;
    file.open(fileName, ios::out | ios::app);
    if(file.is_open()) {
        file << lineToWrite + "\n";
    } 
    if(closeFile == true) {
        file.close();
    }
    return file;
}

void readFromFile(string fileName) {
    string str;
    ifstream file(fileName);
    if(file.is_open()) {
        while(getline(file, str)) {
            cout << str << "\n";
            str = hexToBinary(str, 32);
            writeMemory(numberOfInstructions * 4, 0, str);
            numberOfInstructions++;
        }
        file.close();
    }
    lastInstructionAddress = (numberOfInstructions - 1) * 4;
}

void loadProgramCode(string filePath) {
    readFromFile(filePath);
    fetchInstruction();
}

void printSignalsAndBusses(string state) {
    state += "\n";
    cout << state;
    stringstream buffer;
    buffer << "******** SIGNALS ********\n" <<
            "S2OP: " << s2op << ", " <<
            "ZFlag: " << zflag << ", " <<
            "ALUop: " << aluop << ", " <<
            "CLoad: " << cload << ", " <<
            "REGLoad: " << regload << ", " <<
            "ALoad: " <<  aload << ", " <<
            "Aoe: " << aoe << ", " <<
            "BLoad: " << bload << ", " <<
            "Boe: " << boe << ", " <<
            "REGSelect: " << regselect << ", " <<
            "IRLoad: " << irload << ", " <<
            "IRoes1: " << iroes1 << ", \n" <<
            "IRoes2: " << iroes2 << ", " <<
            "Reset: " << reset << ", " <<
            "PCLoad: " << pcload << ", " <<
            "PCoes1: " << pcoes1 << ", " <<
            "PCMARSelect: " << pcmarselect << ", " <<
            "MARLoad: " << marload << ", " <<
            "MemRead: " << memread << ", " <<
            "MDRLoad: " << mdrload << ", " <<
            "MDRoes2: " << mdroes2 << ", " <<
            "MemWrite: " << memwrite << ", " <<
            "MemOp: " << memop << "\n" <<
            "******** /SIGNALS ********\n\n";

    string signals = buffer.str();
    cout << signals;
    buffer.str(string());

    buffer << "******** REGISTERS ********\n" <<
            "MDR: " << mdr << ", " <<
            "MAR: " << mar << ", " <<
            "PC: " << pc << ", " <<
            "A: " << a << ", " <<
            "B: " << b << ", " <<
            "C: " << c << "\n" <<
            "IR: " << ir << "\n" <<
            "******** /REGISTERS ********\n\n";

    string registers = buffer.str();
    cout << registers;
    buffer.str(string());

    buffer <<  "******** BUSSES ********\n" <<
             "S1: " << s1 << ", " <<
             "S2: " << s2 << ", " <<
             "Address: " << addr << ", " <<
             "Destination: " << dest << ", " <<
             "Data: " << databus << "\n" <<
             "******** /BUSSES ********\n\n";

    string busses = buffer.str();
    cout << busses;
    buffer.str(string());

    buffer << "******** REG FILE ********\n";
    for(int i = 0; i < 32; i++) {
        buffer << registerNameArray[i] << ": " << registerValuesArray[i] << ", ";
        if(i == 15) {
            buffer << "\n";
        }
    }
    buffer << "\n******** /REG FILE ********\n\n";
    string regFile = buffer.str();
    cout << regFile;
    buffer.str(string());
    writeToFile("out.txt", state, false);
    writeToFile("out.txt", signals, false);
    writeToFile("out.txt", registers, false);
    writeToFile("out.txt", busses, false);
    if(pc > lastInstructionAddress && state.find("Write Back") != string::npos) {
        writeToFile("out.txt", regFile, true);
    } else {
        writeToFile("out.txt", regFile, false);
        writeToFile("out.txt", "\n\n\n\n", false);
    }
}

string getOpcode(string instruction) {
    string opcode = "";
    for(int i = 0; i < 6; i++) {
        opcode += instruction[i];
    }
    return opcode;
}

void initialPopulateMemory() {
    for(int i = 0; i < memorysize; i++) {
        memory[i] = "0000";
    }
}

int itypeGetImm() {
    string intermediate = "";
    for(int i = 16; i < 32; i++) {
        intermediate += ir[i];
    }
    return binToInt(intermediate, 0);
}

string getRegisterFromIR(int registerNumber) {
    //registerNumber 0 = rs1
    //               1 = rs2
    //               2 = rs3
    int startIndex = 0;
    string stringToReturn = "";
    if(registerNumber == 0) {
        startIndex = 6;
    } else if(registerNumber == 1) {
        startIndex = 11;
    } else if(registerNumber == 2) {
        startIndex = 16;
    } else {
        return "Invalid Regsiter";
    }
    for(int i = startIndex; i < startIndex + 5; i++) {
        stringToReturn += ir[i];
    }
    return stringToReturn;
}

void fetchInstruction() {
    if(pc > lastInstructionAddress) {
        cout << "Last instruction already ran\n";
        exit(0);
    }
    zflag = 0;
    regload = 0;
    pcmarselect = 0;
    addr = pc;
    memread = 1;
    memop = 0;
    databus = memory[pc];
    databus += memory[pc + 1];
    databus += memory[pc + 2];
    databus += memory[pc + 3];
    irload = 1;
    ir = databus;
    printSignalsAndBusses("Fetching Instruction");
    if(goToNextState() == true) {
        updatePCandLoadAB();
    }
}

void updatePCandLoadAB() {
    instructionsRan++;
    pcoes1 = 1;
    s1 = pc;
    s2op = 7;
    aluop = 0;
    dest = s1 + 4;
    pcload = 1;
    pc = dest;
    aload = 1;
    a = registerValuesArray[binToInt(getRegisterFromIR(0), 1)];
    bload = 1;
    b = registerValuesArray[binToInt(getRegisterFromIR(1), 1)];;
    //print out
    printSignalsAndBusses("Updating PC and Loading A, B");
    if(goToNextState() == true) {
        pickDataPath();
    }
    //aload, bload = 0;
}

int getALUfromRtype() {
    string aluopString = "";
    for(int i = 21; i < 32; i++) {
        aluopString += ir[i];
    }

    int _aluop = binToInt(aluopString, 1);

    //function goes in here... so for add itd be case "00001000000"
    switch(_aluop) {
        case 32:
            //add
            return 0;
            break;
        case 34:
            //sub
            return 1;
            break;
        case 4:
            //sll
            return 8;
            break;
        case 6:
            //srl;
            return 10;
            break;
        case 36:
            //and
            return 4;
            break;
        case 37:
            //or
            return 5;
            break;
        default:
            cout << "Error \n";
            return -1;
            break;
    }
}

void rtypeWriteBack() {
    cload = 1;
    c = dest;
    regload = 1;
    regselect = 0;
    registerValuesArray[binToInt(getRegisterFromIR(2), 1)] = c;
    //go to next instruction
    printSignalsAndBusses("RType: Write Back");
    if(goToNextState() == true) {
        fetchInstruction();
    }
}

void rtypeMemory() {
    //nothig happens here
    //go to writeback
    printSignalsAndBusses("RType: Memory");
    if(goToNextState() == true) {
        rtypeWriteBack();
    }
}

void rtypeExecute(int _aluop) {
    aoe = 1;
    boe = 1;
    s1 = a;
    s2 = b;
    s2op = 0;
    aluop = _aluop;
    switch(aluop) {
        case 0:
            dest = s1 + s2;
            break;
        case 1:
            dest = s1 - s2;
            break;
        case 8:
            dest = s1 * pow(2, s2);
            break;
        case 10:
            dest = s1 / pow(2, s2);
            break;
        case 4:
            dest = s1 & s2;
            break;
        case 5:
            dest = s1 | s2;
            break;
        default:
            cout << "Error \n";
            break;
    }
    printSignalsAndBusses("RType: Execute");
    //go to memory
    if(goToNextState() == true) {
        rtypeMemory();
    }
}

void rtypeDataPath() {
    //get type of ALU OP
    int _aluop = getALUfromRtype();
    //go to execute
    rtypeExecute(_aluop);
}

void loadMemoryDataPathWriteBack(int size) {
    mdroes2 = 1;
    s2 = mdr;
    if(size == 0) {
        s2op = 0;
    } else if(size == 1) {
        s2op = 3;
    } else if(size == 2) {
        s2op = 1;
    }
    aluop = 3;
    dest = s2;
    cload = 1;
    c = dest;
    regload = 1;
    regselect = 1;
    registerValuesArray[binToInt(getRegisterFromIR(1), 1)] = c;
    //go to next instruction
    //printout
    printSignalsAndBusses("Load Instruction: Write Back");
    if(goToNextState() == true) {
        fetchInstruction();
    }
    //mdroes2 = 0;  
}

void loadMemoryDataPathMemory(int size) {
    pcmarselect = 1;
    addr = mar;
    memop = size;
    memread = 1;
    databus = readMemory(addr, size);
    memread = 0;
    mdrload = 1;
    mdr = binToInt(databus, 0);
    //go to write back
    printSignalsAndBusses("Load Instruction: Memeory");
    if(goToNextState() == true) {
        loadMemoryDataPathWriteBack(size);
    }
}

void loadMemoryDataPathExecute(int size) {
    //used for loading word, byte, hw
    //size: 0 = word
    //      1 = half word
    //      2 = byte
    //      other = return error
    aoe = 1;
    iroes1 = 1;
    s1 = a;
    s2 = itypeGetImm();
    s2op = 3;
    aluop = 0;
    dest = s1 + s2;
    marload = 1;
    mar = dest;
    //go to memory
    printSignalsAndBusses("Load Instruction: Execute");
    if(goToNextState() == true) {
        loadMemoryDataPathMemory(size);
    }
}

void branchDataPathWriteBack() {
    pcoes1 = 1;
    iroes2 = 1;
    s1 = pc;
    s2 = itypeGetImm();
    s2op = 3;
    aluop = 1;
    dest = s1 + s2;
    pcload = 1;
    pc = dest;
    //go to next instruction
    printSignalsAndBusses("Branch Instruction: Write Back");
    if(goToNextState() == true) {
        fetchInstruction();
    }
}

void branchDataPathMemory() {
    //nothign happens here
    //continue to write back
    printSignalsAndBusses("Branch Instruction: Memory");
    if(goToNextState() == true) {
        branchDataPathWriteBack();
    }
}

void branchDataPathExecute(int isEqz) {
    //used when branching
    //eqz:  1 = eaz;
    //      0 = neqz;
    aoe = 1;
    s1 = a;
    aluop = 2;
    if(a == 0) {
        zflag = 1;
    } else {
        zflag = 0;
    }
    if(zflag == 1) {
        if(isEqz == 1) {
            //continue
            branchDataPathMemory();
        } else if(isEqz == 0) {
            //break, go to next inst
            fetchInstruction();
        } else {
            cout << "Invalid instruction\n";
        }
    } else if(zflag == 0) {
        if(isEqz == 1) {
            //break, go to next inst
            fetchInstruction();
        } else if(isEqz == 0) {
            //continue
            branchDataPathMemory();
        } else {
            cout << "Invald instruction\n";
        }
    } else {
        cout << "Invalid Zflag\n";
    }
    printSignalsAndBusses("Branch Instruction: Execute");
    if(goToNextState() == true) {
        branchDataPathMemory();
    }
}

void storeMemoryDataPathWriteBack() {
    //nothing happens here
    //go to next instruction
    printSignalsAndBusses("Store Instruction: Write Back");
    if(goToNextState() == true) {
        fetchInstruction();
    }
}

void storeMemoryDataPathMemory(int size) {
    int length = 0;
    if(size == 0) {
        length = 32;
    } else if(size == 1) {
        length = 16;
    } else if(size == 2) {
        length = 8;
    } else {
        cout << "Invalid size\n";
    }
    pcmarselect = 1;
    addr = mar;
    databus = intToBinary(mdr, length);
    memop = size;
    memwrite = 1;
    writeMemory(addr, size, databus);
    //go to write back
    printSignalsAndBusses("Store Instruction: Memory");
    if(goToNextState() == true) {
        storeMemoryDataPathWriteBack();
    }   
}

void storeMemoryDataPathExecute(int size) {
    //used when storing data into memory;
    //size: 0 = word
    //      1 = half word
    //      2 = byte
    //      other = return error
    aoe = 1;
    iroes1 = 1;
    s1 = a;
    s2 = itypeGetImm();
    s2op = 3;
    aluop = 0;
    dest = s1 + s2;
    marload = 1;
    mar = dest;
    boe = 1;
    s2 = b;
    s2op = 0;
    aluop = 3;
    dest = s2;
    memread = 0;
    mdrload = 1;
    mdr = dest;
    //go to memory
    printSignalsAndBusses("Store Instruction: Execute");
    if(goToNextState() == true) {
        storeMemoryDataPathMemory(size);
    }
}

void aluidataPathWriteBack() {
    cload = 1;
    c = dest;
    regload = 1;
    regselect = 1;
    registerValuesArray[binToInt(getRegisterFromIR(1), 1)] = c;
    //go to next instruction
    printSignalsAndBusses("IType Math Instructions: Write Back");
    if(goToNextState() == true) {
        fetchInstruction();
    }
}

void aluIdataPathMemory() {
    //nothing happens here
    //go to writeback
    printSignalsAndBusses("IType Math Instructions: Memory");
    if(goToNextState() == true) {
        aluidataPathWriteBack();
    }
}

void aluIdataPathExecute(int _aluop) {
    //used for i type math instructions
    aoe = 1;
    iroes2 = 1;
    s1 = a;
    s2 = itypeGetImm();
    s2op = 3;
    aluop = _aluop;
    switch(aluop) {
        case 0:
            dest = s1 + s2;
            break;
        case 1:
            dest = s1 - s2;
            break;
        case 5:
            dest = s1 | s2;
            break;
        default:
            cout << "Error \n";
            break;
    }
    printSignalsAndBusses("IType Math Instructions: Execute");
    if(goToNextState() == true) {
        aluIdataPathMemory();
    }
    //go to memory

}

void itypeDataPath() {
    //figure out if ALU, Load, Store, Branch, or Jump
    int opcode = binToInt(getOpcode(ir), 1);
    switch(opcode) {
        case 32:
            //lb
            loadMemoryDataPathExecute(2);
            break;
        case 33:
            //lh
            loadMemoryDataPathExecute(1);
            break;
        case 35:
            //lw
            loadMemoryDataPathExecute(0);
            break;
        case 4:
            //beqz
            branchDataPathExecute(1);
            break;
        case 5:
            //bnez;
            branchDataPathExecute(0);
            break;
        case 40:
            //sb
            storeMemoryDataPathExecute(2);
            break;
        case 41:
            //sh
            storeMemoryDataPathExecute(1);
            break;
        case 43:
            //sw
            storeMemoryDataPathExecute(0);
            break;
        case 8:
            //add
            aluIdataPathExecute(0);
            break;
        case 10:
            //sub
            aluIdataPathExecute(1);
            break;
        case 13:
            //or
            aluIdataPathExecute(5);
            break;
        default:
            cout << "Error, Instruction not Supported";
            break;
    }
}

int jtypeGetImmediate() {
    string intermediate = "";
    for(int i = 6; i < 32; i++) {
        intermediate += ir[i];
    }
    return binToInt(intermediate, 0);
}

void jtypeDataPath() {
    //execute
    pcoes1 = 1;
    s1 = pc;
    iroes2 = 1;
    s2 = jtypeGetImmediate();
    s2op = 5;
    aluop = 0;
    dest = s1 + s2;
    printSignalsAndBusses("Jump Execute");
    if(goToNextState() == true) {
        //just waiting to continue
    }

    //memory
    //nothing happens here
    printSignalsAndBusses("Jump Memory");
    if(goToNextState() == true) {
        //just wating to continues
    }

    //writeback
    pcload = 1;
    pc = dest;
    printSignalsAndBusses("Jump Write Back");
    if(goToNextState() == true) {
        fetchInstruction();
    }
}

void pickDataPath() {
    string opcode = getOpcode(ir);
    if(opcode.compare("000000") == 0) {
        rtypeDataPath();
    } else if(opcode.compare("000010") == 0) {
        jtypeDataPath();
    } else {
        itypeDataPath();
    }
}

int main(int argc, const char * argv[]) {
    //final flow
   ofstream outputFile("out.txt");
   initialPopulateMemory();
   string inputFileName = "";
   if(*(argv + 1) == nullptr) {
       cout << "What is the path to your file?\n";
       cin >> inputFileName;
   } else {
       inputFileName = *(argv + 1);
       cout << "FILE NAME IS: \n";
       cout << inputFileName + "\n";
   }
   loadProgramCode(inputFileName);
    
    
    //readInFileLineByLine
    //pickInstructionDataPath(); //this encompases if(opCode == "000000") -> RType ALU Instrctions else -> IType Instrction
    //if IType instruction, figure out if load or store or alu or branch/jump
    
    //testing
    //continue on enter
    /*
    cout << "e to cont" << '\n';
    if(cin.get() == '\n') {
        cout << "worked" << '\n';
    } else {
        cout << "it didnt" << '\n';
    }
    */
    
//    memory[0] = "10100000";
//    memory[1] = "01000100";
//    memory[2] = "00000000";
//    memory[3] = "00000000";
//    memory[4] = "10000000";
//    memory[5] = "00100001";
//    memory[6] = "00000000";
//    memory[7] = "00000000";
//    lastInstructionAddress += 4;
//    fetchInstruction();
    //storeMemoryDataPathExecute(2);
    
    
    
    return 0;
}

/*
    TODO:   READ IN FILE, READ LINE BY LINE
            RUN IF AND RF, THEN FIGURE OUT 
            WHAT THE NEXT STATE SHOULD BE
            BASED ON THE INSTRUCTION TYPE
*/





