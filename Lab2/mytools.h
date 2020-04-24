#ifndef __MY_TOOLS_H__
#define __MY_TOOLS_H__
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

void split(const string& s,vector<string>& sv,const char flag = ' ') {
    sv.clear();
    istringstream iss(s);
    string temp;

    while (getline(iss, temp, flag)) {
        sv.push_back(temp);
    }
    return;
}

bool deal(const string& s,string& name,string& id){
    vector<string> sv;
    split(s, sv, '&');
    if(sv.size()==2){
        vector<string> cv;
        split(sv[0], cv, '=');
        if(cv[0]=="Name") {
            name=cv[1];
            split(sv[1], cv, '=');
            if(cv[0]=="ID"){
                id=cv[1];
                return true;
            }
        }
    }
    return false;
}
#endif