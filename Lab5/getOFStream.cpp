#include <fstream>
using namespace std;

ofstream getOFStream(char* fileName)
{
	ofstream fout(fileName, ios::binary);
	return fout;
}