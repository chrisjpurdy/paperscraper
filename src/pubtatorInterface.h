#ifndef PUBTATOR_INTERFACE_H
#define PUBTATOR_INTERFACE_H

#import <vector>
#import <string>
#import <thread>
#import "portable_sem.h"
#import <iostream>
#import <sstream>
#import <fstream>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "rapidjson/stream.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/filewritestream.h"

using namespace curlpp::options;

class PubTatorAnnot {
public:
	std::string pmid;
	std::string peripheralText;
	std::string type;
	std::string identifier;
	std::string text;
	
	friend std::ostream& operator<<(std::ostream& os, const PubTatorAnnot& pta);
	friend std::string to_string(const PubTatorAnnot& pta);
};

class PubTatorQuery {
private:
	static int currentSearches;
	
public:
	PubTatorQuery(std::string _searchTerm, std::string mode, rk_sema* sem);
	~PubTatorQuery();
	
	void join();
	
	static size_t responseCallback(void *contents, size_t size, size_t nmemb, void *userp);
	void runSearch();
	void runDump();
	
	void progressBar(float val, float max);
	
	void extractAnnots(rapidjson::Document& json);
	void extractPeriph(rapidjson::Document& json, std::ofstream& outFile);
	
	void outputCSVAnnots(std::string name);
	
	std::thread searchThread;
	std::string searchTerm;
	
	rk_sema* sem;
	
	std::vector<std::string> pmidList;
	
	std::vector<PubTatorAnnot*> annotList;
	
	bool finished;
};

#endif