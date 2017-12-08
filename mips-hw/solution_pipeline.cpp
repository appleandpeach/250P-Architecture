#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>

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
	bool debug;
	int clck;
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
//			cout << this->registers[cnt] << endl;
			cnt ++;
			start = end + delim.length();
			end = regs.find(delim, start);
		}
		this->registers[cnt] = stoi(regs.substr(start, end - start));
//		cout << this->registers[cnt] << endl;
	}


	void setLabelsIndex(string instruction, int cnt){
		string labelName = instruction.substr(0, 5);
		if(labelName != "label")
			return;
		string space = " ";
		size_t end = instruction.find(space);
		labelName = instruction.substr(0, end);
//		cout << "labelName: " << labelName << endl;
		this->labelsPosition[labelName] = cnt;
	}


	string getOperator(string instruction){
		string delim = " ";
		size_t start = 0U;
		size_t end = instruction.find(delim);
		string res = instruction.substr(start, end - start);
		if(res.size() > 5){
			res = res.substr(0, 5);
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
		while(this->wbs.eReg.PC < (int)this->instructions.size()-1){
			this->ifs.Excute();
			this->es.Excute();
			this->wbs.Excute();
			this->es.Shift();
			this->wbs.Shift();
			if(this->debug){
				int i = 0;
				for(; i<7;i++){
					cout << this->registers[i] << ",";
				}
				cout << this->registers[i] << endl;
			}
		}
		return true;
	}


	int getOutputRegister(string instruction){
		string dollar = "$";
		size_t start = instruction.find(dollar);
		string reg = instruction.substr(++start, 1);
		int res = 0;
		try {
			res = stoi(reg);
		} catch (const std::invalid_argument& ia) {
			cerr << "cycle is " << this->cycle << endl;
			cerr << instruction << endl;
			std::cerr << "Dest register is invalid argument: " << ia.what() << '\n';
		}
		return res;
	}


	int getFirstRegister(string instruction){
		string dollar = "$";
		size_t start = instruction.find(dollar);
		start = instruction.find(dollar, ++start);
		if(start == string::npos)
			return -1;
		string reg = instruction.substr(++start, 1);
		int res = 0;
		try {
			res = stoi(reg);
		} catch (const std::invalid_argument& ia) {
			cerr << "cycle is " << this->cycle << endl;
			cerr << instruction << endl;
			std::cerr << "First register is invalid argument: " << ia.what() << '\n';
		}
		return res;
	}


	int getSecondRegister(string instruction){
		if(instruction.find("addi") != string::npos)
			return 0;
		string dollar = "$";
		size_t start = instruction.find(dollar);
		start = instruction.find(dollar, ++start);
		start = instruction.find(dollar, ++start);
		string reg = instruction.substr(++start, 1);
		int res = 0;
		try {
			res = stoi(reg);
		} catch (const std::invalid_argument& ia) {
			cerr << "cycle is " << this->cycle << endl;
			cerr << instruction << endl;
			std::cerr << "Second register is invalid argument: " << ia.what() << '\n';
		}
		return res;
	}


	int getImmediate(string instruction){
		string dollar = "$";
		size_t start = instruction.find(dollar);
		start += 3;
		string reg = "";
		if(instruction.at(start) == '$'){
			size_t third_dollar = instruction.find(dollar, ++start);
			// instruction does not have immediate
			if(third_dollar != string::npos)
				return 0;
			else{
				if(instruction.find("beq") != string::npos || instruction.find("bnq") !=string::npos)
					return 0;
				// addi
				start += 2;
				size_t end = instruction.size();
				reg = instruction.substr(start, end - start);
			}
		}else{
			// beq, bnq
			size_t label_pos = instruction.find("label");
			reg = instruction.substr(start, label_pos - start - 1);
		}

		int res = 0;
		try{
			res = stoi(reg);
		}catch (const std::invalid_argument& ia) {
			cerr << "cycle is " << this->cycle << endl;
			cerr << instruction << endl;
			std::cerr << "Immediate number is invalid argument: " << ia.what() << '\n';
		}
		return res;
	}


	string removeLabelInstruction(string instruction){
		size_t start = instruction.find(" ");
		return instruction.substr(start+1);
	}


	string getLabel(string instruction){
		size_t start = instruction.find("label", 0);
		string res = instruction.substr(start, instruction.size()-start);
		return res;
	}


	int executeInstruction(int operand1, int operand2, int immediate, int indicator, string &opt, string &instruction){
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
			if (indicator == -1) {
				if (operand1 == immediate) {
					string labelName = this->getLabel(instruction);
					// jump to the instruction before the target one, coz IF stage will increase 1
					this->ifs.ifReg.PC = this->labelsPosition[labelName] - 1;
					return true;
				}
			} else {
				if (operand1 == operand2) {
					string labelName = this->getLabel(instruction);
					this->ifs.ifReg.PC = this->labelsPosition[labelName] - 1;
					return true;
				}
			}
			return false;
		}
		case operator_code::bnq: {
			if (operand1 != operand2) {
				string labelName = this->getLabel(instruction);
				this->ifs.ifReg.PC = this->labelsPosition[labelName] - 1;
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
			return this->executeInstruction(operand1, operand2, immediate, indicator, opt, instruction);
		}
		default:{
			cout << "Your inpurt operator is illegal, please check again!" << endl;
			return 0;
		}
		}
	}


	pipeline() : ifs(this), es(this), wbs(this) {
		this->debug = false;
		this->clck = 0;
		this->cycle = 0;
		this->pc = 0;
		this->registers.resize(8,0);
	}


	virtual ~pipeline(){}
};


