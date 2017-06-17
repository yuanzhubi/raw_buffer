
#include "rawbuffer.h"
#include <iostream>
using namespace std;


struct mystruct0 {
    int xx;
    short yy;
    int64_t zz;
}t0;


struct mystruct1: public mystruct0,
                public rawbuf::alignment_hint<8>{
}t1;

std::ostream& operator<< (std::ostream &ost, const mystruct0& rhs){
    ost << "xx="<< rhs.xx << "  " << "yy="<< rhs.yy << "  " << "zz="<< rhs.zz;
    return ost;
}

DEF_STRUCT_BEGIN(test_type)
    DECL_FIELD(required, int , x)
    DECL_FIELD(optional, int , y)
    DECL_FIELD(optional, mystruct0 , z)
    DECL_FIELD(optional, mystruct1 , w)
DEF_STRUCT_END(test_type)

DEF_STRUCT_BEGIN(test_type1)
    DECL_STRUCT_FIELD(required, test_type , xx)
    DECL_FIELD(optional, int , yy)
DEF_STRUCT_END(test_type1)

int main(){
    cout << boolalpha ;

    rawbuf::auto_handler<test_type> instance;
    instance->x(0);
    instance->y(1);
    t1.xx = 2;
    t1.yy = 3;
    t1.zz = 4;
    instance->z(t1);
    instance->output(std::cout, "","\r\n", "\r\n");

    instance->z()->xx = 3;
    instance->z()->yy = 4;
    instance->output(std::cout, "","\r\n", "\r\n");
    cout << *instance->x() << endl;
    cout << *instance->y() << endl;
    instance->x(1);
    cout << *instance->x() << endl;

    rawbuf::auto_handler<test_type> instance1;
    instance1->y(1);
    cout << instance1->check_required() << endl;
    instance1->x(0);
    if(instance1->check_required() != 0){
        cout << "require_check error!" << endl;
    }

    rawbuf::auto_handler<test_type1> instance2;
    cout << instance2->check_required() << endl;
    instance2->xx<rawbuf::allocate>();
    cout << instance2->check_required() << endl;
    instance2->xx()->x(5);
    instance2->yy(6);

    instance2->output(std::cout,"", "\r\n", "\r\n");
    cout << endl;

cout << rawbuf::auto_handler<test_type>::align_cost << endl;
cout << test_type::aligned_1x << endl;
cout << test_type::aligned_2x << endl;
cout << test_type::aligned_4x << endl;
cout << test_type::aligned_8x << endl;
cout << test_type::aligned_16x << endl;
    return 0;
}
