#include "shunting-yard.h"
#include "direct_funcs.h"
#include "errorReporting.h"
#include <vector>
#include <iostream>


using namespace std;

static void init_dir_new();


#define opcount 7

static op_direct ops_direct_new[opcount];


bool getOP_direct_new(string a, op_direct* out)
{
    for (int i = 0; i < opcount; i++)
    {
        while (ops_direct_new[i].literal == a) 
        {
            out->nullable = ops_direct_new[i].nullable;
            out->first_pos = ops_direct_new[i].first_pos;
            out->last_pos = ops_direct_new[i].last_pos;
            out->literal = ops_direct_new[i].literal;
            out->priority = ops_direct_new[i].priority;
            out->unary = ops_direct_new[i].unary;
            return true;
        }
    }
    return false;
}

static inline node_direct* doTree_new(op_direct, vector<op_direct>*, vector<node_direct*>*, vector<string>);

node_direct* dijkstra_direct(vector<string> regex)
{
    init_dir_new();

    vector<op_direct>      opstack;
    vector<node_direct*>   nodestack;

    op_direct* currentOP = new op_direct();
    op_direct tempOP;
    bool foundOpen;
    string ch = "";

    for (int i = 0; i < regex.size(); i++)
    {
        ch = regex[i];
        if (getOP_direct_new(ch, currentOP))
        {
#if logYard
            cout << "Current OP:\t" << currentOP->literal 
                << "\t" << currentOP->priority << "\t" 
                << (bool)currentOP->unary << endl;
#endif
            switch (currentOP->priority)
            {
                case -1:
                    currentOP->regexPosition = i;
                    opstack.push_back(*currentOP);
                    currentOP->regexPosition = -1;
                break;
                case -2:
                    foundOpen = false; 
                while (!foundOpen &&
                    !opstack.empty() && 
                    opstack.back().priority >= currentOP->priority)
                    {
#if logYard
                        cout <<"Poping bracket:\t";
                        for (auto& bruh:opstack)
                            cout << bruh.literal << "\t";
                        cout << endl;
#endif
                        if (opstack.empty())
                            MissingOperand(tempOP, regex);
                        tempOP = opstack.back();
                        opstack.pop_back();
                        
                        node_direct* a;
                        node_direct* b;

                        if (nodestack.empty())
                            MissingOperand(tempOP, regex); 
                        
                        b = nodestack.back();
                        nodestack.pop_back();

                        node_direct* father; 

                        if (tempOP.priority == -1)
                        {
                            father = b;
                            foundOpen = true;
                        }
                        else if (tempOP.unary)
                        {
                            father = new node_direct();

                            father->character = tempOP.literal;
                            father->leftson = b;
                            father->regexPosition = i;
                            father->nullable = tempOP.nullable(father, father->leftson, father->rightson);
                            father->first_pos = tempOP.first_pos(father, father->leftson, father->rightson);
                            father->last_pos = tempOP.last_pos(father, father->leftson, father->rightson);
                        }
                        else 
                        {
                            father = new node_direct();
                            if (nodestack.empty())
                                MissingOperand(tempOP, regex); 

                            a = nodestack.back();
                            nodestack.pop_back();

                            father->character = tempOP.literal;
                            father->leftson = a;
                            father->rightson = b;
                            father->regexPosition = i;
                            father->nullable = tempOP.nullable(father, father->leftson, father->rightson);
                            father->first_pos = tempOP.first_pos(father, father->leftson, father->rightson);
                            father->last_pos = tempOP.last_pos(father, father->leftson, father->rightson);
                        }
                        nodestack.push_back(father);
                        
                    }
                    if (!foundOpen)
                    {
                        currentOP->regexPosition = i;
                        UnbalancedBracket(*currentOP, regex); 
                    }
                break;
                default:
                    while (!opstack.empty() && //no está vacío
                        opstack.back().priority > 0 && //no es un paréntesis que abre
                        opstack.back().priority >= currentOP->priority) //el actual tiene menor presedencia
                        {
#if logYard
                            cout <<"Poping priority:\t"
                            << currentOP->literal << "\t"
                            << currentOP->priority << "\tstack:\t";

                            for (auto& bruh: opstack)
                                cout << bruh.literal << "\t";
                            cout << endl;

#endif
                            if (opstack.empty())
                                MissingOperand(tempOP, regex); 
                            tempOP = opstack.back();
                            opstack.pop_back();

                            node_direct* father = doTree_new(tempOP, &opstack, &nodestack, regex);
                            nodestack.push_back(father);
                        }
                    currentOP->regexPosition = i;
                    opstack.push_back(*currentOP);
                    currentOP->regexPosition = -1;

                break;
            }
        }
        else
        {
#if logYard
            cout << "leaf_node:\t" << ch << endl;
#endif
            node_direct* leaf_node = new node_direct();
            leaf_node->character = ch;
            leaf_node->regexPosition = i;
            leaf_node->nullable = leaf_null(leaf_node, leaf_node->leftson, leaf_node->rightson);
            leaf_node->first_pos = leaf_first(leaf_node, leaf_node->leftson, leaf_node->rightson);
            leaf_node->last_pos = leaf_last(leaf_node, leaf_node->leftson, leaf_node->rightson);
            leaf_node->leaf = true;
            nodestack.push_back(leaf_node);
        }
    }

    while (!opstack.empty())
    {
#if logYard
        cout <<"Final pop:\t";
        for (auto& bruh: opstack)
            cout << bruh.literal << "\t";
        cout << endl;
#endif
        tempOP = opstack.back();
        opstack.pop_back();

        node_direct* father = doTree_new(tempOP, &opstack, &nodestack, regex);
        nodestack.push_back(father);
    }
    
    return nodestack.back();
}


