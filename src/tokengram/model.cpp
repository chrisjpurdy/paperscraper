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
#include <sentencepiece_processor.h>

// int loadModel(std::string& filename) {
// 	std::ifstream myfile;
// }

sentencepiece::SentencePieceProcessor processor;

void tokenizeSentence(std::string& sentence, std::vector<std::string>& tokens) {
	processor.Encode(sentence, &tokens);
	for (const std::string &token : tokens) {
	  std::cout << token << std::endl;
	}
}

int main() {
	
	// load model from file
	const auto status = processor.Load("tator.model");
	if (!status.ok()) {
	   std::cerr << status.ToString() << std::endl;
	   // error
	}
	
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
		
		// if command is score/train_score score the sentence and output to cout
	} while (command != "close");
	
	return 0;
	
}