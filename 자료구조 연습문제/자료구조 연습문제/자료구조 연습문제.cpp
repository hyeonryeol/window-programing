#include <iostream>
#include <windows.h>

using namespace std;

int num = 1;
void printStars(int k)
{
	if (num > k)
	{
		return;
	}
	for (int i = 0; i < num; ++i)
	{
		cout << "*";
		Sleep(1000);
	}
	cout << endl;
	++num;
	printStars(k);
}


int main()
{
	cout << "출력할 줄 수를 입력하시오.: ";
	int a;
	cin >> a;



	printStars(a);
	cout << endl;


}