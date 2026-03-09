#include <iostream>
#include <cctype>
#include <algorithm>

using namespace std;

char sentence[100];
char letters
int main()
{
	for (int i = 0; sentence[i] != '\0'; ++i) //문장검사
	{
		if (isalpha(sentence[i])) //isalpha는 알파벳을 확인하는 함수
		{
			letters[count++] = sentence[i];
		}
	}

	for (int i = 0; i < count - 1; ++i)
	{
		for (int j = i + 1; j < count; ++j)
		{
			if (letters[i] > letters[j])
			{
				char temp = letters[i];
				letters[i] = letters[j]; // 이 세줄이 자리바꾸기
				letters[j] = temp;
			}
		}
	}

	for (int i = 0; i < count; ++i)
	{
		printf("%c%d", letters[i], i + 1);
	}
}