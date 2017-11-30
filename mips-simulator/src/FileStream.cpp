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

bool IsSpace (int ch) {
	return !std::isspace(ch);
}


static inline void rtrim(string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), IsSpace).base(), s.end());
}

bool FileStream::getNextInstruction(string &instruction){
	instruction = "";
	if(!this->inFile.eof()) {
		getline(this->inFile, instruction);
		rtrim(instruction);
		if(instruction == "")
			return false;
		return true;
	}
	return false;
}

FileStream::FileStream(const char* name){
    this->filename = name;
}
