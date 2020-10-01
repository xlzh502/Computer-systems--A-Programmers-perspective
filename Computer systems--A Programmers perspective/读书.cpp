
#include <string>
#include <assert.h>
#include <iostream>



typedef  char const* byte_pointer;

std::string byteHex(unsigned char byte)
{
    const char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    std::string c;
    c += hex[byte / 16];
    c += hex[byte % 16];
    return c;
}

std::string show_bytes(byte_pointer buf, int cnt)
{ // P153:    2.55,  2.66, 2.57, 
    std::string result = "";
    for (int i = 0; i < cnt; i++)
        result += byteHex(buf[i]);
    return result;
}

void inplace_swap(int*  x, int *y)
{
    *x = *x ^ *y;
    *y = *x ^ *y;
    *x = *y ^ *x;
}


// TEMPLATE CLASS is_unsigned

/*template<class _Ty>
struct is_unsigned
    : std::_Sign_base<_Ty>::_Unsigned
{	// determine whether _Ty is an unsigned type
public:
    is_unsigned(_Ty& val) {}
};*/

// 这里声明这个模板函数，而不是上面的模板类，就是为了利用如下特性：模板函数 可以根据 实参的类型，来 推断模板的参数； 但是，模板类却不行。 因此，上面的模板类被注释掉了。
// 有关模板函数 的这一特性，参考《C++程序设计语言（特别版）》第13.3.1节
template <typename T>
bool is_unsigned(const T& t)
{
    return std::is_unsigned<T>::value;
}

// is_longlong 利用了 模板函数的重载、模板函数的专门化
template <typename T>
bool is_longlong(const T& t)
{
    return false;
}

template<>
bool is_longlong(const long long& t)
{
    return true;
}

//本打算仿照is_longlong  复制、粘贴个 is_int 模板函数，但是发现这样写个宏，更好些。
#define IS_TYPE(TYP)  template <typename T>  bool is_##TYP(const T & t) {  return false ;} \
                                                                            bool is_##TYP(const TYP& t) {   return true ;}

IS_TYPE(int) // is_int 模板函数
IS_TYPE(long)

#define IS_TYPE(a,b,c)  template <typename T>  bool is_##a##b##c(const T& t) {  return false ;} \
                                                                            bool is_##a##b##c(const a b c & t) {   return true ;}
IS_TYPE(unsigned, long, long)  // is_unsignedlonglong
IS_TYPE(unsigned, long, )  // is_unsignedlong
IS_TYPE(unsigned, int, )  // is_unsignedint

int uadd_ok(unsigned x, unsigned y)  // u == unsigned
{ // P115,  Practice Problem 2.27
    unsigned result = x + y;
    if (result < x)
        throw std::exception("overflow");
    else
        return result;
}

int tadd_ok(int x, int y) // t == two's complement
{  // P120,  Practice Problem 2.30
    int result = x + y;
    if (result <=-2 && x > 0 && y > 0) // 这里需要注意 result <=-2, 书中  P120   提到是result < 0 虽然不错，但是<=-2更加精确
        throw std::exception("positive overflow");
    else if (result >= 0 && x < 0 && y < 0)  // 这里需要注意 result >= 0, 而不是result > 0. 原因是： P117 对于 -2^(w-1) <= x, y < 2^(w-1)来说， x+y的取值范围是 [ **, ** )  前开后闭区间
        throw std::exception("negative overflow");
    else
        return result;
}

int tsub_ok(int x, int y) // P121  : Practice problem 2.32
{
    // 书中给出的答案是：如果y是TMIN，那么若x是负数，则tadd_ok会抛出异常，但是tsub_ok却不会；我的想法是若y是TMIN，若x是正数，tsub_ok会抛异常，但是tadd_ok却不会
    int result = x - y;
    if (result >= 1 && x < 0 && y>0)
        throw std::exception("negative overflow");
    else if (result <= -1 && x > 0 && y < 0)
        throw std::exception("positive overflow");
    else
        return result;
}

int tmul_ok(int x, int y)
{
#if 笨蛋的实现方法
    long long result = (long long)x*y;
    int high32 = result >> 32; // 取高32位的补码的值
    int low32 = (result << 32) < 0; // 取低32位的 最高位 符号位的值，是1还是0
    if (high32 + low32 == 0)  // 没有溢出，则 根据 Practice Problem 2.35的证明，相加是0
        return x*y;
    else
        throw std::exception("overflow");
#else
    long long result = (long long)x*y;
    if ((int)result != result)
        throw  std::exception("overflow");
    else
        return x*y;
#endif
}

