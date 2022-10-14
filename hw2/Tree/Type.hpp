#pragma once

namespace Type
{
    enum class DeclType { Int, Bool, Char, String };
    enum class ExpType { Int, Bool, Char, String, Void };
    enum class OpAsgnType { Asgn, AddAsgn, SubAsgn, DivAsgn, MulAsgn };
    enum class OpBinaryMathType { Mul, Div, Mod, Add, Sub, Index };
    enum class OpBinaryCompareType { And, Or, LT, LEQ, GT, GEQ, EQ, NEQ };
    enum class OpUnaryType { Chsign, Sizeof, Question, Inc, Dec };
}
