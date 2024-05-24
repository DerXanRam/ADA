#include "eth-rpc.h"
#include <iostream>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ethc/abi.h>
#include <ethc/unit.h>
#include <ethc/hex.h>
#include <ethc/ecdsa.h>
#include <ethc/rlp.h>
#include <ethc/keccak256.h>
#include <gmp.h>

#define ok(ethcop) assert(ethcop >= 0)
using namespace std;
using json = nlohmann::json;
string UniswapFactoryAddress = "0x13f4EA83D0bd40E75C8222255bc855a974568Dd4";
string UniswapexchangeAddress = "0x1b02dA8Cb0d097eB8D57A175b88c7D8b47997506";

using namespace std;

class rpc_uniswap
{
public:
	void abiDecoder(string data)
	{
		char *input = strdup(data.c_str());
		char *fn = "test(uint256,uint256,address[],address,uint256)";
		mpz_t num0, num1, num2, num3;
		mpz_init(num0);
		mpz_init(num1);
		mpz_init(num2);
		uint64_t arrlen = 0;
		char *add1;
		// decoding
		struct eth_abi abi;
		eth_abi_from_hex(&abi, input, data.length());
		eth_abi_call(&abi, &fn, NULL);
		eth_abi_mpint(&abi, num0);
		eth_abi_mpint(&abi, num1);
		eth_abi_array(&abi, &arrlen);

		char *add[arrlen];
		for (int i = 0; i < arrlen; i++)
			eth_abi_address(&abi, &add[i]);

		eth_abi_array_end(&abi);
		eth_abi_address(&abi, &add1);
		eth_abi_mpint(&abi, num2);
		eth_abi_call_end(&abi);
		ok(eth_abi_free(&abi));

		// char *final = (char *)bytes;
		gmp_printf("%s %Zd\n", "num 1", num0);
		gmp_printf("%s %Zd\n", "num 2", num1);
		for (int i = 0; i < arrlen; i++)
			cout << "address " << i + 1 << " " << add[i];
		cout << "address " << add1 << endl;
		gmp_printf("%s %Zd\n", "num 4", num2);
	}

	string balanceOf(string re1, string re2, string add1, string add2, string add3, string re3)
	{
		char *a = strdup(add1.c_str()), *b = strdup(add2.c_str()), *c = strdup(add3.c_str());
		mpz_t r1, r2, r3;
		mpz_init_set_str(r1, re1.c_str(), 0);
		mpz_init_set_str(r2, re2.c_str(), 0);
		mpz_init_set_str(r3, re3.c_str(), 0);

		struct eth_abi abi;
		string function = "test(uint256[],address[],address,uint256)";
		char *fn = strdup(function.c_str()), *hex;
		size_t hexlen;
		uint64_t arr = 2;
		// abi encoding
		eth_abi_init(&abi, ETH_ABI_ENCODE);
		eth_abi_call(&abi, &fn, NULL);
		eth_abi_array(&abi, NULL);
		eth_abi_mpint(&abi, r1);
		eth_abi_mpint(&abi, r2);
		eth_abi_array_end(&abi);
		eth_abi_array(&abi, NULL);
		eth_abi_address(&abi, &a);
		eth_abi_address(&abi, &b);
		eth_abi_array_end(&abi);
		eth_abi_address(&abi, &c);
		eth_abi_mpint(&abi, r3);
		eth_abi_call_end(&abi);
		ok(eth_abi_to_hex(&abi, &hex, &hexlen));
		ok(eth_abi_free(&abi));
		string f(hex);
		return f;
	}
	string *getReserves(string addressToCheckBalance)
	{
		static string fin[2]{"x", "x"};
		char *a = strdup(addressToCheckBalance.c_str());
		// abi encode
		struct eth_abi abi;
		string function = "getReserves()";
		char *fn = strdup(function.c_str()), *hex;
		size_t hexlen;
		// abi encoding
		eth_abi_init(&abi, ETH_ABI_ENCODE);
		eth_abi_call(&abi, &fn, NULL);
		eth_abi_call_end(&abi);
		ok(eth_abi_to_hex(&abi, &hex, &hexlen));
		ok(eth_abi_free(&abi));

		json response = eth_call(hex, addressToCheckBalance); /*sending rpc call*/
		string result;
		// cout << "dump " << response.dump() << endl;
		if (response.contains("result"))
			result = response["result"];
		else
		{
			cout << response.dump() << endl;
			exit(-1);
		}

		char *data = strdup(result.c_str());

		// decoding
		struct eth_abi abi1;
		mpz_t r1, r2, r3;
		mpz_init(r1);
		mpz_init(r2);
		mpz_init(r3);

		eth_abi_from_hex(&abi1, data, -1);
		eth_abi_mpint(&abi1, r1);
		eth_abi_mpint(&abi1, r2);
		eth_abi_mpint(&abi1, r3);
		eth_abi_free(&abi1);

		char *temp0, *temp1;
		gmp_asprintf(&temp0, "%Zd", r1);
		gmp_asprintf(&temp1, "%Zd", r2);
		string t1 = temp0;
		string t2 = temp1;
		free(temp0);
		free(temp1);
		string dec0(t1);
		string dec1(t2);
		// decodedRes+=" "+decode0;
		fin[0] = dec0;
		fin[1] = dec1;
		// cout<<"fins are "<<fin[0]<<" "<<fin[1]<<endl;
		return fin;
	}
	string Symbol(string dat)
	{
		char *data = strdup(dat.c_str());
		// decoding
		struct eth_abi abi1;
		uint8_t *add;
		size_t a; // result.length();
		eth_abi_from_hex(&abi1, data, -1);
		eth_abi_bytes(&abi1, &add, &a);
		eth_abi_free(&abi1);

		char *final = (char *)add;

		string decodedres;
		for (int i = 0; i < a; i++)
		{
			decodedres += final[i];
		}
		return decodedres;
	}

