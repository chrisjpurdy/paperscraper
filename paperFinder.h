#ifndef PAPER_FINDER_H
#define PAPER_FINDER_H

#import <vector>
#import <string>
#import <thread>
#import <iostream>

/**
 * Creating an instance of this spawns a thread to search europepmc for paper titles
 * matching the given search term and filter terms
 *
 *	Idea is that an instance is created, and the search gets automatically started
 * The caller can then check the progress of the job using fetchedTitles (or a getter
 * to make it thread-safe), and then access titles which have been fetched when avaliable
 */
class	PaperFinder {
	
	static int currentSearches;
	
	std::string searchTerm;
	std::vector<std::string> filterTerms;
	
	std::thread searchThread;
	
	std::vector<std::string> titles;
	int fetchedTitles;
	
public:
	
	PaperFinder(std::string _searchTerm, std::string commaSeparatedFilterTerms) : searchTerm(_searchTerm) {
		// splits the commaSeparatedFilterTerms into individual words
		int lastSplit = 0;
		for (int i=0; i<commaSeparatedFilterTerms.length(); i++) {
			if (commaSeparatedFilterTerms[i] == ',') {
				filterTerms.push_back(commaSeparatedFilterTerms.substr(lastSplit, i - lastSplit));
				lastSplit = i+1;
			}
		}
		filterTerms.push_back(commaSeparatedFilterTerms.substr(lastSplit, commaSeparatedFilterTerms.length() - lastSplit));
	}
	
	//void runSearch();
	
};

#endif