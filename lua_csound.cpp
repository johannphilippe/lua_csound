#include <iostream>
#include<chrono>
#include<unordered_map>
#include"mlua.hpp"
#include<plugin.h>
#include<OpcodeBase.hpp>
#include<mutex>
#include"csdl.h"

using namespace std;

csnd::AuxMem<Lua> l;
static Lua *lua = nullptr;
static std::unordered_map<std::string, int> objList;
void * mtx;

struct LuaInit : csnd::InPlug<0> {
    csnd::AuxMem<Lua> instance;
    int init() {
        if (lua == nullptr) {
            mtx = csound->get_csound()->Create_Mutex(0);
            csound::LockGuard criticalSection(csound->get_csound(), mtx);
            lua = new Lua();
            std::string params="ksmps = " + std::to_string(int(ksmps())) + " \nsr = " + std::to_string(int(sr())) + "\ncurrent_sample = nil \n"  + "\n\n";
            lua->pushLightUserData(csound->get_csound());
            lua->setGlobal("csound");

            lua->newTable();
            lua->setGlobal("global_values");

            lua->enableJIT();
        }
        return OK;
    }
};

struct LuaDoFile : csnd::Plugin<1,1> {
    int init() {
        if (lua == nullptr) {
            mtx = csound->get_csound()->Create_Mutex(0);
            csound::LockGuard criticalSection(csound->get_csound(), mtx);
            lua = new Lua();
            std::string params="ksmps = " + std::to_string(int(ksmps())) + " \nsr = " + std::to_string(int(sr())) + "\ncurrent_sample = nil \n"  + "\n\n";
            lua->pushLightUserData(csound->get_csound());
            lua->setGlobal("csound");

            lua->newTable();
            lua->setGlobal("global_values");

            lua->enableJIT();
        }
        //luaJIT_setmode(lua->getLuaState(), -1, LUAJIT_MODE_ALLSUBFUNC | LUAJIT_MODE_ON);
        outargs[0] = lua->doFile(inargs.str_data(0).data);


        std::cout << "DoFile done " << std::endl;
        return OK;
    }
};

struct LuaDoString : csnd::Plugin<1, 1>
{
  int init()
  {
        if (lua == nullptr) {
            mtx = csound->get_csound()->Create_Mutex(0);
            csound::LockGuard criticalSection(csound->get_csound(), mtx);
            lua = new Lua();
            std::string params="ksmps = " + std::to_string(int(ksmps())) + " \nsr = " + std::to_string(int(sr())) + "\ncurrent_sample = nil \n"  + "\n\n";
            lua->pushLightUserData(csound->get_csound());
            lua->setGlobal("csound");

            lua->newTable();
            lua->setGlobal("global_values");

            lua->enableJIT();
        }

        std::cout << inargs.str_data(0).data << std::endl;
       outargs[0] = lua->doString(inargs.str_data(0).data);
      return OK;
  }
};


struct LuaTableRead : csnd::Plugin<1, 64>
{
    int init() {
        csound::LockGuard criticalSection(csound->get_csound(), mtx);

        lua->getGlobal(inargs.str_data(0).data);
        std::cout << "retrieved "<< inargs.str_data(0).data << std::endl;

        std::cout << "global ok " << std::endl;
        std::cout << "table len " << lua_objlen(lua->getLuaState(), -1) << std::endl;
        for(size_t i = 1; i < in_count() - 1; ++i)
        {
            std::cout << "rawgeti " <<  i << " "  << in_count() - 1 << " " << inargs[i] <<  std::endl;
            lua->rawGeti(-1, inargs[i]);
        }

        lua->rawGeti(-1, inargs[in_count() - 1]);

        outargs[0] = lua->toNumber(-1);

        return OK;
    }
};

struct LuaPushGlobalArray : csnd::InPlug<2> {
    int init() {
        csound::LockGuard criticalSection(csound->get_csound(), mtx);

        csnd::Vector<MYFLT>::iterator it = args.vector_data<MYFLT>(1).begin();
         lua->getGlobal("global_values");
         lua->newTable();

         int idx = 1;
         for (;it != args.vector_data<MYFLT>(1).end(); ++it, idx++) {
             lua->pushNumber(idx);
             lua->pushNumber(*it);
             lua->setTable(-3);
         }
         lua->setField(-2, args.str_data(0).data);
         return OK;
    }
};


