# raw_buffer
raw_buffer can do serialization in a raw buffer and allows you to directly access serialized data without unpacking/parsing  like flatbuffer, but you do not need any IDL file and it is much more efficient
either in space or speed! You can direct define your data protocol in your cplusplus header file now! 

raw_buffer能让你像flatbuffer一样在原始缓冲区内直接完成序列化并且直接访问而无需解包或解析字段，有更高效的时间和空间效率。
并且方便的多是，你不需要撰写任何IDL或者schema!你可以直接在你的C++头文件里定义你的数据协议了！

```c++
#include "rawbuffer.h"
#include <iostream>
#include <fstream>
#include<string.h>
using namespace std;

struct mystruct0 {
    int xx;
    short yy;
    int zz;
};//Yes, we support raw struct packed in raw buffer, if all the numeric member is portable!
  //是的，我们支持原生的结构体，只要里面的数值类型都是可移植的。推荐使用int32_t, int16_t这样更有可移植性的类型。

struct mystruct1: public mystruct0,
                public rawbuf::alignment_hint<4>{ 
};

//overload << operator to enable output of mystruct0.
//重载一下<< 操作符让mystruct0支持输出到流
std::ostream& operator<< (std::ostream &ost, const mystruct0& rhs){
    ost << "xx="<< rhs.xx << "  " << "yy="<< rhs.yy << "  " << "zz="<< rhs.zz;
    return ost;
}

//Between DEF_PACKET_BEGIN and DEF_PACKET_END are the data fields. test_type is the packet type name
//在DEF_PACKET_BEGIN 和 DEF_PACKET_END之间的是包的数据成员。test_type是包的类型名字
DEF_PACKET_BEGIN(test_type) 
    
    ADD_FIELD_REQUIRED(int, a)
    //ADD_FIELD_REQUIRED will layout the field "a" like a c struct member "int a" 
    //ADD_FIELD_REQUIRED 会把a当C结构体的整数成员一样进行布局和存储
    
    //The rest fields are optional.
    //剩下的都是可选的字段
    ADD_FIELD(int , x)
    ADD_FIELD(int , y)
    
    ADD_FIELD(mystruct0 , z)		
    //raw struct can be used as data field(either required or optional). But we do not know its inner alignment.
    //We suppose it is 8 byte.
    //原生的结构体可以成为包的数据成员.不过在我们不知道他内部的成员对齐要求。我们会假设是8byte。
    
    ADD_FIELD(mystruct1 , w)		
    //mystruct1 tells us its alignment is 4 via rawbuf::alignment_hint
    //The storage can be more efficient.
    //mystruct1 通过继承rawbuf::alignment_hint告诉了我们他用4字节对齐
    //这样存储会更高效。
    
    //After protocol is published, we can only modify it safely by adding or removing optional fields at the back of the packet.
    //协议公开之后，对协议安全的修改只允许在协议末尾增加或者删除字段。
DEF_PACKET_END(test_type)

//Now lets turn to a harder one
//好，现在我们来点复杂的协议。
DEF_PACKET_BEGIN(test_type1)
    
    //All the optional fields will be accessed via their offset stored in the packet. The default offset type is uint32_t
    //Our packet test_type1 will not be bigger than 1<<16 in design of users. So we can use a smaller unsigned stype instead.
    //所有的可选成员，都会通过在包中存储的他们的位置偏移量来进行访问。默认偏移量类型是uint32_t.
    //我们的数据包按照使用者的设计，不会超过1<<16, 所以可以使用一个小一点的无符号数值类型来替换。
    typedef unsigned short offset_type;
    
    //We can define a fixed length array field: char aa[32];
    //我们可以定义一个定长的数组成员：char aa[32];
    ADD_FIELD_ARRAY_REQUIRED(char , aa, 32)
    
    //Ok, next! 
    //好的，再来一个
	  ADD_FIELD_ARRAY_REQUIRED(int , bb, 32)
    
    //We can define an optioanl variable length vector field. 
    //我们也可以定义一个变长的可选的数组成员。注意32仅仅是个长度估计，用来初始化时评估预分配内存数量的，不会限制cc的大小，可以安全的随意修改。
	  ADD_FIELD_ARRAY(int, cc, 32)	
    //Here 32 just provides an estimation of ccfor the writer pre-allocation. It does not limit the size of cc.
    //32 can be changed into other number after the protocol is published because it just affect the meory allocator.
	  
    //We can define a knownable packet as field. For safety it can not be required!
    //我们还可以定义一个已知的包类型作为数据成员。为了安全（包类型可能增减成员），他不可以被声明为必须的。
    ADD_PACKET(test_type , xx)
    
    //just more fields..
    //再来更多的数据成员
    ADD_FIELD(int , yy)
    ADD_FIELD_VECTOR(char, zz, 32)
	  ADD_PACKET(test_type , ww)
    
    //We can define a optioanl variable length vector of knownable packet as field. For safety it can not be required!
    //我们还可以定义一个已知的包类型的变长数组作为数据成员。为了安全（包类型可能增减成员），他不可以被声明为必须的。
	  ADD_PACKET_ARRAY(test_type, uu, 4)
    
    //After protocol is published, we can only modify it safely by adding or removing optional fields at the back of the packet.
    //Or update the size estimation of the optional array of cc, zz, uu.
    //协议公开之后，对协议安全的修改只允许在协议末尾增加或者删除字段。或者修改对可选数组cc,zz,uu的长度估计。
      
DEF_PACKET_END(test_type1)

```
