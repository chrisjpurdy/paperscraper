#include "tokenlist.hpp"

TokenList::TokenList(std::string vocabFile) {
	/* read vocab file, generated from sentencepiece output */
}

void TokenList::updateTokenList(std::vector<std::string>& tokens) {
	for (std::string& token : tokens) {
		auto lookup = tokenToInfo.find(token);
		if (lookup == tokenToInfo.end()) {
			lookup->second.count++;
		} else {
			tokenToInfo.emplace(std::piecewise_construct, 
										std::forward_as_tuple(token),
										std::forward_as_tuple(indexCounter, 1));
			indexCounter++;
		}
	}
}

int TokenList::getTokenIndex(std::string& token) {
	auto lookup = tokenToInfo.find(token);
	return (lookup == tokenToInfo.end() ? -1 : lookup->second.index);
}