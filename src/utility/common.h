#ifndef _COMMON_H_
#define _COMMON_H_

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <chrono>

using int32  = int32_t;
using int64  = int64_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using std::string;

using vector_string_ptr = std::shared_ptr<std::vector<std::string>>;
using vector_double_ptr = std::shared_ptr<std::vector<double>>;
using vector_datetime_ptr = std::shared_ptr<std::vector<std::chrono::system_clock::time_point>>;
using vector_int_ptr   = std::shared_ptr<std::vector<int>>;

enum STRATEGY_MODE { REAL, BACK_TEST };

// Put this in the declarations for a class to be uncopyable.
#define DISALLOW_COPY(TypeName) TypeName(const TypeName&) = delete
// Put this in the declarations for a class to be unassignable.
#define DISALLOW_ASSIGN(TypeName) TypeName& operator=(const TypeName&) = delete
// Put this in the declarations for a class to be uncopyable and unassignable.
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    DISALLOW_COPY(TypeName);               \
    DISALLOW_ASSIGN(TypeName)
// A macro to disallow all the implicit constructors, namely the
// default constructor, copy constructor and operator= functions.
// This is especially useful for classes containing only static methods.
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
    TypeName() = delete;                         \
    DISALLOW_COPY_AND_ASSIGN(TypeName)


#define TRADE_CONFIG_FILE "setting.ini"
#define INST_CONFIG_FILE "InstrumentInfo.ini"
#endif  // _COMMON_H_
