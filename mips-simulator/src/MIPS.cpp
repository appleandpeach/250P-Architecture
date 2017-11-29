//============================================================================
// Name        : MIPS.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "FileStream.hpp"
#include "MipsSimulator.h"
using namespace std;

int main() {
//	FileStream fs("test1.txt");
//
//	if(fs.openFile()){
//		string regs = "";
//		fs.getNextInstruction(regs);
//		MipsSimulator ms(regs);
//	}

	string regs = "2,4,6,88,67,45,44,89";
	MipsSimulator ms(regs);
	string ins1 = "add $0,$1,$2";

	return 0;
}
