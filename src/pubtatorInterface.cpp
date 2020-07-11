#include "pubtatorInterface.h"

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
	
	// first pass
	std::string url;
	url = "https://www.ncbi.nlm.nih.gov/research/pubtator-api/publications/search?format=json&page=1&q=";
	url += escapedSearchTerm;
	url += "&filters=full.true";
	
	request.setOpt<Url>(url);
	res = curl_easy_perform(request.getHandle());

   rapidjson::Document json;
   json.Parse(readBuffer.c_str());
	
	//std::cout << readBuffer << std::endl;
	
	extractAnnots(json);
	readBuffer = "";
	
	// next request, number of passes given by the total pages listed in the first request
	int numberOfPages = json["total_pages"].GetInt();
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

	finished = true;
	
	std::ofstream output(std::string(escapedSearchTerm)+".csv", std::ofstream::out);
	output << "PMID,Peripheral text,Type,Identifier,Matched Text,Exact Correct,Periph Include,Periph Exclude,Periph Useful" << std::endl;
	for (auto& annot : annotList) output << *annot << std::endl;
	
}

void PubTatorQuery::extractAnnots(rapidjson::Document& json) {
	std::string pmid;
	std::string passageText;
	PubTatorAnnot* ptrPTAnnot;
	// go through each returned paper
	if (!json.HasMember("results")) {
		return;
	}
	
	std::cout << "Parsing page " << json["current"].GetInt() << " out of " << json["total_pages"].GetInt() << std::endl;
	
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
					// char writeBuffer[65536];
	// 				rapidjson::FileWriteStream os(stdout, writeBuffer, sizeof(writeBuffer));
	// 				rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
	// 				annot.Accept(writer);
					/* checking to make sure all the required values are in this json object */
					if (annot.HasMember("infons") && annot.HasMember("text")) {
						if (annot["infons"].HasMember("type") && annot["infons"].HasMember("identifier")) {
							if (!(annot["infons"]["type"].IsNull() || annot["infons"]["identifier"].IsNull() || annot["text"].IsNull())) {
								// makes user we are looking for genes
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