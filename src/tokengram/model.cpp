/** 
 * Way this works is by taking in a new sentence from the cmd line (in double
 * quotes), alongside a command {train, score, train_score}
 *
 * Training will update the model, score will return a
 * rating based on the current model, train_score does one after the other
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <regex>

#include "tokenlist.hpp"
//#include "modelmatrix.hpp"

TokenList tokenList;
//BiGramMatrixModel biGramModel;

void trainModels(std::vector<std::string>& tokens) {
	tokenList.updateTokenList(tokens);
}

void tokenizeSentence(std::string& sentence, std::vector<std::string>& tokens) {
	std::regex punc("[[:punct:]]", std::regex::extended);
	std::string str = std::regex_replace(sentence, punc, " ");
	
	size_t pos = 0;
	std::string token;
	while ((pos = str.find(' ')) != std::string::npos) {
		token = str.substr(0, pos);
		if (token.length() > 0) {
	   	tokens.push_back(token);
		}
	   str.erase(0, pos + 1);
	}
	if (str.length() > 0) {
   	tokens.push_back(str);
	}
}

int main() {
	
	//std::locale::global(std::locale("en_US.utf8"));
	
	std::string command;
	std::string periph;
	std::vector<std::string> tokens;
	do {
		std::getline(std::cin, command);
		std::getline(std::cin, periph);
		// validate input from user
		if (command != "train" && command != "score" && command != "train_score") {
			std::cerr << "Incorrect command" << std::endl;
			// save model
			return 1;
		}
		
		// tokenise sentence
		tokens.clear();
		tokenizeSentence(periph, tokens);
		
		// if command is train/train_score, update model
		if (command == "train" || command == "train_score") {
			// tokens shouldn't be modified
			trainModels(tokens);
		}
		
		// if command is score/train_score score the sentence and output to cout
		if (command == "score" || command == "train_score") {
			// scoreSentence(tokens);
			for (auto& t : tokens) {
				auto tokenInfoPtr = tokenList.getTokenInfo(t);
				std::cout << t << ":\t" << tokenInfoPtr->index << ", " << tokenInfoPtr->count << std::endl;
			}
		}
		
	} while (command != "close");
	
	return 0;
	
}