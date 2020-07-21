#ifndef BI_GRAM_MATRIX_H
#define BI_GRAM_MATRIX_H

#include "tokenlist.hpp"

class BiGramMatrixModel {
public:
	std::vector<std::vector> matrix;
	
	BiGramMatrixModel();
	~BiGramMatrixModel();
	
	void updateWithTokens(std::vector<std::string>& tokens);
	float getProbability(std::string prevToken, std::string token);
};

#endif