//
//  CodeCheckHelper.h
//  ClangCodeCheckPlugin
//
//  Created by KyleWong on 11/10/2016.
//
//

#ifndef CodeCheckHelper_h
#define CodeCheckHelper_h
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "json.hpp"
using namespace std;
using namespace nlohmann;

class CodeCheckHelper{
public:
   static CodeCheckHelper *sharedInstance();
   void synchronize();
   void appendObjcClsMethod(bool isInstanceMethod,string cls,string selector,string filename,unsigned rangeFrom,unsigned rangeTo,string sourcecode);
private:
//    map<string,map<string, string>> *clsMethodInfo;
    json clsMethodJson;
};
#endif /* CodeCheckHelper_h */
