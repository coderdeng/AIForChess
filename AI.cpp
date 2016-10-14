#include "AI.h"
#include <random>
#include <ctime>

using namespace AI;

std::shared_ptr<wuziqiAI> AI::createWuziqiAI()
{
	std::shared_ptr<wuziqiAI> ins(new wuziqiAI());

	return ins;
}

wuziqiAI::~wuziqiAI()
{

}

/*
 *	Function		:	takeNextStep
 *	Description		:	compute the next step of current qipan state
 *	Calls			:	findAtGameTree
 *	Input			:	typq_qipanstate qipanState : the current state of qipan
                    	qiziKind currentKind : the kind of qizi whitch will take at now
 *	Output			:	the res
 *	Return			:	a pos of qipan
 *	Others			:	// 其它说明 
 */
wuziqiPosQiPan wuziqiAI::takeNextStep(type_qipanstate qipanState, qiziKind currentKind)
{
	auto tmp = findAtGameTree(qipanState, currentKind).first;
	int y = tmp.first;
	int x = tmp.second;
	cocos2d::log("y = %d; x = %d", y, x);

	return wuziqiPosQiPan(x, y);
}

/*
 *	Function		:	computeIncome
 *	Description		:	coupute the value of each place
 *	Calls			:	scoringIndex
 *	Input			:	type_qipanstate qipanstate : the current state of qipan 
 *	Return			:	the value of each place of qipan
 *	Others			:	// 其它说明 
 */
std::vector<int> wuziqiAI::computeIncome(type_qipanstate qipanState)
{
	std::vector<int> qipanIncome(19 * 19, 0);
	for (int y = 0; y < 19; ++y) {
		for (int x = 0; x < 19; ++x) {
			qipanIncome[y * 19 + x] = scoringIndex(qipanState, x, y);
		}
	}
	return qipanIncome;
}

/*
 *	Function		:	scoringIndex
 *	Description		:	compute the x,y 's scoring
 *	Input			:	the state of qipan
 *	Return			:	the scoring of index
 *	Others			:	// 其它说明 
 */
int wuziqiAI::scoringIndex(type_qipanstate qipanState, int m_x, int m_y)
{
	if (qipanState.at(m_y).at(m_x) != QIZI_NONE)return 0;

	int min_x = m_x - 4;
	int max_x = m_x + 4;
	int min_y = m_y - 4;
	int max_y = m_y + 4;

	std::vector<std::vector<qiziKind>> qiziArray = std::vector<std::vector<qiziKind>>(4, std::vector<qiziKind>());

	// 保存四个方向上9个棋子的状态
	for (int y = min_y, x = min_x; y <= max_y && x <= max_x; ++x, ++y) {
		if (x >= 0 && x < 19 && y >= 0 && y < 19)
			qiziArray[0].push_back(qipanState.at(m_y).at(x));
		else 
			qiziArray[0].push_back(QIZI_BLOCK);

		if (y >= 0 && y < 19 && x >= 0 && x < 19)
			qiziArray[1].push_back(qipanState.at(y).at(m_x));
		else
			qiziArray[1].push_back(QIZI_BLOCK);

		if (2 * m_y - y >= 0 && x >= 0 && 2 * m_y - y < 19 && x < 19)
			qiziArray[2].push_back(qipanState.at(2 * m_y - y).at(x));
		else
			qiziArray[2].push_back(QIZI_BLOCK);

		if (y >= 0 && x >= 0 && y < 19 && x < 19)
			qiziArray[3].push_back(qipanState.at(y).at(x));
		else
			qiziArray[3].push_back(QIZI_BLOCK);
	}
	
	return checkStateScoring(qiziArray);
}

/*
 *	Function		:	checkStateScoring
 *	Description		:	compute the qizi state and set they scoring 
 *	Input			:	the qizi state from four direction 
 *	Return			:	the hightest scoring 
 *	Others			:	继续优化
 */
