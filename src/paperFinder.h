#ifndef PAPER_FINDER_H
#define PAPER_FINDER_H

#import <vector>
#import <string>
#import <thread>
#import <mutex>
#import <iostream>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "rapidjson/stream.h"
#include "rapidjson/document.h"

using namespace curlpp::options;

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
	static bool initialised;
	
private:
	std::string searchTerm;
	std::string cursorMark;
	std::vector<std::string> filterTerms;

	std::thread searchThread;
	std::mutex progressMutex;
	std::mutex titlesMutex;
			
	std::vector<std::string> titles;
	int fetchedTitles;
	
	void initialise();
	
public:
	
	PaperFinder(std::string _searchTerm, std::string commaSeparatedFilterTerms, std::string _cursorMark);
	
	~PaperFinder();
	
	static size_t responseCallback(void *contents, size_t size, size_t nmemb, void *userp);
	
	void join();
	
	/**
	 * Runs the search for the given term with the filter words
	 */
	void runSearch();
	
	/**
	 * Thread safe access to progress variable (how many titles can be read)
	 */
	int queryProgress();
	
	/**
 	 * Thread safe access to the title at a given index
 	 */
	std::string getTitle(int index);
	
};

#endif