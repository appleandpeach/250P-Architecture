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
#include <unordered_map>
using namespace std;

#define REGS_NUM 8

enum operator_code {
	add,
	addi,
	sub,
	mul,
	div,
	b,
	beq,
	bnq,
	end,
	label,
	err
};

class MipsSimulator {
public:
	int index;
	int registers[8];
	vector<string> instructions;
	unordered_map<string, int> labelsPosition;



	MipsSimulator(string regs);
	virtual ~MipsSimulator();

	void parseRegisters(string regs);
	bool executeInstruction(string instruction);
	bool analyzeInstructions();
	string getOperator(string instruction);
	int getOutputRegister(string instruction);
	operator_code hasHit (string optr);
	void setOperand(string instruction, int &operand1, int &operand2);
	string removeLabelInstruction(string instruction);
};

#endif /* MIPSSIMULATOR_H_ */
