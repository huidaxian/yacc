#ifndef SEUYACC_CFGTOLRDFA_H
#define SEUYACC_CFGTOLRDFA_H

#include"structs.h"
extern map<int, set<int> > firstMap;
extern ProducerVec GlobalProducerVec;
extern SymbolVec GlobalSymbolVec;
extern map<int, pair<int, int> >indexMap;
extern int startProduction;

//计算LR(1)项目集闭包
void epsilon_clousure(const ItemSet&, ItemSet&);
//记录当前LR项目集出边及目标LR项目集
void stateEdge_construct(const vector<Item>&, map<int, ItemSet >&);
//构造LR(1)项目集族
void CFGToLRDFA(Collection& LRStates);

void first_string(set<int>&, const vector<int>&);

void CFGToLRDFA(Collection& LRStates) {
    queue<int> q;//创建未处理项目集队列
    LRStates.clear();//项目集族初始化为空

    Item newItem;
    ItemSet newItemSet;
    newItem.productionrInt = startProduction; //S'->S
    newItem.prediction.insert(GlobalSymbolVec.size() - 1);//文法结束符号
    newItemSet.stateInt = 0;		//0号为初始状态
    newItemSet.itemSet.push_back(newItem);//仅保存内核项
    LRStates.push_back(newItemSet);
    q.push(0);

    while (!q.empty()) {//若未处理的项目集队列不为空
        int x = q.front();
        q.pop();
        ItemSet wholeSet;
        //对队列头部的LR(1)项目状态内扩展
        epsilon_clousure(LRStates[x], wholeSet);
        map<int, ItemSet> stateMap;
        //状态间扩展
        stateEdge_construct(wholeSet.itemSet, stateMap);
        for (auto& edge : stateMap) {//遍历项目集的所有出边
            int le = LRStates.size(), edgeToInt = -1;
            for (size_t i = 0; i < le; i++)
            if (LRStates[i] == edge.second) {
                edgeToInt = i;
                break;
            }
            if (edgeToInt != -1) {//目标项目集已存在
                LRStates[x].edgeMap[edge.first] = edgeToInt;
                continue;
            }
            else {//目标项目集不存在
                edge.second.stateInt = le;
                LRStates.push_back(edge.second);
                LRStates[x].edgeMap[edge.first] = le;
                q.push(le);
            }
        }
    }

    //epsilon-clousure

//    for(auto&i:LRStates){
//        ItemSet wholeSet;
//        epsilon_clousure(i, wholeSet);
//        i.itemSet = wholeSet.itemSet;
//    }

}



void epsilon_clousure(const ItemSet& LRItemSet, ItemSet& wholeLRItemSet) {
    set<int>ifhandling; //用于表示是否在queue中
    map<pair<int, int>, int>index;  //当产生新的项的时候检查是否该项已经存在(position/productionrInt)
    queue<int> q;	//存储在wholeLRItemSet中项序号

    int cnt = 0;
    wholeLRItemSet.stateInt = -1;
    wholeLRItemSet.itemSet.clear();
    wholeLRItemSet.edgeMap.clear();

    for (auto& item : LRItemSet.itemSet) {
        wholeLRItemSet.itemSet.push_back(item);
        index.emplace(make_pair(item.dot_positionInt, item.productionrInt), cnt);
        ifhandling.insert(cnt);
        q.push(cnt); cnt++;
    }

    pair<int, vector<int> > producer;
    int position, symbol;
    while (!q.empty()) {//队列未空时
        auto& it = wholeLRItemSet.itemSet;
        producer = GlobalProducerVec[it[q.front()].productionrInt];
        position = it[q.front()].dot_positionInt;
        set<int>& predictiveSymbol = it[q.front()].prediction;
        ifhandling.erase(q.front());//将项踢出队列
        if (position == producer.second.size()) {//点在末尾，不用处理
            q.pop();
            continue;
        }
        symbol = producer.second[position];//需进行处理的符号
        if (GlobalSymbolVec[symbol].isTerminal) {//终结符不用处理
            q.pop();
            continue;
        }

        //找到圆点后字符为左部的所有产生式
        auto indexPair = indexMap[symbol];
        unordered_set<int> inputSet;
        //对每个产生式创建项目r'
        vector<int> nextSymbolsVec;
        for (int i = position + 1; i < producer.second.size(); ++i) {
            nextSymbolsVec.push_back(producer.second[i]);
        }
        //求项目r的预测符集
        set<int>predictionSymbol;
        first_string(predictionSymbol, nextSymbolsVec);
        if (predictionSymbol.count(-1)) {//有epsilon
            predictionSymbol.erase(-1);
            predictionSymbol.insert(predictiveSymbol.cbegin(), predictiveSymbol.cend());
        }

        q.pop();
        for (int i = indexPair.first; i < indexPair.first + indexPair.second; i++) {
            //对于所有产生式，新建Item之后比较是否重复
            if (index.count(make_pair(0, i))) {//代表该项已经在wholeLRItemSet出现
                int k = index[make_pair(0, i)], s = it[k].prediction.size();
                it[k].prediction.insert(predictionSymbol.cbegin(), predictionSymbol.cend());
                if (s < it[k].prediction.size()) {//如果预测符有更新
                    //若该项不在扩展后状态队列中则加入队列
                    if (ifhandling.find(k) == ifhandling.end()) {
                        ifhandling.insert(k);
                        q.push(k);
                    }
                }
            }
            else {//该项未在wholeLRItem中出现
                Item newItem;
                newItem.productionrInt = i;
                newItem.prediction = predictionSymbol;
                int number = it.size();
                index.emplace(make_pair(0, i), number);
                q.push(number);
                ifhandling.insert(number);
                it.push_back(newItem);
            }
        }
    }
}

void stateEdge_construct(const vector<Item>& LRItemSet, map<int, ItemSet >& newStateMap) {
    pair<int, vector<int> > producer;
    Item newItem;
    int edge;
    for (const auto& lrItem : LRItemSet) {
        //遍历项目集中的每个项目
        producer = GlobalProducerVec[lrItem.productionrInt];
        if (producer.second.size() == lrItem.dot_positionInt)//点在末尾，处理下一个产生式
            continue;
        edge = producer.second[lrItem.dot_positionInt];
        newItem = lrItem;
        newItem.dot_positionInt++;
        auto findIt = newStateMap.find(edge);
        if (findIt == newStateMap.end()) {//没有遍历过该边，新建一个LR状态
            ItemSet newState;
            newState.itemSet.push_back(newItem);//把这个新的项加到集合里
            newStateMap.emplace(edge, newState);
        }
        else {//如果遍历过该边，把新的lrItem加到已有的集合中
            findIt->second.itemSet.push_back(newItem);
        }
    }
}

#endif //SEUYACC_CFGTOLRDFA_H

