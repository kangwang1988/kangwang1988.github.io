//
//  CodeAnalyzer.h
//  ClangCodeCheckPlugin
//
//  Created by KyleWong on 11/10/2016.
//
//

#ifndef CodeAnalyzer_h
#define CodeAnalyzer_h
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "json.hpp"

#define kAppMainEntryClass  "UIApplication"
#define kAppMainEntrySelector  "main"

using namespace std;
using namespace nlohmann;

class CodeAnalyzer{
public:
   static CodeAnalyzer *sharedInstance();
   void appendObjcClsMethodImpl(bool isInstanceMethod,string cls,string selector,string filename,unsigned rangeFrom,unsigned rangeTo,string sourcecode);
   void appendObjcMethodImplCall(bool isInstanceMethod,string cls,string selector,bool calleeIsInstanceMethod,string calleeCls,string calleeSel);
   void appendObjcCls(string cls,string supCls,vector<string> protoVec);
   void appendObjcClsInterf(string cls,bool isInstanceInterf,string selector);
   void appendObjcProto(string proto,vector<string> refProto);
   void appendObjcProtoInterf(string proto,bool isInstanceInterf,string selector);
   void appendObjcAddNotificationCall(bool isInstanceMethod,string cls,string selector,string calleeCls,string calleeSel,string notif);
   void appendObjcPostNotificationCall(bool isInstanceMethod, string cls, string selector, string notif);
    void appendObjcProtoInterfCall(bool isInstanceMethod,string cls,string selector,string proto,string protoSel);
   void synchronize();
private:
    json clsMethodJson;
    json clsInterfHierachy;
    json protoInterfHierachy;
    json clsMethodAddNotifsJson;
    json notifPostedCallersJson;
    json protoInterfCallJson;
};
#endif /* CodeAnalyzer_h */
