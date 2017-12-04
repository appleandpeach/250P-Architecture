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
	int Rdest;
	int Rsrc1;
	int Rsrc2;
	int immediate;
	int PC;
	bool isBranch;
	bool isStall;

	InstructionFetchRegister& operator=(const InstructionFetchRegister &right){
		Rdest = right.Rdest;
		Rsrc1 = right.Rsrc1;
		Rsrc2 = right.Rsrc2;
		immediate = right.immediate;
		PC = right.PC;
		isBranch = right.isBranch;
		isStall = right.isStall;
		return *this;
	}
};


struct ExecuteRegister{
	int Rdest, Rsrc1, Rsrc2;
	int immediate;
	int  RdestVal, Rsrc1Val, Rsrc2Val;
	int PC;
	bool isBranch;
	bool isStall;

	ExecuteRegister& operator=(const ExecuteRegister &right){
		Rdest = right.Rdest;
		Rsrc1 = right.Rsrc1;
		Rsrc2 = right.Rsrc2;
		immediate = right.immediate;
		RdestVal = right.RdestVal;
		Rsrc1Val = right.Rsrc1Val;
		Rsrc2Val = right.Rsrc2Val;
		PC = right.PC;
		isBranch = right.isBranch;
		isStall = right.isStall;
		return *this;
	}
};


class pipeline;


class InstructionFetchStage {
public:
	pipeline *pl;
	InstructionFetchRegister ifReg;

	void Excute();
	InstructionFetchStage(pipeline * p){
		this->pl = p;
		this->ifReg.PC = -1;
		this->ifReg.Rdest = 0;
		this->ifReg.Rsrc1 = 0;
		this->ifReg.Rsrc2 = 0;
		this->ifReg.immediate = 0;
		this->ifReg.isBranch = false;
		this->ifReg.isStall = false;

	}
	virtual ~InstructionFetchStage(){
		this->pl = NULL;
	}
};


class ExecuteStage {
public:
	pipeline *pl;
	InstructionFetchRegister ifReg;
	ExecuteRegister eReg;

	void Excute();
	void Shift();


	ExecuteStage(pipeline * p){
		this->pl = p;

		this->ifReg.PC = -1;
		this->ifReg.Rdest = 0;
		this->ifReg.Rsrc1 = 0;
		this->ifReg.Rsrc2 = 0;
		this->ifReg.immediate = 0;
		this->ifReg.isBranch = false;
		this->ifReg.isStall = false;

		this->eReg.PC = -1;
		this->eReg.Rdest = 0;
		this->eReg.RdestVal = 0;
		this->eReg.Rsrc1 = 0;
		this->eReg.Rsrc1Val = 0;
		this->eReg.Rsrc2 = 0;
		this->eReg.Rsrc2Val = 0;
		this->eReg.immediate = 0;
		this->eReg.isBranch = false;
		this->eReg.isStall = false;

	}
	virtual ~ExecuteStage(){
		this->pl = NULL;
	}
};


class WriteBackStage {
public:
	pipeline *pl;
	ExecuteRegister eReg;

	void Excute();
	void Shift();

	WriteBackStage(pipeline * p){
		this->pl = p;

		this->eReg.PC = -1;
		this->eReg.Rdest = 0;
		this->eReg.RdestVal = 0;
		this->eReg.Rsrc1 = 0;
		this->eReg.Rsrc1Val = 0;
		this->eReg.Rsrc2 = 0;
		this->eReg.Rsrc2Val = 0;
		this->eReg.immediate = 0;
		this->eReg.isBranch = false;
		this->eReg.isStall = false;
	}
	virtual ~WriteBackStage(){
		this->pl = NULL;
	}
};


class pipeline {
public:
	vector<int> registers;
	int cycle;
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
		cout << "Program contains " << this->instructions.size() << " instructions." << endl;
		while(this->wbs.eReg.PC < (int)this->instructions.size()){
			this->ifs.Excute();
			this->es.Excute();
			this->wbs.Excute();
			this->es.Shift();
			this->wbs.Shift();
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
		if(start == string::npos)
			return -1;
		string reg = instruction.substr(++start, 1);
		int res = stoi(reg);
		return res;
	}


	int getSecondRegister(string instruction){
		string opt = this->getOperator(instruction);
		if(opt == "addi")
			return 0;
		string dollar = "$";
		size_t start = instruction.find(dollar);
		start = instruction.find(dollar, ++start);
		start = instruction.find(dollar, ++start);
		string reg = instruction.substr(++start, 1);
		int res = stoi(reg);
		return res;
	}


