/*
 * pipeline.h
 *
 *  Created on: Dec 1, 2017
 *      Author: Mina
 */

#ifndef PIPELINE_H_
#define PIPELINE_H_


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

struct InstructionFetchRegister{
	int Rdest, Rsrc1, Rsrc2;
	int immediate;
	int PC;
	bool isBranch;

	InstructionFetchRegister& operator=(InstructionFetchRegister &right){
		Rdest = right.Rdest;
		Rsrc1 = right.Rsrc1;
		Rsrc2 = right.Rsrc2;
		immediate = right.immediate;
		PC = right.PC;
		isBranch = right.isBranch;
		return *this;
	}
};



struct ExecuteRegister{
	int Rdest, Rsrc1, Rsrc2;
	int immediate;
	int  RdestVal, Rsrc1Val, Rsrc2Val;
	int PC;
	bool isBranch;

	ExecuteRegister& operator=(ExecuteRegister &right){
		Rdest = right.Rdest;
		Rsrc1 = right.Rsrc1;
		Rsrc2 = right.Rsrc2;
		immediate = right.immediate;
		RdestVal = right.RdestVal;
		Rsrc1Val = right.Rsrc1Val;
		Rsrc2Val = right.Rsrc2Val;
		PC = right.PC;
		isBranch = right.isBranch;
		return *this;
	}
};

class pipeline;

class InstructionFetchStage {
public:
	pipeline *pl;
	InstructionFetchRegister ifReg;

	void Excute(){
		this->ifReg.PC++;
		if(this->ifReg.PC >= this->pl->instructions.size()) return;
		string instruction = this->pl->instructions[this->ifReg.PC];
		this->ifReg.Rdest = this->pl->getOutputRegister(instruction);
		this->ifReg.Rsrc1 = this->pl->getFirstRegister(instruction);
		this->ifReg.Rsrc2 = this->pl->getSecondRegister(instruction);
		this->ifReg.immediate = 0;
		string opt = this->pl->getOperator(instruction);
		if(opt == 'b' || opt == "beq" || opt == "bnq")
			this->ifReg.isBranch = true;
		else
			this->ifReg.isBranch = false;
	}

	InstructionFetchStage(pipeline * p){
		this->pl = p;
		this->ifReg = {0, 0, 0, 0, -1, false};
	}
	virtual ~InstructionFetchStage();
};

class ExecuteStage {
public:
	pipeline *pl;
	InstructionFetchRegister ifReg;
	ExecuteRegister eReg;

	void Excute(){
		if(this->ifReg.PC == -1) return;
		this->eReg.PC++;
		if(this->eReg.PC >= this->pl->instructions.size()) return;
		string instruction = this->pl->instructions[this->ifReg.PC];
		string opt = this->pl->getOperator(instruction);
		this->eReg.Rdest = this->ifReg.Rdest;
		this->eReg.Rsrc1 = this->ifReg.Rsrc1;
		this->eReg.Rsrc2 = this->ifReg.Rsrc2;
		this->eReg.Rsrc1Val = this->pl->registers[this->ifReg.Rsrc1];
		this->eReg.Rsrc2Val = this->pl->registers[this->ifReg.Rsrc2];
		this->eReg.immediate = this->pl->getImmediate(instruction);
		this->eReg.RdestVal = this->pl->executeInstruction(opt);
		this->eReg.isBranch = this->ifReg.isBranch;
	}
	void Shift(){
		this->ifReg = this->pl->ifs.ifReg;
	}

	ExecuteStage(pipeline * p){
		this->pl = p;
		this->ifReg = {0, 0, 0, 0, -1, false};
		this->eReg = {0, 0, 0, 0, 0, 0, 0, -1, false};
	}
	virtual ~ExecuteStage();
};

class WriteBackStage {
public:
	pipeline *pl;
	ExecuteRegister eReg;

	void Excute(){
		if(this->eReg.PC == -1) return;
		if(this->eReg.PC >= this->pl->instructions.size()) return;
		this->pl->registers[this->eReg.Rdest] = this->eReg.RdestVal;
	}
	void Shift(){
		this->eReg = this->pl->es.eReg;
	}