struct LuaPushGlobalString : csnd::InPlug<2> {
    int init() {
        csound::LockGuard criticalSection(csound->get_csound(), mtx);
        lua->getGlobal("global_values");
        lua->pushString(args.str_data(1).data);
        lua->setField(-2, args.str_data(0).data);
        return OK;
    }
};

struct LuaExecute : csnd::Plugin<1,1> {
    int init() {
        csound::LockGuard criticalSection(csound->get_csound(), mtx);
        outargs[0] = lua->loadCString(inargs.str_data(0).data);
        return OK;
    }
};

struct LuaCall : csnd::Plugin<1, 2> {
    int init() {
        //std::cout << "Init lua_call" << std::endl;
        csound::LockGuard criticalSection(csound->get_csound(), mtx);
        std::string func = std::string(inargs.str_data(0).data);
        lua->getGlobal(func);

        //std::cout << "Init lua_call" << std::endl;

        csnd::Vector<MYFLT>::iterator it= inargs.vector_data<MYFLT>(1).begin();
        if(inargs.str_data(0).data == NULL) {
            //std::cout << "Lua --- NULLSTRING " << std::endl;
        } else {
            //std::cout << "Lua --- Not NULL string " << std::endl;
        }
        for(;it!=inargs.vector_data<MYFLT>(1).end(); ++it) {
                lua->pushNumber(*it);
        }

        //std::cout << "about to call pcall" << std::endl;
        lua->pcall(inargs.vector_data<MYFLT>(1).len(), 1);

        //std::cout << "called" << std::endl;

        outargs[0] = lua->toNumber(-1);
        lua->pop(1);

        //std::cout << "ok for init " << std::endl;
        return OK;
    }

    int kperf() {
        csound::LockGuard criticalSection(csound->get_csound(), mtx);
        if(lua->checkStack(1)==0) lua->garbageCollector(LUA_GCCOLLECT);
        std::string func = std::string(inargs.str_data(0).data);
        lua->getGlobal(func);
        csnd::Vector<MYFLT>::iterator it= inargs.vector_data<MYFLT>(1).begin();
        for(;it!=inargs.vector_data<MYFLT>(1).end(); ++it) {
                lua->pushNumber(*it);
        }

        lua->pcall(inargs.vector_data<MYFLT>(1).len(), 1);
        outargs[0] = lua->toNumber(-1);
        lua->pop(1);
        return OK;
    }

    int aperf() {
        //std::cout << "Aperf " << std::endl;
        csound::LockGuard criticalSection(csound->get_csound(), mtx);
        if(lua->checkStack(1)==0) lua->garbageCollector(LUA_GCCOLLECT);
        std::string func = std::string(inargs.str_data(0).data);
        int len = inargs.vector_data<MYFLT>(1).len();

        //std::cout << "Len " << len << std::endl;
        ARRAYDAT *data = (ARRAYDAT*) inargs(1);
        int size = (*data->sizes);

        //std::cout << "size ok " << size << std::endl;

        lua->getGlobal(func);
        //std::cout << "offset = " << offset << std::endl;
        for(int id = 0; id < size; id++) 
        {
            //std::cout << "new table : " << std::endl;
            lua->newTable();
            for(size_t i = offset; i < nsmps; i++)
            {
                lua->pushNumber(data->data[i + id * (int) ksmps()]);
                lua->rawSeti(-2, i+1);
            }
            //std::cout << "endofloop" << std::endl;
        }
        //std::cout << "Pcall " << std::endl;
        lua->pcall(len, 1);
        //std::cout << "Lua  pcall error : " << err << std::endl;
        //lua->isError(err);
        for(size_t i = 0; i < nsmps; i++)
        {
            //std::cout << "retrieve res nsmps = " << i  << std::endl;
            lua->rawGeti(-1, i + 1);
            //lua->rawGeti(-1, i );
            //std::cout << "rawgeti ok " << std::endl;
            lua_Number n = lua->toNumber(-1);
            //std::cout << "number retrieved : " <<  n <<std::endl;
            outargs(0)[i] = n;
            lua->pop(1);
        }
        
        return OK;
    }

};

struct LuaString : csnd::Plugin<1,2>{

