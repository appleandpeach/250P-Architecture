/*
 * MipsSimulator.cpp
 *
 *  Created on: Nov 28, 2017
 *      Author: Mina
 */

#include "MipsSimulator.h"

void MipsSimulator::parseRegisters(string regs){
	string delim = ",";
	size_t start = 0U;
	size_t end = regs.find(delim);
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
	size_t start = 0U;
	size_t end = instruction.find(delim);
	string res = instruction.substr(start, end - start);
	if(res.size() > 5){
		res = res.substr(0, 5);
		if(this->labelsPosition.find(res) == this->labelsPosition.end()){
			this->labelsPosition[res] = this->index;
		}
	}
	return res;
}

operator_code MipsSimulator::hasHit (string optr) {
    if (optr == "add") return operator_code::add;
    if (optr == "addi") return operator_code::addi;
    if (optr == "sub") return operator_code::sub;
    if (optr == "mul") return operator_code::mul;
    if (optr == "div") return operator_code::div;
    if (optr == "b") return operator_code::b;
    if (optr == "beq") return operator_code::beq;
    if (optr == "bnq") return operator_code::bnq;
    if (optr == "end") return operator_code::end;
    if (optr == "label") return operator_code::label;
    return operator_code::err;
}


bool MipsSimulator::analyzeInstructions(){
	cout << this->instructions.size() << endl;
	while(this->index < this->instructions.size()){
		this->executeInstruction(this->instructions[this->index]);
	}
	return true;
}

int MipsSimulator::getOutputRegister(string instruction){
	string dollar = "$";
	size_t start = instruction.find(dollar);
	string reg = instruction.substr(++start, 1);
	int res = stoi(reg);
	return res;
}


bool MipsSimulator::setOperand(string instruction, string opt, int &operand1, int &operand2){
	// if opt is b, there's no need to set operands
	if(opt == "b" || opt == "end" || opt == "label")
		return false;

	// different start for setting operands
	size_t start;
	if(opt == "beq" || opt == "bnq")
		start = instruction.find(" ");
	else
		start = instruction.find(",");

	// set operand 1
	start++;
	if(instruction.at(start) == '$'){
		start++;
		string numStr = instruction.substr(start, 1);
		cout << "register num = " << numStr << endl;
		operand1 = this->registers[stoi(numStr)];
		start++;
	}else{
		size_t end = instruction.find(",", start);
		string numStr = instruction.substr(start, end);
		operand1 = stoi(numStr);
		start = end;
	}

	// set operand 2
	start++;
	if(instruction.at(start) == '$'){
		start++;
		string numStr = instruction.substr(start, 1);
		cout << "register num = " << numStr << endl;
		operand2 = this->registers[stoi(numStr)];
		start++;
	} else {
		start++;
		size_t end = instruction.find("\0", start);
		string numStr = instruction.substr(start, end);
		cout << numStr << endl;
		operand2 = stoi(numStr);
		start += numStr.size();
	}

	return true;
}


string MipsSimulator::removeLabelInstruction(string instruction){
	size_t start = instruction.find(" ");
	return instruction.substr(start+1);
}

void MipsSimulator::setLabelsIndex(string instruction, int cnt){
	string labelName = instruction.substr(0, 5);
	if(labelName != "label")
		return;
	string space = " ";
	size_t end = instruction.find(space);
	labelName = instruction.substr(0, end);
	cout << "labelName: " << labelName << endl;
	this->labelsPosition[labelName] = cnt;
}

string MipsSimulator::getLabel(string instruction){
	size_t start = instruction.find("label", 0);
	string res = instruction.substr(start, instruction.size()-start);
	cout << "branch label is " << res << endl;
	return res;
}

bool MipsSimulator::executeInstruction(string instruction){
	cout << endl << instruction << endl;
	// get operator
	string opt = this->getOperator(instruction);

	// set operand
	int operand1 = 0, operand2 = 0;
	this->setOperand(instruction, opt, operand1, operand2);
	cout << "operand1 = " << operand1 << endl;
	cout << "operand2 = " << operand2 << endl;

	// calculate the result
	switch(this->hasHit(opt)){
	case operator_code::add:{
		// get output register
		int outputReg = this->getOutputRegister(instruction);
		// execute
		this->registers[outputReg] = operand1 + operand2;
		break;
	}
	case operator_code::addi: {
		int outputReg = this->getOutputRegister(instruction);
		this->registers[outputReg] = operand1 + operand2;
		break;
	}
	case operator_code::sub: {
		int outputReg = this->getOutputRegister(instruction);
		this->registers[outputReg] = operand1 - operand2;
		break;
	}
	case operator_code::mul: {
		int outputReg = this->getOutputRegister(instruction);
		this->registers[outputReg] = operand1 * operand2;
		break;
	}
	case operator_code::div: {
		int outputReg = this->getOutputRegister(instruction);
		if(operand2 == 0){
			cout << "Numerator should not be 0!" << endl;
			return false;
		}
		this->registers[outputReg] = operand1 / operand2;
		break;
	}
	case operator_code::b: {
		string labelName = this->getLabel(instruction);
		this->index = this->labelsPosition[labelName];
		return true;
	}
	case operator_code::beq: {
		if(operand1 == operand2){
			string labelName = this->getLabel(instruction);
			this->index = this->labelsPosition[labelName];
			cout << "current index is " << this->index << endl;
			return true;
		}
		cout << "current index is " << this->index << endl;
		break;
	}
	case operator_code::bnq: {
		if (operand1 != operand2) {
			string labelName = this->getLabel(instruction);
			this->index = this->labelsPosition[labelName];
			return true;
		}
		break;
	}
	case operator_code::end: {
		break;
	}
	case operator_code::label: {
		instruction = this->removeLabelInstruction(instruction);
		this->executeInstruction(instruction);
		return true;
	}
	default:{
		cout << "Your inpurt operator is illegal, please check again!" << endl;
		this->index++;
		return false;
	}
	}
	this->index++;

	cout << endl;
	for(int i=0;i<8;i++)
		cout << this->registers[i] << " ";
	cout << endl;

	return true;
}


MipsSimulator::MipsSimulator(string regs) {
	this->index = 0;
	parseRegisters(regs);
	// TODO Auto-generated constructor stub
}

MipsSimulator::~MipsSimulator() {
	// TODO Auto-generated destructor stub
}

