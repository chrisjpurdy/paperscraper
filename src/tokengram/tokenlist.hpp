#ifndef TOKEN_LIST_H
#define TOKEN_LIST_H

#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

class TokenInfo {
public:
	TokenInfo(int i, int c) : index(i), count(c) {};
	int index;
	int count;
};

/*
 * Class to store list of tokens in a map, to retrieve an associated index to
 * be used in an n-gram matrix
 */
class TokenList {
public:
	TokenList(std::string vocabFile);
	TokenList();
	
	std::map<std::string, TokenInfo> tokenToInfo;
	int indexCounter;
	
	void updateTokenList(std::vector<std::string>& tokens);
	TokenInfo* getTokenInfo(std::string& token);
};

#endif