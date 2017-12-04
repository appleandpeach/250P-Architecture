//============================================================================
// Name        : MIPS.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "FileStream.hpp"
//#include "MipsSimulator.h"
#include "pipeline.h"
using namespace std;


//bool mipsSimulate(char* filename){
//	FileStream fs(filename);
//	if (fs.openFile()) {
//		string regs = "";
//		// get registers' values
//		fs.getNextInstruction(regs);
//
//		// initiate mips simulator
//		MipsSimulator ms(regs);
//
//		// get instructions
//		string instruction = "";
//		int cnt = 0;
//		while (fs.getNextInstruction(instruction)) {
//			ms.instructions.push_back(instruction);
//			ms.setLabelsIndex(instruction, cnt);
//			cnt++;
//		}
//		// analyze instruction
//		ms.analyzeInstructions();
//	}else{
//		return false;
//	}
//	return true;
//}

bool mipsPipeline(char* filename){
	FileStream fs(filename);
	if (fs.openFile()) {
		string regs = "";
		// get registers' values
		fs.getNextInstruction(regs);

		// initiate mips simulator
		pipeline pl(regs);

		// get instructions
		string instruction = "";
		int cnt = 0;
		while (fs.getNextInstruction(instruction)) {
			pl.instructions.push_back(instruction);
			pl.setLabelsIndex(instruction, cnt);
			cnt++;
		}
		cout << "label map: " << endl;
		for(auto it = pl.labelsPosition.begin(); it!=pl.labelsPosition.end(); it++){
			cout << it->first << ": " << it->second << endl;
		}
		// analyze instruction
		pl.analyzeInstructions();
	} else {
		return false;
	}
	return true;
}

int main() {
//	mipsSimulate("test1.txt");
	mipsPipeline("test2.txt");
	return 0;
}
