#include "rawbuffer_interface.h"

struct mystruct0 {
    int xx;
    short yy;
    int zz;
};//Yes, we support raw struct packed in raw buffer, if all the numeric member is portable!
  //是的，我们支持原生的结构体，只要里面的数值类型都是可移植的。推荐使用int32_t, int16_t这样更有可移植性的类型。

//Do not use multiple inheritance. It will leads to several unportable layouts between several compilers.
//别使用多继承。会在各个编译器之间产生很多无法移植的内存布局。
struct mystruct1: public mystruct0{ 
};

//overload << operator to enable output of mystruct0.
//重载一下<< 操作符让mystruct0支持输出到流
std::ostream& operator<< (std::ostream &ost, const mystruct0& rhs){
    ost << "xx="<< rhs.xx << "  " << "yy="<< rhs.yy << "  " << "zz="<< rhs.zz;
    return ost;
}



//Yes, we can use the packet data type that defined later as fields.
//是的，我们还可以使用延迟定义的包数据类型作为包的数据成员。
struct test_type1;


//Between DEF_PACKET_BEGIN and DEF_PACKET_END are the data fields. test_type is the packet type name
//在DEF_PACKET_BEGIN 和 DEF_PACKET_END之间的是包的数据成员。test_type是包的类型名字
DEF_PACKET_BEGIN(test_type) 
    
    ADD_FIELD_REQUIRED(int, a)
    //ADD_FIELD_REQUIRED will layout the field "a" like a c struct member "int a" 
    //ADD_FIELD_REQUIRED 会把a当C结构体的整数成员一样进行布局和存储
    //So the packet in raw_buffer is a tree struct and the required fields are value of the tree node.
	//所以在raw_buffer中包是一个树形的结构体，必须的字段会被视为树的节点的值。


    //The rest fields are optional. They are son node of the current packet
    //剩下的都是可选的字段. 他们是当前包的子节点。
    ADD_FIELD(int , x)
    ADD_FIELD(long long , y)	// do not use 64bit integer as required field,their alignment is not portable
    
    ADD_FIELD(mystruct0 , z)		   
    ADD_FIELD(mystruct1 , w)		

	//"ANY" field definition can be used for any other packet type that even only declared.
	//It does not lose any performanse but we can not estimate the packet(test_type) max size at compiling time.
	//"ANY" 式的成员定义可以用于任何其他仅仅有声明的包类型。
	//它不会带来任何性能的损失，但是会让我们无法在编译时估计包(test_type)的最大空间开销。

	//test_type1 need to be defined by DEF_PACKET_BEGIN when test_type is used in your codes and test_type1 must be another packet type!
	//当你开始使用test_type的时候，test_type1需要已被定义并且他必须也是通过DEF_PACKET_BEGIN定义的数据包类型。
	ADD_PACKET_ANY(test_type1, u);

	//test_type1 need to be defined by DEF_PACKET_BEGIN when test_type is used in your codes and test_type1 must be another packet type!
	//当你开始使用test_type的时候，test_type1需要已被定义并且他必须也是通过DEF_PACKET_BEGIN定义的数据包类型。
	ADD_PACKET_ARRAY_ANY(test_type1, v, 2);

    //After protocol is published, we can only modify it safely by adding or removing optional fields at the back of the packet.
    //协议公开之后，对协议安全的修改只允许在协议末尾增加或者删除字段。
DEF_PACKET_END


//Now lets turn to a harder one: test_type1
//好，现在我们来点复杂的协议: test_type1。
DEF_PACKET_BEGIN(test_type1)
    
    //All the optional fields will be visited via their offset stored in the packet. The default offset type is uint32_t
	//Our packet test_type1 here will not be bigger than 1<<16 in design of users. So we can use a smaller unsigned type instead to save space.
    //所有的可选成员，都会通过在包中存储的他们的位置偏移量来进行访问。默认偏移量类型是uint32_t.
	//这里我们的数据包test_type1按照使用者的设计，不会超过1<<16, 所以可以使用一个小一点的无符号数值类型来节省空间。
    typedef unsigned short offset_type;
    
    //We can define a fixed length array field like: char aa[12];
    //我们可以定义一个定长的数组成员：char aa[12];
