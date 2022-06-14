#ifndef SEUYACC_FIRST_H
#define SEUYACC_FIRST_H

#include"structs.h"
extern map<int, set<int> > firstMap;
extern ProducerVec GlobalProducerVec;
extern SymbolVec GlobalSymbolVec;
extern map<int, pair<int, int> >indexMap;

void sets_union(set<int>&, const set<int>&);
void first_symbol(set<int>&, int&, set<int>&);
void first_string(set<int>&, const vector<int>&);

//����epsilon ��ʾΪ-1
void first_symbol(set<int>& firstSet, int& symbol, set<int>& handlingSymbol) {

    if (firstMap.count(symbol) != 0) {
        sets_union(firstSet, firstMap[symbol]);
        return;
    }//�����Ѿ������first��,��firstMapӳ�䲢��first��

    if (symbol == -1)return;//epsilon

    if (GlobalSymbolVec[symbol].isTerminal) {//�ս��
        firstSet.insert(symbol);
        return;
    }

    else {//���ս��
        handlingSymbol.insert(symbol);
        auto indexPair = indexMap[symbol];
        for (int i = indexPair.first; i < indexPair.first + indexPair.second; i++) {
            //��������symbol����ߵĲ���ʽ
            auto& producer = GlobalProducerVec[i];
            int le = producer.second.size();

            //symbol->epsilon
            if (le == 1 && producer.second[0] == -1) {
                firstSet.insert(-1);
                continue;
            }
            //���ұ������е���
            set<int> tempSet;//��ʱ��ŵ�ǰ���ŵ�first
            for (int j = 0; j < le; j++) {
                tempSet.clear();
                if (handlingSymbol.find(producer.second[j]) != handlingSymbol.end()) {//��ֹ��ݹ�
                    break;
                }
                first_symbol(tempSet, producer.second[j], handlingSymbol);
                if (tempSet.count(-1) == 0) {
                    sets_union(firstSet, tempSet);
                    break;
                }
                if (j == le - 1) {//���һ���epsilon
                    firstSet.insert(-1);
                    break;
                }
                tempSet.erase(-1);//-1����жϻ᲻��������epsilon
                sets_union(firstSet, tempSet);//��ǰfirst���ϲ����ܼ���
            }
        }
    }
}

void sets_union(set<int>& leftSet, const set<int>& rightSet) {
    leftSet.insert(rightSet.cbegin(), rightSet.cend());
}

void calc_first() {
    set<int> firstSet, handlingSymbol;
    int le = GlobalSymbolVec.size();
    for (int symbol = 0; symbol < le; symbol++) {
        firstSet.clear();
        handlingSymbol.clear();
        first_symbol(firstSet, symbol, handlingSymbol);
        firstMap.emplace(symbol, firstSet);
    }
}

//����LR1����Ŀ�� �������ں����ʱ��ó�Ԥ���
void first_string(set<int>& inputSet, const vector<int>& symbols) {

    if (symbols.size() == 0) {//Բ�������
        inputSet.insert(-1);
        return;
    }

    int le = symbols.size();
    for (int i = 0; i < le; i++) {
        auto& firstSet = firstMap[symbols[i]];
        if (firstSet.count(-1) == 0) {  //epsilon����first��
            sets_union(inputSet, firstSet);
            break;
        }
        if (i == le - 1) {//���һ���epsilon
            inputSet.insert(-1);
            break;
        }
        firstSet.erase(-1); //-1����ж��Ƿ����inputSet  ���᲻��������epsilon
        sets_union(inputSet, firstSet); //��ǰfirst���ϲ����ܼ���
    }
}

#endif //SEUYACC_FIRST_H

