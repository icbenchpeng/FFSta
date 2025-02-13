#ifndef  DataModel_h_fast
#define  DataModel_h_fast

#include <cstddef>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

/*Load/Store structure*/

enum OpType { // 2bits
  CA = 0x0, // calc
  JP = 0x1, // jump
  LD = 0x2, // load
  ST = 0x3, // store
};
typedef uint32_t Word;
struct StIns {
  OpType  op:2;
  Word    unknown:30;
};

enum IdType {
  IdTy_Pin = 0,
};

typedef uint32_t Id;
struct StLoadStore { // low -> high
  OpType   op:2;
  IdType type:4;
  Id       id:26;
};

typedef uint16_t ShortAddr;
typedef uint32_t SchedLevel;

enum JumpType {
  JTyDirectly  = 0x0, // jump directly
  JTyNotEqual  = 0x1, // jump not equal
  JTyNotZero   = 0x2, // jump not zero
  JTyZero      = 0x3, // jump zero
  JTySched     = 0x4, // jump sched with level
};
struct StJump {
  OpType       op : 2;      // can only be JP
  JumpType   type : 6;
  SchedLevel free : 8;      //  jsc level, 0 for large level after jmp offset,
  ShortAddr  addr : 16;     // 0 for long jmp, otherwise address
};

enum PopOp {
  PopDefault = 0,
  PopManual  = 1,
};
typedef uint32_t CalcType;
struct StCalc {
  OpType       op : 2;      // can only be CA
  PopOp       pop : 1;      // if true, use manual pop of next word
  CalcType   type : 30;
};

#ifdef __cplusplus
} // end extern "C"
#endif

#endif /*DataModel_h_fast*/
