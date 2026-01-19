#include "system/obj/test_obj.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/object_server.h"

TestObj::TestObj(){
    BIND_CLASS(TestObj);
}
TestObj::~TestObj(){

}


void TestObj::ready(){
    DEBUG_MSG("READY");
}
void TestObj::pre_process(){
    DEBUG_MSG("PRE-PROCESS");

}
void TestObj::process(){
    DEBUG_MSG("PROCESS");

}
void TestObj::post_process(){
    DEBUG_MSG("POST-PROCESS");

}
void TestObj::draw(){
    DEBUG_MSG("POST-PROCESS");

}



