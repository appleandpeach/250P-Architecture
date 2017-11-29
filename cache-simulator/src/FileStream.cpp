//
//  FileStream.cpp
//  cache
//
//  Created by MaBingying on 21/11/2017.
//  Copyright © 2017 Mina. All rights reserved.
//

#include "FileStream.hpp"

bool FileStream::openFile(){
    this->inFile.open(this->filename, std::ifstream::in);

    if(!inFile){
        cout << "Unable to open the file" << endl;
        return false;
    }
    return true;
}

bool FileStream::closeFile(){
    this->inFile.close();
    return true;
}

bool FileStream::getNextInstruction(string &instruction, string &type){
	instruction = "";
	type = "";
	if(!this->inFile.eof()) {
		this->inFile >> instruction >> type;
		if(instruction == "" || type == "")
			return false;
		return true;
	}
	return false;
}

FileStream::FileStream(const char* name){
    this->filename = name;
}
