//
//  FileStream.hpp
//  cache
//
//  Created by MaBingying on 21/11/2017.
//  Copyright © 2017 Mina. All rights reserved.
//

#ifndef FileStream_hpp
#define FileStream_hpp

#include <stdio.h>
#include <fstream>
#include <iostream>

#define EOF (-1)

using namespace std;

class FileStream{
public:
    const char* filename;
    ifstream inFile;
    
    FileStream(const char* name);
    
    bool openFile();
    bool closeFile();
    bool isEndOfFile();
    bool getNextInstruction(string &instruction);
    
};

#endif /* FileStream_hpp */
