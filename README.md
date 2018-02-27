# raw_buffer
raw_buffer can do packet serialization in a raw buffer and allows you to directly visit serialized data without unpacking/parsing(zero copy) like flatbuffer, but you do not need any IDL file and it is much more efficient
either in space or speed(supporting POD type as required field!)! You can directly define your data protocol in your cplusplus header file now! It supports forward declared (incomplete) type as optional type, too. C++ supported only till now.

Charactors:

0. Tree data structure stored in a raw buffer

1. Unpack not needed, IDL not needed, API generating tool not needed

2. Safe against malicious input, including buffer overflow upward or downward, data is not aligned. You can do the safe check for any sub-tree starting from any node of the tree. When visit an array as c-string, use a "c_str" command to guarantee '\0' ending.

3. POD struct, POD union are suppoted in packet definition.

4. A node can be referenced by one more parent nodes that created ealier(which avoids the ring) to save space.

5. You can use forward declared packet type or even packet type itself as field type. This function enhances 4（enabling instrusive list and tree）!

6. Accurate buffer pre-allocation in packeting, according to evaluate the worst case in compile time.

7. Enable user to optimize for small packet. You can redefine the integer type of the offset.

8. Header only, C++ only. GCC3.4-5.3 test passed. VC2008-2017 test passed. clang 3.5-3.6 test passed. Intel C++ 16.0 test passed. More test are welcome!

9. (Since 1.0.2)Template packet now are supported https://github.com/yuanzhubi/raw_buffer/releases/tag/1.0.2. Notice the compiler limit.


Portable suggestion:

0. Using int32_t, int16_t instead of int, short.

1. Do not use long double. sizeof(long double) is different between compilers.

2. Do not use 8bytes integer as required field. In x86 it is 4bytes aligned but in x64 it is 8bytes aligned.

3. Buffer should 8bytes aligned.

See https://yuanzhubi.github.io/raw_buffer/ as example for almost the all api.


raw_buffer能让你像flatbuffer一样在原始缓冲区内直接完成序列化，也可以直接访问缓冲区内数据而无需解包或解析字段(都是0拷贝)，有更高效的时间和空间效率(POD类型可以当required成员来用)。
并且方便的多是，你不需要撰写任何IDL或者schema!你可以直接在你的C++头文件里定义你的数据协议了！还支持前向声明的数据类型为可选类型。目前为止仅支持C++

特点：

0. 包被看成树形的结构存储在原始的缓冲区中.
1. 访问包无需解包，协议定义无需IDL，API生成工具也就不需要了.
2. 可以安全的对抗各种恶意制造的输入，包括缓冲区上溢或者下溢，数据未对齐。可以在树的任何一个节点上开始对子树进行检查。当把数组当成c字符串来访问时，注意使用一个c_str命令来保证'\0'结尾.
3. 你可以在包定义中使用POD结构体，POD联合.
4. 一个节点可以被一个或者多个比他创建的早（这个限制是为了避免环）的节点所引用来节省内存空间.
5. 你可以使用前向声明的未完整类型甚至包类型自己作为包数据成员类型。这加强了功能4（允许链表，树等数据结构类型在包中实做）！
6. 打包时有较为准确的缓冲区大小预分配，得益于对包最坏情况的大小的编译时计算.
7. 允许用户去优化小包。你可以重新定义偏移量整数类型
8. Header only, C++ only. GCC3.4-5.3 测试通过. VC2008-2017 测试通过. clang 3.5-3.6 测试通过. intel c++ 2016 测试通过。
9. (Since 1.0.2)模版类定义的包也可以支持了https://github.com/yuanzhubi/raw_buffer/releases/tag/1.0.2. 注意编译器限制。

可移植性建议：

0. 使用int32_t, int16_t等代替int,short.
1. 不要使用long double. sizeof(long double)在各个编译器中有所不同。
2. 不要使用64位整数为required类型，他在x86中4字节对齐而在x64中8字节对齐。
3. 收包的缓冲区需要8字节对齐。

https://yuanzhubi.github.io/raw_buffer/ 看大部分api的例子。
