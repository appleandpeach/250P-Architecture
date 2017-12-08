#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <stdexcept>

#include <stdio.h>
#include <algorithm>

using namespace std;

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


class solution {

private:

	bool DEBUG;
	int clck;
	int index;
	int cycle;
	vector<string> vect_lines;
	vector<int>* t_vars;
	unordered_map<string, int> labelsMap;

public:

	solution(ifstream &file_in, int clck_in = 10, bool DEBUG_in = false){
		this->t_vars = new vector<int>(8,0);
		this->mips_clock();
		this->clck = clck_in;
		this->DEBUG = DEBUG_in;
		this->index = 0;
		this->cycle = 0;

		string regs = "";
		// get registers' values
		this->getNextInstruction(file_in, regs);

		// initiate mips simulator
		this->parseRegisters(regs);
		string instruction = "";
		int cnt = 0;
		while (this->getNextInstruction(file_in, instruction)) {
			this->vect_lines.push_back(instruction);
			this->setLabelsIndex(instruction, cnt);
			cnt++;
		}

		// print original registers
		this->dbg("");
	}

	void dbg(const string &msg);

	vector<int>* alu(){

		this->analyzeInstructions();
		return this->t_vars;
	}


	int mips_clock();

	// helper
	void parseRegisters(string regs);
	void setLabelsIndex(string instruction, int cnt);
	bool setOperand(string instruction, string opt, int &operand1, int &operand2);
	int getOutputRegister(string instruction);
	string getLabel(string instruction);
	string getOperator(string instruction);
	string removeLabelInstruction(string instruction);
	operator_code hasHit(string optr);
	bool analyzeInstructions();
	bool executeInstruction(string instruction);



	static bool IsSpace(int ch) {
		return !std::isspace(ch);
	}

	static inline void rtrim(string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), IsSpace).base(), s.end());
	}

	bool getNextInstruction(ifstream &file_in, string &instruction) {
		instruction = "";
		if (!file_in.eof()) {
			getline(file_in, instruction);
			rtrim(instruction);
			if (instruction == "")
				return false;
			return true;
		}
		return false;
	}

};

void solution::dbg(const string &msg) {
	if(msg != "")
		cout << msg << endl;
	if (msg != "end") {
		int i = 0;
		for (; i < 7; i++)
			cout << this->t_vars->at(i) << ",";
		cout << this->t_vars->at(i) << endl;
	}
}


int solution::mips_clock() {
	chrono::milliseconds timespan(clck);

	this_thread::sleep_for(timespan);
	static int cycle = 0;
	if (cycle == 0)
		cycle = 1;
	else
		cycle = 0;
	return cycle;
}

void solution::parseRegisters(string regs) {
	string delim = ",";
	size_t start = 0U;
	size_t end = regs.find(delim);
	int cnt = 0;
	while (end != std::string::npos) {
		this->t_vars->at(cnt) = stoi(regs.substr(start, end - start));
		cnt++;
		start = end + delim.length();
		end = regs.find(delim, start);
	}
	this->t_vars->at(cnt) = stoi(regs.substr(start, end - start));
}