int wuziqiAI::checkStateScoring(type_qipanstate qiziArray)
{
	STATE_KIND w_state = STATE_1H, b_state = STATE_1H;
	int w_scoring = 0, b_scoring = 0; // 白子和黑子的得分
	for (int i = 0; i < 4; ++i) {
		auto tmp_array = qiziArray.at(i);
		int w_begin = 0, w_end = 0, b_begin = 0, b_end = 0; // 记录当前黑白子开始的位置

		// 放白棋子
		tmp_array[4] = QIZI_WHITE;
		for (int j = 0; j < tmp_array.size(); ++j) {
			if (tmp_array.at(j) == QIZI_WHITE) {
				w_end++;
			}
			else {
				if (j < 4) {
					w_begin = j + 1;
					w_end = w_begin + 1;
				}
				if (j > 4) {
					w_end = j;
					break;
				}
			}
		}
		// 放黑棋子
		tmp_array[4] = QIZI_BLACK;
		for (int j = 0; j < tmp_array.size(); ++j) {
			if (tmp_array.at(j) == QIZI_BLACK) {
				b_end++;
			}
			else {
				if (j < 4) {
					b_begin = j + 1;
					b_end = b_begin + 1;
				}
				if (j > 4) {
					b_end = j;
					break;
				}
			}
		}

		// 白棋得分
		switch (w_end - w_begin)
		{
		case 1:
			if ((tmp_array.at(w_begin - 1) == QIZI_BLACK && tmp_array.at(w_end) == QIZI_BLACK) || 
				(tmp_array.at(w_begin - 1) == QIZI_BLOCK && tmp_array.at(w_end) == QIZI_BLOCK)) {
				w_state = STATE_DT;
			}
			else if (tmp_array.at(w_begin - 1) == QIZI_BLACK || tmp_array.at(w_end) == QIZI_BLACK ||
				tmp_array.at(w_begin-1) == QIZI_BLOCK || tmp_array.at(w_end) == QIZI_BLOCK) {
				w_state = STATE_1S;
			}
			else {
				w_state = STATE_1H;
			}
			break;
		case 2:
			if ((tmp_array.at(w_begin - 1) == QIZI_BLACK && tmp_array.at(w_end) == QIZI_BLACK) || 
				(tmp_array.at(w_begin - 1) == QIZI_BLOCK && tmp_array.at(w_end) == QIZI_BLOCK)) {
				w_state = STATE_DT;
			}
			else if (tmp_array.at(w_begin - 1) == QIZI_BLACK || tmp_array.at(w_end) == QIZI_BLACK ||
				tmp_array.at(w_begin - 1) == QIZI_BLOCK || tmp_array.at(w_end) == QIZI_BLOCK) {
				w_state = STATE_2S;
			}
			else {
				w_state = STATE_2H;
			}
			break;
		case 3:
			if ((tmp_array.at(w_begin - 1) == QIZI_BLACK && tmp_array.at(w_end) == QIZI_BLACK) ||
				(tmp_array.at(w_begin - 1) == QIZI_BLOCK && tmp_array.at(w_end) == QIZI_BLOCK)) {
				w_state = STATE_DT;
			}
			else if (tmp_array.at(w_begin - 1) == QIZI_BLACK || tmp_array.at(w_end) == QIZI_BLACK ||
				tmp_array.at(w_begin - 1) == QIZI_BLOCK || tmp_array.at(w_end) == QIZI_BLOCK) {
				w_state = STATE_3S;
			}
			else {
				w_state = STATE_3H;
			}
			break;
		case 4:
			if ((tmp_array.at(w_begin - 1) == QIZI_BLACK && tmp_array.at(w_end) == QIZI_BLACK) ||
				(tmp_array.at(w_begin - 1) == QIZI_BLOCK && tmp_array.at(w_end) == QIZI_BLOCK)) {
				w_state = STATE_DT;
			}
			else if (tmp_array.at(w_begin - 1) == QIZI_BLACK || tmp_array.at(w_end) == QIZI_BLACK ||
				tmp_array.at(w_begin - 1) == QIZI_BLOCK || tmp_array.at(w_end) == QIZI_BLOCK) {
				w_state = STATE_4S;
			}
			else {
				w_state = STATE_4H;
			}
			break;
		default:
			w_state = STATE_5H;
			break;
		}
		w_scoring += w_state;

		// 黑旗得分
		switch (b_end - b_begin)
		{
		case 1:
			if ((tmp_array.at(b_begin - 1) == QIZI_WHITE && tmp_array.at(b_end) == QIZI_WHITE) || 
				(tmp_array.at(b_begin - 1) == QIZI_BLOCK && tmp_array.at(b_end) == QIZI_BLOCK)) {
				b_state = STATE_DT;
			}
			else if (tmp_array.at(b_begin - 1) == QIZI_WHITE || tmp_array.at(b_end) == QIZI_WHITE ||
				tmp_array.at(b_begin - 1) == QIZI_BLOCK || tmp_array.at(b_end) == QIZI_BLOCK) {
				b_state = STATE_1S;
			}
			else {
				b_state = STATE_1H;
			}
			break;
		case 2:
			if ((tmp_array.at(b_begin - 1) == QIZI_WHITE && tmp_array.at(b_end) == QIZI_WHITE) || 
				(tmp_array.at(b_begin - 1) == QIZI_BLOCK && tmp_array.at(b_end) == QIZI_BLOCK)) {
				b_state = STATE_DT;
			}
			else if (tmp_array.at(b_begin - 1) == QIZI_WHITE || tmp_array.at(b_end) == QIZI_WHITE ||
				tmp_array.at(b_begin - 1) == QIZI_BLOCK || tmp_array.at(b_end) == QIZI_BLOCK) {
				b_state = STATE_2S;
			}
			else {
				b_state = STATE_2H;
			}
			break;
		case 3:
			if ((tmp_array.at(b_begin - 1) == QIZI_WHITE && tmp_array.at(b_end) == QIZI_WHITE) || 
				(tmp_array.at(b_begin - 1) == QIZI_BLOCK && tmp_array.at(b_end) == QIZI_BLOCK)) {
				b_state = STATE_DT;
			}
			else if (tmp_array.at(b_begin - 1) == QIZI_WHITE || tmp_array.at(b_end) == QIZI_WHITE ||
				tmp_array.at(b_begin - 1) == QIZI_BLOCK || tmp_array.at(b_end) == QIZI_BLOCK) {
				b_state = STATE_3S;
			}
			else { 
				b_state = STATE_3H;
			}
			break;
		case 4:
			if ((tmp_array.at(b_begin - 1) == QIZI_WHITE && tmp_array.at(b_end) == QIZI_WHITE) ||
				(tmp_array.at(b_begin - 1) == QIZI_BLOCK && tmp_array.at(b_end) == QIZI_BLOCK)) {
				b_state = STATE_DT;
			}
			else if (tmp_array.at(b_begin - 1) == QIZI_WHITE || tmp_array.at(b_end) == QIZI_WHITE ||
				tmp_array.at(b_begin - 1) == QIZI_BLOCK || tmp_array.at(b_end) == QIZI_BLOCK) {
				b_state = STATE_4S;
			}
			else {
				b_state = STATE_4H;
			}
			break;
		default:
			b_state = STATE_5H;
			break;
		}
		b_scoring += b_state;
	}
	return b_scoring + w_scoring;
}

