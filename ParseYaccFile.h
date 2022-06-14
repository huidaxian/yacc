#ifndef SEUYACC_PARSE_YACC_H
#define SEUYACC_PARSE_YACC_H
#include "structs.h"
#include "fstream"
#include "iostream"
#include"map"
#include <stdio.h>

using namespace std;

//��yacc.y�ļ����н���,�õ�����ʽvec/��������/Symbol
extern vector<string> FuncVec;     //��Ÿ�������
extern vector<Symbol> GlobalSymbolVec;        //����ս��
extern ProductionVec GlobalProductionVec;//��Ų���ʽ
extern ProducerVec GlobalProducerVec;     //���ְ汾

extern map<int, pair<int, int> >indexMap;
extern int startProduction;
//��¼һ�����ս������Ӧ�Ĳ���ʽ���������
//pair ��first�ǿ�ʼ�±� second�Ǹ÷��ս���ж��ٸ�����ʽ


//����yacc��ַ,�޸Ĳ���ʽvec
//��ȷ���� return 1
//������� return 0
int Parse_Yacc(const string& filename, vector<Symbol>& symbolvec,
    ProductionVec& production,ProducerVec& production_int,vector<string>& FuncVec) {
    ifstream in;
    in.open(filename, ios::in);
    if (!in) {
        cout << "ERROR:Can't open yacc.y!" << endl;
        return 0;
    }

    //����yacc.y�﷨��������ļ�
    string str;     //��ǰ�����ַ���

    //1.����Token,������symbolvec,symbolvec�ж�Ϊ�ս��
    in >> str;
    while (str != "%start") {
        //����,����%token
        if ("%token" == str) {
            in >> str;
        }
        Symbol item;
        item.symbol = str;
        item.isTerminal = true;
        symbolvec.push_back(item);
        in >> str;
    }

    //2.�������ʽ,������production
    while (str != "%%")
        in >> str;
    in >> str;
    vector<int> vec_left_int; //��¼�ڲ���ʽ���г��ֵ�int

    //vector<pair<string,vector<string> > > ProductionVec;
    //��һ��%%�Ǹ����ӳ���
    int COUNT = 0;
    int symbol_index = 0;

    while (str != "%%") {
        //��ʼ��¼����ʽ��
        if (str == ";")
            in >> str;
        if (str == "%%")
            break;
        string left = str;
        int left_int;
        //left�Ƿ���SymbolVec��,����¼��symbol_index
        bool isExist = false;
        bool isQuotation = false;
        bool isT = false;
        if (str[0] == '\'') {
            isQuotation = true;//���ñ�־''
            str = str[1];
            isT = true;
        }
        for (int i = 0; i < symbolvec.size(); i++) {
            //�����ַ�����
            if (str == symbolvec[i].symbol) {
                //left��SymbolVec�У���¼��symbol_index
                symbol_index = i;
                left_int = i;
                isExist = true;
                break;
            }
        }
        if (!isExist) {
            //left����SymbolVec��
            symbol_index = symbolvec.size();
            Symbol item;
            item.symbol = str;
            //�ж��Ƿ��ս��
            item.isTerminal = false;
            if (isT)
                item.isTerminal = true;
            symbolvec.push_back(item);//��left����SymbolVec
            left_int = symbolvec.size() - 1;
        }
        vec_left_int.push_back(left_int);

        int left_index = COUNT;
        int right_index = 0;

        in >> str;
        if (str == ":") {
            in >> str;
        }
        while (str != ";") {
            //����left�������������ʽ�Ҳ�
            vector<string> right;
            vector<int> right_int; //��¼����ʽ�Ҳ��еı��
            int count = 0;//��¼����''�ĸ���
            do {
                if (str == "\'")
                    count++;
                if (count)
                    count--;

                bool isExist = false;
                //�����µ�Symbol
                bool isQuotation = false;
                bool isT = false;
                if (str[0] == '\'') {
                    isQuotation = true;
                    str = str[1];
                    isT = true;
                }
                right.push_back(str);//�����¼����ʽ�Ҳ���string


                for (int i = 0; i < symbolvec.size(); i++) {
                    string temp_symbol = "\'" + symbolvec[i].symbol + "\'";//''�����е��ַ�
                    if (str == symbolvec[i].symbol ||
                        str == temp_symbol) {
                        right_int.push_back(i); //�����¼����ʽ�Ҳ���int
                        isExist = true;
                        break;
                    }
                }
                if (!isExist) {
                    //����SymbolVec��
                    Symbol item;
                    item.symbol = str;
                    item.isTerminal = false;
                    if (isT)
                        item.isTerminal = true;
                    symbolvec.push_back(item);
                    right_int.push_back(symbolvec.size() - 1);
                    //�����ǰ�ոռ����Ϊ3��,����Ϊ2
                }
                in >> str;

            } while (str != "|" && str != ";" && str != "%%" && count == 0);
            //����Ĳ���ʽ����ͬ,����ʽstring�汾
            pair<string, vector<string> > production_item(left, right);
            production.push_back(production_item);

            //����ʽint�汾
            pair<int, vector<int> > production_item_int(left_int, right_int);
            production_int.push_back(production_item_int);

            COUNT++;
            right_index++;

            if (str == ";")
                continue;//����ͬһ������ʽ�󲿵����в���ʽ
            in >> str;
        }
        pair<int, int> temp(left_index, right_index);
        indexMap[symbol_index] = temp; //��¼ͬһ������ʽ�󲿵����в���ʽ
    }

    //�������S,S'
    //����symbol
    //����S'->translation_unit
    //����GlobalProductionVec GlobalProducerVec
    //����indexMap

    //����S'
    int l1 = symbolvec.size();
    Symbol item;
    item.symbol = "S'";
    item.isTerminal = false;
    symbolvec.push_back(item);

    int prodcution_size = production.size();
    //����S'->translation_unit
    string l = "S'";
    vector<string> r{ "translation_unit" };
    pair<string, vector<string> > production_item(l, r);

    //��S'->translation_unit�������ʽstring�汾
    production.push_back(production_item); 
    startProduction = production.size() - 1;

    //���뵽����ʽ���ְ汾
    int trans_item = 0;
    for (int i = 0; i < symbolvec.size(); i++) {
        if (symbolvec[i].symbol == "translation_unit")
        {//�ҵ�S���ַ����
            trans_item = i;
            break;
        }
    }

    //��S'->translation_unit�������ʽint�汾
    vector<int> vec_l1{ trans_item };
    pair<int, vector<int> > production_item_int(l1, vec_l1);
    production_int.push_back(production_item_int);


    //����S'��indexMap
    pair<int, int> temp(prodcution_size, 1);
    indexMap[l1] = temp;

    //����$r����symbolvec
    l1 = symbolvec.size();
    Symbol item1;
    item1.symbol = "$r";
    item1.isTerminal = true;
    symbolvec.push_back(item1);


    //3.��ȡ�����ӳ������
    if (str == "%%")
    {
        while (!in.eof())
        {
            getline(in, str);
            while (str.empty() && !in.eof())
            {
                getline(in, str);

            }
        }
        FuncVec.push_back(str);
    }

    //��ȷ����
    return 1;
}


#endif //SEUYACC_PARSE_YACC_H