static inline node_direct* doTree_new(op_direct tempOP, vector<op_direct>* opstack, vector<node_direct*>* nodestack, vector<string> regex)
{
    if (tempOP.literal == "(")
        UnbalancedBracket(tempOP, regex); 
    
    node_direct* a = nullptr;
    node_direct* b = nullptr;

    if (nodestack->empty())
        MissingOperand(tempOP, regex); 

    b = nodestack->back();
    nodestack->pop_back();


    node_direct* father;
    if (tempOP.unary)
    {
        father = new node_direct();

        father->character = tempOP.literal;
        father->leftson = b;
        father->regexPosition = tempOP.regexPosition;
        father->nullable = tempOP.nullable(father, father->leftson, father->rightson);
        father->first_pos = tempOP.first_pos(father, father->leftson, father->rightson);
        father->last_pos = tempOP.last_pos(father, father->leftson, father->rightson);
    }
    else 
    {
        father = new node_direct();
        if (nodestack->empty())
            MissingOperand(tempOP, regex); 

        a = nodestack->back();
        nodestack->pop_back();

        father->character = tempOP.literal;
        father->leftson = a;
        father->rightson = b;
        father->regexPosition = tempOP.regexPosition;
        father->nullable = tempOP.nullable(father, father->leftson, father->rightson);
        father->first_pos = tempOP.first_pos(father, father->leftson, father->rightson);
        father->last_pos = tempOP.last_pos(father, father->leftson, father->rightson);
    }
    return father;
}

static void init_dir_new() 
{
    static bool initialized_dir_new = false;
    if (initialized_dir_new) return;
    ops_direct_new[0].literal  = "|";
    ops_direct_new[0].priority = 1;
    ops_direct_new[0].nullable = or_null;
    ops_direct_new[0].first_pos = or_first;
    ops_direct_new[0].last_pos = or_last;

    ops_direct_new[1].literal  = ".";
    ops_direct_new[1].priority = 3;
    ops_direct_new[1].nullable = concat_null;
    ops_direct_new[1].first_pos = concat_first;
    ops_direct_new[1].last_pos = concat_last;

    ops_direct_new[2].literal  = "?";
    ops_direct_new[2].priority = 4;
    ops_direct_new[2].nullable = qmark_null;
    ops_direct_new[2].first_pos = qmark_first;
    ops_direct_new[2].last_pos = qmark_last;
    ops_direct_new[2].unary = true;

    ops_direct_new[3].literal  = "*";
    ops_direct_new[3].priority = 4;    
    ops_direct_new[3].nullable = kleene_null;
    ops_direct_new[3].first_pos = kleene_first;
    ops_direct_new[3].last_pos = kleene_last;
    ops_direct_new[3].unary = true;

    ops_direct_new[4].literal  = "+";
    ops_direct_new[4].priority =  4;
    ops_direct_new[4].nullable = positive_null;
    ops_direct_new[4].first_pos = positive_first;
    ops_direct_new[4].last_pos = positive_last;
    ops_direct_new[4].unary = true;

    ops_direct_new[5].literal  = "(";
    ops_direct_new[5].priority = -1;
    ops_direct_new[5].nullable = 0;
    ops_direct_new[5].first_pos = 0;
    ops_direct_new[5].last_pos = 0;

    ops_direct_new[6].literal  = ")";
    ops_direct_new[6].priority = -2;
    ops_direct_new[6].nullable = 0;
    ops_direct_new[6].first_pos = 0;
    ops_direct_new[6].last_pos = 0;


    initialized_dir_new = true;
}