	WriteBackStage(pipeline * p){
		this->pl = p;
		this->eReg = {0, 0, 0, 0, 0, 0, 0, -1, false};
	}
	virtual ~WriteBackStage();
};

class pipeline {
public:
	vector<int> registers;
	int pc;
	vector<string> instructions;
	unordered_map<string, int> labelsPosition;

	InstructionFetchStage  ifs;
	ExecuteStage es;
	WriteBackStage wbs;


	void parseRegisters(string regs){
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


	void setLabelsIndex(string instruction, int cnt){
		string labelName = instruction.substr(0, 5);
		if(labelName != "label")
			return;
		string space = " ";
		size_t end = instruction.find(space);
		labelName = instruction.substr(0, end);
		cout << "labelName: " << labelName << endl;
		this->labelsPosition[labelName] = cnt;
	}


	string getOperator(string instruction){
		string delim = " ";
		size_t start = 0U;
		size_t end = instruction.find(delim);
		string res = instruction.substr(start, end - start);
		if(res.size() > 5){
			res = res.substr(0, 5);
			if(this->labelsPosition.find(res) == this->labelsPosition.end()){
				this->labelsPosition[res] = this->pc;
			}
		}
		return res;
	}


	operator_code hasHit (string optr) {
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


	bool analyzeInstructions(){
		cout << "program contains " << this->instructions.size() << "instructions" << endl;
		while(this->pc < this->instructions.size()){
			this->ifs.Excute();
			this->es.Excute();
			this->wbs.Excute();
			this->es.Shift();
			this->wbs.Shift();
			this->executeInstruction(this->instructions[this->pc]);
		}
		return true;
	}


	int getOutputRegister(string instruction){
		string dollar = "$";
		size_t start = instruction.find(dollar);
		string reg = instruction.substr(++start, 1);
		int res = stoi(reg);
		return res;
	}


	int getFirstRegister(string instruction){
		string dollar = "$";
		size_t start = instruction.find(dollar);
		start = instruction.find(dollar, ++start);
		string reg = instruction.substr(++start, 1);
		int res = stoi(reg);
		return res;
	}


	int getSecondRegister(string instruction){
		string dollar = "$";
		size_t start = instruction.find(dollar);
		start = instruction.find(dollar, ++start);
		start = instruction.find(dollar, ++start);
		string reg = instruction.substr(++start, 1);
		int res = stoi(reg);
		return res;
		return 0;
	}


	int getImmediate(string instruction){
		string sharp = "#";
		size_t start = instruction.find(sharp);
		string reg = instruction.substr(++start, 1);
		int res = stoi(reg);
		return res;
	}
	bool setOperand(string instruction, string opt, int &operand1, int &operand2){
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


	string removeLabelInstruction(string instruction){
		size_t start = instruction.find(" ");
		return instruction.substr(start+1);
	}


	string getLabel(string instruction){
		size_t start = instruction.find("label", 0);
		string res = instruction.substr(start, instruction.size()-start);
		cout << "branch label is " << res << endl;
		return res;
	}


	bool executeInstruction(string instruction){
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
			this->pc = this->labelsPosition[labelName];
			return true;
		}
		case operator_code::beq: {
			if(operand1 == operand2){
				string labelName = this->getLabel(instruction);
				this->pc = this->labelsPosition[labelName];
				cout << "current index is " << this->pc << endl;
				return true;
			}
			cout << "current index is " << this->pc << endl;
			break;
		}
		case operator_code::bnq: {
			if (operand1 != operand2) {
				string labelName = this->getLabel(instruction);
				this->pc = this->labelsPosition[labelName];
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
			this->pc++;
			return false;
		}
		}
		this->pc++;

		cout << endl;
		for(int i=0;i<8;i++)
			cout << this->registers[i] << " ";
		cout << endl;

		return true;
	}


	pipeline(string regs) : ifs(this), es(this), wbs(this) {
		this->pc = 0;
		this->registers.resize(8,0);
		this->parseRegisters(regs);
	}


	virtual ~pipeline();
};


#endif /* PIPELINE_H_ */
