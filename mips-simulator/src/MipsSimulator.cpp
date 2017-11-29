/*
 * MipsSimulator.cpp
 *
 *  Created on: Nov 28, 2017
 *      Author: Mina
 */

#include "MipsSimulator.h"

void MipsSimulator::parseRegisters(string regs){
	string delim = ",";
	auto start = 0U;
	auto end = regs.find(delim);
	int cnt = 0;
	while (end != std::string::npos){
		this->registers[cnt] = stoi(regs.substr(start, end - start));
		cout << this->registers[cnt] << endl;
		cnt ++;
		start = end + delim.length();
		end = regs.find(delim, start);
	}
	this->registers[cnt] = stoi(regs.substr(start, end - start));
	cout << this->registers[cnt] << endl;
}

string MipsSimulator::getOperator(string instruction){
	string delim = " ";
	auto start = 0U;
	auto end = instruction.find(delim);
	string res = instruction.substr(start, end - start);
	if(res.size() > 5)
		res = res.substr(0, 5);
	return res;
}

bool MipsSimulator::executeInstruction(string instruction){
	string opt = this->getOperator(instruction);
	string opr1 = 0;
	switch(opt){
	case "add":{

		break;
	}
	case "addi": {
		break;
	}
	case "sub": {
		break;
	}
	case "mul": {
		break;
	}
	case "div": {
		break;
	}
	case "beq": {
		break;
	}
	case "bnq": {
		break;
	}
	case "end": {
		break;
	}
	case "label": {
		break;
	}
	default:{
		cout << "Your inpurt operator is illegal, please check again!" << endl;
		return false;
	}
	}
	return true;
}


MipsSimulator::MipsSimulator(string regs) {
	parseRegisters(regs);
	// TODO Auto-generated constructor stub
}

MipsSimulator::~MipsSimulator() {
	// TODO Auto-generated destructor stub
}

