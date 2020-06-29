#ifndef PUBTATOR_INTERFACE_H
#define PUBTATOR_INTERFACE_H

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

class PubTatorQuery {
private:
	static int currentSearches;
	
public:
	PubTatorQuery(std::string _searchTerm);
	~PubTatorQuery();
	
	void join();
	
	static size_t responseCallback(void *contents, size_t size, size_t nmemb, void *userp);
	void runSearch();
	
	std::thread searchThread;
	std::string searchTerm;
	
	std::vector<std::string> pmidList;
	
	bool finished;
};

#endif