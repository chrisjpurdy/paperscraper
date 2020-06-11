#include "paperFinder.h"

int PaperFinder::currentSearches = 0;
bool PaperFinder::initialised = false;

size_t PaperFinder::responseCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void PaperFinder::runSearch() {
	
	curlpp::Cleanup myCleanup;
	cURLpp::Easy request;
	CURLcode res;
	std::string readBuffer;
	
	/* set up url with query */
	std::string escapedQuery("\"");
	escapedQuery += curl_easy_escape(request.getHandle(), searchTerm.c_str(), 0);
	
	escapedQuery += "\"(";
	int i=0;
	for (; i<filterTerms.size() - 1; i++) {
		escapedQuery += curl_easy_escape(request.getHandle(), filterTerms[i].c_str(), 0);
		escapedQuery += "*%20OR%20";
	}
	escapedQuery += curl_easy_escape(request.getHandle(), filterTerms[i].c_str(), 0);
	escapedQuery += "*)";
	
	curl_easy_setopt(request.getHandle(), CURLOPT_WRITEFUNCTION, PaperFinder::responseCallback);
	curl_easy_setopt(request.getHandle(), CURLOPT_WRITEDATA, &readBuffer);
	
	std::string url;
	
	while (titles.size() <= 1000) {
	
		url = "https://www.ebi.ac.uk/europepmc/webservices/rest/search?query=";
		url += escapedQuery;
		url += "&resultType=lite&cursorMark=";
		url += cursorMark;
		url += "&pageSize=1000&format=json";
	
		request.setOpt<Url>(url);
		res = curl_easy_perform(request.getHandle());

	   rapidjson::Document json;
	   json.Parse(readBuffer.c_str());
	
		cursorMark = json["nextCursorMark"].GetString();
		auto results = json["resultList"]["result"].GetArray();
	
		for (auto& r : results) {
			titles.emplace_back(r["source"].GetString());
			titles.back() += ":";
			titles.back() += r["id"].GetString();
			fetchedTitles++;
		}
		readBuffer = "";
	}
	
	std::cout << "DONE" << std::endl;

}

PaperFinder::PaperFinder(std::string _searchTerm, std::string commaSeparatedFilterTerms, std::string _cursorMark) : searchTerm(_searchTerm), 
							cursorMark(_cursorMark), fetchedTitles(0) {
	/* splits the commaSeparatedFilterTerms into individual words */
	int lastSplit = 0;
	for (int i=0; i<commaSeparatedFilterTerms.length(); i++) {
		if (commaSeparatedFilterTerms[i] == ',') {
			filterTerms.push_back(commaSeparatedFilterTerms.substr(lastSplit, i - lastSplit));
			lastSplit = i+1;
		}
	}
	filterTerms.push_back(commaSeparatedFilterTerms.substr(lastSplit, commaSeparatedFilterTerms.length() - lastSplit));
	initialise();
	
	/* start search thread */
	searchThread = std::thread(&PaperFinder::runSearch, this);
	currentSearches++;
}

PaperFinder::~PaperFinder() {
	join();
}

void PaperFinder::initialise() {
	if (!initialised) {
		initialised = true;
	}
}

void PaperFinder::join() {
	if (searchThread.joinable()) {
		searchThread.join();
		currentSearches--;
	}
}

std::string PaperFinder::getTitle(int index) {
	
}

int PaperFinder::queryProgress() {
	
}

int main(int argc, char** argv) {
	PaperFinder search("Biological Nitrogen Fixation","why,where,what,lorem,ipsum","*");
	//search.join();
}