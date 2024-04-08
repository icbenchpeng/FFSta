#ifndef  DataModel_h_fast
#define  DataModel_h_fast

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

enum IdType {
  IdTy_Pin = 0,
};

typedef uint32_t Id;
struct StLoadStore { // low -> high
  OpType   op:2;
  IdType type:4;
  Id       id:26;
};

#ifdef __cplusplus
} // end extern "C"
#endif

#endif /*DataModel_h_fast*/
