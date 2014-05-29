//类中static静态成员····
/*类中的static成员属于类本身，不属于任何对象····
java中可以直接用：类名.成员名  访问，C++不支持，C++中用 类名::成员名访问
C++中静态成员变量的注意比java多了很多，
C++的静态变量成员只是在类中声明，定义必须放在类外····
相对静态函数成员可以在在类中声明、类外定义，也可以在声明时定义····
在内存分配中静态成员和代码区共用一块区域，不属于栈，也不属于堆
*/

#include <iostream>
#include <stdlib.h>

using namespace std;

class A
{
public:
	int i;//int i=1错误，在C++中只有静态成员变量才能在类中初始化
	static int a;//static int a＝0错误，只有静态常量整型数据成员才可以在类中初始化
	static void t()//静态方法只能访问静态成员
	{
		cout << "静态a＝ " << a <<endl;
		//cout << i <<endl;//错误，静态方法不能访问非静态成员
		//b();//错误，静态方法不能访问非静态成员
	}
	void b()//非静态方法可以访问静态成员
	{
		cout << "非静态a＝"<< a << "   "<<endl;
		cout <<  "非静态i＝" <<i<<endl<<endl;
		t();//非静态成员可以访问静态成员
	}
	A()
	{
		i= 1;//在构造函数中初始化值
	}
}; //C++中，自定义类型的大括号后要加 ; （分号）
int A::a = 0;//初始化a;静态变量C++中用 类名::成员名访问

int main ()
{
	    A aa;
		A bb;
		
		aa.b();//a=0,i=1对象默认值
		bb.b();//a=0,i=1对象默认值
		cout << "\n\n" ;
		//cout<<"A.a = " <<A.a<<endl;//静态对象可以直接用 类名.成员名 访问
		
		aa.i = 10;
		aa.a = 20;
		aa.b();//a=20,i=10 aa中i已被赋值改变，
		bb.b();//a=20,i=1 a是静态成员，无论生成多少对象静态成员一直都只是一个，所以a属于类，所以那个对象访问结果都一样
		A::t();
		cout << "\n\n" ;
		
		bb.i = 30;
		bb.a = 40;
		aa.b();//a=40, i=20 另一个对象成员的值改变，不影响该类生成的其他对象
		bb.b();//a=40, i=30
		A::t();
		cout << "\n\n" ;
		
		A::a = 60;
		A::t(); //a=60;
		aa.b();//a=60, i=20 
		bb.b();//a=60, i=30
		
	return 0;
}

/*
C++静态成员初始化：
　静态数据成员的使用方法和注意事项如下：

　　1、静态数据成员在定义或说明时前面加关键字static。

　　2、静态成员初始化与一般数据成员初始化不同。静态数据成员初始化的格式如下：

　　　　<数据类型><类名>::<静态数据成员名>=<值>

　　这表明：

      　　(1) 初始化在类体外进行，而前面不加static，以免与一般静态变量或对象相混淆。

　　(2) 初始化时不加该成员的访问权限控制符private，public等。

　　(3) 初始化时使用作用域运算符来标明它所属类，因此，静态数据成员是类的成员，而不是对象的成员。

　　3、静态数据成员是静态存储的，它是静态生存期，必须对它进行初始化。

　　4、引用静态数据成员时，采用如下格式：

　　　<类名>::<静态成员名>
*/
