#include "modelmatrix.hpp"

BiGramMatrixModel::BiGramMatrixModel(std::shared_ptr<TokenList> _tokenList) : tokenList(_tokenList) {}

/*
 * Assumption is that TokenList::updateTokenList is called before calling this - this
 * will not add missing tokens
 */
void BiGramMatrixModel::updateWithSentenceTokens(std::vector<std::string>& tokens) {
	int tokenListIndex, tokenListIndexNext, currentProb;
	
	for (int i = 0; i < tokens.size() - 1; i++) {
		if (i == 0) {
			/* probability of first token given <s> */
		} else if (i == tokens.size() - 2) {
			/* probability of </s> token given last token */
		} else {
			/* probability of token i+1 given token i */
			tokenListIndex = tokenList.getTokenIndex(tokens[i]);
			tokenListIndexNext = tokenList.getTokenIndex(tokens[i+1]);
			matrix[i][i+1]; // P( i+1 | i )
		}
	}
}