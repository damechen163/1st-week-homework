#include "QuadraticEquation.h"
#include <iostream>
static const Big ZERO{false, {0}}, ONE{false, {1}}, TWO{false, {2}}, TEN{false, {0, 1}};
static const Rat R0{ZERO, ONE};
static void trim(Big& a) {
    while (a.d.size() > 1 && a.d.back() == 0) {
        a.d.pop_back();
    }
    if (a.d.size() == 1 && a.d[0] == 0) {
        a.neg = false;
    }
}
bool isZero(const Big& a) {
    return a.d.size() == 1 && a.d[0] == 0;
}
Big absBig(Big a) {
    a.neg = false;
    return a;
}
Big negate(Big a) {
    if (!isZero(a)) {
        a.neg = !a.neg;
    }
    return a;
}
Big pow10(size_t k) {
    Big a;
    a.d.assign(k + 1, 0);
    a.d[k] = 1;
    return a;
}
Big fromStr(const std::string& s) {
    Big a;
    size_t i = 0;
    if (i < s.size() && (s[i] == '+' || s[i] == '-')) {
        a.neg = (s[i] == '-');
        i++;
    }
    for (size_t j = s.size(); j-- > i; ) {
        if (s[j] >= '0' && s[j] <= '9') {
            a.d.push_back((unsigned char)(s[j] - '0'));
        }
    }
    if (a.d.empty()) {
        a.d.push_back(0);
    }
    trim(a);
    return a;
}
std::string toStr(const Big& a) {
    std::string s = a.neg ? "-" : "";
    for (size_t i = a.d.size(); i-- > 0; ) {
        s += (char)('0' + a.d[i]);
    }
    return s;
}
int cmpAbs(const Big& a, const Big& b) {
    if (a.d.size() != b.d.size()) {
        return a.d.size() < b.d.size() ? -1 : 1;
    }
    for (size_t i = a.d.size(); i-- > 0; ) {
        if (a.d[i] != b.d[i]) {
            return a.d[i] < b.d[i] ? -1 : 1;
        }
    }
    return 0;
}
Big addAbs(const Big& a, const Big& b) {
    Big r;
    int carry = 0;
    for (size_t i = 0; i < a.d.size() || i < b.d.size() || carry; i++) {
        int v = carry + (i < a.d.size() ? a.d[i] : 0) + (i < b.d.size() ? b.d[i] : 0);
        r.d.push_back((unsigned char)(v % 10));
        carry = v / 10;
    }
    trim(r);
    return r;
}
Big subAbs(const Big& a, const Big& b) {
    Big r;
    int borrow = 0;
    for (size_t i = 0; i < a.d.size(); i++) {
        int v = a.d[i] - borrow - (i < b.d.size() ? b.d[i] : 0);
        if (v < 0) {
            v += 10;
            borrow = 1;
        }
        else {
            borrow = 0;
        }
        r.d.push_back((unsigned char)v);
    }
    trim(r);
    return r;
}
Big add(const Big& a, const Big& b) {
    if (a.neg == b.neg) {
        Big r = addAbs(a, b);
        r.neg = a.neg;
        trim(r);
        return r;
    }
    int c = cmpAbs(a, b);
    if (c == 0) {
        return ZERO;
    }
    Big r = c > 0 ? subAbs(a, b) : subAbs(b, a);
    r.neg = c > 0 ? a.neg : b.neg;
    trim(r);
    return r;
}
Big sub(const Big& a, const Big& b) {
    return add(a, negate(b));
}
Big mul(const Big& a, const Big& b) {
    Big r;
    r.d.assign(a.d.size() + b.d.size(), 0);
    for (size_t i = 0; i < a.d.size(); i++) {
        int carry = 0;
        for (size_t j = 0; j < b.d.size() || carry; j++) {
            int v = r.d[i + j] + carry + (j < b.d.size() ? a.d[i] * b.d[j] : 0);
            r.d[i + j] = (unsigned char)(v % 10);
            carry = v / 10;
        }
    }
    r.neg = a.neg != b.neg;
    trim(r);
    return r;
}
void divmod(const Big& a, const Big& b, Big& q, Big& r) {
    Big A = absBig(a);
    Big B = absBig(b);
    q = ZERO;
    r = ZERO;
    if (isZero(B)) {
        return;
    }
    for (size_t i = A.d.size(); i-- > 0; ) {
        r.d.insert(r.d.begin(), A.d[i]);
        trim(r);
        unsigned char cnt = 0;
        while (cmpAbs(r, B) >= 0) {
            r = subAbs(r, B);
            cnt++;
        }
        q.d.insert(q.d.begin(), cnt);
    }
    trim(q);
    trim(r);
    q.neg = a.neg != b.neg && !isZero(q);
    r.neg = a.neg && !isZero(r);
}
Big quo(const Big& a, const Big& b) {
    Big q, r;
    divmod(a, b, q, r);
    return q;
}
Big gcd(Big a, Big b) {
    a = absBig(a);
    b = absBig(b);
    while (!isZero(b)) {
        Big q, r;
        divmod(a, b, q, r);
        a = b;
        b = r;
    }
    return a;
}
Big isqrt(const Big& n) {
    if (isZero(n)) {
        return ZERO;
    }
    Big x = pow10((n.d.size() + 1) / 2);
    for (;;) {
        Big q, r;
        divmod(n, x, q, r);
        Big y = quo(add(x, q), TWO);
        if (cmpAbs(y, x) >= 0) {
            return x;
        }
        x = y;
    }
}
Rat ri(int v) {
    return rNorm(fromStr(std::to_string(v)), ONE);
}
Rat rNorm(Big p, Big q) {
    if (q.neg) {
        p = negate(p);
        q = negate(q);
    }
    Big g = gcd(p, q);
    if (!isZero(g)) {
        p = quo(p, g);
        q = quo(q, g);
    }
    return {p, q};
}
Rat rFromStr(const std::string& s) {
    size_t sl = s.find('/');
    if (sl != std::string::npos) {
        return rNorm(fromStr(s.substr(0, sl)), fromStr(s.substr(sl + 1)));
    }
    size_t dot = s.find('.');
    if (dot != std::string::npos) {
        std::string ip = s.substr(0, dot);
        std::string fp = s.substr(dot + 1);
        if (ip.empty() || ip == "+" || ip == "-") {
            ip += "0";
        }
        return rNorm(fromStr(ip + fp), pow10(fp.size()));
    }
    return rNorm(fromStr(s), ONE);
}
std::string rStr(const Rat& a) {
    return cmpAbs(a.q, ONE) == 0 ? toStr(a.p) : toStr(a.p) + "/" + toStr(a.q);
}
Rat rAdd(Rat a, Rat b) {
    return rNorm(add(mul(a.p, b.q), mul(b.p, a.q)), mul(a.q, b.q));
}
Rat rSub(Rat a, Rat b) {
    return rNorm(sub(mul(a.p, b.q), mul(b.p, a.q)), mul(a.q, b.q));
}
Rat rMul(Rat a, Rat b) {
    return rNorm(mul(a.p, b.p), mul(a.q, b.q));
}
Rat rDiv(Rat a, Rat b) {
    return rNorm(mul(a.p, b.q), mul(a.q, b.p));
}
std::string decStr(Big num, Big den, int prec) {
    bool neg = num.neg != den.neg && !isZero(num);
    num = absBig(num);
    den = absBig(den);
    Big q, r;
    divmod(num, den, q, r);
    std::string s = toStr(q);
    std::string f;
    for (int i = 0; i <= prec && !isZero(r); i++) {
        Big d, rr;
        divmod(mul(r, TEN), den, d, rr);
        r = rr;
        f += toStr(d);
    }
    if ((int)f.size() > prec) {
        int g = f.back() - '0';
        f.pop_back();
        if (g >= 5) {
            std::string t = s + f;
            int i = (int)t.size() - 1;
            for (; i >= 0 && t[i] == '9'; i--) {
                t[i] = '0';
            }
            if (i >= 0) {
                t[i]++;
            }
            else {
                t = "1" + t;
            }
            s = t.substr(0, t.size() - f.size());
            f = t.substr(t.size() - f.size());
        }
    }
    return (neg ? "-" : "") + s + (f.empty() ? "" : "." + f);
}
void sqrtSplit(const Big& n, Big& s, Big& r) {
    s = ONE;
    r = n;
    for (Big f = TWO; f.d.size() <= 5; f = add(f, ONE)) {
        Big f2 = mul(f, f);
        if (cmpAbs(f2, r) > 0) {
            break;
        }
        Big q, rem;
        for (;;) {
            divmod(r, f2, q, rem);
            if (!isZero(rem)) {
                break;
            }
            r = q;
            s = mul(s, f);
        }
    }
}
std::string oneRoot(Big A, Big B, Big C, const Big& r, bool plus) {
    if (cmpAbs(r, ONE) == 0) {
        return rStr(rNorm(plus ? add(A, B) : sub(A, B), C));
    }
    Big b = plus ? B : negate(B);
    std::string rad = "sqrt(" + toStr(r) + ")";
    std::string co = cmpAbs(b, ONE) == 0 ? "" : toStr(absBig(b));
    std::string t = (b.neg ? "-" : "") + co + rad;
    if (isZero(A)) {
        return t + (cmpAbs(C, ONE) == 0 ? "" : "/" + toStr(C));
    }
    t = toStr(A) + (b.neg ? " - " : " + ") + co + rad;
    return cmpAbs(C, ONE) == 0 ? t : "(" + t + ")/" + toStr(C);
}
std::string oneDec(const Big& A, const Big& B, const Big& C, const Big& r, bool plus, int prec) {
    if (cmpAbs(r, ONE) == 0) {
        return decStr(plus ? add(A, B) : sub(A, B), C, prec);
    }
    int k = prec + (int)toStr(B).size() + 2;
    Big sc = pow10(k);
    Big S = isqrt(mul(r, mul(sc, sc)));
    Big num = plus ? add(mul(A, sc), mul(B, S)) : sub(mul(A, sc), mul(B, S));
    return decStr(num, mul(C, sc), prec);
}
static bool isDig(char c) {
    return (unsigned)(c - '0') < 10;
}
static bool validNum(const std::string& s) {
    size_t i = 0;
    size_t n = s.size();
    if (i < n && (s[i] == '+' || s[i] == '-')) {
        ++i;
    }
    size_t d = 0;
    while (i < n && isDig(s[i])) {
        ++i;
        ++d;
    }
    if (i < n && s[i] == '.') {
        ++i;
        size_t f = 0;
        while (i < n && isDig(s[i])) {
            ++i;
            ++f;
        }
        if (!f) {
            return false;
        }
    }
    else if (!d) {
        return false;
    }
    if (i < n && s[i] == '/') {
        ++i;
        size_t q = 0;
        bool nz = false;
        while (i < n && isDig(s[i])) {
            nz |= s[i] != '0';
            ++i;
            ++q;
        }
        if (!q || !nz) {
            return false;
        }
    }
    return i == n;
}
bool parseEquation(const std::string& s, Rat& A, Rat& B, Rat& C) {
    A = B = C = R0;
    std::vector<std::string> v;
    std::string tok;
    for (char ch : s + " ") {
        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == ',') {
            if (!tok.empty()) {
                v.push_back(tok);
                tok.clear();
            }
        }
        else {
            tok += ch;
        }
    }
    if (v.size() != 3 || !validNum(v[0]) || !validNum(v[1]) || !validNum(v[2])) {
        return false;
    }
    A = rFromStr(v[0]);
    B = rFromStr(v[1]);
    C = rFromStr(v[2]);
    return true;
}
Solution solve(const Rat& a, const Rat& b, const Rat& c) {
    Solution s;
    s.a = a;
    s.b = b;
    s.c = c;
    if (isZero(a.p)) {
        return s;
    }
    s.D = rSub(rMul(b, b), rMul(ri(4), rMul(a, c)));
    if (s.D.p.neg) {
        s.kind = Solution::NoReal;
        return s;
    }
    if (isZero(s.D.p)) {
        Rat x = rDiv(Rat{negate(b.p), b.q}, rMul(ri(2), a));
        s.kind = Solution::Double;
        s.A = x.p;
        s.B = ZERO;
        s.C = x.q;
        s.r = ONE;
        return s;
    }
    Big t, r;
    sqrtSplit(mul(s.D.p, s.D.q), t, r);
    Rat nb{negate(b.p), b.q};
    Rat num = rMul(nb, Rat{s.D.q, ONE});
    Rat den = rMul(rMul(ri(2), a), Rat{s.D.q, ONE});
    Big A = mul(num.p, den.q);
    Big B = mul(t, den.q);
    Big C = mul(num.q, den.p);
    if (C.neg) {
        A = negate(A);
        B = negate(B);
        C = negate(C);
    }
    Big g = gcd(gcd(A, B), C);
    if (!isZero(g)) {
        A = quo(A, g);
        B = quo(B, g);
        C = quo(C, g);
    }
    s.kind = Solution::Two;
    s.A = A;
    s.B = B;
    s.C = C;
    s.r = r;
    return s;
}
static std::string fmtRat(const Rat& x, bool math) {
    return math ? rStr(x) : decStr(x.p, x.q, PREC);
}
static std::string eqStr(const Solution& s, bool math) {
    const Rat* v[3] = {&s.a, &s.b, &s.c};
    const char* xs[3] = {"x^2", "x", ""};
    std::string e;
    for (int i = 0; i < 3; i++) {
        if (isZero(v[i]->p)) {
            continue;
        }
        bool neg = v[i]->p.neg;
        bool one = cmpAbs(v[i]->p, ONE) == 0 && cmpAbs(v[i]->q, ONE) == 0;
        std::string body = (one && i < 2) ? "" : fmtRat(*v[i], math);
        if (neg && !body.empty()) {
            body.erase(0, 1);
        }
        e += (e.empty() ? (neg ? "-" : "") : (neg ? " - " : " + ")) + body + xs[i];
    }
    return e.empty() ? "0" : e;
}
void printSolution(const Solution& s, bool math) {
    if (s.kind == Solution::BadFormat) {
        std::cout << "Error: Incorrect Format\n";
        return;
    }
    std::cout << "Equation: " << eqStr(s, math) << " = 0\n";
    if (s.kind == Solution::Nonquadratic) {
        std::cout << "Error: Nonquadratic\n";
        return;
    }
    std::cout << "Delta = " << fmtRat(s.D, math) << "\n";
    if (s.kind == Solution::NoReal) {
        std::cout << "No real roots\n";
        return;
    }
    int n = s.kind == Solution::Double ? 1 : 2;
    for (int i = 0; i < n; i++) {
        std::string v = math ? oneRoot(s.A, s.B, s.C, s.r, i == 0) : oneDec(s.A, s.B, s.C, s.r, i == 0, PREC);
        std::cout << (n == 1 ? "x1 = x2 = " : "x" + std::to_string(i + 1) + " = ") << v << "\n";
    }
}