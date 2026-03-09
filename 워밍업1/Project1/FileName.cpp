#include <iostream>
#include <cctype>
#include <algorithm>

using namespace std;

int main() {

	char str[100]{};
	cout << "Enter the sentence:";
	//문장의 길이가 최대 40자인 
	cin.getline(str, 40);

	if (cin.fail()) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	char origin[100]{};
	for (int i = 0; i < 40; ++i)
		origin[i] = str[i];

	bool pressed = false; 
	bool pressed4 = false;

	while (1)
	{
		char command;
		cout << "command" << endl;
		cin >> command; //명령어
		int count = 0;
		switch (command) //여러가지 명령어를 정리
		{
		case '1':
			for (int i = 0; str[i] != '\0'; ++i)
			{
				if (str[i] == ' ')
				{ // 공백 덩어리의 시작을 찾음

					for (int j = i; str[j] != '\0'; j++)
					{
						str[j] = str[j + 1];
					}



					while (str[i] == ' ' && str[i] != '\0') //끝문장이 공백일 때
					{
						++i;
					}
				}

			}
			cout << str << endl;
			break;

		case '2':
			for (int i = 0; i < strlen(origin); ++i)
			{
				if (origin[i] == ' ')
				{
					//  현재 위치의 연속 공백 개수 세기
					int spaceCount = 0;
					int k = i;
					while (origin[k] == ' ' && origin[k] != '\0')
					{
						spaceCount++;
						k++;
					}

					//  공백이 5개 미만일 때만 추가
					if (spaceCount < 5)
					{
						int length = 0;
						while (origin[length] != '\0') ++length;

						for (int k = length; k >= i; --k)
							origin[k + 1] = origin[k];

						origin[i] = ' ';
					}

					//  현재 공백 덩어리 끝까지 i를 이동
					while (origin[i] == ' ' && origin[i] != '\0')
						++i;
				}
			}
			cout << origin << endl;
			break;
		case '3':
		{
			if (!pressed)
			{
				char letters[40]; //알파벳만 parsing
				int cnt = 0;
				for (int i = 0; str[i] != '\0'; ++i)
				{
					if (isalpha(str[i]))
					{
						letters[cnt++] = str[i];
					}
				}



				for (int i = 0; i < cnt - 1; ++i) //count -1 하는 이유가 맨 오른쪽 친구는 비교할 필요가 없음
				{
					for (int j = i + 1; j < cnt; ++j) //j < count 인 이유는 맨 끝까지 검사해야해서
					{
						if ((islower(letters[j]) && isupper(letters[i])) ||   // 대문자가 앞에 있으면 교환
							((islower(letters[i]) == islower(letters[j])) && letters[i] > letters[j])) // 같은 종류면 오름차순
						{
							char temp = letters[i]; //바꿔주는 친구
							letters[i] = letters[j];
							letters[j] = temp;
						}



					}
				}
				int counting = 1;
				for (int i = 0; i < cnt; ++i) 
				{
					if (letters[i] == letters[i + 1])  //알파벳이 같으면
					{
						++counting;  //카운팅이 올라간다
					}
					else
					{
						cout << letters[i] << counting;  //다른 알파벳을 발견하면 알파벳을 출력하고 카운팅도 출력한다
						counting = 1;
					}
				}

				cout << endl;
				pressed = 1;  
			}
			else
			{
				cout << origin << endl; //한번 더 누르면 원래 문장 출력
				pressed = 0;
			}


			break;
		}



		case '4':
		{
			if (!pressed4)
			{
				// 단어 추출
				char words[50][50]; // 단어들을 저장할 2차원 배열
				int wordCount = 0;
				int len = strlen(origin);
				int i = 0;

				while (i < len)
				{
					while (i < len && !isalpha(origin[i])) i++; // 알파벳 아니면 건너뜀
					if (i >= len) break;

					int cnt = 0;
					while (i < len && isalpha(origin[i]))
						words[wordCount][cnt++] = origin[i++]; // 단어 복사
					words[wordCount][cnt] = '\0'; // 문자열 끝 표시
					wordCount++;
				}

				// 단어 길이 기준 오름차순 버블 정렬
				for (int a = 0; a < wordCount - 1; ++a)
				{
					for (int b = a + 1; b < wordCount; ++b)
					{
						if (strlen(words[a]) > strlen(words[b])) // 앞 단어가 더 길면
						{	
							char temp[50];
							for (int k = 0; k < 50; ++k)
							{
								temp[k] = words[a][k];      // temp에 a 저장
								words[a][k] = words[b][k];  // a에 b 저장
								words[b][k] = temp[k];      // b에 temp 저장
							}
						}
					}
				}

				// 출력 (공백 1개로 고정)
				for (int a = 0; a < wordCount; ++a)
				{
					cout << words[a];
					if (a < wordCount - 1) cout << " ";
				}
				cout << endl;
				pressed4 = true;
			}
			else
			{
				cout << origin << endl; // 다시 누르면 원래 문장
				pressed4 = false;
			}
			break;
		}
		

		case '0':
			return 0;
			break;

		default:
			if (tolower(command) >= 'a' && tolower(command) <= 'z')
			{
				for (int i = 0; i < 40; ++i)
				{ //0부터 40까지 쭉 훑으면서 command를 찾아줌

					if (tolower(command) == tolower(str[i])) { //소문자를 넣든 대문자를 넣든 같은 조건에서 실행하기 위해 toupper를 씌움
						if (isupper(str[i])) //대문자이면
							str[i] = tolower(str[i]); //소문자로
						else
							str[i] = toupper(str[i]); //아님 대문자
					}

				}
			}
			cout << str << endl;
			break;
		}
	}
}