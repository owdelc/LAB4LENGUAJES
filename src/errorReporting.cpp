#include "errorReporting.h"
#include <string>
#include <structs.h>
#include <stdexcept>


using namespace std;


void MissingOperand(op_direct tempOP, string regex)
{
    string errmsg = "El operador ";
    errmsg+=tempOP.literal;
    errmsg+=" en la posición ";
    errmsg+= to_string(tempOP.regexPosition);
    errmsg+= " carece de al menos un operando\n";
    
    for (int i = 0; i < regex.size(); i++)
        if (regex[i])
            errmsg+=regex[i];
        else
            errmsg+="ε";
    errmsg += "\n";
    for (int i = 0; i < regex.size()+1; i++)
        if (i == tempOP.regexPosition)
        {
            errmsg+="^";
            break;
        }
        else
            errmsg+=" ";
    throw invalid_argument(errmsg);
}

void UnbalancedBracket(op_direct tempOP, std::string regex)
{
    string errmsg = "Paréntesis desbalanceado en posición: ";
    errmsg+= to_string(tempOP.regexPosition);
    errmsg+=":\n";

    for (int i = 0; i < regex.size(); i++)
        if (regex[i])
            errmsg+=regex[i];
        else
            errmsg+="ε";

    errmsg+="\n";
    for (int i = 0; i < regex.size(); i++)
        if (i == tempOP.regexPosition)
        {
            errmsg+="^";
            break;
        }
        else
            errmsg+=" ";
    throw invalid_argument(errmsg);
}


void MissingOperand(op_direct tempOP, vector<string> regex)
{
    string errmsg = "El operador ";
    errmsg+=tempOP.literal;
    errmsg+=" en la posición ";
    errmsg+= to_string(tempOP.regexPosition);
    errmsg+= " carece de al menos un operando\n";
    
    for (int i = 0; i < regex.size(); i++)
        errmsg+=regex[i];

    errmsg += "\n";
    for (int i = 0; i < regex.size()+1; i++)
        if (i == tempOP.regexPosition)
        {
            errmsg+="^";
            break;
        }
        else
            errmsg+=" ";
    throw invalid_argument(errmsg);
}

void UnbalancedBracket(op_direct tempOP, vector<string> regex)
{
    string errmsg = "Paréntesis desbalanceado en posición: ";
    errmsg+= to_string(tempOP.regexPosition);
    errmsg+=":\n";

    for (int i = 0; i < regex.size(); i++)
        errmsg+=regex[i];


    errmsg+="\n";
    for (int i = 0; i < regex.size(); i++)
        if (i == tempOP.regexPosition)
        {
            errmsg+="^";
            break;
        }
        else
            errmsg+=" ";
    throw invalid_argument(errmsg);
}