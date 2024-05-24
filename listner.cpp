#include <iostream>
#include <thread>
#include <cpprest/ws_client.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <sys/resource.h>
#include "includes/ABI-Utils.h"
#include "includes/sign-Utils.h"

using namespace std::chrono;
using namespace std;
using namespace web;
using namespace web::websockets::client;
using json = nlohmann::json;

vector<string> prepair(vector<string> decodeResult)
{
    vector<string> res;
    res.push_back(decodeResult[decodeResult.size() - 2]); // dexaddress
    res.push_back(decodeResult[2]);                       // fromToken
    res.push_back(decodeResult[3]);                       // toToken
    res.push_back(decodeResult[0]);                       // amt in
    return res;
}
bool is_valubale(string token)
{
    string peggeds[3] = {
        "0x55d398326f99059ff775485246999027b3197955", // usdt
        "0xe9e7cea3dedca5984780bafc599bd69add087d56", // busd
        "0x8ac76a51cc950d9822d68b83fe1ad97b32cd580d"  // usdc
    };

    for (int i = 0; i < 3; i++)
    {
        if (token == peggeds[i])
        {
            return true;
        }
    }
    return false;
}
void callFarmer(string to, string prepaired1, string prepaired2, string prepaired3, string hash)
{
    cout<<"calling farmer \n";
    string command = "./farmer " + to + " " + prepaired1 + " " + prepaired2 + " " + prepaired3 + " " + hash + " terminator";
    system(command.c_str());
    cout<<"farmer called \n";
}
void FunDecoder(string data, string to, string hash)
{
    
    cout << "here in fun decoder \n";
    vector<pair<string, string>> FunctionSignitures;
    vector<string> result;
    vector<string> details;
    bool flag = false;
    string funName;
    // slicing function signiture from input data hash
    string funSign;
    if (data.length() > 9)
    {
        for (int i = 0; i < 10; i++)
        {
            funSign += data[i];
        }

        cout << "here after collecting first 10 sign of tx: " << hash << "\n";
        /*initializing function signitures*/
        FunctionSignitures.push_back(make_pair("swapExactTokensForTokens(uint256,uint256,address[],address,uint256)", "0x38ed1739"));
        // FunctionSignitures.push_back(make_pair("swapETHForExactTokens(uint256,address[],address,uint256)", "0xfb3bdb41"));
        /*checking if the scliced signiture founds in predetermined signitures*/

        vector<pair<string, string>>::iterator it;
        for (it = FunctionSignitures.begin(); it != FunctionSignitures.end(); ++it)
        {
            if (funSign == it->second)
            {

                result = DecodeABIinputs(it->first, data);
                // details=swapExactTokenForToken(result);
                cout << "\n************found****************\n";

                if (result.size() > 5)
                {
                    vector<string> prepaired = prepair(result);
                    if (is_valubale(prepaired[1]) || is_valubale(prepaired[2]))
                    {
                        ofstream out("decodedtnx.data", ios::app);
                        for (int i = 0; i < result.size(); i++)
                        {
                            out << result[i] << " ";
                        }
                        cout << "writting to decodedtx finished\n";
                        out << "\n";
                        out.close();
                        // thread(callContract, to, prepaired[1], prepaired[2], prepaired[3], hash).detach();
                        // string command="./farmer "+to+" "+prepaired[1]+" "+prepaired[2]+" "+prepaired[3]+" "+hash+" terminator";
                        thread(callFarmer, to, prepaired[1], prepaired[2], prepaired[3], hash).detach();
                    }
                    else
                    {
                        cout << "no valuable token from either of them !! \n";
                    }
                }
                else
                {
                    cout << "invalid decoded vector data to collect inputs\n";
                }
            }
            else
            {
                cout << "not needed data tx \n";
            }
        }
    }
    else
    {
        cout << "invalid data to collect fun signiture\n";
    }
}

