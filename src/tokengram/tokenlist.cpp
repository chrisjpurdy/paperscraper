#include "tokenlist.hpp"

inline bool fileExists (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

TokenList::TokenList(std::string vocabFile) {
	/* if no vocabFile, make a new set of vocab */
	if (vocabFile == "" || !fileExists(vocabFile)) {
		TokenList();
	} else {
		/* loading vocab list and associated count of occurences from file */
		std::ifstream tokenFile;
		std::string token;
		int count;
		int i = 0;
		tokenFile.open(vocabFile);
		for (std::string line; std::getline(tokenFile, line); ) {
			token = line.substr(0, line.find('\t'));
			line.erase(0, line.find('\t') + 1);
			count = std::stoi(line.substr(0, line.find('\t')));
			tokenToInfo.emplace(std::piecewise_construct, \
				std::forward_as_tuple(token), \
				std::forward_as_tuple(i, count));
		}
	}
}

TokenList::TokenList() {
	/* set up <unk>, <s> and </s> tokens */
	tokenToInfo.emplace(std::piecewise_construct, \
		std::forward_as_tuple("<unk>"), \
		std::forward_as_tuple(0, 0));
	tokenToInfo.emplace(std::piecewise_construct, \
		std::forward_as_tuple("<s>"), \
		std::forward_as_tuple(1, 0));
	tokenToInfo.emplace(std::piecewise_construct, \
		std::forward_as_tuple("</s>"), \
		std::forward_as_tuple(2, 0));
	indexCounter = 3;
}

void TokenList::updateTokenList(std::vector<std::string>& tokens) {
	for (auto& token : tokens) {
		auto lookup = tokenToInfo.find(token);
		if (lookup != tokenToInfo.end()) {
			lookup->second.count++;
		} else {
			tokenToInfo.emplace(std::piecewise_construct, \
				std::forward_as_tuple(token), \
					std::forward_as_tuple(indexCounter, 1));
			indexCounter++;
		}
	}
}

TokenInfo* TokenList::getTokenInfo(std::string& token) {
	auto lookup = tokenToInfo.find(token);
	return (lookup == tokenToInfo.end() ? nullptr : &lookup->second);
}