void InstructionFetchStage::Excute() {

	this->pl->cycle++;
	if(this->pl->debug) cout << this->pl->cycle << endl;

	// if instruction is end, only increase cycle number
	if (this->ifReg.PC >= (int)(this->pl->instructions.size() - 1)) {
		return;
	}

	// if last instrucion is branch, stall a cycle
	if (this->ifReg.isBranch == true) {
		this->ifReg.isBranch = false;
		this->ifReg.isStall = true;
		return;
	} else {
		this->ifReg.isStall = false;
	}

	this->ifReg.PC++;
	string instruction = this->pl->instructions[this->ifReg.PC];
	if(this->pl->debug) cout << "if: " << instruction << endl;
	if(instruction == "end"){
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

}


void ExecuteStage::Excute() {
	// at beginning EX stage is empty or end
	if(this->ifReg.PC == -1) return;

	if (this->ifReg.PC >= (int) (this->pl->instructions.size() - 1)) {
		this->eReg.PC = this->ifReg.PC;
		return;
	}

	// if last IF stage stalls, current EX stage should stall
	if(this->ifReg.isStall == true){
		this->eReg.isStall = this->ifReg.isStall;
		return;
	}

	// increase EX stage PC
	this->eReg.PC++;

	// get operator
	string instruction = this->pl->instructions[this->ifReg.PC];
	string opt = this->pl->getOperator(instruction);
	if(this->pl->debug)
		cout << "ex: " <<instruction << endl;

	// data forwarding:
	// if current operand register is the same as last EX stage destination register
	// read operand register value from eReg that keeps last time EX stage result
	if(this->ifReg.Rsrc1 == this->eReg.Rdest)
		this->eReg.Rsrc1Val = this->eReg.RdestVal;
	else
		this->eReg.Rsrc1Val = this->pl->registers[this->ifReg.Rsrc1];
	if(this->ifReg.Rsrc2 == this->eReg.Rdest)
		this->eReg.Rsrc2Val = this->eReg.RdestVal;
	else
		this->eReg.Rsrc2Val = this->pl->registers[this->ifReg.Rsrc2];

	// calculate the result and store result in eReg
	this->eReg.RdestVal = this->pl->executeInstruction(this->eReg.Rsrc1Val, this->eReg.Rsrc2Val, this->ifReg.immediate, this->ifReg.Rsrc2, opt, instruction);
	this->eReg.Rdest = this->ifReg.Rdest;
	this->eReg.Rsrc1 = this->ifReg.Rsrc1;
	this->eReg.Rsrc2 = this->ifReg.Rsrc2;
	this->eReg.isBranch = this->ifReg.isBranch;
	this->eReg.isStall = this->ifReg.isStall;
	this->eReg.PC = this->ifReg.PC;

	// print cycle for branch
	if(opt == "beq" || opt == "bnq" || opt == "b")
		cout << instruction << "\t<"<< pl->cycle << ">" << endl;
}


void ExecuteStage::Shift() {
	this->ifReg = this->pl->ifs.ifReg;
}


void WriteBackStage::Excute() {
	// if at the beginning of pipeline, last EX stage stalls, or exceeds instruction size, stall a cycle
	if (this->eReg.PC == -1 || this->eReg.isStall == true || this->eReg.PC >= (int)(this->pl->instructions.size()-1)){
		return;
	}

	// write value back to registers
	this->pl->registers[this->eReg.Rdest] = this->eReg.RdestVal;

	// instructions except branch ends in this stage, print them out
	string instruction = this->pl->instructions[this->eReg.PC];

	string opt = this->pl->getOperator(instruction);
	if(opt != "b" && opt != "beq" && opt != "bnq"){
		if(this->pl->debug) cout << "wb: " <<instruction << endl;
		cout << instruction << "\t<"<< pl->cycle << ">" << endl;
	}


}


void WriteBackStage::Shift() {
	this->eReg = this->pl->es.eReg;
}



class solution {

private:

	bool DEBUG;
	int clck;
	vector<string> vect_lines;
	vector<int>* t_vars;
	pipeline pl;

public:

	solution(ifstream &file_in, int clck_in = 10, bool DEBUG_in = false){
		this->t_vars = new vector<int>(8, 0);
		this->clck = clck_in;
		this->DEBUG = DEBUG_in;
		string regs = "";
		// get registers' values
		this->getNextInstruction(file_in, regs);

		// initiate mips simulator
		this->pl.parseRegisters(regs);
		this->pl.debug = this->DEBUG;
		this->pl.clck = this->clck;
		string instruction = "";
		int cnt = 0;
		while (this->getNextInstruction(file_in, instruction)) {
			this->pl.instructions.push_back(instruction);
			this->pl.setLabelsIndex(instruction, cnt);
			cnt++;
		}

		// print original registers
		if (this->DEBUG) {
			int i = 0;
			for (; i < 7; i++) {
				cout << this->pl.registers[i] << ",";
			}
			cout << this->pl.registers[i] << endl;
		}
	}
	void dbg(const string &msg);

	vector<int>* alu(){
		this->pl.analyzeInstructions();
		for (int i = 0; i < this->pl.registers.size(); i++) {
			this->t_vars->at(i) = this->pl.registers[i];
		}
		return this->t_vars;
	}

	int mips_clock();

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