	string quote(uint64_t amountA, uint64_t reserveB, uint64_t reserveC, string exchangeAddress = UniswapexchangeAddress) // default is uniswap v2 address
	{
		struct eth_abi abi;
		string function = "quote(uint256,uint256,uint256)";
		char *fn = strdup(function.c_str()), *hex;
		size_t hexlen;
		eth_abi_init(&abi, ETH_ABI_ENCODE);
		eth_abi_call(&abi, &fn, NULL);
		eth_abi_uint64(&abi, &amountA);
		eth_abi_uint64(&abi, &reserveB);
		eth_abi_uint64(&abi, &reserveC);
		eth_abi_call_end(&abi);
		ok(eth_abi_to_hex(&abi, &hex, &hexlen));
		ok(eth_abi_free(&abi));

		json response = eth_call(hex, exchangeAddress); /*end of abi encoding*/

		string result = response.dump(); /*sending rpc call*/

		if (response.contains("result"))
			result = response["result"];
		else
			result = response.dump();
		return result;
	}
	string getAmountOut(string amountA, string reserveIn, string reserveOut, string exchangeAddress = UniswapexchangeAddress) // default is uniswap v2 address
	{

		char *a = strdup(reserveIn.c_str());
		char *b = strdup(reserveOut.c_str());
		char *c = strdup(amountA.c_str());

		mpz_t reA, reB, amt;
		mpz_init_set_str(reA, a, 0);
		mpz_init_set_str(reB, b, 0);
		mpz_init_set_str(amt, c, 0);

		struct eth_abi abi;
		string function = "getAmountOut(uint256,uint256,uint256)";
		char *fn = strdup(function.c_str()), *hex;
		size_t hexlen;
		eth_abi_init(&abi, ETH_ABI_ENCODE);
		eth_abi_call(&abi, &fn, NULL);
		eth_abi_mpint(&abi, amt);
		eth_abi_mpint(&abi, reA);
		eth_abi_mpint(&abi, reB);
		eth_abi_call_end(&abi);
		ok(eth_abi_to_hex(&abi, &hex, &hexlen));
		ok(eth_abi_free(&abi));
		json response = eth_call(hex, exchangeAddress); /*end of abi encoding*/

		string result = response.dump(); /*sending rpc call*/
										 // cout << response.dump() << endl;
		if (response.contains("result"))
			result = response["result"];
		else
		{
			cout << response.dump() << endl;
			exit(-1);
		}

		/*copying ABI result to data*/
		char *data = strdup(result.c_str());
		// decoding
		struct eth_abi abi1;
		mpz_t r1;
		mpz_init(r1);

		uint64_t arrSize;
		eth_abi_from_hex(&abi1, data, -1);
		eth_abi_mpint(&abi1, r1);
		eth_abi_free(&abi1);
		// gmp_printf("%Zd %s\n", r1, "test");

		char *temp;
		gmp_asprintf(&temp, "%Zd", r1);
		string t3 = temp;
		free(temp);

		string decodedRes(t3);
		return decodedRes;
	}
	string getAmountsOut(string amountIn, string pathA, string pathB, string exchangeAddress = UniswapexchangeAddress)
	{
		char *a, *b, *c;
		a = strdup(pathA.c_str());
		b = strdup(pathB.c_str());
		c = strdup(amountIn.c_str());
		mpz_t amt;
		mpz_init_set_str(amt, c, 0);
		/*cout<<"amt is ";
		gmp_printf("%Zd\n", amt);
		cout<<endl;*/
		struct eth_abi abi;
		string function = "getAmountsOut(uint256,address[])";
		char *fn = strdup(function.c_str()), *hex;
		uint64_t arraySize = 2;
		size_t hexlen;
		eth_abi_init(&abi, ETH_ABI_ENCODE);
		eth_abi_call(&abi, &fn, NULL);
		eth_abi_mpint(&abi, amt);
		eth_abi_array(&abi, &arraySize);
		eth_abi_address(&abi, &a);
		eth_abi_address(&abi, &b);
		eth_abi_array_end(&abi);
		eth_abi_call_end(&abi);

		ok(eth_abi_to_hex(&abi, &hex, &hexlen));
		ok(eth_abi_free(&abi));
		cout << hex << endl;
		return "hi";
		/*end of abi encoding*/
		// cout<<"ABI is "<<hex<<endl;
		json response = eth_call(hex, exchangeAddress); /*sending rpc call*/

		string result;
		// cout << "dump " << response.dump() << endl;
		if (response.contains("result"))
			result = response["result"];
		else
		{
			cout << response.dump() << endl;
			exit(-1);
		}

		/*copying ABI result to data*/
		char *data = strdup(result.c_str());
		// decoding
		struct eth_abi abi1;
		mpz_t r1, r2;
		mpz_init(r1);
		mpz_init(r2);

		uint64_t arrSize = 2;
		eth_abi_from_hex(&abi1, data, -1);
		eth_abi_array(&abi1, &arrSize);
		eth_abi_mpint(&abi1, r1);
		eth_abi_mpint(&abi1, r2);
		eth_abi_array_end(&abi1);
		eth_abi_free(&abi1);
		// gmp_printf("%Zd %s\n", r1, "test");

		char *temp;
		gmp_asprintf(&temp, "%Zd", r2);
		string t3 = temp;
		free(temp);

		string decodedRes(t3);
		// return decodedRes;
	}
	string Decimals(string addressToCheckDecimal)
	{
		// abi encode
		struct eth_abi abi;
		string function = "decimals()";
		char *fn = strdup(function.c_str()), *hex;
		size_t hexlen;
		// abi encoding
		eth_abi_init(&abi, ETH_ABI_ENCODE);
		eth_abi_call(&abi, &fn, NULL);
		eth_abi_call_end(&abi);
		ok(eth_abi_to_hex(&abi, &hex, &hexlen));
		ok(eth_abi_free(&abi));
		// cout<<"address passed to rpc "<<addressToCheckSymb<<endl;
		//  rpc call
		json response = eth_call(hex, addressToCheckDecimal);
		string result; /*sending rpc call*/
		// cout << "dump " << response.dump() << endl;
		if (response.contains("result"))
			result = response["result"];
		else
		{
			cout << response.dump() << endl;
			exit(-1);
		}

		if (result == "0x")
			result += "000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

		/*copying ABI result to data*/
		char *data = strdup(result.c_str());

		// decoding
		struct eth_abi abi1;
		mpz_t add;
		mpz_init(add);

		size_t a; // result.length();
		eth_abi_from_hex(&abi1, data, -1);
		eth_abi_mpint(&abi1, add);
		eth_abi_free(&abi1);
		char *temp;
		gmp_asprintf(&temp, "%Zd", add);
		string t3 = temp;

		string decoded(temp);
		// cout << "decoded symbol is " << decodedres << endl;
		return decoded;
	}
};