bool solution::executeInstruction(string instruction) {
		// get operator
		string opt = this->getOperator(instruction);

		// set operand
		int operand1 = 0, operand2 = 0;
		this->setOperand(instruction, opt, operand1, operand2);

		// calculate the result
		switch (this->hasHit(opt)) {
		case operator_code::add: {
			int outputReg = this->getOutputRegister(instruction);
			this->t_vars->at(outputReg) = operand1 + operand2;
			break;
		}
		case operator_code::addi: {
			int outputReg = this->getOutputRegister(instruction);
			this->t_vars->at(outputReg) = operand1 + operand2;
			break;
		}
		case operator_code::sub: {
			int outputReg = this->getOutputRegister(instruction);
			this->t_vars->at(outputReg) = operand1 - operand2;
			break;
		}
		case operator_code::mul: {
			int outputReg = this->getOutputRegister(instruction);
			this->t_vars->at(outputReg) = operand1 * operand2;
			break;
		}
		case operator_code::div: {
			int outputReg = this->getOutputRegister(instruction);
			if (operand2 == 0) {
				cout << "Numerator should not be 0!" << endl;
				return false;
			}
			this->t_vars->at(outputReg) = operand1 / operand2;
			break;
		}
		case operator_code::b: {
			string labelName = this->getLabel(instruction);
			this->index = this->labelsMap[labelName];
			if(this->DEBUG) this->dbg(instruction);
			return true;
		}
		case operator_code::beq: {
			if (operand1 == operand2) {
				string labelName = this->getLabel(instruction);
				this->index = this->labelsMap[labelName];
				if(this->DEBUG) this->dbg(instruction);
				return true;
			}
			break;
		}
		case operator_code::bnq: {
			if (operand1 != operand2) {
				string labelName = this->getLabel(instruction);
				this->index = this->labelsMap[labelName];
				if(this->DEBUG) this->dbg(instruction);
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
		default: {
			cout << "Your inpurt operator is illegal, please check again!" << endl;
			this->index++;
			return false;
		}
		}
		this->index++;

		if(this->DEBUG) this->dbg(instruction);
		return true;
	}



bool solution::analyzeInstructions() {
	while (this->index < this->vect_lines.size()) {
		if (!this->mips_clock()) {
			continue;
		} else {
			string instruction = this->vect_lines[this->index];

			if (this->DEBUG){
				cout << this->cycle << endl;
			}

			this->cycle++;
			this->executeInstruction(instruction);
		}
	}
	return true;

}

	// helper
	operator_code solution::hasHit(string optr) {
		if (optr == "add")
			return operator_code::add;
		if (optr == "addi")
			return operator_code::addi;
		if (optr == "sub")
			return operator_code::sub;
		if (optr == "mul")
			return operator_code::mul;
		if (optr == "div")
			return operator_code::div;
		if (optr == "b")
			return operator_code::b;
		if (optr == "beq")
			return operator_code::beq;
		if (optr == "bnq")
			return operator_code::bnq;
		if (optr == "end")
			return operator_code::end;
		if (optr == "label")
			return operator_code::label;
		return operator_code::err;
	}


	string solution::removeLabelInstruction(string instruction) {
		size_t start = instruction.find(" ");
		return instruction.substr(start + 1);
	}


	// get and set functions
	string solution::getOperator(string instruction) {
		string delim = " ";
		size_t start = 0U;
		size_t end = instruction.find(delim);
		string res = instruction.substr(start, end - start);
		if (res.size() > 5) {
			res = res.substr(0, 5);
			if (this->labelsMap.find(res) == this->labelsMap.end()) {
				this->labelsMap[res] = this->index;
			}
		}
		return res;
	}


	string solution::getLabel(string instruction) {
		size_t start = instruction.find("label", 0);
		string res = instruction.substr(start, instruction.size() - start);
		return res;
	}


	int solution::getOutputRegister(string instruction) {
		string dollar = "$";
		size_t start = instruction.find(dollar);
		string reg = instruction.substr(++start, 1);
		int res = 0;
		try {
			res = stoi(reg);
		} catch (const std::invalid_argument& ia) {
			cerr << instruction << endl;
			std::cerr << "Dest register has an invalid argument: " << ia.what() << '\n';
		}
		return res;
	}


	bool solution::setOperand(string instruction, string opt, int &operand1, int &operand2) {
		// if opt is b, there's no need to set operands
		if (opt == "b" || opt == "end" || opt == "label")
			return false;

		// different start for setting operands
		size_t start;
		if (opt == "beq" || opt == "bnq")
			start = instruction.find(" ");
		else
			start = instruction.find(",");

		// set operand 1
		start++;
		if (instruction.at(start) == '$') {
			start++;
			string numStr = instruction.substr(start, 1);
//			cout << "register num = " << numStr << endl;
			try {
				operand1 = this->t_vars->at(stoi(numStr));
			} catch (const std::invalid_argument& ia) {
				cerr << instruction << endl;
				std::cerr << "1. First reg has an invalid argument: " << ia.what() << '\n';
			}
			start++;
		} else {
			size_t end = instruction.find(",", start);
			string numStr = instruction.substr(start, end);
			try {
				operand1 = stoi(numStr);
			} catch (const std::invalid_argument& ia) {
				cerr << instruction << endl;
				std::cerr << "2. First reg has an invalid argument: " << ia.what() << '\n';
			}
			start = end;
		}

		// set operand 2
		start++;
		if (instruction.at(start) == '$') {
			start++;
			string numStr = instruction.substr(start, 1);
//			cout << "register num = " << numStr << endl;
			try {
				operand2 = this->t_vars->at(stoi(numStr));
			} catch (const std::invalid_argument& ia) {
				cerr << instruction << endl;
				std::cerr << "1. Second reg has an invalid argument: " << ia.what() << '\n';
			}

			start++;
		} else {
			size_t end = instruction.find("\0", start);
			string numStr = instruction.substr(start, end);
//			cout << numStr << endl;
			try {
				operand2 = stoi(numStr);
			} catch (const std::invalid_argument& ia) {
				cerr << instruction << endl;
				std::cerr << "2. Second reg has an invalid argument: " << ia.what() << '\n';
			}
			start += numStr.size();
		}

		return true;
	}


	void solution::setLabelsIndex(string instruction, int cnt) {
		string labelName = instruction.substr(0, 5);
		if (labelName != "label")
			return;
		string space = " ";
		size_t end = instruction.find(space);
		labelName = instruction.substr(0, end);
//		cout << "labelName: " << labelName << endl;
		this->labelsMap[labelName] = cnt;
	}