	int getImmediate(string instruction){
		string sharp = "#";
		size_t start = instruction.find(sharp);
		if(start == string::npos)
			return 0;
		size_t label = instruction.find("label");
		string reg = "";
		if(label == string::npos)
			reg = instruction.substr(++start, instruction.size() - start);
		else
			reg = instruction.substr(++start, label - start + 1);
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


	int executeInstruction(int operand1, int operand2, int immediate, string &opt, string &instruction){
		switch(this->hasHit(opt)){
		case operator_code::add:{
			return operand1 + operand2;
		}
		case operator_code::addi: {
			return operand1 + immediate;
		}
		case operator_code::sub: {
			return operand1 - operand2;
		}
		case operator_code::mul: {
			return operand1 * operand2;
		}
		case operator_code::div: {
			if(operand2 == 0){
				cout << "Numerator should not be 0!" << endl;
				return 0;
			}
			return operand1 / operand2;
		}
		case operator_code::b: {
			string labelName = this->getLabel(instruction);
			this->ifs.ifReg.PC = this->labelsPosition[labelName] - 1;
			return true;
		}
		case operator_code::beq: {
			if (instruction.find("#") != string::npos) {
				if (operand1 == immediate) {
					string labelName = this->getLabel(instruction);
					// jump to the instruction before the target one, coz IF stage will increase 1
					this->ifs.ifReg.PC = this->labelsPosition[labelName] - 1;
					cout << "updated index is " << this->ifs.ifReg.PC << endl;
					return true;
				}
			} else {
				if (operand1 == operand2) {
					string labelName = this->getLabel(instruction);
					this->ifs.ifReg.PC = this->labelsPosition[labelName] - 1;
					cout << "updated index is " << this->ifs.ifReg.PC << endl;
					return true;
				}
			}
			cout << "current index is " << this->ifs.ifReg.PC << endl;
			return false;
		}
		case operator_code::bnq: {
			if (operand1 != operand2) {
				string labelName = this->getLabel(instruction);
				this->ifs.ifReg.PC = this->labelsPosition[labelName];
				return 1;
			}
			return -1;
		}
		case operator_code::end: {
			return 0;
		}
		case operator_code::label: {
			instruction = this->removeLabelInstruction(instruction);
			opt = this->getOperator(instruction);
			return this->executeInstruction(operand1, operand2, immediate, opt, instruction);
		}
		default:{
			cout << "Your inpurt operator is illegal, please check again!" << endl;
			return 0;
		}
		}
	}


	pipeline(string regs) : ifs(this), es(this), wbs(this) {
		this->cycle = 0;
		this->pc = 0;
		this->registers.resize(8,0);
		this->parseRegisters(regs);
	}


	virtual ~pipeline(){}
};


void InstructionFetchStage::Excute() {
	cout << "***************************************************************" << endl;
	cout << endl << "IF stage:" << endl;

	// if last instrucion is branch, stall a cycle
	cout << "isBranch: " << this->ifReg.isBranch << endl;
	if (this->ifReg.isBranch == true) {
		this->ifReg.isBranch = false;
		this->ifReg.isStall = true;
		this->pl->cycle++;
		return;
	} else {
		this->ifReg.isStall = false;
	}

	this->ifReg.PC++;
	// if instruction is end, only increase cycle number
	string instruction = this->pl->instructions[this->ifReg.PC];
	cout << instruction << endl;
	if (instruction == "end")
		return;
	if (this->ifReg.PC >= this->pl->instructions.size()) {
		this->pl->cycle++;
		return;
	}

	// if instruction is branch, set all -1.
	// if instruction is comparion branch, set dest -1 and others relative values.
	string opt = this->pl->getOperator(instruction);
	if (opt == "b" || opt == "beq" || opt == "bnq") {
		if (opt == "b") {
			this->ifReg.Rdest = -1;
			this->ifReg.Rsrc1 = -1;
			this->ifReg.Rsrc2 = -1;
			this->ifReg.immediate = 0;
		} else {
			this->ifReg.Rdest = -1;
			this->ifReg.Rsrc1 = this->pl->getOutputRegister(instruction);
			this->ifReg.Rsrc2 = this->pl->getFirstRegister(instruction);
			this->ifReg.immediate = this->pl->getImmediate(instruction);
		}
		this->ifReg.isBranch = true;
	} else {
		this->ifReg.Rdest = this->pl->getOutputRegister(instruction);
		this->ifReg.Rsrc1 = this->pl->getFirstRegister(instruction);
		this->ifReg.Rsrc2 = this->pl->getSecondRegister(instruction);
		this->ifReg.immediate = this->pl->getImmediate(instruction);
		this->ifReg.isBranch = false;
	}

	cout << "dest: " << this->ifReg.Rdest << ", src1: " << this->ifReg.Rsrc1
			<< ", src2: " << this->ifReg.Rsrc2 << ", immediate: "
			<< this->ifReg.immediate << endl;

	this->pl->cycle++;
	cout << "current cycle is " << this->pl->cycle << endl;
}


void ExecuteStage::Excute() {
	cout << endl << "EX stage:" << endl;
	cout << "this->ifReg.PC " << this->ifReg.PC << endl;

	cout << "isbranch: " << this->eReg.isBranch << endl;
	if (this->ifReg.PC == -1)
		return;
	if(this->ifReg.isStall == true){
		this->eReg.isStall = this->ifReg.isStall;
		return;
	}

	this->eReg.PC++;
	if (this->eReg.PC >= this->pl->instructions.size())
		return;
	string instruction = this->pl->instructions[this->ifReg.PC];
	string opt = this->pl->getOperator(instruction);
	cout  << "instruction is \"" << instruction << "\"" << endl;

	// data forwarding
	if(this->ifReg.Rsrc1 == this->eReg.Rdest)
		this->eReg.Rsrc1Val = this->eReg.RdestVal;
	else
		this->eReg.Rsrc1Val = this->pl->registers[this->ifReg.Rsrc1];
	if(this->ifReg.Rsrc2 == this->eReg.Rdest)
		this->eReg.Rsrc2Val = this->eReg.RdestVal;
	else
		this->eReg.Rsrc2Val = this->pl->registers[this->ifReg.Rsrc2];

	// calculate the result
	this->eReg.RdestVal = this->pl->executeInstruction(this->eReg.Rsrc1Val, this->eReg.Rsrc2Val, this->ifReg.immediate, opt, instruction);
	cout << "eReg.Rsrc1Val: " << eReg.Rsrc1Val<< " this->eReg.Rsrc2Val: " << this->eReg.Rsrc2Val << " this->eReg.RdestVal: "<< this->eReg.RdestVal << endl;
	this->eReg.isBranch = this->ifReg.isBranch;
	this->eReg.isStall = this->ifReg.isStall;
	this->eReg.PC = this->ifReg.PC;
	this->eReg.Rdest = this->ifReg.Rdest;
	this->eReg.Rsrc1 = this->ifReg.Rsrc1;
	this->eReg.Rsrc2 = this->ifReg.Rsrc2;
	cout << "e.dest: " << eReg.Rdest<< " src1: " << this->eReg.Rsrc1 << " src2: "<< this->eReg.Rsrc2 << endl;
	cout << "this->eReg.isStall: " << this->eReg.isStall <<endl;

	// print cycle for branch
	if(opt == "beq" || opt == "bnq" ||opt == "b")
		cout << instruction << "\t<"<< pl->cycle << ">" << endl;
}


void ExecuteStage::Shift() {
	this->ifReg = this->pl->ifs.ifReg;
}


void WriteBackStage::Excute() {
	cout << endl << "WB stage:" << endl;
	cout << "this->eReg.PC: " << this->eReg.PC << endl;

	cout << "wb isStall? " << this->eReg.isStall <<endl;
	if (this->eReg.PC == -1 || this->eReg.isStall == true){
		return;
	}

	if (this->eReg.PC >= this->pl->instructions.size())
		return;
	this->pl->registers[this->eReg.Rdest] = this->eReg.RdestVal;
	cout << "this->eReg.RdestVal "<<this->eReg.RdestVal << endl;
	cout << "e.dest: " << eReg.Rdest<< " src1: " << this->eReg.Rsrc1 << " src2: "<< this->eReg.Rsrc2 << endl;
	string instruction = this->pl->instructions[this->eReg.PC];
	string opt = this->pl->getOperator(instruction);
	if(opt != "b" || opt != "beq" || opt != "bnq")
		cout  << "instruction is \"" << instruction << "\"" << endl;

	// print cycle number except branch
	cout << instruction << "\t<"<< pl->cycle << ">" << endl;

	for (int i = 0; i < 8; i++)
		cout << this->pl->registers[i] << " ";
	cout << endl;
}

void WriteBackStage::Shift() {
	this->eReg = this->pl->es.eReg;
}



#endif /* PIPELINE_H_ */
