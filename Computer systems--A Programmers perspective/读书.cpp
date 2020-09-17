
#include <string>
#include <assert.h>

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
{
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
        assert(is_int(-0x7FFFFFFF - 1) == true);
        assert(is_unsignedlonglong(-9223372036854775808));
        assert(is_unsignedlonglong(0x8000000000000000));
    }


}