class rpc_uniswap_factory
{
public:
	string allPairs(uint64_t index, string contractAddress = UniswapFactoryAddress) // returns 'n'th index of pair list in uniswap v2
	{
		// abi encode
		struct eth_abi abi;
		string function = "allPairs(uint256)";
		char *fn = strdup(function.c_str()), *hex;
		size_t hexlen;
		eth_abi_init(&abi, ETH_ABI_ENCODE);
		eth_abi_call(&abi, &fn, NULL);
		eth_abi_uint64(&abi, &index);
		eth_abi_call_end(&abi);
		ok(eth_abi_to_hex(&abi, &hex, &hexlen));
		ok(eth_abi_free(&abi));

		// rpc call
		json response = eth_call(hex, contractAddress);
		string result = response.dump(); /*sending rpc call*/
		if (response.contains("result"))
			result = response["result"];
		else
		{
			cout << response.dump() << endl;
			exit(-1);
		}

		// cout<<response.dump()<<endl;
		/*copying ABI result to data*/
		char *data = strdup(result.c_str());

		// decoding
		struct eth_abi abi1;
		char *add;
		eth_abi_from_hex(&abi1, data, -1);
		eth_abi_address(&abi1, &add);
		eth_abi_free(&abi1);
		string temp = add;
		string decodedResult = "0x";
		decodedResult += temp;
		return decodedResult;
	}
	string getPair(string address0, string address1, string contractAddress = UniswapFactoryAddress) // returns 'n'th index of pair list in uniswap v2
	{
		char *a = strdup(address0.c_str());
		char *b = strdup(address1.c_str());
		// abi encode
		struct eth_abi abi;
		string function = "getPair(address,address)";
		char *fn = strdup(function.c_str()), *hex;
		size_t hexlen;
		eth_abi_init(&abi, ETH_ABI_ENCODE);
		eth_abi_call(&abi, &fn, NULL);
		eth_abi_address(&abi, &a);
		eth_abi_address(&abi, &b);
		eth_abi_call_end(&abi);
		ok(eth_abi_to_hex(&abi, &hex, &hexlen));
		ok(eth_abi_free(&abi));

		// rpc call
		json response = eth_call(hex, contractAddress);
		string result = response.dump(); /*sending rpc call*/
		if (response.contains("result"))
			result = response["result"];
		else
		{
			cout << response.dump() << endl;
			// exit(-1);
		}

		// cout<<response.dump()<<endl;
		/*copying ABI result to data*/
		char *data = strdup(result.c_str());

		// decoding
		struct eth_abi abi1;
		char *add;
		eth_abi_from_hex(&abi1, data, -1);
		eth_abi_address(&abi1, &add);
		eth_abi_free(&abi1);
		string temp = add;
		string decodedResult = "0x";
		decodedResult += temp;
		return decodedResult;
	}

