#include "pubtatorInterface.h"

#define DUMP_FILE_PATH "periphs.txt"
#define MAX_PAGES_PER_TERM 100

int PubTatorQuery::currentSearches = 0;

/* PubTatorAnnot */
std::ostream& operator<<(std::ostream& os, const PubTatorAnnot& pta) {
	os << "\"" << pta.pmid << "\","
		<< "\"" << pta.peripheralText << "\","
		<< "\"" << pta.type << "\","
		<< "\"" << pta.identifier << "\","
		<< "\"" << pta.text << "\"";
	return os;
}

std::string to_string(const PubTatorAnnot& pta) {
	std::stringstream ss;
	
	ss << '{' << std::endl 
		<< " pmid:         " << pta.pmid << ',' << std::endl 
		<< " peripheral:   " << pta.peripheralText << ',' << std::endl
		<< " type:         " << pta.type << ',' << std::endl
		<< " identifier:   " << pta.identifier << ',' << std::endl
		<< " matched text: " << pta.text << ',' << std::endl 
		<< '}' << std::endl;
	return ss.str();
}
/* ---------- */

PubTatorQuery::PubTatorQuery(std::string _searchTerm, std::string mode, rk_sema* _sem) : finished(false), sem(_sem), searchTerm(_searchTerm) {	
	/* THIS NEEDS TO BE REPLACED WITH POLYMORPHISM FOR BETTER FURTURE PROOFING ETC. */
	rk_sema_wait(sem);
	if (mode == "search") {
		searchThread = std::thread(&PubTatorQuery::runSearch, this);
	} else if (mode == "dump") {
		searchThread = std::thread(&PubTatorQuery::runDump, this);
	} else {
		return;
	}
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

void PubTatorQuery::progressBar(float val, float max) {
	if (max <= 0 || val < 0) {
		return;
	}
	float percent = val / max;
	int numOfBars = percent * 30.f;
	std::cout << '|';
	for (int i = 1; i<=30; i++) {
		if (i <= numOfBars) {
			std::cout << '=';
		} else {
			std::cout << ' ';
		}
	}
	std::cout << "| (" << (int)(percent*100.0) << "%)    \r" << std::flush;
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
	
	// first pass
	std::string url;
	url = "https://www.ncbi.nlm.nih.gov/research/pubtator-api/publications/search?format=json&page=1&q=";
	url += escapedSearchTerm;
	url += "&filters=full.true";
	
	request.setOpt<Url>(url);
	res = curl_easy_perform(request.getHandle());

   rapidjson::Document json;
   rapidjson::ParseResult parseOK = json.Parse(readBuffer.c_str());
	if (!parseOK) {
		// TODO need to try again to parse/to perform the request in the case of an error code of 1
		std::cerr << "JSON parse error: " << parseOK.Code() << std::endl;
		exit(1);
	}
	
	//std::cout << readBuffer << std::endl;
	
	extractAnnots(json);
	readBuffer = "";
	
	// next request, number of passes given by the total pages listed in the first request
	int numberOfPages = std::min(json["total_pages"].GetInt(), 200);
	for (int page = 2; page <= numberOfPages; page++) {
		
		curlpp::Cleanup myCleanup;
		
		url = "https://www.ncbi.nlm.nih.gov/research/pubtator-api/publications/search?format=json&page=";
		url += std::to_string(page);
		url += "&q=";
		url += escapedSearchTerm;
		url += "&filters=full.true";
		
		request.setOpt<Url>(url);
		res = curl_easy_perform(request.getHandle());
		
	   json.Parse(readBuffer.c_str());
		
		extractAnnots(json);
		readBuffer = "";
	}
	outputCSVAnnots(escapedSearchTerm);
	finished = true;
	rk_sema_post(sem);
}

void PubTatorQuery::runDump() {
	curlpp::Cleanup myCleanup;
	cURLpp::Easy request;
	CURLcode res;
	std::string readBuffer;
	
	/* set up url with query */
	std::replace(searchTerm.begin(), searchTerm.end(), ' ', '+');
	std::string escapedSearchTerm = curl_easy_escape(request.getHandle(), searchTerm.c_str(), 0);
	
	curl_easy_setopt(request.getHandle(), CURLOPT_WRITEFUNCTION, PubTatorQuery::responseCallback);
	curl_easy_setopt(request.getHandle(), CURLOPT_WRITEDATA, &readBuffer);
	
	// first pass
	std::string url;
	url = "https://www.ncbi.nlm.nih.gov/research/pubtator-api/publications/search?format=json&page=1&q=";
	url += escapedSearchTerm;
	url += "&filters=full.true";
	
	request.setOpt<Url>(url);
	res = curl_easy_perform(request.getHandle());
	if (res != 0) {
		std::cerr << "CURL request error: " << res << std::endl;
	}
	
   rapidjson::Document json;
   rapidjson::ParseResult parseOK = json.Parse(readBuffer.c_str());
	if (!parseOK) {
		// TODO need to try again to parse/to perform the request in the case of an error code of 1
		std::cerr << "JSON parse error: " << parseOK.Code() << std::endl;
		exit(1);
	}
	
	// setup out file
	std::ofstream outFile;
	outFile.open(DUMP_FILE_PATH, std::ios::out | std::ios::app | std::ios::ate);
	
	extractPeriph(json, outFile);
	readBuffer = "";
	
	// next request, number of passes given by the total pages listed in the first request
	// takes maximum 200 pages
	int numberOfPages = std::min(json["total_pages"].GetInt(), MAX_PAGES_PER_TERM);
	progressBar(1, numberOfPages);
	for (int page = 2; page <= numberOfPages; page++) {
		
		curlpp::Cleanup myCleanup;
		
		url = "https://www.ncbi.nlm.nih.gov/research/pubtator-api/publications/search?format=json&page=";
		url += std::to_string(page);
		url += "&q=";
		url += escapedSearchTerm;
		url += "&filters=full.true";
		
		request.setOpt<Url>(url);
		res = curl_easy_perform(request.getHandle());
		
	   json.Parse(readBuffer.c_str());
		
		extractPeriph(json, outFile);
		progressBar(page, numberOfPages);
		readBuffer = "";
	}
	std::cout << std::endl;
	outFile.close();
	finished = true;
	rk_sema_post(sem);
}

void PubTatorQuery::outputCSVAnnots(std::string name) {
	std::ofstream output(name + ".csv", std::ofstream::out);
	output << "PMID,Peripheral text,Type,Identifier,Matched Text,Exact Correct,Periph Include,Periph Exclude,Periph Useful" << std::endl;
	for (const auto& annot : annotList) output << *annot << std::endl;
}

void PubTatorQuery::extractAnnots(rapidjson::Document& json) {
	std::string pmid;
	std::string passageText;
	PubTatorAnnot* ptrPTAnnot;
	// go through each returned paper
	if (!json.HasMember("results")) {
		return;
	}
	
	//std::cout << "Parsing page " << json["current"].GetInt() << " out of " << json["total_pages"].GetInt() << std::endl;
	
	for (auto& article : json["results"].GetArray()) {
		if (article.HasMember("id")) {
			pmid = article["id"].GetString();
		} else if (article.HasMember("pmid")) {
			pmid = std::to_string(article["pmid"].GetInt());
		} else {
			// doesn't have a pmid, so pass
			continue;
		}
		// go through each passage
		for (auto& passage : article["passages"].GetArray()) {
			if (!passage["text"].IsNull()) {
				passageText = passage["text"].GetString();
				std::replace( passageText.begin(), passageText.end(), '"', '`');
				if (!passage.HasMember("annotations")) {
					// no annotations so pass
					continue;
				}
				// go through each annotation
				for (auto& annot : passage["annotations"].GetArray()) {
					// store relevant info in a new PubTatorAnnot object and append to the vector
					/* checking to make sure all the required values are in this json object */
					if (annot.HasMember("infons") && annot.HasMember("text")) {
						if (annot["infons"].HasMember("type") && annot["infons"].HasMember("identifier")) {
							if (!(annot["infons"]["type"].IsNull() || annot["infons"]["identifier"].IsNull() || annot["text"].IsNull())) {
								// makes sure we are looking for gene related annotations
								if (annot["infons"]["type"] == "Gene") {
									ptrPTAnnot = new PubTatorAnnot();
									ptrPTAnnot->pmid = pmid;
									ptrPTAnnot->peripheralText = passageText;
									ptrPTAnnot->type = annot["infons"]["type"].GetString();
									ptrPTAnnot->identifier = annot["infons"]["identifier"].GetString();
									ptrPTAnnot->text = annot["text"].GetString();
									annotList.push_back(ptrPTAnnot);
								}
							}
						}
					}
					/* ------------------------------ */
				}
			}
		}
	}
}

void PubTatorQuery::extractPeriph(rapidjson::Document& json, std::ofstream& outFile) {
	std::string pmid;
	std::string passageText;
	// go through each returned paper

	if (!json.HasMember("results")) {
		return;
	}
	/*
	rapidjson::StringBuffer buffer;
 	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
 	json.Accept(writer);
 	std::cout << buffer.GetString() << std::endl;
	*/
	//std::assert(json.IsObject());
	
	//std::cout << "Parsing page " << json["current"].GetInt() << " out of " << json["total_pages"].GetInt() << std::endl;
	
	for (auto& article : json["results"].GetArray()) {
		if (article.HasMember("id")) {
			if (article["id"].IsString()) {
				pmid = article["id"].GetString();
			} else if (article["id"].IsInt()) {
				pmid = std::to_string(article["id"].GetInt());
			}
		} else if (article.HasMember("pmid")) {
			pmid = std::to_string(article["pmid"].GetInt());
		} else {
			// doesn't have a pmid, so pass
			continue;
		}
		// go through each passage
		for (auto& passage : article["passages"].GetArray()) {
			if (!passage["text"].IsNull()) {
				if (passage["text"].IsString()) {
					passageText = passage["text"].GetString();
					// append to end of output txt file
					outFile << passageText << "\n";
				}		
			}
		}
	}
}