//
//  Cache.hpp
//  cache
//
//  Created by MaBingying on 20/11/2017.
//  Copyright © 2017 Mina. All rights reserved.
//

#ifndef Cache_hpp
#define Cache_hpp

#include <stdio.h>
#include <math.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;


# define CACHE_SIZE 4096
# define ASSOCIATIVITY 2
# define BLOCK 16
# define ADDRESS_SIZE 32

class Cache{
public:
    long hitCount;
    long missCount;
    long cacheSize;
    	int offsetBits;
    	int indexBits;
    	int tagBits;
    int bases[8] = {1, 16, 256, 4096, 65536, 1048576, 16777216, 268435456};
    unordered_map<long, long> lru;
    unordered_map<long, long> mru;

    	void parseAddress(string address, short &offset, long &index, long &tag);
    	int stringHexToInt(string address);
    	void checkCacheHit(short offset, long index, long tag);

    	Cache();
    	~Cache();



};

#endif /* Cache_hpp */
