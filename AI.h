/*
 *	Copyright		:	open
 *	Author			:	YihaiDeng
 *	Date			:	2016-10-14
 *	Description		:	
 */

#pragma once
#include <memory>

enum qiziKind
{
	QIZI_NONE = 0,
	QIZI_WHITE = 1,
	QIZI_BLACK = 2,
	QIZI_BLOCK = 3
};


namespace AI {
	class wuziqiAI;
	std::shared_ptr<wuziqiAI> createWuziqiAI();
}

class AI::wuziqiAI
{
	using type_qipanstate = std::vector<std::vector<qiziKind>>;
public:
	~wuziqiAI();
	wuziqiPosQiPan takeNextStep(type_qipanstate qipanState, qiziKind currentKind);

private:
	int scoringIndex(type_qipanstate qipanState, int x, int y);
	std::vector<int> computeIncome(type_qipanstate qipanState);
	int checkStateScoring(type_qipanstate qiziArray);
	std::pair<std::pair<int, int>, int> 
		findAtGameTree(type_qipanstate qipanState, 
			qiziKind currentKind, wuziqiPosQiPan newPos = wuziqiPosQiPan(-1, -1), int deep = 0);
	bool checkGameOver(type_qipanstate qipanState, wuziqiPosQiPan newPos);

private:
	qiziKind _AIqiziKind;
	enum STATE_KIND
	{
		STATE_DT = -10,
		STATE_1S = 0,
		STATE_1H = 1,
		STATE_2S = 10,
		STATE_2H = 25,
		STATE_3S = 20,
		STATE_2D = 30,
		STATE_3H = 70,
		STATE_4S = 35,
		STATE_3D = 80,
		STATE_4H = 200,
		STATE_5H = 500
	};
};