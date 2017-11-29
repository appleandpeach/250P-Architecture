//
//  main.cpp
//  cache
//
//  Created by MaBingying on 20/11/2017.
//  Copyright © 2017 Mina. All rights reserved.
//

#include <iostream>

#include "FileStream.hpp"
#include "Cache.hpp"


bool instance(const char* filename){
	FileStream fs(filename);
	if (!fs.openFile()) {
		return false;
	}
	string instruction, type;
	short offset = 0;
	long index = 0;
	long tag = 0;
	Cache c;
	int readCounter = 0;
	int writeCounter = 0;
	int totalTrace = 0;
	while(fs.getNextInstruction(instruction, type)){
		totalTrace++;
		if(type == "R"){
			readCounter++;
		}else{
			writeCounter++;
		}
		c.parseAddress(instruction, offset, index, tag);
		c.checkCacheHit(offset, index, tag);

	}
	cout << "Total Traces: " << totalTrace << endl;
	cout << "Reads: " << readCounter << endl;
	cout << "writes: " << writeCounter << endl;
	cout << "Cache hits: " << c.hitCount << endl;
	cout << "Cache misses: " << c.missCount << endl;


	fs.closeFile();
	return true;
}


int main(int argc, const char * argv[]) {
	const char * filename = argv[1];
    if(!instance(filename))
    		return false;
    
    return 0;
}
