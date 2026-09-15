#pragma once
#include <string>
#include <vector>
// SCI 模式默认输出的小数位数
const int PREC = 35;

// 高精度整数：符号 + 小端十进制数位
struct Big {
    bool neg = false;              // 是否为负数
    std::vector<unsigned char> d;  // d[0] 是个位，每个元素保存一位数字
};

// 有理数 p/q，每次运算后约分，负号统一放在分子上
struct Rat {
    Big p, q;  // 分子 / 分母
};

Big fromStr(const std::string& s);       // 十进制字符串 → 大整数
std::string toStr(const Big& a);         // 大整数 → 十进制字符串
bool isZero(const Big& a);               // 是否为零
int cmpAbs(const Big& a, const Big& b);  // 比较绝对值，返回 -1 / 0 / 1
Big absBig(Big a), negate(Big a);        // 绝对值 / 相反数
Big pow10(size_t k);                     // 返回 10^k
Big addAbs(const Big& a, const Big& b);  // |a| + |b|
Big subAbs(const Big& a, const Big& b);  // |a| - |b|，要求 |a| >= |b|
Big add(const Big& a, const Big& b), sub(const Big& a, const Big& b);  // 加法 / 减法，O(n)
Big mul(const Big& a, const Big& b);     // 大整数乘法，O(n^2)
Big quo(const Big& a, const Big& b);     // 求商（丢弃余数）
void divmod(const Big& a, const Big& b, Big& q, Big& r);  ///< 带余除法，O(n^2)
Big gcd(Big a, Big b);                   // 最大公约数（欧几里得算法）
Big isqrt(const Big& n);                 // 整数平方根 floor(sqrt(n))（x - f(x)/f'(x)）

Rat ri(int v);                                        // 小整数 → 有理数
Rat rNorm(Big p, Big q);                              // 约分，并把负号归到分子
Rat rFromStr(const std::string& s);                   // 解析整数 / 小数 / 分数
std::string rStr(const Rat& a);                       // 有理数 → "p" 或 "p/q"
Rat rAdd(Rat a, Rat b), rSub(Rat a, Rat b);           // 有理数加 / 减
Rat rMul(Rat a, Rat b), rDiv(Rat a, Rat b);           // 有理数乘 / 除

std::string decStr(Big num, Big den, int prec);  // num/den → 高精度十进制（四舍五入）
void sqrtSplit(const Big& n, Big& s, Big& r);    // 分解 n = s^2 · r，r 无平方因子
std::string oneRoot(Big A, Big B, Big C, const Big& r, bool plus);  // (A ± B√r)/C 的 MATH 形式
std::string oneDec(const Big& A, const Big& B, const Big& C, const Big& r, bool plus, int prec);

//求解结果：根的情形 + 判别式 + 最简根式 x = (A ± B√r)/C
struct Solution {
    enum Kind { Nonquadratic, NoReal, Double, Two };  // 依次为 a = 0、D < 0、D = 0、D > 0
    Kind kind = Nonquadratic;  // 根的情形
    Rat a, b, c;               // 原方程系数（输出时回显用）
    Rat D;                     // 判别式 D = b^2 - 4ac
    Big A, B, C, r;            // 根式；Double 时 B = 0、r = 1，根即 A / C
};

void parseEquation(const std::string& s, Rat& A, Rat& B, Rat& C);  // 取下划线处的 a b c
Solution solve(const Rat& a, const Rat& b, const Rat& c);  // 求解
void printSolution(const Solution& s, bool math);          // 输出
