#include "rawbuffer.h"
#include <iostream>
#include <fstream>
#include <assert.h>
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
    ADD_FIELD_ARRAY(char, zz, 32)
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
     
	//The allocation or assignment of the packet of array will return a rawbuf_writer_iterator, a sub-class of rawbuf_writer at the begin element.
	//It supports the increament, [] or other pointer "shifting" operation. It becomes like a real pointer.
	//对于包的数组的赋值或者分配会返回一个rawbuf_writer_iterator，他是rawbuf_writer的子类并位于数组第一个元素。
	//他支持自增，[]下标操作或者其它移动操作。他变得像一个真正的指针了。
	rawbuf_writer_iterator<test_type> it = instance2->uu<test_type1::alloc>(2);
	it->x(3);
	++it;
	it->x(4);
	it[-1]->a(2);
	
	//Now we write the date to file
	//现在我们把数据写到文件里去
	{
		ofstream test_out;
		test_out.open("./test.data", ios_base::binary | ios_base::out);
		//data() returns the buffer pointer
		//size() returns the buffer size
		test_out.write(instance2.data(), instance2.size());
		test_out.flush();
		test_out.close();
	}
	
	/***************************************************************
	Now we can talk about the reader from received data
	现在我们谈谈如何从从外部获取的数据中读取包成员
	***************************************************************/
	
	 //do not use "char buffer[4096]" such local variable, it may not be properly 8bytes aligned! Only malloc or new gurantee the 8 bytes alignment (the local array of long long buffer[4096] in GCC-x86 system V ABI is still not aligned in 8 bytes but x64 aligned).
	//不要使用"char buffer[4096]" 这样的局部变量, 这样的buffer未必已经8字节对齐. 只有malloc或者new可以确保8字节对齐，long long buffer[4096]在x86上gcc遵照system V ABI的实现也是没有8字节对齐的不过x64已经对齐了。
	char *buffer = new char[4096];

	ifstream test_in;
	test_in.open("./test.data", ios_base::binary | ios_base::in | ios_base::ate);
	int length = test_in.tellg();
	test_in.seekg (0, test_in.beg);
	test_in.read(buffer, length);
	
	bool result = rawbuf_check<test_type1>(buffer, length); 
	//This will check buffer as test_type1 and recursively check all the offset of the optional fields, whether they are out of buffer or not properly aligned, or the packet array has different optional fields.
	//这会把buffer当成数据包test_type1去检查, 并且会递归的检查所有可选字段的偏移量是否有存在超出buffer范围或者没有正确对齐的情况,和包数组中的元素是否不具有相同的可选字段个数等异常情况。
	
	if(result){	
		//Now we can use buffer as a struct if we guarantee the 8 bytes alignment of buffer! Every required fields must exits!
		//检查通过！由于我们保证了缓冲区首部的8字节对齐，我们可以把缓冲区当结构体来用了。其中每个必须的字段都肯定是存在的！
		((test_type1*)buffer)->output(std::cout);
		cout << ((test_type1*)buffer)->bb()[0] << endl;

		//Optional fields may not assigned. We do not have zero length optional array.
		//可选字段未必已经赋值.我们不会有0长度的可选数组。
		int* cc = ((test_type1*)buffer)->cc();
		test_type1::array_count_type* ccsize = ((test_type1*)buffer)->cc<test_type1::get_size>();;
		if(cc){
			assert(ccsize);
			assert(*ccsize != 0);
			cout << cc[0] << endl;
		}
		
		//But we do not check whether the char array is ended with '\0'. 
		//We do not assert every char array is stored with string.
		//Using c_str command for either required or optional fields to visit it safely.
		//但是我们并没有检查char数组是否用'\0'来结尾，因为我们并不假设char数组里存的就是字符串。
		//我们可以使用c_str命令来安全的访问这些必须的或者可选的字段
		
		const char* aa = ((test_type1*)buffer)->aa<test_type1::c_str>();
		//For required fields, aa == 0 means '\0' is not found in its array(leading a strnlen cost).
		if(aa != 0){
			cout << aa << endl;
		}
		
		const char* zz = ((test_type1*)buffer)->zz<test_type1::c_str>();
		//For optional fields, zz == 0 means zz was not assigned or not ended with '\0' .
		if(zz != 0){
			cout << zz << endl;
		}
		
		//For packet array, we do not get the array pointer but an iterator because outer data may have different count of optional fields from our protocol so it is dangerous to use ++ or [] for the array pointer with wrong sizeof(test_type).
		//对于包数组，我们并无法直接得到数组的地址指针而是一个迭代器，这是因为从外部来的数据的可选字段数量和我们未必相同，所以使用指向数组的指针去做++或者[]操作是危险的，如果基于我们错误的sizeof(test_type)去移动指针。
		rawbuf_packet_iterator<test_type> it = ((test_type1*)buffer)->uu();
		test_type1::array_count_type *psize = ((test_type1*)buffer)->uu<test_type::get_size>();
		if(psize != 0  ){
			for(size_t i = 0; i<*psize; ++i,++it){
				it->output(std::cout);
			}
		}
	}
	
	//If a packet has so many optional fields and sub-fields but we only use very small number of them, rawbuf_check will be expensive.
	//For example, a game role may have 1000+ attributes after level 50 but 10 before level 5， while most gamers are junior players(with low level). Then always full check of all the possible optional fields are usually useless.
	//So we can check the fields only before we use it. But like jsoncpp, the "check and read" pattern is a little more complicated.
	//如果一个包有很多可选字段，但是我们仅仅只用其中很少一部分的话，rawbuf_check就显得很昂贵了。
	//例如一个游戏角色可能在50级后有1000多个属性，但是在5级前只有10个。并且多数玩家都是新手，等级不高。所以总是全量检查所有可能存在的可选字段就往往不必要了。
	//所以我们可以仅仅在我们使用某个字段的时候去检查他。但是类似jsoncpp这种“检查并读”的模式用起来稍微有点复杂。
	
	rawbuf_reader<test_type1> reader2;
	if(!reader2.init(buffer, length)){	
		//Init failed! It just checked the top level validity of test_type1, not including its fields.
		//初始化失败了。reader2.init 只会检查test_type1结构体自身的合法性，还不会去检查他的成员。
		cout << reader2.error_msg() << endl;
		delete []buffer;
		return 0;
	}
	
	//Each data visiting function of reader will check the data validity. If the data was corrupted, reader will be destroyed to stop you to visit other fields! 
	//Data corruption is not local but always global!
	//通过reader去访问数据成员会检查数据的合法性，如果发现数据被损坏了，reader会变得不可用来阻止你去访问其它数据！
	//数据被损坏了的原因和影响不可能是局部的而总是全局的！
	rawbuf_reader<test_type> reader1 = reader2->xx();	
	if(!reader1){
		cout << "The field xx may be empty, or the data was totally corrupted!" << endl;
		//To distinguish it, we need to check the reader: reader2.
		//返回值没有获得数据？可能是xx根本就没有被赋值，也可能数据xx被损坏了
		//为了区分这2种情况，我们需要再去检查reader。
		if(!reader2){
			cout << "The data was corrupted!" << endl;
			//Through reader can not visit other fields, it can get the error message.
			//尽管reader不能再用于访问数据, 他能告诉你到底检查到了什么样的错误。
			cout << reader2.error_msg() << endl;
		}
		else{
			cout << "The field xx is empty" << endl;
		}
	}
	else{
	//reader2->xx() does not check the sub-fields of xx. So if we need to visit deeper for the optional fields of xx, we need to check its fields
	//reader2->xx() 不会检查xx的子可选项，所以如果想更深入的访问xx的可选字段，需要检查对应的字段。
		int *x = reader1->x();
		if(!x){
			if(!reader1){
				cout << "The data was corrupted!" << endl;
				cout << reader1.error_msg() << endl;
			}
			else{
				cout << "The field x is empty" << endl;
			}
		}
		else{
			cout << "Oh, I just get the fields x: " << *x << endl;
			//maybe you are tired... you can take a full check for any reader, at any level
			//可能你被这样抽丝剥茧挨个检查并访问数据弄的很疲惫了是吧，，你随时可以反悔，来个以某个局部节点为树根的全量检查
			if(rawbuf_check(reader1)){//Ok, you can visit every sub-optional fields of reader1 now.
				// For the reader, () operation also results the raw pointer
				test_type* pinstance = reader1();
				pinstance->output(std::cout);
			}
		}
	}
	
	//Now lets talk about the safely visiting the packet array via subclass of reader:rawbuf_reader_iterator
	//现在我们再来谈谈如何通过reader的子类rawbuf_reader_iterator来安全的访问包数组
	reader2.init(buffer, length);
	rawbuf_reader_iterator<test_type> uu = reader2->uu();
	test_type1::array_count_type *psize = reader2->uu<test_type1::get_size>();
	if(psize != 0 ){
		for(size_t i = 0; i<*psize && uu; ++i,++uu){
			if(rawbuf_check(uu)){	//Do a full check as following output will visiting all the members.
				uu()->output(std::cout);
			}
			else{
				cout << uu->error_msg() << endl;
				break;
			}
		}
	}
	
	//Finally we can examine the packet size
	//最后我们看看这个包的真正大小
	cout << "size:" << instance2.size () << endl;
	return 0;
}