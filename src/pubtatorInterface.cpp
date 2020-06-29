#include "pubtatorInterface.h"

int PubTatorQuery::currentSearches = 0;

PubTatorQuery::PubTatorQuery(std::string _searchTerm) : finished(false), searchTerm(_searchTerm) {	
	searchThread = std::thread(&PubTatorQuery::runSearch, this);
	currentSearches++;
}

PubTatorQuery::~PubTatorQuery() {
	join();
}

void PubTatorQuery::join() {
	if (searchThread.joinable()) {
		searchThread.join();
		currentSearches--;
	}
}

size_t PubTatorQuery::responseCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void PubTatorQuery::runSearch() {
	curlpp::Cleanup myCleanup;
	cURLpp::Easy request;
	CURLcode res;
	std::string readBuffer;
	
	/* set up url with query */
	std::replace(searchTerm.begin(), searchTerm.end(), ' ', '+');
	std::string escapedSearchTerm = curl_easy_escape(request.getHandle(), searchTerm.c_str(), 0);
	
	curl_easy_setopt(request.getHandle(), CURLOPT_WRITEFUNCTION, PubTatorQuery::responseCallback);
	curl_easy_setopt(request.getHandle(), CURLOPT_WRITEDATA, &readBuffer);
	
	std::string url;
	url = "https://www.ncbi.nlm.nih.gov/research/pubtator-api/publications/search?format=json&page=1&q=";
	url += escapedSearchTerm;
	
	request.setOpt<Url>(url);
	res = curl_easy_perform(request.getHandle());

   rapidjson::Document json;
   json.Parse(readBuffer.c_str());
	
	std::cout << readBuffer << std::endl;
	
	// for (auto& r : results) {
// 		pmidList.emplace_back(r["pmid"].GetString());
// 	}
	
	finished = true;
	
}