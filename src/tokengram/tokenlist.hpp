#ifndef TOKEN_LIST_H
#define TOKEN_LIST_H

#include <map>
#include <vector>
#include <string>

struct TokenInfo {
	int index;
	int count;
};

/*
 * Class to store list of tokens in a map, to retrieve an associated index to
 * be used in an n-gram matrix
 */
class TokenList {
public:
	std::map<std::string,TokenInfo> tokenToInfo;
	int indexCounter;
	
	void updateTokenList(std::vector<std::string>& tokens);
	int getTokenIndex(std::string& token);
};

#endif