	string allPairsLength(string factoryAddress = UniswapFactoryAddress)
	{
		json response = eth_call("574f2ba3", factoryAddress); // 0902f1ac is encoded abi form of allPairsLength()

		string result;

		if (response.contains("result"))
			result = response["result"];
		else
		{
			cout << response.dump() << endl;
			exit(-1);
		}

		char *data = strdup(result.c_str());

		// decoding
		struct eth_abi abi1;
		mpz_t r1;
		mpz_init(r1);

		eth_abi_from_hex(&abi1, data, -1);
		eth_abi_mpint(&abi1, r1);
		eth_abi_free(&abi1);

		char *temp0;
		gmp_asprintf(&temp0, "%Zd", r1);
		string t1 = temp0;
		free(temp0);
		string dec0(t1);
		return dec0;
	}

	string Token0(string addressToCheckSymb)
	{
		// abi encode
		struct eth_abi abi;
		string function = "token0()";
		char *fn = strdup(function.c_str()), *hex;
		size_t hexlen;
		eth_abi_init(&abi, ETH_ABI_ENCODE);
		eth_abi_call(&abi, &fn, NULL);
		eth_abi_call_end(&abi);
		ok(eth_abi_to_hex(&abi, &hex, &hexlen));
		ok(eth_abi_free(&abi));

		// rpc call
		json response = eth_call(hex, addressToCheckSymb);
		string result; /*sending rpc call*/
		// cout<<"dump "<<response.dump()<<endl;
		if (response.contains("result"))
			result = response["result"];
		else
		{
			cout << response.dump() << endl;
			exit(-1);
		}

		/*copying ABI result to data*/
		char *data = strdup(result.c_str());

		// decoding
		struct eth_abi abi1;
		char *add;
		size_t a = 10;
		eth_abi_from_hex(&abi1, data, -1);
		eth_abi_address(&abi1, &add);
		eth_abi_free(&abi1);
		string temp = add;
		string decodedRes = "0x";
		decodedRes += temp;
		return decodedRes;
	}
	string Token1(string addressToCheckSymb)
	{
		// abi encode
		struct eth_abi abi;
		string function = "token1()";
		char *fn = strdup(function.c_str()), *hex;
		size_t hexlen;
		eth_abi_init(&abi, ETH_ABI_ENCODE);
		eth_abi_call(&abi, &fn, NULL);
		eth_abi_call_end(&abi);
		ok(eth_abi_to_hex(&abi, &hex, &hexlen));
		ok(eth_abi_free(&abi));

		// rpc call
		json response = eth_call(hex, addressToCheckSymb);
		string result; /*sending rpc call*/
		// cout << "dump " << response.dump() << endl;
		if (response.contains("result"))
			result = response["result"];
		else
		{
			cout << response.dump() << endl;
			exit(-1);
		}

		/*copying ABI result to data*/
		char *data = strdup(result.c_str());

		// decoding
		struct eth_abi abi1;
		char *add;
		size_t a = 10;
		eth_abi_from_hex(&abi1, data, -1);
		eth_abi_address(&abi1, &add);
		eth_abi_free(&abi1);
		string temp = add;
		string decodedRes = "0x";
		decodedRes += temp;
		return decodedRes;
	}
};

