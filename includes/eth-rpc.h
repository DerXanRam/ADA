#include <iostream>
#include "eth-api.h"
#include <unordered_map>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;
string _RPCurl = "https://bsc-mainnet.blastapi.io/";
string _RPCSocket = "wss://bsc-mainnet.blastapi.io/";
string _websocketKey = "c0f8ce24-de37-4003-ab56-9967b73f87e9";
string _RPCkey = "01af2e8d-fbe7-4c5c-b1e4-86591aaed49f";
string _myAddress = "0xb438D71c1E6E8E53637e1F1deC37c2E635e335fc";
json eth_call(string data, string toAddress, string from = _myAddress, string url = _RPCurl, string path = _RPCkey) // no need to check recipt. sends direct return of the method
{
    unordered_map<string, string> parameters;
    string params0 = R"({"from":")" + from + R"(","to":)";
    string toadd = "\"" + toAddress + "\"";
    string params1 = R"(,"gas":"0x2DC6C0","gasPrice":"0xB2D05E00","value":"0x0","data":)"; //
    params0 += toadd + params1;
    string dataToSend = "\"0x" + data + "\"},\"latest\"";
    params0 += dataToSend;
    string method = "eth_call";
    parameters.insert({{"id", "0"},
                       {"params", params0},
                       {"method", method},
                       {"jsonrpc", "2.0"}

    });
    json response = RPC("POST", parameters, url, path);
    return response;
}

// gossip
json eth_sendRawTransaction(string tnx, string url = _RPCurl, string path = _RPCkey)
{
    unordered_map<string, string> parameters;
    string params = R"()";
    string data1 = "\"" + tnx + "\"";
    params += data1;
    string method = "eth_sendRawTransaction";
    parameters.insert({{"id", "1"},
                       {"params", params},
                       {"method", method},
                       {"jsonrpc", "2.0"}

    });

    json response = RPC("POST", parameters, url, path);
    return response;
}

json eth_blockNumber(string url = _RPCurl, string path = _RPCkey)
{
    unordered_map<string, string> parameters1;
    string method = "eth_blockNumber";
    json res;
    parameters1.insert({{"id", "0"},
                        {"params", ""},
                        {"method", method},
                        {"jsonrpc", "2.0"}

    });
    res = RPC("POST", parameters1, url, path);
    return res;
}
json eth_getTransactionByHash(string tnx, string url = _RPCurl, string path = _RPCkey)
{
    unordered_map<string, string> parameters1;
    string params = '\"' + tnx + '\"'; // raw string
    string method = "eth_getTransactionByHash";

    parameters1.insert({{"id", "0"},
                        {"params", params},
                        {"method", method},
                        {"jsonrpc", "2.0"}

    });
    json res = RPC("POST", parameters1, url, path);
    return res;
}

json eth_getTransactionReceipt(string tnx, string url = _RPCurl, string path = _RPCkey)
{
    unordered_map<string, string> parameters1;
    string params = '\"' + tnx + '\"'; // raw string
    string method = "eth_getTransactionReceipt";
    json res;
    parameters1.insert({{"id", "0"},
                        {"params", params},
                        {"method", method},
                        {"jsonrpc", "2.0"}

    });
    res = RPC("POST", parameters1, url, path);
    return res;
}
json eth_subscribe(string url = _RPCSocket, string path = _RPCkey)
{
    unordered_map<string, string> parameters1;
    string method = "eth_subscribe";
    json res;
    parameters1.insert({{"id", "1"},
                        {"params", "\"newPendingTransactions\""},
                        {"method", "eth_subscribe"},
                        {"jsonrpc", "2.0"}

    });
    res = RPC("POST", parameters1, url, path);
    return res;
}

json eth_getTransactionCount(string address, string url = _RPCSocket, string path = _RPCkey)
{
    unordered_map<string, string> parameters1;
    string method = "eth_getTransactionCount";
    string from = "\"" + address + "\",\"pending\"";
    json res;
    parameters1.insert({{"id", "0"},
                        {"params", from},
                        {"method", method},
                        {"jsonrpc", "2.0"}

    });
    res = RPC("POST", parameters1, url, path);
    return res;
}
json eth_estimateGas(string data, string toAddress, string from = _myAddress, string url = _RPCurl, string path = _RPCkey)
{
    unordered_map<string, string> parameters;
    string params0 = R"({"from":")" + from + R"(","to":)";
    string toadd = "\"" + toAddress + "\"";
    string params1 = R"(,"gas":"0x2DC6C0","gasPrice":"0xB2D05E00","value":"0x0","data":)"; //
    params0 += toadd + params1;
    string dataToSend = "\"0x" + data + "\"},\"latest\"";
    params0 += dataToSend;
    string method = "eth_estimateGas";
    parameters.insert({{"id", "0"},
                       {"params", params0},
                       {"method", method},
                       {"jsonrpc", "2.0"}

    });
    json response = RPC("POST", parameters, url, path);
    return response;
}

string eth_getUncleByBlockHashAndIndex();
string eth_getUncleByBlockNumberAndIndex();
string eth_getTransactionByBlockHashAndIndex();
string eth_getTransactionByBlockNumberAndIndex();
string eth_getBlockTransactionCountByHash();
string eth_getBlockTransactionCountByNumber();
string eth_getUncleCountByBlockHash();
string eth_getUncleCountByBlockNumber();
string eth_getBlockByHash();
string eth_getBalance();
string eth_getStorageAt();

string eth_getCode();

string eth_getBlockByNumber();
string eth_getBalance();
string eth_getCode();
string eth_getTransactionCount();
string eth_getStorageAt();