    int init() {
        csound::LockGuard criticalSection(csound->get_csound(), mtx);
       lua->getGlobal(inargs.str_data(0).data);

       csnd::Vector<STRINGDAT>::iterator it = inargs.vector_data<STRINGDAT>(1).begin();
       int size = inargs.vector_data<STRINGDAT>(1).len();

       std::cout << "Lua string array size : " << size << std::endl;

        for(;it != inargs.vector_data<STRINGDAT>(1).end(); ++it) {
            std::cout << "Lua -- string : " << it->data << std::endl;
            lua->pushString(it->data);
        }

       lua->pcall(size,1);

       outargs.str_data(0).data = lua->toCString(-1);
       lua->pop(1);

        return OK;
    }


    int kperf() {
        csound::LockGuard criticalSection(csound->get_csound(), mtx);
        lua->getGlobal(inargs.str_data(0).data);

       csnd::Vector<STRINGDAT>::iterator it = inargs.vector_data<STRINGDAT>(1).begin();
       int size = inargs.vector_data<STRINGDAT>(1).len();


        for(;it != inargs.vector_data<STRINGDAT>(1).end(); ++it) {
            lua->pushString(it->data);
        }

       lua->pcall(size,1);

       outargs.str_data(0).data = lua->toCString(-1);
       lua->pop(1);
        return OK;
    }
};


struct LuaNString : csnd::Plugin<1,2> {
    int init() {
        csound::LockGuard criticalSection(csound->get_csound(), mtx);
        lua->getGlobal(inargs.str_data(0).data);
        csnd::Vector<MYFLT>::iterator it = inargs.vector_data<MYFLT>(1).begin();
        int size = inargs.vector_data<MYFLT>(1).len();

        for(;it != inargs.vector_data<MYFLT>(1).end(); ++it) {
            lua->pushNumber(*it);
        }
        lua->pcall(size,1);

       outargs.str_data(0).data = lua->toCString(-1);
       lua->pop(1);
        return OK;
    }

    int kperf() {
        csound::LockGuard criticalSection(csound->get_csound(), mtx);
        lua->getGlobal(inargs.str_data(0).data);

        csnd::Vector<MYFLT>::iterator it = inargs.vector_data<MYFLT>(1).begin();
        int size = inargs.vector_data<MYFLT>(1).len();

        for(;it != inargs.vector_data<MYFLT>(1).end(); ++it) {
            lua->pushNumber(*it);
        }
        lua->pcall(size,1);

        outargs.str_data(0).data = lua->toCString(-1);
       lua->pop(1);
        return OK;
    }

};



struct LuaStringN : csnd::Plugin<1,2> {

    int init() {
        csound::LockGuard criticalSection(csound->get_csound(), mtx);
        lua->getGlobal(inargs.str_data(0).data);

          csnd::Vector<STRINGDAT>::iterator it = inargs.vector_data<STRINGDAT>(1).begin();
          for(;it != inargs.vector_data<STRINGDAT>(1).end(); ++it) {
              lua->pushString(it->data);
          }

          lua->pcall(inargs.vector_data<STRINGDAT>(1).len(),1);
          outargs[0] = lua->toNumber(-1);

          lua->pop(1);
          return OK;
    }

    int kperf() {
        csound::LockGuard criticalSection(csound->get_csound(), mtx);
        lua->getGlobal(inargs.str_data(0).data);

          csnd::Vector<STRINGDAT>::iterator it = inargs.vector_data<STRINGDAT>(1).begin();
          for(;it != inargs.vector_data<STRINGDAT>(1).end(); ++it) {
              lua->pushString(it->data);
          }

          lua->pcall(inargs.vector_data<STRINGDAT>(1).len(),1);
          outargs[0] = lua->toNumber(-1);

          lua->pop(1);
          return OK;
    }
};




struct LuaObj : csnd::Plugin<1,3> {

   int objnumber;

   csnd::AuxMem<char> classname;
   csnd::AuxMem<char> objname;
   //try a destructor to see if I can remove object from memory, and remove its references in Lua.

   int deinit() {
        lua->pushNil();
        lua->setGlobal(objname.data());
        std::cout << "Delete object" << endl;
       return OK;
   }

