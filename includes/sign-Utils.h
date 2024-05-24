#include <stdio.h>
#include <assert.h>
#include <ethc/ecdsa.h>
#include <ethc/rlp.h>
#include <ethc/keccak256.h>
#include <string.h>
#include <string>
#include "eth-rpc.h"
using namespace std;
#define ok(ethcop) assert(ethcop >= 0)
uint8_t getNonce()
{
	ifstream counter("nonce.data",ios::app);
	int txCount=0;
	string data;
	while(counter>>txCount)
	{
		data=to_string(txCount);
	}
  
  data=stringnumtohex(data);
  cout<<data<<endl;
	uint8_t nonce = hex_str_to_uint8(data);
	printf("uint8 content is %d \n",nonce);
	return nonce;
}
string rawTransaction(string inputData, string gasLimit,string gasPrice, string to,string val="0x00", string from="0xb438D71c1E6E8E53637e1F1deC37c2E635e335fc")
{
	struct eth_rlp rlp0, rlp1;
	struct eth_ecdsa_signature sign;
   
	 unsigned char privkey[32] = {
	 	  0xb7, 0x4c, 0x55, 0x8c, 0xbf, 0x1a, 0x15, 0x5d,
	 		0x8a, 0xd1, 0xb9, 0x0a, 0x4a, 0x0e, 0x87, 0x9e,
	 		0x5e, 0x6b, 0x21, 0x77, 0x08, 0x57, 0x01, 0x96,
	 		0x0a, 0xa8, 0x9a, 0xe7, 0xe9, 0xb1, 0x36, 0xed
						 
	 					 }; // real privare key of 0xb438D71c1E6E8E53637e1F1deC37c2E635e335fc
  // uint8_t privkey[] = {0x80, 0x3e, 0x53, 0x83, 0x51, 0x93, 0xdd, 0x6e,
	// 		      0xad, 0x61, 0xa2, 0x5a, 0x74, 0x5f, 0xa6, 0x35,
	// 		      0x86, 0xb4, 0xb2, 0xdd, 0x7b, 0x6a, 0xbe, 0x4a,
	// 		      0x06, 0xfa, 0x7f, 0x33, 0xc1, 0x1a, 0xed, 0x81}; // real privare key of 0xa9d0a7dC416f586491f2fb596731598F937617b5
  uint8_t nonce=getNonce(), zero=0x00, keccak[32], *rlp0bytes, *r, *s;
  char *gasprice = strdup(gasPrice.c_str()), *gaslimit = strdup(gasLimit.c_str()), *value = strdup(val.c_str()), *input=strdup(inputData.c_str());
  char *toaddr = strdup(to.c_str()), *txn;
  uint8_t chainid = 0x38, v;
  size_t rlp0len, rlp1len, siglen=32;

  ok(eth_rlp_init(&rlp0, ETH_RLP_ENCODE));

  ok(eth_rlp_array(&rlp0));                  //[
    ok(eth_rlp_uint8(&rlp0, &nonce));        //  
    ok(eth_rlp_hex(&rlp0, &gasprice, NULL)); //  
    ok(eth_rlp_hex(&rlp0, &gaslimit, NULL)); //   
    ok(eth_rlp_address(&rlp0, &toaddr));     //   
    ok(eth_rlp_hex(&rlp0, &value, NULL));    //   value
    ok(eth_rlp_hex(&rlp0, &input,NULL));     //   abi data
    ok(eth_rlp_uint8(&rlp0, &chainid));     //   0x7a69,
    ok(eth_rlp_uint8(&rlp0, &zero));         //   0x,
    ok(eth_rlp_uint8(&rlp0, &zero));         //   0x,
  ok(eth_rlp_array_end(&rlp0));              // ]

  ok(eth_rlp_to_bytes(&rlp0bytes, &rlp0len, &rlp0));
  ok(eth_rlp_free(&rlp0));

  // compute the keccak hash of the encoded rlp elements
  ok(eth_keccak256(keccak, rlp0bytes, rlp0len));
  free(rlp0bytes);

  // sign the transaction
  ok(eth_ecdsa_sign(&sign, privkey, keccak));

  // calculate v
  v = sign.recid + chainid * 2 + 35;
  r = sign.r;
  s = sign.s;

  ok(eth_rlp_init(&rlp1, ETH_RLP_ENCODE));

  ok(eth_rlp_array(&rlp1));                 //[
    ok(eth_rlp_uint8(&rlp1, &nonce));
    ok(eth_rlp_hex(&rlp1, &gasprice, NULL));
    ok(eth_rlp_hex(&rlp1, &gaslimit, NULL));
    ok(eth_rlp_address(&rlp1, &toaddr));
    ok(eth_rlp_hex(&rlp1, &value, NULL));     //value
    ok(eth_rlp_hex(&rlp1, &input,NULL));      //abi data
    ok(eth_rlp_uint8(&rlp1, &v));
    ok(eth_rlp_bytes(&rlp1, &r, &siglen));
    ok(eth_rlp_bytes(&rlp1, &s, &siglen));
  ok(eth_rlp_array_end(&rlp1));             //]

  ok(eth_rlp_to_hex(&txn, &rlp1));
  ok(eth_rlp_free(&rlp1));
  string raw(txn);
  free(txn);
  return raw;
}

string pythonSigner(string from,string abi,string gasprice,string Gas,string toAddress,string val,string chainid="56")
{
	const string pk = "b74c558cbf1a155d8ad1b90a4a0e879e5e6b2177085701960aa89ae7e9b136ed";
	string From = from;
	string abiData = abi;
	string gasPrice = gasprice;
	string gas = Gas;
	string to = toAddress;
	string value = val;
	string chainID = chainid;
	string HttpProvider = "https://bsc-mainnet.blastapi.io/01af2e8d-fbe7-4c5c-b1e4-86591aaed49f";

	
	string args = "python3 rawtnx.py " + pk + " " + From + " " + to + " " + abiData + " " + gasPrice + " " + chainID + " " + HttpProvider + " " + value + " " + gas;
	string data;
	if (system(args.c_str()) == 0)
	{
		
		ifstream signData("sign.data");
		int k = 0;
		while (signData >> data)
			k++;
	}
	else
	{
		cout << "failed to run the python signer ... exiting \n";
		exit(-3);
	}

	return data;
}