bool RestartSignal = false;
string ret_str = "";
int j = 0;
void sock()
{
restart:
    websocket_callback_client client;
    // 0x10ed43c718714eb63d5aa57b78b54704e256024e
    client.connect("wss://morning-twilight-sheet.bsc.quiknode.pro/5bdca30ff1fb35d78d7497264a5d7048f5ad3f94/").wait();
    std::string body_str = R"({"jsonrpc":"2.0","method":"eth_subscribe","params":["newPendingTransactions",true],"id":1})";
    std::vector<unsigned char> body(body_str.begin(), body_str.end());

    pplx::task_completion_event<void> receiveEvent;
    // make sure client works fine without setting receive handler
    websocket_outgoing_message msg;
    msg.set_utf8_message(body_str);
    client.send(msg).wait();

    // set receive handler
    client.set_message_handler([body_str, &receiveEvent](websocket_incoming_message ret_msg)
                               {
                                   string body = ret_msg.extract_string().get();
                                   ret_str = body;
                                   //cout << j << " " << ret_str;
                                   j++; });
    while (!RestartSignal)
    {
        usleep(1000000);
    }
    cout << "signal Recived !!!  signal Recived !!! signal Recived !!! signal Recived !!! signal Recived !!! signal Recived !!! signal Recived !!! signal Recived !!! signal Recived !!! signal Recived !!!\n";
    // string unsub=R"({"id":1,"jsonrpc":"2.0","method":"eth_unsubscribe","params":["0xe9549ac54eeec07f"]})";
    // msg.set_utf8_message(unsub);
    // client.send(msg);
    receiveEvent.set();
    pplx::create_task(receiveEvent).wait();
    client.close().wait();
    RestartSignal = false;
    goto restart;
}

void getHash0()
{
    bool first = true;
    vector<std::thread> threads;
    int k = 0;
    string prev = "";
    while (true)
    {
        string body = ret_str;

        if (body != "" && body != " " && prev != body && json::accept(body))
        {
            json res = json::parse(body);
            if (!res["params"]["result"].is_null() && !first)
            {
                first = false;
                if (res["params"]["result"]["to"] == "0x10ed43c718714eb63d5aa57b78b54704e256024e" || res["params"]["result"]["to"] == "0x3a6d8ca21d1cf76f653a67577fa0d27453350dd8" || res["params"]["result"]["to"] == "0x1b02dA8Cb0d097eB8D57A175b88c7D8b47997506")
                {
                    ofstream pro("procsesedtnx.data", ios::app);
                    pro << res["params"]["result"]["to"] << " " << res["params"]["result"]["hash"] << " " << res["params"]["result"]["input"] << "\n";
                    pro.close();
                    FunDecoder(res["params"]["result"]["input"], res["params"]["result"]["to"], res["params"]["result"]["hash"]);
                }
            }
            first = false;
            prev = body;
            string fn = "tnxlogs.data";
            ofstream out(fn, ios::app);
            out << " " << body;
            out.close();
        }
    }
}

void SocketManager()
{
    string previ = "";
    int repititionCounter = 0;
    while (true)
    {
        string bodyi = ret_str;

        if (previ == bodyi)
        {
            cout << repititionCounter << " SAME SAME SAME SAME SAME SAME SAME SAME SAME SAME SAME SAME SAME SAME SAME SAME SAME SAME SAME SAME \n";
            if (repititionCounter == 3)
            {
                RestartSignal = true;
                repititionCounter = 0;
            }
            else
            {
                repititionCounter++;
            }
        }
        previ = bodyi;
        usleep(3000000);
    }
}

int main()
{
    const rlim_t KStackSize=40*1024*1024;
    thread t2(getHash0);
    thread t(sock);
    thread t3(SocketManager);
    t3.join();
    t2.join();
    t.join();
    /*cout << getNonce() << endl;
    string encodedData = "0x095ea7b30000000000000000000000005302086a3a25d473aabbd0356eff8dd811a4d89b0000000000000000000000000000000000000000000000000de0b6b3a7640000";
    string signedData = "0x" + rawTransaction(encodedData, "0x186A0", "0xEE6B2800", "0x55d398326f99059ff775485246999027b3197955");
    cout<<"raw is: "<<signedData<<endl;
    json res=eth_sendRawTransaction(signedData);
    cout<<res.dump()<<endl;
    */
}