#include <vector>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ethc/abi.h>
#include <gmp.h>

#include "utils.h"

#define ok(ethcop) assert(ethcop >= 0)
using namespace std;

using namespace std;

string EncodeABI(string functionName, vector<pair<string, string>> parameters,string fun="") // beanchmark--for hard work =210 micro-sec
{
    string function = functionName + "(";
    int NumOfParametrs = parameters.size();

    // separating parameters
    string conParamValues[NumOfParametrs];
    string ParamName[NumOfParametrs];
    vector<pair<string, string>>::iterator it;
    int k = 0;
    for (it = parameters.begin(); it != parameters.end(); ++it)
    {
        conParamValues[k] = it->second;
        ParamName[k] = it->first;
        function += ParamName[k] + ",";
        k++;
    }
    // encoding
    struct eth_abi abi;
    if (function[function.length() - 1] == ',')
        function[function.length() - 1] = ')';
    else
        function += ")";
    if(!fun.empty())
    {
        function=functionName+fun;
    }

    cout << "function is " << function << endl;
    char *fn = strdup(function.c_str()), *hex;
    size_t hexlen;
    eth_abi_init(&abi, ETH_ABI_ENCODE);
    eth_abi_call(&abi, &fn, NULL);
    if (NumOfParametrs != 0)
    {
        for (int i = 0; i < k; i++)
        {
            if (ParamName[i] == "uint8" || ParamName[i] == "uint8[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, NULL); // array beign
                    string value = conParamValues[i];
                    string temp;
                    for (int j = 0; j < value.length(); j++)
                    {
                        if (value[j] == ',')
                        {
                            uint8_t val = bstod(temp);
                            eth_abi_uint8(&abi, &val);
                            temp = "";
                        }
                        else if (j == value.length() - 1)
                        {
                            temp += value[j];
                            uint8_t val = bstod(temp);
                            eth_abi_uint8(&abi, &val);
                        }
                        else
                        {
                            temp += value[j];
                        }
                    }
                    eth_abi_array_end(&abi); // array end
                }
                else
                {
                    uint8_t val = bstod(conParamValues[i]);
                    eth_abi_uint8(&abi, &val);
                }
            }

            if (ParamName[i] == "uint16" || ParamName[i] == "uint16[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, NULL); // array beign
                    string value = conParamValues[i];
                    string temp;
                    for (int j = 0; j < value.length(); j++)
                    {
                        if (value[j] == ',')
                        {
                            uint16_t val = bstod(temp);
                            eth_abi_uint16(&abi, &val);
                            temp = "";
                        }
                        else if (j == value.length() - 1)
                        {
                            temp += value[j];
                            uint16_t val = bstod(temp);
                            eth_abi_uint16(&abi, &val);
                        }
                        else
                        {
                            temp += value[j];
                        }
                    }
                    eth_abi_array_end(&abi); // array end
                }
                else
                {
                    uint16_t val = bstod(conParamValues[i]);
                    eth_abi_uint16(&abi, &val);
                }
            }

            if (ParamName[i] == "uint32" || ParamName[i] == "uint32[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, NULL); // array beign
                    string value = conParamValues[i];
                    string temp;
                    for (int j = 0; j < value.length(); j++)
                    {
                        if (value[j] == ',')
                        {
                            uint32_t val = bstod(temp);
                            eth_abi_uint32(&abi, &val);
                            temp = "";
                        }
                        else if (j == value.length() - 1)
                        {
                            temp += value[j];
                            uint32_t val = bstod(temp);
                            eth_abi_uint32(&abi, &val);
                        }
                        else
                        {
                            temp += value[j];
                        }
                    }
                    eth_abi_array_end(&abi); // array end
                }
                else
                {
                    uint32_t val = bstod(conParamValues[i]);
                    eth_abi_uint32(&abi, &val);
                }
            }
            if (ParamName[i] == "uint64" || ParamName[i] == "uint64[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, NULL); // array beign
                    string value = conParamValues[i];
                    string temp;
                    for (int j = 0; j < value.length(); j++)
                    {
                        if (value[j] == ',')
                        {
                            uint64_t val = bstod(temp);
                            eth_abi_uint64(&abi, &val);
                            temp = "";
                        }
                        else if (j == value.length() - 1)
                        {
                            temp += value[j];
                            uint64_t val = bstod(temp);
                            eth_abi_uint64(&abi, &val);
                        }
                        else
                        {
                            temp += value[j];
                        }
                    }
                    eth_abi_array_end(&abi); // array end
                }
                else
                {
                    uint64_t val = bstod(conParamValues[i]);
                    eth_abi_uint64(&abi, &val);
                }
            }
            if (ParamName[i] == "address" || ParamName[i] == "address[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    cout<<"in address array"<<endl;
                    eth_abi_array(&abi, NULL); // array beign
                    string value = conParamValues[i];
                    string temp;
                    for (int j = 0; j < value.length(); j++)
                    {
                        if (value[j] == ',')
                        {
                            char *val = strdup(temp.c_str());
                            eth_abi_address(&abi, &val);
                            temp = "";
                        }
                        else if (j == value.length() - 1)
                        {
                            temp += value[j];
                            char *val = strdup(temp.c_str());
                            eth_abi_address(&abi, &val);
                        }
                        else
                        {
                            temp += value[j];
                        }
                    }
                    eth_abi_array_end(&abi); // array end
                }
                else
                {
                    cout<<"in address"<<endl;
                    char *val = strdup(conParamValues[i].c_str());
                    eth_abi_address(&abi, &val);
                }
            }
            if (ParamName[i] == "bool" || ParamName[i] == "bool[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, NULL); // array beign
                    string value = conParamValues[i];
                    string temp;
                    for (int j = 0; j < value.length(); j++)
                    {
                        if (value[j] == ',')
                        {
                            bool sym = false;
                            if (temp == "true")
                                sym = true;
                            uint8_t val = sym;
                            eth_abi_bool(&abi, &val);
                            temp = "";
                        }
                        else if (j == value.length() - 1)
                        {
                            temp += value[j];
                            bool sym = false;
                            if (temp == "true")
                                sym = true;
                            uint8_t val = sym;
                            eth_abi_bool(&abi, &val);
                        }
                        else
                        {
                            temp += value[j];
                        }
                    }
                    eth_abi_array_end(&abi); // array end
                }
                else
                {
                    bool sym = false;
                    if (conParamValues[i] == "true")
                        sym = true;
                    uint8_t val = sym;
                    eth_abi_bool(&abi, &val);
                }
            }

            if (ParamName[i] == "uint256" || ParamName[i] == "uint256[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, NULL); // array beign

                    string value = conParamValues[i];
                    string temp;
                    for (int j = 0; j < value.length(); j++)
                    {
                        if (value[j] == ',')
                        {
                            char *num = strdup(temp.c_str());
                            mpz_t val;
                            mpz_init_set_str(val, num, 0);
                            eth_abi_mpint(&abi, val);
                            temp = "";
                        }
                        else if (j == value.length() - 1)
                        {
                            temp += value[j];
                            char *num = strdup(temp.c_str());
                            mpz_t val;
                            mpz_init_set_str(val, num, 0);
                            eth_abi_mpint(&abi, val);
                        }
                        else
                        {
                            temp += value[j];
                        }
                    }
                    eth_abi_array_end(&abi); // array end
                }
                else
                {
                    cout<<"in uint256"<<endl;
                    char *num = strdup(conParamValues[i].c_str());
                    mpz_t val;
                    mpz_init_set_str(val, num, 0);
                    eth_abi_mpint(&abi, val);
                }
            }
            if (ParamName[i] == "bytes" || ParamName[i] == "bytes[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, NULL); // array beign
                    string value = conParamValues[i];
                    string temp;
                    for (int j = 0; j < value.length(); j++)
                    {
                        if (value[j] == ',')
                        {
                            uint8_t *val = (uint8_t *)temp.c_str();
                            eth_abi_bytes(&abi, &val, NULL);
                            temp = "";
                        }
                        else if (j == value.length() - 1)
                        {
                            temp += value[j];
                            uint8_t *val = (uint8_t *)temp.c_str();
                            eth_abi_bytes(&abi, &val, NULL);
                        }
                        else
                        {
                            temp += value[j];
                        }
                    }
                    eth_abi_array_end(&abi); // array end
                }
                else
                {
                    size_t byteLen = conParamValues[i].size();
                    uint8_t *val = (uint8_t *)conParamValues[i].c_str();
                    char *final = (char *)val;

                    string decodedres;
                    for (int i = 0; i < 4; i++)
                    {
                        decodedres += final[i];
                    }
                    eth_abi_bytes(&abi, &val, &byteLen);
                }
            }
        }
    }

    eth_abi_call_end(&abi);
    ok(eth_abi_to_hex(&abi, &hex, &hexlen));
    ok(eth_abi_free(&abi));
    string data(hex);
    return data;
}

