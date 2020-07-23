#ifndef BI_GRAM_MATRIX_H
#define BI_GRAM_MATRIX_H

#include "tokenlist.hpp"

class BiGramMatrixModel {
public:
	std::vector<std::vector<float>> matrix;
	std::shared_ptr<TokenList> tokenList;
	
	BiGramMatrixModel(std::shared_ptr<TokenList> _tokenList);
	~BiGramMatrixModel();
	
	void updateWithSentenceTokens(std::vector<std::string>& tokens);
	float getProbability(std::string prevToken, std::string token);
};

#endif