/*
 *	Function		:	findAtGameTree
 *	Description		:	find the best divide at the game tree
 *	Input			:	the state of qipan , and what kind of qizi will take next
 *	Return			:	a pair of x,y 
 *	Others			:	
 */
std::pair<std::pair<int, int>, int> 
wuziqiAI::findAtGameTree(type_qipanstate qipanState, qiziKind currentKind, wuziqiPosQiPan newPos, int deep)
{
	// 边界条件找到一个解， 即五子连珠， 或者超出最大搜索层
	if (newPos._y != -1) {
		qipanState[newPos._y][newPos._x] = currentKind;
		if (currentKind == QIZI_WHITE)
			currentKind = QIZI_BLACK;
		else if (currentKind == QIZI_BLACK)
			currentKind = QIZI_WHITE;
		if (checkGameOver(qipanState, newPos)) {
			if (currentKind == _AIqiziKind)return std::make_pair(std::make_pair(newPos._y, newPos._x), 1);
			else return std::make_pair(std::make_pair(newPos._y, newPos._x), -1);
		}
		else if (deep >= 3)
			return std::make_pair(std::make_pair(newPos._y, newPos._x), 0);
	}

	// 计算出当前棋盘状态上所有的位置的得分
	auto qipanIncome = computeIncome(qipanState);
	
	std::vector<std::pair<int, int>> may_vec;
	int offset = 0;
	auto it = std::max_element(qipanIncome.begin(), qipanIncome.end());
	auto tmp_it = it;
	int i = 0;
	while (tmp_it != qipanIncome.end()) {
		int index = tmp_it - qipanIncome.begin();
		int t_y = index / 19;
		int t_x = index % 19;
		may_vec.push_back(std::make_pair(t_y, t_x));
		tmp_it = std::find(tmp_it + 1, qipanIncome.end(), *it);
	}
	//////////////////////////////////////////////////////////////////////////
	cocos2d::log("may value size:%d", may_vec.size());
	for (auto v : may_vec) {
		cocos2d::log("may value: y=%d,x=%d,v=%d", v.first, v.second, qipanIncome.at(v.first*19+v.second));
	}
	//////////////////////////////////////////////////////////////////////////

	if (may_vec.size() > 225) {
		return std::make_pair(std::make_pair(9, 9), 0);
	}

	// 递归搜索上面得到的每一个最优解， 即考虑当将当前颜色放到该位置时后面可能的最优解
	std::vector<int> winRates(may_vec.size(), 0); // 存放该分支的胜率
	std::vector<std::pair<int, int>> indexs(may_vec.size(), std::pair<int, int>(0, 0)); // 存放该分支的下一个最佳位置
	for (int i = 0; i < may_vec.size(); ++i) {
		auto tmp_ret =
			findAtGameTree(qipanState, currentKind, wuziqiPosQiPan(may_vec.at(i).second, may_vec.at(i).first), deep + 1);
		indexs[i] = tmp_ret.first;
		winRates[i] += tmp_ret.second;
	}
	
	// 赢的概率
	auto its = std::max_element(winRates.begin(), winRates.end());
	auto tmp_its = its;
	std::vector<int> index_vec;
	while (tmp_its != winRates.end()) {
		index_vec.push_back(tmp_its - winRates.begin());
		tmp_its = std::find(tmp_its + 1, winRates.end(), *its);
	}
	cocos2d::log("winRate size:%d", winRates.size());
	for (int i = 0; i < index_vec.size(); ++i) {
		cocos2d::log("winRate: x = %d, y = %d, w = %d", may_vec.at(i).second, may_vec.at(i).first, winRates.at(i));
	}
	std::default_random_engine e(time(0));
	std::uniform_int_distribution<int> u(0, index_vec.size() - 1);
	int is = u(e);
	// 以下考虑如果多个结果相同的情况
	// 往后补充
	if (is >= indexs.size()) {
		cocos2d::log("over size, size: %d, is: %d", indexs.size(), is);
		return std::make_pair(std::make_pair(newPos._y, newPos._x), -1);
	}
	return std::make_pair(may_vec[is], winRates[is]);
}