string rawTransaction(string inputData, string gasLimit,string gasPrice, string to)
{
	struct eth_rlp rlp0, rlp1;
	struct eth_ecdsa_signature sign;
	uint8_t privkey[] = {0x80, 0x3e, 0x53, 0x83, 0x51, 0x93, 0xdd, 0x6e,
			      0xad, 0x61, 0xa2, 0x5a, 0x74, 0x5f, 0xa6, 0x35,
			      0x86, 0xb4, 0xb2, 0xdd, 0x7b, 0x6a, 0xbe, 0x4a,
			      0x06, 0xfa, 0x7f, 0x33, 0xc1, 0x1a, 0xed, 0x81}; // real privare key of 0xa9d0a7dC416f586491f2fb596731598F937617b5
	uint16_t chainid = 0x38; // 56 BSC
	char *gasprice=strdup(gasPrice.c_str());
	char *gaslimit=strdup(gasLimit.c_str());
	char *toaddress=strdup(to.c_str());
	char *input=strdup(inputData.c_str());
	char *tnx;
	uint8_t v;
	uint8_t nonce = 0x00, zero = 0x00, keccak[32], *rlp0bytes, *r, *s;
	size_t rlp0len, rlp1len, siglen = 32;
	
	ok(eth_rlp_init(&rlp0, ETH_RLP_ENCODE));
	ok(eth_rlp_array(&rlp0));
	ok(eth_rlp_uint8(&rlp0, &nonce));
	ok(eth_rlp_hex(&rlp0, &gasprice, NULL));
	ok(eth_rlp_hex(&rlp0, &gaslimit, NULL));
	ok(eth_rlp_address(&rlp0, &toaddress));
	ok(eth_rlp_uint8(&rlp0, &zero));//this is value
	ok(eth_rlp_hex(&rlp0, &input, NULL));
	ok(eth_rlp_uint16(&rlp0, &chainid));
	ok(eth_rlp_uint8(&rlp0, &zero)); //   0x,
	ok(eth_rlp_uint8(&rlp0, &zero)); //   0x,
	ok(eth_rlp_array_end(&rlp0));	 // ]

	ok(eth_rlp_to_bytes(&rlp0bytes, &rlp0len, &rlp0));
	ok(eth_rlp_free(&rlp0));

	// compute the keccak hash of the encoded rlp elements
	ok(eth_keccak256p(keccak, rlp0bytes, rlp0len));
	free(rlp0bytes);

	// sign the transaction
	ok(eth_ecdsa_sign(&sign, privkey, keccak));

	// calculate v
	v = sign.recid + chainid * 2 + 35;
	r = sign.r;
	s = sign.s;

	ok(eth_rlp_init(&rlp1, ETH_RLP_ENCODE));
	ok(eth_rlp_array(&rlp1));
	ok(eth_rlp_uint8(&rlp1, &nonce));
	ok(eth_rlp_hex(&rlp1, &gasprice, NULL));
	ok(eth_rlp_hex(&rlp1, &gaslimit, NULL));
	ok(eth_rlp_address(&rlp1, &toaddress));
	ok(eth_rlp_uint8(&rlp1, &zero));//this is value
	ok(eth_rlp_hex(&rlp1, &input, NULL)); 
	ok(eth_rlp_uint8(&rlp1, &v));
	ok(eth_rlp_bytes(&rlp1, &r, &siglen));
	ok(eth_rlp_bytes(&rlp1, &s, &siglen));
	ok(eth_rlp_array_end(&rlp1));

	// FIX3: this actually returns the output length
	ok(eth_rlp_to_hex(&tnx, &rlp1) > 0);
	ok(eth_rlp_free(&rlp1));
}