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

int loadModel(std::string& filename) {
	ifstream myfile;
}

void tokenizeSentence(std::string& sentence, std::string delimiter, std::vector<std::string>& tokens) {
	std::vector<std::string> tokens;
	size_t pos = 0;
	while ((pos = sentence.find(delimiter)) != std::string::npos) {
	    tokens.push_back(sentence.substr(0, pos));
	    sentence.erase(0, pos + delimiter.length());
	}
}

int main() {
	
	// load model from file
	
	std::string command;
	std::string periph;
	std::vector<std::string> tokens;
	do {
		std::getline(std::cin, command);
		std::getline(std::cin, periph);
		// validate input from user
		if (command != "train" && command != "score" && command != "train_score") {
			std::err << "Incorrect command" << std::endl;
			// save model
			return 1;
		}
		
		// tokenise sentence
		tokens.clear();
		tokenizeSentence(periph, " ", tokens);
		
		// if command is train/train_score, update model
		
		// if command is score/train_score score the sentence and output to cout
	} while (input != "close");
	
}