ADD_FIELD_ARRAY_REQUIRED(char, aa, 12)

//Ok, next! 
//好的，再来一个
ADD_FIELD_ARRAY_REQUIRED(int, bb, 3)

//We can define an optioanl variable length vector field. 
//我们也可以定义一个变长的可选的数组成员。注意32仅仅是个长度估计，用来初始化时评估预分配内存数量的，不会限制cc的大小，可以安全的随意修改。
ADD_FIELD_ARRAY(int, cc, 32)
//Here 32 just provides an estimation of ccfor the writer pre-allocation. It does not limit the size of cc.
//32 can be changed into other number after the protocol is published because it just affect the meory allocator.

//We can define a knownable packet as field. For safety it can not be required!
//我们还可以定义一个已知的包类型作为数据成员。为了安全（包类型可能增减成员），他不可以被声明为必须的。
ADD_PACKET(test_type, xx)

//just more fields..
//再来更多的数据成员
ADD_FIELD(int, yy)
ADD_FIELD_ARRAY(char, zz, 32)
ADD_PACKET(test_type, ww)

//We can define a optioanl variable length vector of knownable packet as field. For safety it can not be required!
//我们还可以定义一个已知的包类型的变长数组作为数据成员。为了安全（包类型可能增减成员），他不可以被声明为必须的。
ADD_PACKET_ARRAY(test_type, uu, 4)

ADD_PACKET_ARRAY(test_type, tt, 4)

//test_type1 can use the packet type itself, make a link list.
//test_type1 还可以使用自己类型,可以构造出链表效果。
ADD_PACKET_ANY(test_type1, vv)

//test_type1 can use the packet type itself as array, make a tree.
//test_type1 还可以使用自己类型做数组,可以构造出树的效果。
ADD_PACKET_ARRAY_ANY(test_type1, dd, 3)

//After protocol is published, we can only modify it safely by adding or removing optional fields at the back of the packet.
//Or update the size estimation of the optional array of cc, zz, uu.
//协议公开之后，对协议安全的修改只允许在协议末尾增加或者删除字段。或者修改对可选数组如cc,zz,uu,tt, dd的长度估计。

DEF_PACKET_END

//template packet definition
//使用模板的包类型定义
template<typename T
	, typename Array_count_type = rawbuf_config::array_count_type
	, bool is_raw_buf_test_result = rawbuf::is_rawbuf_struct<T>::result
>
DEF_PACKET_BEGIN(test_type2)
	typedef Array_count_type array_count_type;
	ADD_FIELD_REQUIRED(T, o)
	ADD_FIELD(T, a)
	ADD_PACKET_ANY(test_type2, b)
	ADD_PACKET_ARRAY_ANY(test_type2, c, 3)
DEF_PACKET_END



//template specification or partial specification, and you need to mention how to specify.
//模板特化或者偏特化，需要你写出特化形式
template<typename T, typename Array_count_type> //
//DEF_PACKET_BEGIN(test_type2<T, false>) No! The c++ preproccesor will think "test_type2<T, false>"  as two arguments "test_type2<T", " false>"
//宏不认识<> 所以他认为里面的,是分割用的。
//using ARGS_LIST to wrap any template arguments list.
//使用ARGS_LIST来把所有的模板参数列表给包起来
DEF_PACKET_BEGIN(test_type2<ARGS_LIST(T, Array_count_type, true)>)
	typedef Array_count_type array_count_type;
	ADD_PACKET(T, a)
	ADD_PACKET(test_type2<int>, b)		//Use 
	ADD_PACKET_ARRAY_ANY(test_type2, c, 3)	//Here test_type2 is test_type2<ARGS_LIST(T, Array_count_type, true)> itself
	ADD_FIELD(std::pair<ARGS_LIST(int, double)>, d)	//ARGS_LIST should be used again.
	ADD_FIELD_REQUIRED(std::pair<ARGS_LIST(int, double)>, e)
	ADD_PACKET_ARRAY_ANY(test_type2<int>, f, 3)
DEF_PACKET_END


