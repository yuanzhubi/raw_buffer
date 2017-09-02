
#include "rawbuffer.h"
#include <iostream>
#include <fstream>
#include<string.h>
using namespace std;


struct mystruct0 {
    int xx;
    short yy;
    int zz;
}t0;


struct mystruct1: public mystruct0,
                public rawbuf::alignment_hint<4>{
}t1;

std::ostream& operator<< (std::ostream &ost, const mystruct0& rhs){
    ost << "xx="<< rhs.xx << "  " << "yy="<< rhs.yy << "  " << "zz="<< rhs.zz;
    return ost;
}

DEF_PACKET_BEGIN(test_type) 
	ADD_FIELD_REQUIRED(int, a)
    ADD_FIELD(int , x)
    ADD_FIELD(int , y)
    ADD_FIELD(mystruct0 , z)		//raw struct can be used as member field. But we do not know its alignment, so we suppose it is 8.
    ADD_FIELD(mystruct1 , w)		//mystruct1 tells us its alignment is 4.
DEF_PACKET_END(test_type)

typedef char char_array[32];

DEF_PACKET_BEGIN(test_type1)
    ADD_FIELD_VECTOR_REQUIRED(char , aa, 32)
	ADD_FIELD_VECTOR_REQUIRED(int , bb, 32)
	ADD_FIELD_VECTOR(int, cc, 32)	//32 is just an estimation for the writer pre-allocation. You can assign for more. 32 does not affect the packet size.
	ADD_PACKET(test_type , xx)
    ADD_FIELD(int , yy)
    ADD_FIELD_VECTOR(char, zz, 32)
	ADD_PACKET(test_type , ww)
	ADD_PACKET_VECTOR(test_type, uu, 4)
DEF_PACKET_END(test_type1)

#ifdef COMPABILITY_TEST
bool read_file = false;
#else
bool read_file = true;
#endif

int main(){
    cout << boolalpha ;

    rawbuf_builder<test_type> instance;		//Builder is the sub-class of writer, used for building the root packet. It pre-allocate memory for all the required or optional fields.
											//Reallocation may happen if the optional vector is assigned with size beyond its estimation.
									
    instance->x(0);					// You can use the writer or builder for assignment
    instance->y(1);
	instance()->a(2);				//() always results a raw pointer, either from the writer, builder or reader.
									// For the required fields, you can use either the writer or the raw pointer(with better performance) for assignment.

    t1.xx = 2;
    t1.yy = 3;
    t1.zz = 4;
    instance->z(t1);
    instance()->output(std::cout);	//Output function is only available for the raw pointer. You need gurantee the data safety yourself.
									//The output is in json style. char vector will be output as c-string.

    instance()->z()->xx = 3;		//Raw pointer usage.
    instance()->z()->yy = 4;
    instance()->output(std::cout);

    instance->x(1);
	
    rawbuf_builder<test_type> instance1;
    instance1->y(1);
    instance1->x(0);
	

    rawbuf_builder<test_type1> instance2;
    rawbuf_writer<test_type> packet = instance2->xx<test_type1::alloc>();	//For the sub-packet, you need to alloc it first for deeper assignment. Or you can directly assign: instace2->xx(*instance1)
    packet->x(5);															//rawbuf_writer is like a safe field iterator. You do not need to be worried about the possible relocation.
	packet->w(t1);
	instance2->aa("Sorry!");					//We support array direct assignment!
    instance2->yy(6);
	instance2->bb(instance2()->yy(), 1);		//The length of bb is 32(the length the required vector is fixed!), but we modify the first.
	instance2->cc(instance2()->bb(), 1);		//The length of cc is 1(the length the optional vector is depending on the assignment!).
    instance2->zz("I say :\"Hello \\ world!\"");	


    instance2()->output(std::cout);
    instance2->xx(*instance);	//w of "instance2.xx" was eliminated! Because the "instace" does not assign field w. 
								//The elimation does not reduce the space cost. So re-assignment ususally leads to waste of space.
    instance2()->output(std::cout); 
	instance2->ww(*instance);
    instance2->zz("I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  ); //This size is far beyond the size estimation of zz and it will lead to reallocation of instance2. But every writer like "packet" is safe still.
    instance2->uu(instance2()->ww(), 1);
	instance2()->output(std::cout);
    cout << "size: " << *(instance2()->zz<rawbuf_cmd::get_size>()) << endl;
    cout << endl;
	if(read_file){
		ofstream test_out;
		test_out.open("./test.data", ios_base::binary | ios_base::out);
		test_out.write(instance2.data(), instance2.size());
		test_out.flush();
		test_out.close();
	}
	/***************************************************************
	Now we can talk about the reader
	***************************************************************/
	char *buffer = new char[4096]; // do not use "char buffer[4096]", it is not properly aligned! Only malloc or new gurantee the 8 bytes alignment.
	ifstream test_in;
	test_in.open("./test.data", ios_base::binary | ios_base::in | ios_base::ate);
    int length = test_in.tellg();
    test_in.seekg (0, test_in.beg);
	test_in.read(buffer, length);


	bool result = rawbuf_check<test_type1>(buffer, length); //This will check the whole packet as test_type1.
	if(result){											//Now we can use buffer as a struct if we guarantee	the 8 bytes alignment of buffer!
		((test_type1*)buffer)->output(std::cout);		//Output will visiting all the fields and sub-fields.
		//If the data safety is guaranteed, you can direct use the buffer as packet even without packet check. For example, data communication between friend proccess.
	}

	
	//If the packet has so many fields and sub-fields but we only use very small number of them, rawbuf_check will be expensive.
	//For example, a game role may have 1000+ attributes after level 50, but 10 before level 5 while most gamers are junior players. Then the full check are usually useless.
	//So we can check the fields only before we use it. But the "read and check" pattern is a little complicated.
	
	rawbuf_reader<test_type1> reader2;
	if(!reader2.init(buffer, length)){	//Init failed! It will check the top level validity of test_type1, not including its fields.
		cout << reader2.error_msg() << endl;
		delete []buffer;
		return 0;
	}

	//The reader is a smart pointer.
	rawbuf_reader<test_type> reader1 = reader2->xx();	//Each data visiting function of reader will check the data validity. If failed, reader2 will be destroyed!
	if(!reader1){
		cout << "The field xx may be empty, or the data was totally corrupted!" << endl;
		//To distinguish it, we need to check the parent reader: reader2.
		if(!reader2){
			cout << "The data was corrupted!" << endl;
			cout << reader2.error_msg() << endl;
		}
		else{
			cout << "The field xx is empty" << endl;
		}
	}
	else{
		//reader2->xx() does not check the sub-fields of xx. So if we need to visit deeper, we need to check its fields
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
			cout << "Oh, I get the fields x: " << *x << endl;
		}
	}

	//So the packet is like a data tree. When we "init" a reader, it checks the root; when we "rawbuf_check" the reader, it checks the whole tree; when we use "reader->xx()", it checks the son node xx of reader.
	delete []buffer;

cout << rawbuf_builder<test_type1>::align_cost << endl;

cout << test_type1::members_iterator<test_type1, 3>::sizer  << endl;
cout << test_type1::sizer << endl;
cout << sizeof(test_type1) << endl;
cout << rawbuf::rawbuf_property<test_type1>::size_result << endl;

cout << rawbuf_builder<test_type1>::init_capacity << endl;
cout << test_type1::aligned_1x << endl;
cout << test_type1::aligned_2x << endl;
cout << test_type1::aligned_4x << endl;
cout << test_type1::aligned_8x << endl;

    return 0;
}
