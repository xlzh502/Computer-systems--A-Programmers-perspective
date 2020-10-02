
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

// �����������ģ�庯���������������ģ���࣬����Ϊ�������������ԣ�ģ�庯�� ���Ը��� ʵ�ε����ͣ��� �ƶ�ģ��Ĳ����� ���ǣ�ģ����ȴ���С� ��ˣ������ģ���౻ע�͵��ˡ�
// �й�ģ�庯�� ����һ���ԣ��ο���C++����������ԣ��ر�棩����13.3.1��
template <typename T>
bool is_unsigned(const T& t)
{
    return std::is_unsigned<T>::value;
}

// is_longlong ������ ģ�庯�������ء�ģ�庯����ר�Ż�
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

//���������is_longlong  ���ơ�ճ���� is_int ģ�庯�������Ƿ�������д���꣬����Щ��
#define IS_TYPE(TYP)  template <typename T>  bool is_##TYP(const T & t) {  return false ;} \
                                                                            bool is_##TYP(const TYP& t) {   return true ;}

IS_TYPE(int) // is_int ģ�庯��
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
    if (result <=-2 && x > 0 && y > 0) // ������Ҫע�� result <=-2, ����  P120   �ᵽ��result < 0 ��Ȼ��������<=-2���Ӿ�ȷ
        throw std::exception("positive overflow");
    else if (result >= 0 && x < 0 && y < 0)  // ������Ҫע�� result >= 0, ������result > 0. ԭ���ǣ� P117 ���� -2^(w-1) <= x, y < 2^(w-1)��˵�� x+y��ȡֵ��Χ�� [ **, ** )  ǰ���������
        throw std::exception("negative overflow");
    else
        return result;
}

int tsub_ok(int x, int y) // P121  : Practice problem 2.32
{
    // ���и����Ĵ��ǣ����y��TMIN����ô��x�Ǹ�������tadd_ok���׳��쳣������tsub_okȴ���᣻�ҵ��뷨����y��TMIN����x��������tsub_ok�����쳣������tadd_okȴ����
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
#if ������ʵ�ַ���
    long long result = (long long)x*y;
    int high32 = result >> 32; // ȡ��32λ�Ĳ����ֵ
    int low32 = (result << 32) < 0; // ȡ��32λ�� ���λ ����λ��ֵ����1����0
    if (high32 + low32 == 0)  // û��������� ���� Practice Problem 2.35��֤���������0
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
    int diff = (((x + x)<<27) >>27) >> 1;  // ���д�����и����⣬����x+x���ܻ����������ˣ��ǲ���ȫ�ġ�
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
    // P141  �������Ƚϴ�С��������и������㣻ֻ�谴��unsigned integer�����бȽϣ� ���������� ����ֵԽ����ԽС��
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

    //   x == (int)(float)x  �ķ���
    int x = 0x1000001;
    assert((int)(float)x  == 0x1000000);  // Round-to-even
    x = 0x2000003;
    assert((int)(float)x == 0x2000004);  // round-up
    x = 0x2000006;
    assert((int)(float)x == 0x2000008);  // round-to-even
    x = 0x2000001;
    assert((int)(float)x == 0x2000000); // round-down

    //  (f+d)-f == d  �ķ���
    double d = 9007199254740994L;  // 2^53  == 9007199254740992  ;    ��    9007199254740994 = 2^53  +  ��  �� ���� �� = (2^(-52)) * (2^53) = 2 
    float f = 1.0;
    assert((f + d) - f != d);  // f+d���� round-to-even��Ϊ d+2�� �� (f+d)-f�Ľ����Ӧ����d+1�����ǣ��ٴγ���round-to-even������d+2;

    d = 1e20;  // 1e20 == 2^(66.34),  ����  �� = 2^(66-52) = 2^14
    f = 8192; // 8192 == 2^13 == (2^(66-52)) / 2 ==  ��/2
    assert((f+d) - d == 0);  //��Ϊ��/2��halfway between������f+d������round-to-even, ����1e20���Ա� 2^(66-52)���������Զ϶��޾�����ʧ������1e20 mod ��2^(66-52+1)��== 0 �����Զ϶�fraction�����һλ ��0������round-to-even����f+d��Ȼ����1e20 
    f = 8192 + 1;  // 8192 + 1 ������ halfway between,���� f+d������Round-up�� ��� (f + d) �� d֮�� ���һ�� ��
    assert((f + d) - d == 2*8192); // f+d ������Round-up, ���� (f+d)-d  ����  ��


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
    return a == 0x12345678;  // Big endian�Ļ�������λ���ִ��ڵ͵�ַ�� small endian���� ��λ���ִ��ڵ͵�ַ��
}

bool prob_2_61(int x, char func)
{
    // Problem 2.61
    bool result = false;

    switch (func)
    {
    case 'A':
        result = x;
        break;
    case 'B':
        result = (x + 1);
        break;
    case 'C':
        result = (x & 0xff000000);
        break;
    case 'D':
        result = (x & 0xff - 0xff);
        break;
    default:
        throw("function is illegal");
    }
    return result;
}

bool int_shifts_are_logical()
{
    // Prob 2.62
    int totalBits = sizeof(int) <<3; 
    int d = -INT_MAX - 1;  // -INT_MAX-1   ==  INT_MIN
    return  (d >> (totalBits - 1)) == 1;
}

int sra(int x, int k)
{
    // problem 2.63
    int xsrl = (unsigned)x >> k;
    int mostSigBit = (1 << (sizeof(int) * 8 - k - 1))  & xsrl;  // mostSigBit == most significant bit
    int signExtention = ((1 << (sizeof(int) * 8 - 1)) - mostSigBit) << 1;
    return xsrl | signExtention;
}

