#include<iostream>
#include "Judgment.h"

#include "2020180001.h"
#include "2020182001.h"
using namespace std;

int main(void)
{
	Judgment judgment;

	judgment.SetYourFunc(
		BlackAttack_2020180001/*����*/,
		BlackDefence_2020180001,
		WhiteAttack_2020182001/*�İ�*/,
		WhiteDefence_2020182001
		);
	judgment.GamePlay();

}