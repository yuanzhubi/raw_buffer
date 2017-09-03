# raw_buffer
raw_buffer can do serialization in a raw buffer and allows you to directly visit serialized data without unpacking/parsing like flatbuffer, but you do not need any IDL file and it is much more efficient
either in space or speed! You can direct define your data protocol in your cplusplus header file now! 

raw_buffer能让你像flatbuffer一样在原始缓冲区内直接完成序列化，也可以直接访问缓冲区内数据而无需解包或解析字段，有更高效的时间和空间效率。
并且方便的多是，你不需要撰写任何IDL或者schema!你可以直接在你的C++头文件里定义你的数据协议了！

```c++
#include "rawbuffer.h"
#include <iostream>
#include <fstream>

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
    
    //All the optional fields will be visited via their offset stored in the packet. The default offset type is uint32_t
    //Our packet test_type1 here will not be bigger than 1<<16 in design of users. So we can use a smaller unsigned type instead to save space.
    //所有的可选成员，都会通过在包中存储的他们的位置偏移量来进行访问。默认偏移量类型是uint32_t.
    //这里我们的数据包test_type1按照使用者的设计，不会超过1<<16, 所以可以使用一个小一点的无符号数值类型来节省空间。
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

//Now let us use our protocol
//好，我们来试试我们定义的协议。
int main(){
    cout << boolalpha ;

    //If the packet test_type can be a tree, rawbuf_builder<test_type> is use to pre-allocate the tree and build the root node of the tree. Rellocation may happen if your optional array is assigned with size greater than its estimation in protocol.
    //如果把包test_type看成一个数据包树，那么rawbuf_builder<test_type>就是用来初始化分配树的内存开销和构造这颗树的根的。如果可选的变长数组被赋值的长度超过了协议中的估计，那么可能会发生reallocation.
    rawbuf_builder<test_type> instance;		
    
    //builder can be used as a smart pointer. Fields name is the function name for assignment.
    //builder可以当成智能指针来用。数据成员名字则被当成函数名字用来进行赋值。
    instance->x(0);					
    instance->y(1);
    instance->a(2);
    
    //() operator always results a raw pointer(of test_type), required fields can assigend via the raw pointer and a little quicker（can be ignored in fact）
    //()操作符会返回一个原生的（test_type）指针，必须的字段可以通过原生指针进行赋值, 会快一点点（简直可以忽略）
    instance()->a(3);
    								
    mystruct1 t1;
    t1.xx = 2;
    t1.yy = 3;
    t1.zz = 4;
    instance->z(t1);
    
    //output function is only available for the raw pointer. The output is in json style. Char array will be output as c-string with proper escape.
    //output函数只能通过原生指针来调用。输出是json格式的。char数组会自动当字符串来打印，并且会进行适当的转义。
    instance()->output(std::cout);
    
    test_type* pinstance = instance();
    
    //Fields name is also the data visiting function name. It will return the data 地址.
    //数据成员的名字也是数据访问的函数名字, 会返回数据成员的地址。
    pinstance->z()->xx = 4;	
    pinstance->z()->yy = 4;
    pinstance->output(std::cout);

    //Harder usage
    //更复杂的用法
    rawbuf_builder<test_type1> instance2;
    
    //Every packet nodes in the packet tree can be created by assignment or alloc like following (for deeper assignment)
    //数据包树里的每一个非根节点可以通过上面的赋值或者像下面这样分配来进行创建(然后再进一步的对子成员赋值)
    rawbuf_writer<test_type> packet = instance2->xx<test_type1::alloc>();
    //rawbuf_writer is the base class of rawbuf_builder, which provides the data assignment and visiting function shown above.
    //rawbuf_writer 是rawbuf_builder的基类，提供了前面所展示的所有数据访问和赋值功能。
    
     packet->x(5);									
     packet->w(t1);
     instance2->yy(6);
     
     instance2->aa("Sorry!");				//We support array direct assignment!
     instance2->aa("Sorry!", 7);			//Same effection...
     
     //The writer can not used for reading. So using () to cast to raw pointer to visit address of assigned yy.
     //writer是不能用来读取数据的. 所以得使用()来转换成原生指针来访问yy。()->可以看成一个操作符嘛^_^
     instance2->bb(instance2()->yy(), 1);		//The length of bb is still 32(the length of the required array is always fixed!), but we modify its first value.
     instance2->cc(instance2()->bb(), 1);		//The length of cc is 1(the length of the optional array is depending on the assignment!).
     
     memcpy(instance2()->aa(), "Sorry!", 7);		//It is safe as the size of aa is 32.
     instance2->zz("I say :\"Hello \\ world!\"");
     instance2()->output(std::cout);			//The output of zz will be "I say :\"Hello \\ world!\""
     
     //Now we create packet node ww directly via assignment.
     //现在我们通过赋值来创建非根节点ww
     packet = instance2->ww(*instance);	
     packet->w(t1);
     
     //Now we try re-assignment the packet node.
     //现在我们来试试对已经赋值的节点重新赋值
     packet = instance2->ww(*instance);	
     //instance2()->ww()->w() becomes null (as instance()->w() is null) and the memory previously used (in "packet->w(t1);" ) is waste (that memory becomes a "memory fragment" and for time efficiency, we will not try to reuse it.) So be careful for using re-assignment for packet node!
     //instance2的ww字段的w字段现在变成了空，而且刚才w占据的空间出于时间效率会被浪费掉了而不会被重用。所以谨慎使用重新赋值这个功能。
     
      instance2->zz("I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  ); 
	//This assignment size is far beyond the size estimation of zz in protocol and it will lead to reallocation of instance2. But every writer like "packet" is safe still.
	//这个赋值已经大大超过了zz在协议中的长度估计, 会导致instance2的重新分配空间。不过所有的现存的rawbuf_writer，像"packet"依然是安全的
     
     	instance2->uu(instance2()->ww(), 1);
     
	//Now we write the date to file
	//现在我们把数据写到文件里去
	{
		ofstream test_out;
		test_out.open("./test.data", ios_base::binary | ios_base::out);
		test_out.write(instance2.data(), instance2.size());
		test_out.flush();
		test_out.close();
	}
	
	/***************************************************************
	Now we can talk about the reader 
	现在我们谈谈如何从缓冲区中读数据
	***************************************************************/
	
	char *buffer = new char[4096]; // do not use "char buffer[4096]", it is not properly aligned! Only malloc or new gurantee the 8 bytes alignment (the local array of long long in GCC-x86 is still not aligned in 8 bytes).
	ifstream test_in;
	test_in.open("./test.data", ios_base::binary | ios_base::in | ios_base::ate);
	int length = test_in.tellg();
	test_in.seekg (0, test_in.beg);
	test_in.read(buffer, length);
	
	bool result = rawbuf_check<test_type1>(buffer, length); 
	//This will check buffer as test_type1. 
	//这会把buffer当成数据包test_type1去检查
	
	if(result){	
		//Now we can use buffer as a struct if we guarantee the 8 bytes alignment of buffer!
		//检查通过！由于我们保证了缓冲区首部的8字节对齐，我们可以把缓冲区当结构体来用了。
		((test_type1*)buffer)->output(std::cout);
	}

```