    int init() {
        csound::LockGuard criticalSection(csound->get_csound(), mtx);

        if(lua->checkStack(nsmps)==0) lua->garbageCollector(LUA_GCCOLLECT);
        classname.allocate(csound, inargs.str_data(0).size);
        objname.allocate(csound, inargs.str_data(0).size + 10);
        ::memcpy(classname.data(), inargs.str_data(0).data, sizeof(char) * inargs.str_data(0).size);

       if(objList.find(classname.data()) == objList.end()) {
           objList[classname.data()] = 1;
       } else {
           objList[classname.data()] +=1;
       }

       objnumber = objList[classname.data()];
       string on = inargs.str_data(0).data + std::to_string(objnumber);
       ::memcpy(objname.data(), on.data(), sizeof(char) * on.size());

       lua->getGlobal(classname.data());
       lua->getField(-1,"new");
       lua->getGlobal(classname.data());
       lua->pcall(1,1);
       lua->setGlobal(objname.data());

       lua->getGlobal(objname.data());
       lua->pushNumber(sr());
       lua->setField(-2, "sr");
       lua->pushNumber(ksmps());
       lua->setField(-2, "ksmps");

       lua->getField(-1, "init");
       lua->getGlobal(objname.data());

       int len = inargs.vector_data<MYFLT>(1).len();
       for(int i = 0; i < len; i++) {
           lua->pushNumber(inargs.vector_data<MYFLT>(1)[i]);
       }
       lua->remove(-(len + 3));

       lua->pcall(1 + len,0);

       lua->pop(1);
        return OK;
    }

    int kperf() {
        csound::LockGuard criticalSection(csound->get_csound(), mtx);

        lua->getGlobal(objname.data());
        lua->getField(-1,"kperf");
        lua->pushValue(-2);

        const size_t len = inargs.vector_data<MYFLT>(2).len();
        for(size_t i=0; i<len; i++) {
            lua->pushNumber(inargs.vector_data<MYFLT>(2)[i]);
        }

        lua->remove(-(len + 3));

        lua->pcall(inargs.vector_data<MYFLT>(2).len() + 1, 1);

        outargs[0] = lua->toNumber(-1);
        lua->pop(1);
        return OK;
    }

    int aperf() {
        csound::LockGuard criticalSection(csound->get_csound(), mtx);

        ARRAYDAT *data = (ARRAYDAT *)inargs(2);

        int len = data->sizes[0];

        lua->getGlobal(objname.data());
        lua->getField(-1, "aperf");
        lua->pushValue(-2);

        for(int id = 0; id < len; id++) {
            lua->newTable();
            for(size_t i = 0; i < nsmps; ++i) {
                lua->pushNumber(data->data[i + id * (int)ksmps()]);
                lua->rawSeti(-2, i + 1);
            }
        }

        lua->remove(-(len + 3));
        bool res = lua->pcall(len + 1, 1);
        if(!res) 
        {
            std::cout <<  "Lua error " << lua->toString(-1) << std::endl;
            lua->pop(2);
            return NOTOK;
        }

        if(!lua->isTable(-1)) 
        {
            std::cout << "Lua - aperf output must be a table and is : " <<  lua->getType(-1) << std::endl;
            lua->pop(2);
            return NOTOK;
        }

        for(size_t i = 0; i < nsmps; i++) {
            lua->rawGeti(-1, i + 1);
            lua_Number n = lua->toNumber(-1);
            outargs(0)[i] = n;
            lua->pop(1);
        }
        return OK;
    }
};

struct LuaObjArr : csnd::Plugin<1,4> {

   int objnumber;

    csnd::AuxMem<char> classname;
    csnd::AuxMem<char> objname;
    csnd::AuxMem<MYFLT> audio_mem;
    size_t out_cnt;
   //try a destructor to see if I can remove object from memory, and remove its references in Lua.

   int deinit() {
        lua->pushNil();
        lua->setGlobal(objname.data());
        std::cout << "Delete object" << endl;
       return OK;
   }

