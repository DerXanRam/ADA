#include <iostream>
#include <thread>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <fstream>

#include "includes/ABI-Utils.h"
#include "includes/sign-Utils.h"

using json = nlohmann::json;

void makeOrder(vector<string> inputs)
{
    vector<pair<string, string>> parm;
    parm = {
        {"address", inputs[3]},  // tokenpath[0]
        {"address", inputs[4]},  // tokenpath[1]
        {"address", inputs[5]},  // dexpath[0]
        {"address", inputs[6]},  // dexpath[1]
        {"uint256", "0"},        // amount0
        {"uint256", inputs[2]},  // amount1
        {"uint256", inputs[0]},  // expecteds[0]
        {"uint256", inputs[1]},  // expecteds[1]
        {"address", inputs[7]},  // loaninfo[0]
        {"address", inputs[8]},  // loaninfo[1]
        {"address", inputs[9]}}; // loaninfo[2]
    string encodedData = "0x" + EncodeABI("FlashloanRequest", parm, "(address[2],address[2],uint256,uint256,uint256[2],address[3])");
    string signedData = "0x" + rawTransaction(encodedData, "0xDBBA0", "0xB2D05E00", "0x8ED27E2896fC2b2E2c21E5eb225F7Abc5fdd2Ec7");
    json tx = eth_sendRawTransaction(signedData);
    ofstream profit("arbitrage.data", ios::app);
    if (tx.contains("result") || tx.contains("timeout"))
    {
        profit << tx["result"] << endl;
        fstream manu("nonce.data");
        string indata;
        int count;
        while (manu >> indata)
        {
            count = stod(indata);
        }
        count++;
        string nonce = to_string(count);
        manu << nonce;
        manu.close();
    }
    else
        profit << tx.dump() << endl;
    profit.close();
}
vector<string> prepair(vector<string> decodeResult)
{
    //cout << "in prepair \n";
    vector<string> res;
    res.push_back(decodeResult[decodeResult.size() - 2]); // dexaddress
    res.push_back(decodeResult[2]);                       // fromToken
    res.push_back(decodeResult[3]);                       // toToken
    res.push_back(decodeResult[0]);                       // amt in
   // cout << "out prepair \n";
    return res;
}

static size_t cont(void *contents, size_t size, size_t nmemb, void *userp)
{
    //cout << "in callback\n";
    ((string *)userp)->append((char *)contents, size * nmemb);
    //cout << "out callback\n";
    return size * nmemb;
}
json cont_call(string data, string toAddress, string from = _myAddress, string url = _RPCurl, string path = _RPCkey) // no need to check recipt. sends direct return of the method
{
    string final = "{}";
    unordered_map<string, string> parameters;
    string params0 = R"({"from":")" + from + R"(","to":)";
    string toadd = "\"" + toAddress + "\"";
    string params1 = R"(,"gas":"0x2DC6C0","gasPrice":"0xB2D05E00","value":"0x0","data":)"; //
    params0 += toadd + params1;
    string dataToSend = "\"0x" + data + "\"},\"latest\"";
    params0 += dataToSend;
    string method = "eth_call";

    string query = R"({"jsonrpc":"2.0","method":"eth_call","params":[)" + params0 + R"(],"id":1})";
    //cout << query << endl;
    CURL *curl;
    CURLcode res;
    string response;

    //cout << "quesr setted \n";
    string baseURL = "https://bsc-mainnet.blastapi.io/01af2e8d-fbe7-4c5c-b1e4-86591aaed49f";
    curl = curl_easy_init();
    //cout << "curl_easy_init finished \n";
    if (curl)
    {
        string header = "Content-Type: application/json";
        char *chuk = strdup(header.c_str());
        struct curl_slist *chunk = NULL;
        chunk = curl_slist_append(chunk, (chuk));

        curl_easy_setopt(curl, CURLOPT_URL, baseURL.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (query).c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cont);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 1500);
        //cout << "all parms faild \n";
        res = curl_easy_perform(curl);
        parameters.clear();
       // cout << "curl perform finished \n";

        // Check for errors
        if (res != CURLE_OK)
        {
            curl_easy_cleanup(curl);
           // cout << "failed\n";
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
            final = "{}";
        }
       // cout << "*********curl resp: " << response << endl;
        if (response != "" && json::accept(response))
        {
            final = response;
            //cout << "curl performed response assigned to final \n" << endl;
        }
    }
    else
    {
        //cout << "curl object construction failed\n";
    }
    json result = json::parse(final);
    curl_easy_cleanup(curl);
    //cout << "cont call finised\n";
    return result;
}
void callContract(string dexAddress, string fromToken, string toToken, string amtIn, string hash)
{
    map<string, string> dexInfo;
    dexInfo["0x10ed43c718714eb63d5aa57b78b54704e256024e"] = "0xca143ce32fe78f1f7019d7d551a6402fc5350c73";
    dexInfo["0x3a6d8ca21d1cf76f653a67577fa0d27453350dd8"] = "0x858e3312ed3a876947ea49d572a7c42de08af7ee";
    dexInfo["0x1b02dA8Cb0d097eB8D57A175b88c7D8b47997506"] = "0xc35dadb65012ec5796536bd9864ed8773abc74c4";
    ofstream outt("called_data.log", ios::app);
    outt << dexAddress << " " << dexInfo[dexAddress] << " " << fromToken << " " << toToken << " " << amtIn << "\n";
    outt.close();

    vector<pair<string, string>> pr;
    pr = {
        {"address", dexAddress},
        {"address", dexInfo[dexAddress]},
        {"address[]", fromToken + ',' + toToken},
        {"uint256", amtIn},
        {"uint256", "100"}};
    string data = EncodeABI("AnalyseTnx", pr);
    json res = cont_call(data, "0x0FaaE98e378075f4AbBb652e2E152E3A6787dC9E", "0xb438D71c1E6E8E53637e1F1deC37c2E635e335fc", "https://bsc-mainnet.blastapi.io/", "01af2e8d-fbe7-4c5c-b1e4-86591aaed49f");
    ofstream out("search_report", ios::app);
    out << res.dump() << " " << hash << endl;
    out.close();
    if (res.contains("result"))
    {
        vector<string> opp = DecodeABIOutput("uint256,uint256,uint256,address,address,address,address,address,address,address", res["result"]);
        makeOrder(opp);
    }
}


int main(int argc, char **argv)
{
    //cout << argv[1] << endl;
    //to,prepaied[1],prepaied[2],prepaied[3],hash
    callContract(argv[1], argv[2], argv[3], argv[4], argv[5]);
}