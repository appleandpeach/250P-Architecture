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
	vector<int> registers;
	vector<string> instructions;
	unordered_map<string, int> labelsPosition;



	MipsSimulator(string regs);
	virtual ~MipsSimulator();

	// logical functions
	void parseRegisters(string regs);
	bool executeInstruction(string instruction);
	bool analyzeInstructions();

	// helper
	operator_code hasHit (string optr);
	string removeLabelInstruction(string instruction);


	// get and set functions
	string getOperator(string instruction);
	string getLabel(string instruction);
	int getOutputRegister(string instruction);

	bool setOperand(string instruction, string opt, int &operand1, int &operand2);
	void setLabelsIndex(string instruction, int cnt);
};

#endif /* MIPSSIMULATOR_H_ */
