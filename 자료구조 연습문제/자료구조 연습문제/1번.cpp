#include <iostream>
#include<windows.h>

using namespace std;

int main()
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j <= i; ++j)
		{
			Sleep(1000);
			cout << "*";
		}
		cout << endl;
	}
}