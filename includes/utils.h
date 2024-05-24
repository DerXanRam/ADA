#include <sstream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <math.h>
#include <string>
#include <map>

using namespace std;
using namespace std::chrono;

string getTimestamp()
{
	long long ms_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	return to_string(ms_since_epoch);
}

string dref(string *str, int index)
{
	string value = str[index];
	return value;
}
long double bstod(string a)
{
	stringstream s(a);
	long double ret = 0;
	s >> ret;
	return ret;
}
string hex2dec(string hex)
{
	string::size_type sz = 0;
	return to_string(stoll(hex, &sz, 16));
}
string dec2hex(long long hex)
{
	auto temp = (std::stringstream{} << std::hex << hex).str();
	string res = "0x";
	res += temp;
	return res;
}
string stringnumtohex(string number)
{
    string digits = "0123456789abcdef"; // Decimal to Hexadecimal function
    long length = number.length();
    string result = "";
    vector<long> nibbles;
    for (long i = 0; i < length; i++)
    {
        nibbles.push_back(digits.find(number[i]));
    }
    long newlen = 0;
    do
    {
        long value = 0;
        newlen = 0;
        for (long i = 0; i < length; i++)
        {
            value = (value * 10) + nibbles[i];
            if (value >= 16)
            {
                nibbles[newlen++] = value / 16;
                value %= 16;
            }
            else if (newlen > 0)
            {
                nibbles[newlen++] = 0;
            };
        };
        length = newlen;
        result = digits[value] + result;
    } while (newlen != 0);
    return result;
}
uint8_t hex_str_to_uint8(string hexForm) 
{
	uint8_t result;
	unsigned int x = std::stoul(hexForm, nullptr, 16);
	result=(uint8_t)x;
    return result;
}
string GetgasLimit(string data)
{
	int sizeInBytes = data.size();
	long long limit = 21000 + (68 * sizeInBytes);
	return to_string(limit);
}
string gasfee(string gasLimit, string gasPrice) // more fix here
{
	double gwei = bstod(gasPrice) / 1000000000;
	string gasFee=to_string(gwei *bstod(gasLimit));
	return gasFee;
}

void logger(string data, string filename, string separator = " ")
{
	ofstream log(filename, ios::app);
	if (!log)
	{
		cout << "open file failed \n";
		exit(-2);
	}
	/*for time log*/
	time_t t;
	struct tm *tt;
	/*-------------*/
	time(&t);
	tt = localtime(&t);
	string time = asctime(tt);
	time[time.length() - 1] = ' ';
	data[data.length()] = ' ';
	string temp = time + separator + data + "\n";
	log << temp;
	log.close();
}
string DeciRemover(string number)
{
	int size = number.size();
	string Formated;
	for (int i = 0; i < size; i++)
	{
		if (number[i] != '.')
		{
			Formated += number[i];
		}
		else
		{
			break;
		}
	}
	return Formated;
}
