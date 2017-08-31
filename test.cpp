
#include "rawbuffer.h"
#include <iostream>

#include<string.h>
using namespace std;


struct mystruct0 {
    int xx;
    short yy;
    long long zz;
}t0;


struct mystruct1: public mystruct0,
                public rawbuf::alignment_hint<8>{
}t1;

std::ostream& operator<< (std::ostream &ost, const mystruct0& rhs){
    ost << "xx="<< rhs.xx << "  " << "yy="<< rhs.yy << "  " << "zz="<< rhs.zz;
    return ost;
}

DEF_PACKET_BEGIN(test_type) //4+4+4+4+4+4
	ADD_FIELD_REQUIRED(int, a)
    ADD_FIELD(int , x)
    ADD_FIELD(int , y)
    ADD_FIELD(mystruct0 , z)
    ADD_FIELD(mystruct1 , w)
DEF_PACKET_END(test_type) //16+16+4+4

typedef char char_array[32];

DEF_PACKET_BEGIN(test_type1)
    ADD_FIELD_VECTOR_REQUIRED(char , aa, 32)
	ADD_PACKET(test_type , xx)
    ADD_FIELD(int , yy)
    ADD_FIELD_VECTOR(char , zz, 32)
	ADD_PACKET(test_type , ww)
	ADD_PACKET_VECTOR(test_type, uu, 4)
DEF_PACKET_END(test_type1)


int main(){
    cout << boolalpha ;

    rawbuf_builder<test_type> instance;
    instance->x(0);
    instance->y(1);
	instance()->a(2);

    t1.xx = 2;
    t1.yy = 3;
    t1.zz = 4;
    instance->z(t1);
    instance()->output(std::cout);

    instance()->z()->xx = 3;
    instance()->z()->yy = 4;
    instance()->output(std::cout);

    instance->x(1);

    rawbuf_builder<test_type> instance1;
    instance1->y(1);
    instance1->x(0);

    rawbuf_builder<test_type1> instance2;
    rawbuf_writer<test_type> packet = instance2->xx<test_type1::alloc>();
    packet->x(5);
	packet->w(t1);
	instance2->aa("Sorry!");
    instance2->yy(6);
    instance2->zz("I say :\"Hello \\ world!\"");


    instance2()->output(std::cout);
    instance2->xx(*instance());
    instance2()->output(std::cout); //w was eliminated!
	//instance2->ww(*instance());
    instance2->zz("I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\"" "I say :\"Hello \\ world!\""
                  );
    instance2()->output(std::cout);
    cout << "size: " << *(instance2()->zz<rawbuf_cmd::size>()) << endl;
    cout << endl;


	rawbuf_reader<test_type1> reader2;
	reader2.init(instance2.data(), instance2.size());
	rawbuf_check(reader2);
	cout << reader2->zz() << endl;
	cout << *reader2->zz<test_type1::size>() << endl;
	cout << *reader2->yy() << endl;
	cout << *(reader2->xx()->z())<< endl;

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
