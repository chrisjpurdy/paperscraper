#include "pubtatorInterface.h"

/* ./comp.bat pubtatorInterface.cpp - compiles this with the pubtator interface class, use ./a.out to run */

/* To train the model with the scraped data use the below as reference - NOT USING ANYMORE, was to be used with sentencepiece
 * spm_train --input=periphs.txt --model_prefix=tator --vocab_size=8000 --character_coverage=1.0 --model_type=unigram
 */
void scrapeKeywords() {
	std::ifstream keywordFile;
	std::string line;
	keywordFile.open("keywords.txt");
	
	rk_sema sem;
	rk_sema_init(&sem, 1); // init binary sem
	
	std::getline(keywordFile, line);
	while (!keywordFile.eof()) {
		std::cout << "Dumping periph from search term: " << line << std::endl; 
		PubTatorQuery dump(line.c_str(), "dump", &sem);
		rk_sema_wait(&sem);
		rk_sema_post(&sem);
		std::getline(keywordFile, line);
	}
	rk_sema_destroy(&sem);
	keywordFile.close();
}

int main(int argc, char** argv) {
	scrapeKeywords();
	//PubTatorQuery search("Biological Nitrogen Fixation", "search");
	//search.join();
}