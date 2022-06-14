#ifndef SEUYACC_CFGTOLRDFA_H
#define SEUYACC_CFGTOLRDFA_H

#include"structs.h"
extern map<int, set<int> > firstMap;
extern ProducerVec GlobalProducerVec;
extern SymbolVec GlobalSymbolVec;
extern map<int, pair<int, int> >indexMap;
extern int startProduction;

//����LR(1)��Ŀ���հ�
void epsilon_clousure(const ItemSet&, ItemSet&);
//��¼��ǰLR��Ŀ�����߼�Ŀ��LR��Ŀ��
void stateEdge_construct(const vector<Item>&, map<int, ItemSet >&);
//����LR(1)��Ŀ����
void CFGToLRDFA(Collection& LRStates);

void first_string(set<int>&, const vector<int>&);

void CFGToLRDFA(Collection& LRStates) {
    queue<int> q;//����δ������Ŀ������
    LRStates.clear();//��Ŀ�����ʼ��Ϊ��

    Item newItem;
    ItemSet newItemSet;
    newItem.productionrInt = startProduction; //S'->S
    newItem.prediction.insert(GlobalSymbolVec.size() - 1);//�ķ���������
    newItemSet.stateInt = 0;		//0��Ϊ��ʼ״̬
    newItemSet.itemSet.push_back(newItem);//�������ں���
    LRStates.push_back(newItemSet);
    q.push(0);

    while (!q.empty()) {//��δ�������Ŀ�����в�Ϊ��
        int x = q.front();
        q.pop();
        ItemSet wholeSet;
        //�Զ���ͷ����LR(1)��Ŀ״̬����չ
        epsilon_clousure(LRStates[x], wholeSet);
        map<int, ItemSet> stateMap;
        //״̬����չ
        stateEdge_construct(wholeSet.itemSet, stateMap);
        for (auto& edge : stateMap) {//������Ŀ�������г���
            int le = LRStates.size(), edgeToInt = -1;
            for (size_t i = 0; i < le; i++)
            if (LRStates[i] == edge.second) {
                edgeToInt = i;
                break;
            }
            if (edgeToInt != -1) {//Ŀ����Ŀ���Ѵ���
                LRStates[x].edgeMap[edge.first] = edgeToInt;
                continue;
            }
            else {//Ŀ����Ŀ��������
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
    set<int>ifhandling; //���ڱ�ʾ�Ƿ���queue��
    map<pair<int, int>, int>index;  //�������µ����ʱ�����Ƿ�����Ѿ�����(position/productionrInt)
    queue<int> q;	//�洢��wholeLRItemSet�������

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
    while (!q.empty()) {//����δ��ʱ
        auto& it = wholeLRItemSet.itemSet;
        producer = GlobalProducerVec[it[q.front()].productionrInt];
        position = it[q.front()].dot_positionInt;
        set<int>& predictiveSymbol = it[q.front()].prediction;
        ifhandling.erase(q.front());//�����߳�����
        if (position == producer.second.size()) {//����ĩβ�����ô���
            q.pop();
            continue;
        }
        symbol = producer.second[position];//����д���ķ���
        if (GlobalSymbolVec[symbol].isTerminal) {//�ս�����ô���
            q.pop();
            continue;
        }

        //�ҵ�Բ����ַ�Ϊ�󲿵����в���ʽ
        auto indexPair = indexMap[symbol];
        unordered_set<int> inputSet;
        //��ÿ������ʽ������Ŀr'
        vector<int> nextSymbolsVec;
        for (int i = position + 1; i < producer.second.size(); ++i) {
            nextSymbolsVec.push_back(producer.second[i]);
        }
        //����Ŀr��Ԥ�����
        set<int>predictionSymbol;
        first_string(predictionSymbol, nextSymbolsVec);
        if (predictionSymbol.count(-1)) {//��epsilon
            predictionSymbol.erase(-1);
            predictionSymbol.insert(predictiveSymbol.cbegin(), predictiveSymbol.cend());
        }

        q.pop();
        for (int i = indexPair.first; i < indexPair.first + indexPair.second; i++) {
            //�������в���ʽ���½�Item֮��Ƚ��Ƿ��ظ�
            if (index.count(make_pair(0, i))) {//��������Ѿ���wholeLRItemSet����
                int k = index[make_pair(0, i)], s = it[k].prediction.size();
                it[k].prediction.insert(predictionSymbol.cbegin(), predictionSymbol.cend());
                if (s < it[k].prediction.size()) {//���Ԥ����и���
                    //���������չ��״̬��������������
                    if (ifhandling.find(k) == ifhandling.end()) {
                        ifhandling.insert(k);
                        q.push(k);
                    }
                }
            }
            else {//����δ��wholeLRItem�г���
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
        //������Ŀ���е�ÿ����Ŀ
        producer = GlobalProducerVec[lrItem.productionrInt];
        if (producer.second.size() == lrItem.dot_positionInt)//����ĩβ��������һ������ʽ
            continue;
        edge = producer.second[lrItem.dot_positionInt];
        newItem = lrItem;
        newItem.dot_positionInt++;
        auto findIt = newStateMap.find(edge);
        if (findIt == newStateMap.end()) {//û�б������ñߣ��½�һ��LR״̬
            ItemSet newState;
            newState.itemSet.push_back(newItem);//������µ���ӵ�������
            newStateMap.emplace(edge, newState);
        }
        else {//����������ñߣ����µ�lrItem�ӵ����еļ�����
            findIt->second.itemSet.push_back(newItem);
        }
    }
}

#endif //SEUYACC_CFGTOLRDFA_H

