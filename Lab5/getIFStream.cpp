#include <fstream>
using namespace std;

ifstream getIFStream(char* fileName)
{
	ifstream fin(fileName, ios::binary);
	return fin;
}