/**/
# include <stdio.h>
void f(int * , int);//作用是可以输出任何一个一维数组内容 
int main (void)
{
    int a[5]={1,2,3,4,5};//a是数组名。
    
    printf ("%#X\n",&a[0]);//地址常用16进制表示。 
    printf ("%#X\n",a); //一维数组名是个指针常量，存存放的是一维数组第一个元素的地址。
    
    int * p;
    p = a;
    printf ("%d#%d\n%d#%d\n",a[0],*p,a[3],*(p+3));//a[i]永远等价于*(p+i)
    
    int b[4]={1,8,56,97855};     
    f(a,5);
    printf("\n%d#%d\n%d\n",a[2],*(a+2),a[1]);//p[i]等价于*(p+i)，也等价于a[i],也等价于*(a+i)
//因为p与a代表的都是a[i]数组的首地址（a[0]的地址），所以在被调函数中p也就是a的备份，所以p[i]也就是a[i]的副本 
    f(b,4);
   
    system ("pause");
    return 0;
}
void f(int * p, int len)//确定一个一维数组需要数组名和元素个数。 数组名是指针常量，所以用指针变量接受数组名 
{
     int j;
     for (j=0; j<len ;++j)
       printf ("%d  ", *(p+j)); 
     p[2] = 50;
     printf ("\n%d#%d\n",p[2],*(p+2)) ;//p[i]等价于*(p+i)
     *(p+1)=45;
     printf ("%d\n",*(p+1)) ;
}
/*
指针和一位数组: 
  一维数组名：
       一维数组名是个指针常量，存存放的是一维数组第一个元素的地址。
  下标和指针的关系
       如果p是指针变量，则 p[i]永远等价于*(p+i)
 确定一个一维数组需要哪些参数：（如果一个函数要处理一个一位数组，则需要接受该数组的：）
       数组名（第一个元素的首地址） 
     元素个数 （数组长度） 
 
*/ 
/*
指针变量的运算：
     指针变量不可进行加法，乘法，除法运算，
     如果两个指针变量指向的是同一块连续空间中的不同储存单元，
     则这两个指针变量可以相减。 
*/
