#ifndef SEUYACC_HELPER_H
#define SEUYACC_HELPER_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <vector>
#include <queue>
using namespace std;

//����LR1�е���Ŀ
typedef struct Item {
    int dot_positionInt = 0;      //��ʾ����λ��
    int productionrInt = -1;      //����ʽ�ı��
    set<int> prediction;         //��ʾԤ�����ŵ�set����
    inline bool operator==(const Item& item)const {
        //�ж�LR(1)��Ŀ�Ƿ����
        if (dot_positionInt != item.dot_positionInt ||
            productionrInt != item.productionrInt ||
            prediction != item.prediction
            )
            return false;
        else
            return true;
    }
    Item& operator=(const Item& item) {
        //��ֵ
        dot_positionInt = item.dot_positionInt;
        productionrInt = item.productionrInt;
        prediction.clear();
        prediction.insert(item.prediction.cbegin(), item.prediction.cend());
        return *this;
    }
    bool operator<(const Item& rhs) const
    {
        if (productionrInt < rhs.productionrInt) {
            return true;
        }  //��Ŀ��Լ�ƽ���¼�ıȽ�
        else {
            if (productionrInt == rhs.productionrInt) {
                if (dot_positionInt < rhs.dot_positionInt) {
                    return true;
                }
            }
        }
        return false;
    }
    Item() {
    }
    Item(int a, int b, set<int>c) {
        dot_positionInt = a;
        productionrInt = b;
        prediction = c;
    }

}Item;


//����LR1�е���Ŀ��
typedef struct ItemSet {
    int stateInt = -1;                    //״̬��
    unordered_map<int, int> edgeMap;     //<�ַ���ţ�Ŀ��״̬��>
    vector<Item> itemSet;        //��Ŀ���ڸ���Ŀ

    inline bool operator==(const ItemSet& BSet)const {
        //�ж�����LR(1)��Ŀ���Ƿ����
        int la = itemSet.size(), lb = BSet.itemSet.size();
        if (la != lb)
            return false;
        for (int i = 0; i < la; i++) {
            int j = 0;
            for (; j < lb; j++)
                if (itemSet[i] == BSet.itemSet[j])break;
            if (j == lb)return false;
        }
        return true;
    }
}ItemSet;



//����LR1�е���Ŀ����
typedef vector<ItemSet> Collection;

//��yacc.y�ļ����н���,�õ�����ʽ/��������/Symbol��vector
// 
//����ַ��Ľṹ��
typedef struct Symbol {
    string symbol;          //�ַ�����
    bool isTerminal;        //�Ƿ����ս����
}Symbol;

//���Symbol��ȫ�ֱ���
typedef vector<Symbol> SymbolVec;
SymbolVec GlobalSymbolVec;

//��Ų���ʽ��<����ʽ�󲿣�����ʽ�Ҳ�> string�汾
typedef vector<pair<string, vector<string> > > ProductionVec;
ProductionVec GlobalProductionVec;

//��Ų���ʽ�� ���ְ汾
typedef vector<pair<int, vector<int> > > ProducerVec;
ProducerVec GlobalProducerVec;

vector<string> FuncVec;//��Ÿ����ӳ���
map<int, pair<int, int> >indexMap;//��¼һ�����ս������Ӧ�Ĳ���ʽ���������
//pair.first�ǿ�ʼ�±� second�Ǹ÷��ս���ж��ٸ�����ʽ
int startProduction;

//���first����
map<int, set<int> > firstMap;

//Ԥ�������
typedef unordered_map<int, unordered_map < int, int > >  Parse_Table;
//vector�±��ʾ״̬�ţ�<int,int> <Token��TokenVec��ţ���Լ/�ƽ����>

#endif //SEUYACC_HELPER_H