unsigned srl(unsigned x, int k)
{
        // problem 2.63
    unsigned xsra = (int)x >> k;
    unsigned mostSigBit = (1 << (sizeof(int) * 8 - k - 1)) & xsra;
    return  xsra + (mostSigBit << 1); 
}

int any_even_one(unsigned x)
{
    // problem 2.64
    int mask = 0x55555555;
    return  x & mask;
}

int even_ones(unsigned x)
{
    // problem 2.65�� ����⣬�һ���һ���죬��ͻȻ�������ô��: Ŀ���������е�λ����������ż����1����ô�����һ����0�����������˲������������ԣ���Ҫ������θ���Ч���������˾���������ļ��㡣
    unsigned result = (x >> 8) ^ x;
    result = (x >> 16) ^ result;
    result = (x >> 24) ^ result;
    
    result = (result >> 4) ^ result;

    result = (result >> 2) ^ result;

    result = (result >> 1) ^ result;
    return  !(result & 0x1);
}

int leftmost_one(unsigned x)
{
    // problem 2.66�� �����Ŀ�����ĥ�ˣ��ҿ���hint������Ȼû���κ�˼·��ڤ˼�����˺ü���СʱҲû�㶨�� ����ǵ����ϲ�𰸣�������ͷ���г��򣬲����׵ġ� ������Ѿ�����
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x - (x >> 1);
}

int int_size_is_32(char part)
{  // Problem 2.67
    switch (part)
    {
    case 'B':
        //     B����
            {
                int set_msb = 1 << 31;
                int beyond_msb = set_msb << 1;
                return set_msb && !beyond_msb;
            }
    case 'C':
        //  C���� :  �涨 ��λ �������0
            {
                int bit15th = 1 << 15;
                int bit16th = bit15th << 1;
                int bit31th = bit16th << 15;
                int bit32th = bit31th << 1;
                return !bit32th && bit31th && bit16th && bit15th;
            }
    }

}



int lower_bits(int x, int n)
{
    // prob 2.68
    int mask = (1 << (n-1));
    mask = mask << 1;
    mask -= 1;
    return  x&mask;
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
   P89  Aside  C++������������߼� �� C��������̫һ���� 
    int k = 0xfedcba98 << 32;
    assert(0xfedcba98 << 32 == 0xfedcba98);
    assert(0xfedcba98 >> 36 == 0xffedcba9);
    assert(0xfedcba98U >> 40 == 0x00fedcba); */
    
    assert(1 << 2 + 3 << 4 == 1 << (2 + 3) << 4); // P90  Aside ��������ȼ�������


    //  P99 : conversion between signed and unsigned  ----   keep the bit values identical but change how these bits are interpreted.
    //  P100:   General rule for how C handle conversion between signed and unsigned numbers with same word size ---- the numeric values might change , but the bit patterns do not.
    short  v = -12345;
    unsigned short uv = v;
    assert(show_bytes((byte_pointer)&v, sizeof(short)) == show_bytes((byte_pointer)&uv, sizeof(unsigned short)));

    int m = -1;
    unsigned int n = UINT_MAX + m+1;  // P101:   ��ʽ��2.5��  ����һ���������з��������Ҹ�������������������ͬ λģʽ �� �޷�������
    assert(show_bytes((byte_pointer)&m, sizeof(int)) == show_bytes((byte_pointer)&n, sizeof(unsigned int)));

    // P105:  Figure 2.18   ����Υ����������
    //  �����ʱ����Ҫȥ��SDL��飬���򣬻ᱨ ��C4146��һԪ�������Ӧ�����޷������ͣ������Ϊ�޷������͡� ���� 
    assert(-1 < 0U == false);
    assert(2147483647U > -2147483647 - 1 == false);
    assert(2147483647 > (int)2147483648U == true);
    assert(is_unsigned(0x80000000) == true);
    assert(is_unsigned(-2147483647 - 1) == false);

    if (sizeof(char*) == 8)
    {
        // 64λ����
        
        assert(is_unsignedlong(-2147483648) == true);
        assert(is_int(-0x7FFFFFFF - 1) == true);
        if (sizeof(long) == 4)
        {
            // Visual C++ 2013:   sizeof(long) == 4�� 
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
         // 32λ����
        assert(sizeof(long) == 4 && sizeof(long long) == 8);   // Visual C++ 2013�������ġ� �����GCC�������ġ�
        assert(is_unsigned(-2147483648) == true);
        assert(is_int(INT_MIN) == true);
       
        assert(is_unsignedlonglong(-9223372036854775808));   // ��֤�� DATA:TMIN �ĵ��У� Practice  Problem4�ı�� �ź����ǣ�C++��C�����������ǲ�һ���ġ�
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

    std::cout << "int right-shift is " << (!int_shifts_are_logical() ? " NOT " : "" )<< "logical" << std::endl;

    assert(sra(0xff000000, 2) == ((-INT_MAX-1+0x7f000000) >> 2));
    assert(srl(0xff000000, 2) == (0xff000000u >> 2));
    assert(even_ones(0x01041040) == 1);
    assert(even_ones(0x01041014) == 0);

    assert(leftmost_one(0xff00) == 0x8000);
    assert(leftmost_one(0x6600) == 0x4000);
    assert(leftmost_one(0) == 0);

    assert(int_size_is_32('B') && int_size_is_32('C'));
    assert(lower_bits(0x78abcdef, 8) == 0xef);
    assert(lower_bits(0x78abcdef, 16) == 0xcdef);
    assert(lower_bits(0x78abcdef, 32) == 0x78abcdef);
    assert(lower_bits(0x78abcdef, 1) == 1);


    getchar();

}

