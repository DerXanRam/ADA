#include <iostream>
#include <curl/curl.h>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <chrono>

using namespace std::chrono;
using namespace std;
using json = nlohmann::json;

string dbaseUrl;
static string dgs_strLastResponse;
 
/* concatenate the query parameters */
string djoinQueryParameters(const unordered_map<string, string> &parameters)
{
	string queryString = "{";
	for (auto it = parameters.cbegin(); it != parameters.cend(); ++it)
	{
		if (it == parameters.cbegin())
		{
			queryString += '\"' + it->first + '\"' + ':' + '\"' + it->second + '\"';
		}
		else if (it->first == "params")
		{
			queryString += ",\"" + it->first + '\"' + ':' + '[' + it->second + ']';
		}
		else if (it->first == "id")
		{
			queryString += ",\"" + it->first + '\"' + ':' + it->second;
		}
		else
		{
			queryString += ",\"" + it->first + '\"' + ':' + '\"' + it->second + '\"';
		}
	}
	queryString += "}";
	cout << queryString << endl;
	return queryString;
}

/*Sending the HTTP Request */
void dexecuteHTTPRequest(CURL *curl)
{
	CURLcode res;
	dgs_strLastResponse = "";
	/* Perform the request, res will get the return code */
	auto start=high_resolution_clock::now();
	res = curl_easy_perform(curl);
	
	/* Check for errors */
	if (res != CURLE_OK)
	{
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
      dgs_strLastResponse=R"({"timeout":true})";
	}
	auto stop=high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	//cout <<"xxxxxxxxxxxxxxxxxxxxx "<< duration.count() << endl;
}

/*Write the server response to string "gs_strLastResponse"*/
size_t dWriteCallback(char *contents, size_t size, size_t nmemb, void *userp)
{
	dgs_strLastResponse += (const char *)contents;
//	dgs_strLastResponse += '\n';
//cout<<dgs_strLastResponse<<endl;
	return size * nmemb;
}

void dsendSignedRequest(CURL *curl, string httpMethod, unordered_map<string, string> &parameters)
{
	string url = dbaseUrl;
	string queryString = "";
	if (!parameters.empty())
	{
		string res = djoinQueryParameters(parameters);
		queryString += res;
	}
	else
	{
		cout << "error!! Empty parameter Detected" << endl;
		exit(-1);
	}
    //cout<<"url "<<url<<endl;
	curl_easy_setopt(curl, CURLOPT_URL, (dbaseUrl).c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, queryString.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dWriteCallback);
	curl_easy_setopt(curl,CURLOPT_NOSIGNAL,"0");//added to turn of signal checker
	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 16000);

	parameters.clear();
	dexecuteHTTPRequest(curl);
}

json RPC(string protocol, unordered_map<string, string> parameters, string url, string path, string header="Content-Type: application/json")
{
	CURL *curl;
	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */
	curl = curl_easy_init();
    
	char *chuk=strdup(header.c_str());
	/* Adding API key to header */
	struct curl_slist *chunk = NULL;
	chunk = curl_slist_append(chunk, (chuk));
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	dbaseUrl = url + path;
	using namespace std::chrono;
    auto start = high_resolution_clock::now();
	
	dsendSignedRequest(curl, protocol, parameters);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	//cout <<"req response duration "<< duration.count() << endl;

	auto start1 = high_resolution_clock::now();
	if (!json::accept(dgs_strLastResponse)) // prevents wierd nlohmann parse error due to bad json format caused by the api endpoint.
	{
		string temp;
		for (int i = 0; i < dgs_strLastResponse.length(); i++)
		{
			if (!isspace(dgs_strLastResponse[i]))
				temp += dgs_strLastResponse[i];
		}
		dgs_strLastResponse = temp;
	}

	if(!json::accept(dgs_strLastResponse) || "" || " ")
	{
		dgs_strLastResponse="{}";
	}
    auto stop1 = high_resolution_clock::now();
	auto duration1 = duration_cast<microseconds>(stop1 - start1);
	//cout <<"space remover duration "<< duration1.count() << endl;
	json JsonForm = json::parse(dgs_strLastResponse);
	/* always cleanup */
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return JsonForm;
}