int simpl_mult(int x, int y)
{ // P128   Form A
    int result = 0;
    int beishu = (y > 0) ? y : (-y);
    int mult = 0;
    while ((beishu >>1) != 0)
    {
        result += (beishu % 2) ? (x<<mult) : 0;
        beishu = beishu >> 1;
        mult +=1;
    }
    result += beishu ? (x<<mult) : 0;
    result = (y < 0) ? -result : result;
    return result;
}

int div16(int x)
{
    // P131   Practice problem 2.42
    int diff = (((x + x)<<27) >>27) >> 1;  // 这个写法，有个问题，就是x+x可能会有溢出，因此，是不安全的。
    return (x - diff) >> 4;
}

unsigned f2u(float* x)
{
    // Problem 2.83
    return  *((unsigned*)x);
}

int float_ge(float x, float y)
{
    // Problem 2.83
    // P141  浮点数比较大小，无需进行浮点运算；只需按照unsigned integer来进行比较； 负数，则是 绝对值越大则越小。
    unsigned ux = f2u(&x);
    unsigned uy = f2u(&y);

    unsigned sx = ux >> 31;
    unsigned sy = uy >> 31;

    return  ((ux << 1) == 0 && (uy << 1) == 0) ||
        (!sx && sy) ||
        (!sx && !sy && ux >= uy) ||
        (sx && sy && ux <= uy);
}

void  prob_2_54()
{
    // P151   Practice problem 2.54

    //   x == (int)(float)x  的反例
    int x = 0x1000001;
    assert((int)(float)x  == 0x1000000);  // Round-to-even
    x = 0x2000003;
    assert((int)(float)x == 0x2000004);  // round-up
    x = 0x2000006;
    assert((int)(float)x == 0x2000008);  // round-to-even
    x = 0x2000001;
    assert((int)(float)x == 0x2000000); // round-down

    //  (f+d)-f == d  的反例
    double d = 9007199254740994L;  // 2^53  == 9007199254740992  ;    而    9007199254740994 = 2^53  +  ε  ， 其中 ε = (2^(-52)) * (2^53) = 2 
    float f = 1.0;
    assert((f + d) - f != d);  // f+d导致 round-to-even成为 d+2， 而 (f+d)-f的结果本应该是d+1，但是，再次出发round-to-even所以是d+2;

    d = 1e20;  // 1e20 == 2^(66.34),  所以  ε = 2^(66-52) = 2^14
    f = 8192; // 8192 == 2^13 == (2^(66-52)) / 2 ==  ε/2
    assert((f+d) - d == 0);  //因为ε/2是halfway between，所以f+d发生了round-to-even, 由于1e20可以被 2^(66-52)整除，所以断定无精度损失；由于1e20 mod （2^(66-52+1)）== 0 ，所以断定fraction的最后一位 是0，所以round-to-even导致f+d依然等于1e20 
    f = 8192 + 1;  // 8192 + 1 不再是 halfway between,所以 f+d发生了Round-up， 因此 (f + d) 与 d之间 相差一个 ε
    assert((f + d) - d == 2*8192); // f+d 发生了Round-up, 所以 (f+d)-d  等于  ε


}

unsigned prob_2_59(unsigned x, unsigned y)
{
    return (x & 0xffffff00) | (y & 0xff);
}

bool is_big_endian()
{
    // Problem 2.58
    char number[4] = {0x12, 0x34, 0x56, 0x78};
    unsigned a = *(unsigned*)number;
    return a == 0x12345678;  // Big endian的机器，高位数字存在低地址； small endian则是 低位数字存在低地址。
}

unsigned put_byte(unsigned x, unsigned char b, int i)
{
    // problem 2.60
    unsigned temp = ((unsigned)b) << (i * 8);
    unsigned mask = 0xff << (i * 8);
    return  (x & (~mask)) | temp;
}