vector<string> DecodeABIOutput(string outputDataTypes, string data) // beanchmark--for hard work =210 micro-sec
{
    vector<string> result;
    string temp;
    uint64_t arrlen = 0; // for array
    vector<string> ParamName;
    int NumOfParametrs = 0;
    for (int i = 0; i < outputDataTypes.length(); i++)
    {
        if (outputDataTypes[i] == ',')
        {
            ParamName.push_back(temp);
            temp = "";
            NumOfParametrs++;
        }
        else if (i == outputDataTypes.length() - 1 && !outputDataTypes.empty())
        {
            temp += outputDataTypes[i];
            ParamName.push_back(temp);
            temp = "";
            NumOfParametrs++;
            break;
        }
        else
        {
            temp += outputDataTypes[i];
        }
    }

    // decoding result
    struct eth_abi abi;
    size_t hexlen;

    char *input = strdup(data.c_str());
    eth_abi_from_hex(&abi, input, data.length());
    if (NumOfParametrs != 0)
    {
        for (int i = 0; i < NumOfParametrs; i++)
        {
            if (ParamName[i] == "uint8" || ParamName[i] == "uint8[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, &arrlen); // array beign
                    uint8_t val[arrlen];
                    for (int j = 0; j < arrlen; j++)
                    {
                        eth_abi_uint8(&abi, &val[j]);
                        result.push_back(to_string(val[j]));
                    }
                    eth_abi_array_end(&abi); // array end
                    arrlen = 0;
                }
                else
                {
                    uint8_t val;
                    eth_abi_uint8(&abi, &val);
                    result.push_back(to_string(val));
                }
            }

            if (ParamName[i] == "uint16" || ParamName[i] == "uint16[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, &arrlen); // array beign
                    uint16_t val[arrlen];
                    for (int j = 0; j < arrlen; j++)
                    {
                        eth_abi_uint16(&abi, &val[j]);
                        result.push_back(to_string(val[j]));
                    }
                    eth_abi_array_end(&abi); // array end
                    arrlen = 0;
                }
                else
                {
                    uint16_t val;
                    eth_abi_uint16(&abi, &val);
                    result.push_back(to_string(val));
                }
            }

            if (ParamName[i] == "uint32" || ParamName[i] == "uint32[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, &arrlen); // array beign
                    uint32_t val[arrlen];
                    for (int j = 0; j < arrlen; j++)
                    {
                        eth_abi_uint32(&abi, &val[j]);
                        result.push_back(to_string(val[j]));
                    }
                    eth_abi_array_end(&abi); // array end
                    arrlen = 0;
                }
                else
                {
                    uint32_t val;
                    eth_abi_uint32(&abi, &val);
                    result.push_back(to_string(val));
                }
            }
            if (ParamName[i] == "uint64" || ParamName[i] == "uint64[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, &arrlen); // array beign
                    uint64_t val[arrlen];
                    for (int j = 0; j < arrlen; j++)
                    {
                        eth_abi_uint64(&abi, &val[j]);
                        result.push_back(to_string(val[j]));
                    }
                    eth_abi_array_end(&abi); // array end
                    arrlen = 0;
                }
                else
                {
                    uint64_t val;
                    eth_abi_uint64(&abi, &val);
                    result.push_back(to_string(val));
                }
            }
            if (ParamName[i] == "uint256" || ParamName[i] == "uint256[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;

                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, &arrlen); // array beign
                    mpz_t val[arrlen];
                    cout<<"in uint256[]\n";
                    cout<<"arrsize is "<<arrlen<<endl;
                    for (int j = 0; j < arrlen; j++)
                    {
                        mpz_init(val[j]);
                        eth_abi_mpint(&abi, val[j]);
                        char *mpnum;
                        gmp_asprintf(&mpnum, "%Zd", val[j]);
                        string n(mpnum);
                        result.push_back(n);
                        free(mpnum);
                    }
                    eth_abi_array_end(&abi); // array end
                    arrlen = 0;
                }
                else
                {

                    mpz_t val;
                    mpz_init(val);
                    eth_abi_mpint(&abi, val);
                    char *mpnum;
                    gmp_asprintf(&mpnum, "%Zd", val);
                    string n(mpnum);
                    result.push_back(n);
                }
                cout<<"uint256 finished \n";
            }
            if (ParamName[i] == "address" || ParamName[i] == "address[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, &arrlen); // array beign
                    char *val[arrlen];
                    cout<<"in address[]\n";
                    cout<<"arrsize is "<<arrlen<<endl;
                    for (int j = 0; j < arrlen; j++)
                    {
                        eth_abi_address(&abi, &val[j]);
                        string a(val[j]);
                        result.push_back("0x"+a);
                    }
                    eth_abi_array_end(&abi); // array end
                    arrlen = 0;
                }
                else
                {
                    char *val;
                    eth_abi_address(&abi, &val);
                    string a(val);
                    result.push_back("0x"+a);
                }
            }

            if (ParamName[i] == "bool" || ParamName[i] == "bool[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, &arrlen); // array beign
                    uint8_t val[arrlen];
                    for (int j = 0; j < arrlen; j++)
                    {
                        eth_abi_bool(&abi, &val[j]);
                        result.push_back(to_string(val[j]));
                    }
                    eth_abi_array_end(&abi); // array end
                    arrlen = 0;
                }
                else
                {
                    uint8_t val;
                    eth_abi_bool(&abi, &val);
                    result.push_back(to_string(val));
                }
            }
            if (ParamName[i] == "bytes" || ParamName[i] == "bytes[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, &arrlen); // array beign
                    uint8_t *val[arrlen];
                    size_t len;
                    for (int j = 0; j < arrlen; j++)
                    {
                        eth_abi_bytes(&abi, &val[j], &len);
                        char *final = (char *)val[j];
                        string decoded;
                        for (int i = 0; i < len; i++)
                        {
                            decoded += final[i];
                        }
                        result.push_back(decoded);
                    }
                    eth_abi_array_end(&abi); // array end
                    arrlen = 0;
                }
                else
                {
                    uint8_t *val;
                    size_t len;
                    int a = eth_abi_bytes(&abi, &val, &len);
                    len = 4;
                    char *final = (char *)val;
                    string decoded;
                    for (int j = 0; j < len; j++)
                    {
                        decoded += final[j];
                    }
                    result.push_back(decoded);
                }
            }
        }
    }

    eth_abi_call_end(&abi);
    ok(eth_abi_free(&abi));
    return result;
}

