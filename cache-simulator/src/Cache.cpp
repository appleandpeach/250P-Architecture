//
//  Cache.cpp
//  cache
//
//  Created by MaBingying on 20/11/2017.
//  Copyright © 2017 Mina. All rights reserved.
//

#include "Cache.hpp"

Cache::Cache() {
	this->hitCount = 0;
	this->missCount = 0;
	this->cacheSize = CACHE_SIZE * 1024;
	this->offsetBits = log2(BLOCK);
	this->indexBits = log2(this->cacheSize / (ASSOCIATIVITY * BLOCK));
	this->tagBits = ADDRESS_SIZE - this->offsetBits - this->indexBits;
}
Cache::~Cache() {

}

int getNum(char a) {
	switch (a) {
	case 'a':
		return 10;
	case 'b':
		return 11;
	case 'c':
		return 12;
	case 'd':
		return 13;
	case 'e':
		return 14;
	case 'f':
		return 15;
	default:
		return a - '0';
	}
}

int Cache::stringHexToInt(string address) {
	int res = 0;
	for (int i = 7; i > -1; i--) {
		res += getNum(address[i]) * this->bases[7 - i];
	}
	return res;
}

void Cache::parseAddress(string address, short &offset, long &index, long &tag) {
	stringHexToInt(address);
	int add = stringHexToInt(address);
	offset = (add >> 0) & ((1 << offsetBits) - 1);
	index = (add >> offsetBits) & ((1 << indexBits) - 1);
	tag = (add >> (offsetBits + indexBits)) & ((1 << tagBits) - 1);
}

void Cache::checkCacheHit(short offset, long index, long tag) {
	if (lru.count(index) == 0 && mru.count(index) == 0 ){
		this->missCount++;
		mru[index] = tag;
		return;
	}else {
		if(mru.count(index)>0 && mru[index]==tag){
			mru[index] = tag;
			hitCount++;
			return;
		}
		if(lru.count(index)>0 && lru[index] == tag){
			long tmp = mru[index];
			mru[index] = lru[index];
			lru[index] = tmp;
			hitCount++;
			return;
		}

		if(mru.count(index)>0){
			lru[index] = mru[index];
			mru[index] = tag;
		}else{
			mru[index] = tag;
		}
		missCount++;
	}
}



