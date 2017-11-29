/*
 * MipsSimulator.h
 *
 *  Created on: Nov 28, 2017
 *      Author: Mina
 */

#ifndef MIPSSIMULATOR_H_
#define MIPSSIMULATOR_H_



#include <iostream>
#include <string>
#include <vector>
using namespace std;

#define REGS_NUM 8

class MipsSimulator {
public:
	int registers[8];

	MipsSimulator(string regs);
	virtual ~MipsSimulator();

	void parseRegisters(string regs);
	bool executeInstruction(string instruction);
	string getOperator(string instruction);
};

#endif /* MIPSSIMULATOR_H_ */
