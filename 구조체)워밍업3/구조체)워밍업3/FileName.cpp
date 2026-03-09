#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>

using namespace std;


struct reservation //구조체 정의
{
	string movie; //영화제목
	string time; //영화시간
	int seat1, seat2; //영화좌석 (행, 열)


	int fseat1, fseat2; //좌석 인덱스용 (-1할거임)
	bool reserved = false;  // 예매 했는지 여부
	int number; //예매번호
	int seatCount;
};
struct Movie
{
	string name;           // 영화 이름
	string times[3];       // 상영 시간 3개
	int board[3][10][10];  // [시간][행][열] 좌석 배열
	// board[0] = 첫 번째 시간 좌석
	// board[1] = 두 번째 시간 좌석
};
Movie movies[3];  // 영화 3개


int getMovieIndex(string input) //영화 인덱스 찾기
{
	if (input == "1" || input == "Jaws")        return 0;
	if (input == "2" || input == "Kingkong")    return 1;
	if (input == "3" || input == "Transformer") return 2;
	return -1;
}
int getTimeIndex(int mindex, string time) //시간 인덱스 찾기

{
	for (int t = 0; t < 3; t++)
		if (movies[mindex].times[t] == time)
			return t;
	return -1;
}


void board(int mindex, int tindex, reservation res[], int resCount) //보드출력함수
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //커서가 가리키는
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{

			if (movies[mindex].board[tindex][i][j] == 1)
			{
				int num = -1;
				for (int k = 0; k < resCount; k++)
				{
					if (res[k].reserved &&
						res[k].fseat1 == j &&  // j = 열
						res[k].fseat2 == i &&  // i = 행
						getMovieIndex(res[k].movie) == mindex &&
						getTimeIndex(mindex, res[k].time) == tindex)
					{
						num = res[k].number;
						break;
					}
				}
				SetConsoleTextAttribute(hConsole, 7); // 예약한 자리 표시
				if (num != -1)
					cout << num << "  "; // 예매번호 출력
				else
					cout << "O  ";
			}
			else
			{
				SetConsoleTextAttribute(hConsole, 7);  // 흰색(기본)
				cout << "-" << " ";
			}

		}

		cout << endl;
	}
}
void movieinfo(int mindex, reservation res[], int resCount)
{
	for (int t = 0; t < 3; t++)
	{
		cout << movies[mindex].name << " " << movies[mindex].times[t] << endl;
		board(mindex, t, res, resCount); // res, resCount 넘겨주기
	}
}