int main(int argc, char* argv[])
{
    int i = 12345;
    assert(show_bytes((byte_pointer)&i, sizeof(i)) == "39300000");  // P79  Figure 2.6

    std::string str = "abcdef";
    assert(show_bytes(str.c_str(), str.length()) == "616263646566"); // P80  Practice Problem 2.7;    Text data is more platform-independent than binary data.

    int j = 98765;
    inplace_swap(&i, &j);
    assert(i == 98765 && j == 12345);   // P85  Practice problem 2.10

    /*
   P89  Aside  C++编译器处理的逻辑 和 C编译器不太一样。 
    int k = 0xfedcba98 << 32;
    assert(0xfedcba98 << 32 == 0xfedcba98);
    assert(0xfedcba98 >> 36 == 0xffedcba9);
    assert(0xfedcba98U >> 40 == 0x00fedcba); */
    
    assert(1 << 2 + 3 << 4 == 1 << (2 + 3) << 4); // P90  Aside 运算符优先级的陷阱


    //  P99 : conversion between signed and unsigned  ----   keep the bit values identical but change how these bits are interpreted.
    //  P100:   General rule for how C handle conversion between signed and unsigned numbers with same word size ---- the numeric values might change , but the bit patterns do not.
    short  v = -12345;
    unsigned short uv = v;
    assert(show_bytes((byte_pointer)&v, sizeof(short)) == show_bytes((byte_pointer)&uv, sizeof(unsigned short)));

    int m = -1;
    unsigned int n = UINT_MAX + m+1;  // P101:   公式（2.5）  给出一个负数（有符号数，且负数），求与它具有相同 位模式 的 无符号数。
    assert(show_bytes((byte_pointer)&m, sizeof(int)) == show_bytes((byte_pointer)&n, sizeof(unsigned int)));

    // P105:  Figure 2.18   几个违反常理的情况
    //  编译的时候需要去掉SDL检查，否则，会报 “C4146：一元负运算符应用于无符号类型，结果仍为无符号类型” 错误。 
    assert(-1 < 0U == false);
    assert(2147483647U > -2147483647 - 1 == false);
    assert(2147483647 > (int)2147483648U == true);
    assert(is_unsigned(0x80000000) == true);
    assert(is_unsigned(-2147483647 - 1) == false);

    if (sizeof(char*) == 8)
    {
        // 64位机器
        
        assert(is_unsignedlong(-2147483648) == true);
        assert(is_int(-0x7FFFFFFF - 1) == true);
        if (sizeof(long) == 4)
        {
            // Visual C++ 2013:   sizeof(long) == 4。 
            assert(is_unsignedlonglong(-9223372036854775808));
            assert(is_unsignedlonglong(0x8000000000000000));
        }
        else if (sizeof(long) == 8)
        {
            // GCC  ?
            assert(is_unsignedlong(-9223372036854775808));
            assert(is_unsignedlong(0x8000000000000000));
        }
    }
    else if (sizeof(char*) == 4)
    {
         // 32位机器
        assert(sizeof(long) == 4 && sizeof(long long) == 8);   // Visual C++ 2013是这样的。 不清楚GCC是怎样的。
        assert(is_unsigned(-2147483648) == true);
        assert(is_int(INT_MIN) == true);
       
        assert(is_unsignedlonglong(-9223372036854775808));   // 验证： DATA:TMIN 文档中， Practice  Problem4的表格。 遗憾的是，C++和C编译器，还是不一样的。
        assert(is_unsignedlonglong(0x8000000000000000));
    }

    try {
        uadd_ok(UINT_MAX, 2);  // P115,  Practice Problem 2.27
    }
    catch (...)
    {
        std::cout<<"overflow exception happend"<<"\n";
    }

    try {
        tadd_ok(INT_MAX, INT_MAX);  // P120,  Practice Problem 2.30
    }
    catch (std::exception e)
    {
        std::cout << e.what() << "\n";
    }
    
    try {
        tadd_ok(-INT_MAX, -3); // P120,  Practice Problem 2.30
    }
    catch (std::exception e)
    {
        std::cout << e.what() << "\n";
    }

    try {
        tsub_ok(INT_MIN, INT_MAX); // P121,  Practice Problem 2.32
    }
    catch (std::exception e)
    {
        std::cout << e.what() << "\n";
    }

    try {
        tsub_ok(INT_MAX, INT_MIN); // P121,  Practice Problem 2.32
    }
    catch (std::exception e)
    {
        std::cout << e.what() << "\n";
    }

    try {
        tmul_ok(-3, 4); // P121,  Practice Problem 2.32
        tmul_ok(INT_MAX, 0x1000); // P121,  Practice Problem 2.32
    }
    catch (std::exception e)
    {
        std::cout << e.what() << "\n";
    }
    simpl_mult(-3, 4);
    simpl_mult(-3, -4);


    assert(div16(-33) == -2 && div16(33) == 2);

    assert(float_ge(-0.0, 0.0)==1);
    assert(float_ge(4.0, 3.0) == 1);
    assert(float_ge(-3.0, -4.0) == 1);
    assert(float_ge(4.0, 3.0) == 1);

    prob_2_54();
    
    assert(prob_2_59(0x89abcdef, 0x76543210) == 0x89abcd10);
    assert(put_byte(0x12345678,0xab,2) == 0x12ab5678);
    assert(put_byte(0x12345678,0xab,0) == 0x123456ab);
    
    std::cout <<"this machine is " << (is_big_endian() ? "big-endian" : "small-endian")<<std::endl;
    getchar();

}