    int init() {
        csound::LockGuard criticalSection(csound->get_csound(), mtx);
        auto &ain = inargs.vector_data<csnd::AudioSig>(2);

        out_cnt = (inargs[3] > 0) ? inargs[3] : 16;
        // Problem
        MYFLT * o_arg = outargs(0);

        // This condition is not so cool > Checking if it is audio
        if( ain.len() > 0  && ain[0].GetNsmps() == nsmps) // Aperf
        {
            csnd::Vector<csnd::AudioSig> &out = outargs.vector_data<csnd::AudioSig>(0);
            out.init(csound, out_cnt);
            audio_mem.allocate(csound, ksmps() * out_cnt);
            MYFLT* ptr = audio_mem.data();
            for(size_t i = 0; i < out_cnt; ++i)
            {
                size_t mem_idx = i * ain[0].GetNsmps();
                auto it = out[i].begin();
                //out[i] = csnd::AudioSig(this, ptr, true);
                ptr += size_t(ksmps());
            }
        } 
        else  // Kperf 
        {
            // K
            csnd::Vector<MYFLT> &out = outargs.myfltvec_data(0);
            out.init(csound, out_cnt);
        }
        // Check 

        if(lua->checkStack(nsmps)==0) lua->garbageCollector(LUA_GCCOLLECT);
        classname.allocate(csound, inargs.str_data(0).size);
        objname.allocate(csound, inargs.str_data(0).size + 10);
        ::memcpy(classname.data(), inargs.str_data(0).data, sizeof(char) * inargs.str_data(0).size);

       if(objList.find(classname.data()) == objList.end()) {
           objList[classname.data()] = 1;
       } else {
           objList[classname.data()] +=1;
       }

       objnumber = objList[classname.data()];
       string on = inargs.str_data(0).data + std::to_string(objnumber);
       ::memcpy(objname.data(), on.data(), sizeof(char) * on.size());

       lua->getGlobal(classname.data());
       lua->getField(-1,"new");
       lua->getGlobal(classname.data());
       lua->pcall(1,1);
       lua->setGlobal(objname.data());

       lua->getGlobal(objname.data());
       lua->pushNumber(sr());
       lua->setField(-2, "sr");
       lua->pushNumber(ksmps());
       lua->setField(-2, "ksmps");

       lua->getField(-1, "init");
       lua->pushValue(-2);

       int len = inargs.vector_data<MYFLT>(1).len();
       for(int i = 0; i < len; i++) {
           lua->pushNumber(inargs.vector_data<MYFLT>(1)[i]);
       }
       lua->remove(-(len + 3));

       lua->pcall(1 + len,0);

       lua->pop(1);
        return OK;
    }

    int kperf() {
        csound::LockGuard criticalSection(csound->get_csound(), mtx);
        
        csnd::Vector<MYFLT> &out = outargs.myfltvec_data(0);

        lua->getGlobal(objname.data());
        lua->getField(-1,"kperf");

        lua->pushValue(-2); // Push object 

        size_t len = inargs.vector_data<MYFLT>(2).len();
        for(size_t i=0; i< len; i++) {
            lua->pushNumber(inargs.vector_data<MYFLT>(2)[i]);
        }

        lua->remove( -(3 + len) );

        lua->pcall(len + 1, 1);

        if(!lua->isTable(-1)) {
            std::cout << "Error, Return value from Lua should be a table" << std::endl;
            std::cout << "current type :: " << lua->getType(-1) << std::endl;
            return NOTOK;
        }

        int nres = lua->rawLen(-1);
        
        if(nres > out_cnt) 
        {
            std::cout << "Error, too many outputs from Lua, increase the size of array with 4th arg" << std::endl;
            return NOTOK;
        } 

        for(size_t i = 0; i < nres; ++i) 
        {
            lua->rawGeti(-1, i + 1);
            out[i] = lua->toNumber(-1); //lua_tonumber(lua->getLuaState(), base + i + 1);
            lua->pop(1);
        }
        lua->pop(1);
        return OK;
    }