/*
 *	Function		:	checkGameOver
 *	Description		:	check the game weather over
 *	Input			:	type_qipanstate qipanState, wuziqiPosQiPan newPos  
 *	Output			:	// 对输出参数的说明。 
 *	Return			:	true : game over; false : game not over
 *	Others			:	// 其它说明 
 */
bool wuziqiAI::checkGameOver(type_qipanstate qipanState, wuziqiPosQiPan newPos)
{
	int min_x = newPos._x - 4;
	int max_x = newPos._x + 4;
	int min_y = newPos._y - 4;
	int max_y = newPos._y + 4;
	qiziKind itemCurrent = qipanState.at(newPos._y).at(newPos._x);
	std::vector<int> cnt = std::vector<int>(4, 0);

	for (int y = min_y, x = min_x; y <= max_y && x <= max_x; ++x, ++y) {

		if (x >= 0 && x < 19 && y >= 0 && y < 19 && qipanState.at(newPos._y).at(x) == itemCurrent) {
			// y , x++
			cnt[0]++;
		}
		else
			cnt[0] = 0;
		if (y >= 0 && y < 19 && x >= 0 && x < 19 && qipanState.at(y).at(newPos._x) == itemCurrent) {
			// y++, x
			cnt[1]++;
		}
		else
			cnt[1] = 0;
		if (2 * newPos._y - y >= 0 && 2 * newPos._y - y < 19 && x >= 0 && x < 19 &&
			qipanState.at(2 * newPos._y - y).at(x) == itemCurrent) {
			// y--, x++
			cnt[2]++;
		}
		else
			cnt[2] = 0;
		if (y >= 0 && x >= 0 && y < 19 && x < 19 && qipanState.at(y).at(x) == itemCurrent) {
			// y++, x++
			cnt[3]++;
		}
		else
			cnt[3] = 0;

		if (*std::max_element(cnt.begin(), cnt.end()) >= 5)
			return true;
	}
	return false;
}