vector<string> DecodeABIinputs(string func, string data) // beanchmark--for hard work =210 micro-sec
{

    vector<string> result;
    uint64_t arrlen = 0; // for array

    // separating parameters
    vector<string> ParamName;
    vector<string>::iterator it;
    int k = 0;
    bool flag = false;
    ;
    string con;
    for (int i = 0; i < func.length(); i++)
    {
        if (func[i] == '(')
        {
            for (int j = i + 1; j < func.length(); j++)
            {
                if (func[j] == ')')
                {
                    if (k != 0 || flag)
                    {
                        ParamName.push_back(con);
                        con = "";
                        k++;
                    }
                    break;
                }
                else if (func[j] == ',')
                {
                    ParamName.push_back(con);
                    con = "";
                    k++;
                }
                else
                {
                    flag = true;
                    con += func[j];
                }
            }
        }
    }

    // encoding
    struct eth_abi abi;
    // cout << "function is " << func << endl;
    char *fn = strdup(func.c_str()), *hex;
    size_t hexlen;

    char *input = strdup(data.c_str());
    eth_abi_from_hex(&abi, input, data.length());
    eth_abi_call(&abi, &fn, NULL);
    if (k != 0)
    {
        for (int i = 0; i < k; i++)
        {
            if (ParamName[i] == "uint8" || ParamName[i] == "uint8[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, &arrlen); // array beig
                    uint8_t val[arrlen];
                    for (int j = 0; j < arrlen; j++)
                    {
                        eth_abi_uint8(&abi, &val[j]);
                        result.push_back(to_string(val[j]));
                    }
                    eth_abi_array_end(&abi); // array end
                    arrlen = 0;
                }
                else
                {
                    uint8_t val;
                    eth_abi_uint8(&abi, &val);
                    result.push_back(to_string(val));
                }
            }

            if (ParamName[i] == "uint16" || ParamName[i] == "uint16[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, &arrlen); // array beign
                    uint16_t val[arrlen];
                    for (int j = 0; j < arrlen; j++)
                    {

                        eth_abi_uint16(&abi, &val[j]);
                        result.push_back(to_string(val[j]));
                    }
                    eth_abi_array_end(&abi); // array end
                    arrlen = 0;
                }
                else
                {
                    uint16_t val;
                    eth_abi_uint16(&abi, &val);
                    result.push_back(to_string(val));
                }
            }

            if (ParamName[i] == "uint32" || ParamName[i] == "uint32[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, &arrlen); // array beign
                    uint32_t val[arrlen];
                    for (int j = 0; j < arrlen; j++)
                    {
                        eth_abi_uint32(&abi, &val[j]);
                        result.push_back(to_string(val[j]));
                    }
                    eth_abi_array_end(&abi); // array end
                    arrlen = 0;
                }
                else
                {
                    uint32_t val;
                    eth_abi_uint32(&abi, &val);
                    result.push_back(to_string(val));
                }
            }
            if (ParamName[i] == "uint64" || ParamName[i] == "uint64[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, &arrlen); // array beign
                    uint64_t val[arrlen];
                    for (int j = 0; j < arrlen; j++)
                    {
                        eth_abi_uint64(&abi, &val[j]);
                        result.push_back(to_string(val[j]));
                    }
                    eth_abi_array_end(&abi); // array end
                    arrlen = 0;
                }
                else
                {
                    uint64_t val;
                    eth_abi_uint64(&abi, &val);
                    result.push_back(to_string(val));
                }
            }
            if (ParamName[i] == "uint256" || ParamName[i] == "uint256[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;

                if (temp0[templen] == ']')
                {

                    eth_abi_array(&abi, &arrlen); // array beign
                    mpz_t val[arrlen];
                    for (int j = 0; j < arrlen; j++)
                    {

                        mpz_init(val[j]);
                        eth_abi_mpint(&abi, val[j]);
                        char *mpnum;
                        gmp_asprintf(&mpnum, "%Zd", val[j]);
                        string n(mpnum);
                        result.push_back(n);
                        free(mpnum);
                    }
                    eth_abi_array_end(&abi); // array end
                    arrlen = 0;
                }
                else
                {

                    mpz_t val;
                    mpz_init(val);
                    eth_abi_mpint(&abi, val);
                    char *mpnum;
                    gmp_asprintf(&mpnum, "%Zd", val);
                    string n(mpnum);
                    result.push_back(n);
                }
            }
            if (ParamName[i] == "address" || ParamName[i] == "address[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, &arrlen); // array beign
                    char *val[arrlen];
                    for (int j = 0; j < arrlen; j++)
                    {
                        eth_abi_address(&abi, &val[j]);
                        string a(val[j]);
                        result.push_back("0x"+a);
                    }
                    eth_abi_array_end(&abi); // array end
                    arrlen = 0;
                }
                else
                {
                    char *val;
                    eth_abi_address(&abi, &val);
                    string a(val);
                    result.push_back("0x"+a);
                }
            }

            if (ParamName[i] == "bool" || ParamName[i] == "bool[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, &arrlen); // array beign
                    uint8_t val[arrlen];
                    for (int j = 0; j < arrlen; j++)
                    {
                        eth_abi_bool(&abi, &val[j]);
                        result.push_back(to_string(val[j]));
                    }
                    eth_abi_array_end(&abi); // array end
                    arrlen = 0;
                }
                else
                {
                    uint8_t val;
                    eth_abi_bool(&abi, &val);
                    result.push_back(to_string(val));
                }
            }

            if (ParamName[i] == "bytes" || ParamName[i] == "bytes[]")
            {
                string temp0 = ParamName[i];
                int templen = temp0.length() - 1;
                if (temp0[templen] == ']')
                {
                    eth_abi_array(&abi, &arrlen); // array beign

                    uint8_t *val[arrlen];
                    size_t len;
                    for (int j = 0; j < arrlen; j++)
                    {
                        eth_abi_bytes(&abi, &val[j], &len);
                        char *final = (char *)val[j];
                        string decoded;
                        for (int i = 0; i < len; i++)
                        {
                            decoded += final[i];
                        }
                        result.push_back(decoded);
                    }
                    eth_abi_array_end(&abi); // array end
                    arrlen = 0;
                }
                else
                {
                    uint8_t *val;
                    size_t len;
                    int a = eth_abi_bytes(&abi, &val, &len);
                    len = 4;
                    char *final = (char *)val;
                    string decoded;
                    for (int j = 0; j < len; j++)
                    {
                        decoded += final[j];
                    }
                    result.push_back(decoded);
                }
            }
        }
    }

    eth_abi_call_end(&abi);
    ok(eth_abi_free(&abi));
    return result;
}