int main()
{
	reservation res[100];   // 예약 최대 100개
	int resCount = 0;       // 현재 예약 수


	//  영화 정보 초기화
	movies[0].name = "Jaws";
	movies[0].times[0] = "1100";
	movies[0].times[1] = "1400";
	movies[0].times[2] = "1600";

	movies[1].name = "Kingkong";
	movies[1].times[0] = "1000";
	movies[1].times[1] = "1500";
	movies[1].times[2] = "1800";

	movies[2].name = "Transformer";
	movies[2].times[0] = "0900";
	movies[2].times[1] = "1900";
	movies[2].times[2] = "2000";

	//  좌석 전부 0으로 초기화
	for (int m = 0; m < 3; m++)
		for (int t = 0; t < 3; t++)
			for (int i = 0; i < 10; i++)
				for (int j = 0; j < 10; j++)
					movies[m].board[t][i][j] = 0;

	while (true)
	{
		char command;
		cout << "\ncommand: ";
		cin >> command;
		switch (command)
		{
		case 'd': //영화목록
		{
			//jaws
			cout << "1" << " ";
			cout << "Jaws" << " ";
			cout << "1100 1400 1600" << endl;
			//kingkong
			cout << "2" << " ";
			cout << "Kingkong" << " ";
			cout << "1000 1500 1800" << endl;
			//transformer
			cout << "3" << " ";
			cout << "Transformer" << " ";
			cout << "0900 1900 2000" << endl;
			break;
		}
		case 'p': //영화관 정보
		{
			string next;
			cin >> next;
			int mindex = getMovieIndex(next);
			if (mindex == -1) { cout << "없는 영화입니다." << endl; break; }
			movieinfo(mindex, res, resCount); // movies 배열 기반으로 출력 (예매된 좌석 반영)
			break;
		}
		case 'r':
		{
			int seatCount;
			cout << "how many seats? (1~4): ";
			cin >> seatCount;
			if (seatCount < 1 || seatCount > 4) { cout << "1~4 사이로 입력하세요." << endl; break; }

			if (resCount >= 100) { cout << "full reservation." << endl; break; }

			cout << "Which movie: ";
			cin >> res[resCount].movie;

			int mindex = getMovieIndex(res[resCount].movie);
			if (mindex == -1) { cout << "no movie." << endl; break; }

			cout << "What time: ";
			cin >> res[resCount].time;

			int tindex = getTimeIndex(mindex, res[resCount].time);
			if (tindex == -1) { cout << "no movie." << endl; break; }

			cout << "Seat you want : ";
			cin >> res[resCount].seat1 >> res[resCount].seat2;

			res[resCount].fseat1 = res[resCount].seat2 - 1;  // fseat1 = 열 인덱스 (seat2)
			res[resCount].fseat2 = res[resCount].seat1 - 1;  // fseat2 = 행 인덱스 (seat1)

			// 이미 예매된 좌석인지 확인
			for (int s = 0; s < seatCount; s++)
			{
				if (movies[mindex].board[tindex][res[resCount].fseat2][res[resCount].fseat1 + s] == 1)
				{
					cout << res[resCount].seat1 << res[resCount].seat2 + s << "already reservation." << endl;
					break;
				}
			}

			// 좌석 예매 처리
			for (int s = 0; s < seatCount; s++)
				movies[mindex].board[tindex][res[resCount].fseat2][res[resCount].fseat1 + s] = 1;
			res[resCount].seatCount = seatCount;
			res[resCount].reserved = true;
			res[resCount].number = resCount + 1; // 예매번호 = 순서

			cout << "reservation" << endl;
			cout << "Movie name: " << res[resCount].movie << endl;
			cout << "Time: " << res[resCount].time << endl;
			cout << "Seat number: " << res[resCount].seat1 << " " << res[resCount].seat2 << endl;
			cout << "reservation number: " << res[resCount].number << endl;

			resCount++; // 예약 수 증가
			break;
		}
		case 'c':
		{
			cout << "reservation number: ";
			int inputNum;
			cin >> inputNum;

			bool found = false;
			for (int k = 0; k < resCount; k++)
			{
				if (res[k].number == inputNum && res[k].reserved)
				{
					cout << "reservation" << endl;
					cout << "Movie name: " << res[k].movie << endl;
					cout << "Time: " << res[k].time << endl;
					cout << "Seat number: " << res[k].seat1 << res[k].seat2 << endl;

					// 좌석 복구
					int mindex = getMovieIndex(res[k].movie);
					int tindex = getTimeIndex(mindex, res[k].time);
					movies[mindex].board[tindex][res[k].fseat2][res[k].fseat1] = 0;

					res[k].reserved = false;
					cout << "Reservation is cancelled." << endl;
					found = true;
					break;
				}
			}
			if (!found) cout << "not found reservation number." << endl;
			break;
		}
		case 'q':
		{
			return 0;
		}
		case 'h':
		{

			for (int m = 0; m < 3; m++)
			{
				for (int t = 0; t < 3; t++)
				{
					int count = 0;
					for (int i = 0; i < 10; i++)
						for (int j = 0; j < 10; j++)
							if (movies[m].board[t][i][j] == 1)
								count++;
					cout << movies[m].name << " " << movies[m].times[t];
					cout << "  " << count << "/100 (" << count << "%)" << endl;
				}
			}
			break;
		}
		}
	}

}