    int aperf() {
        csound::LockGuard criticalSection(csound->get_csound(), mtx);
        
        ARRAYDAT *data = (ARRAYDAT *)inargs(2);
        ARRAYDAT *out = (ARRAYDAT *)outargs(0);

        int len = data->sizes[0];
        lua->getGlobal(objname.data());
        lua->getField(-1, "aperf");
        lua->pushValue(-2);

        for(int id = 0; id < len; id++) {
            lua->newTable();
            for(size_t i = offset; i < nsmps; i++) {
                lua->pushNumber(data->data[i + id * (int)ksmps()]);
                lua->rawSeti(-2, i + 1);
            }
        }
        //lua->dump_stack();
        lua->remove(-(len + 3));
        //lua->dump_stack();
        bool res = lua->pcall(len + 1, 1);
        //lua->dump_stack();

        if(!res) 
        {
            std::cout <<  "Lua error " << lua->toString(-1) << std::endl;
            lua->pop(2);
            return NOTOK;
        }

        if(!lua->isTable(-1)) 
        {
            std::cout << "Lua - aperf output must be a table and is : " <<  lua->getType(-1) << std::endl;
            lua->pop(2);
            return NOTOK;
        }
        
        //lua->dump_stack();
        int nres = lua->rawLen(-1);

        const size_t out_ch = outargs.vector_data<csnd::AudioSig>(0).len();

        if(nres > out_ch) 
        {
            std::cout << "Error, too many outputs from Lua, increase the size of array with 4th argument" << std::endl;
            return NOTOK;
        } 

        for(size_t ch = 0; ch < nres; ++ch )
        {
            lua->rawGeti(-1, ch + 1);

            for(size_t n = 0; n < nsmps; ++n)
            {
               
                lua->rawGeti(-1, n + 1);
                double smp = lua->toNumber(-1);
                lua->pop(1);
                
                out->data[n + ch * (int)ksmps()] = smp;
            }
            lua->pop(1);
        }
        int top = lua->getTop();
        lua->pop(top);
        return OK;
    }

};

#include<modload.h>

void csnd::on_load(Csound *csound) {
    std::cout << "Loading lua module " << std::endl;
    //init Lua state _ can only be called once
  csnd::plugin<LuaInit>(csound, "lua_init", "" , "P",  csnd::thread::i);

    // execute a lua file - returns error code
  csnd::plugin<LuaDoFile>(csound, "lua_dofile", "i" , "S",  csnd::thread::i);

  // Executes lua string
  csnd::plugin<LuaDoString>(csound, "lua_dostring", "i", "S", csnd::thread::i);

        //executes a lua string - returns error code
  csnd::plugin<LuaExecute>(csound, "lua_exec","i","S", csnd::thread::i);

  csnd::plugin<LuaTableRead>(csound, "lua_table_read", "i", "Sm", csnd::thread::i);

        // number to number
  csnd::plugin<LuaCall>(csound, "lua_call","i","Si[]",csnd::thread::i);
  csnd::plugin<LuaCall>(csound, "lua_call","k","Sk[]",csnd::thread::k);
  csnd::plugin<LuaCall>(csound, "lua_call","a","Sa[]",csnd::thread::a);

        // string to string
  csnd::plugin<LuaString>(csound, "lua_str", "S", "SS[]", csnd::thread::i);
  csnd::plugin<LuaString>(csound, "lua_str", "S", "SS[]", csnd::thread::k);

        // number to string - for string concatenations facilities
  csnd::plugin<LuaNString>(csound, "lua_nstr", "S", "Si[]", csnd::thread::i);
  csnd::plugin<LuaNString>(csound, "lua_nstr", "S", "Sk[]", csnd::thread::k);

        // string to number strn (ntom for example)
  csnd::plugin<LuaStringN>(csound, "lua_strn", "i", "SS[]", csnd::thread::i);
  csnd::plugin<LuaStringN>(csound, "lua_strn", "k", "SS[]", csnd::thread::k);

        // lua obj
  csnd::plugin<LuaObj>(csound, "lua_obj", "k", "Si[]k[]", csnd::thread::ik, 1);
  csnd::plugin<LuaObj>(csound, "lua_obj", "a", "Si[]a[]", csnd::thread::ia, 1);
  // missing another opcode with audio output and control input (and another for both inputs ?)

  // Same obj with array return 
  csnd::plugin<LuaObjArr>(csound, "lua_obj_arr", "k[]", "Si[]k[]o", csnd::thread::ik, 1);
  csnd::plugin<LuaObjArr>(csound, "lua_obj_arr", "a[]", "Si[]a[]o", csnd::thread::ia, 1);

  csnd::plugin<LuaPushGlobalArray>(csound, "lua_push_array","", "Si[]", csnd::thread::i);
  csnd::plugin<LuaPushGlobalString>(csound, "lua_push_string", "","SS",csnd::thread::i);

}
