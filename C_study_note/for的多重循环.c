/*for的多重使用*/ 
# include <stdio.h>
int main ()
{
    int i,j;
    int a=0; 
    for (i=1;i<=5;++i)
    {
        printf ("外部for的循环、下面进入内部for······\n");
       for (j=2;j<=5;j++)
       {
           a++;
           printf ("内部for第%d次循环，哈哈！！！\n",a);
       }
     }
    printf ("循环结束\n");
    system ("pause");
    return 0;
}
/*for (1;2;3)
    for (4;5;6)
       7;
  8;
  循环流程： 
  1到2，成立执行4（不成立结束），再5成立7，再6，再5成立7（接着再6，再5成立7直到不成立），不成立3，
  再2 成立执行4，再5成立7，再6，再5成立7，不成立3，再2成立4···· 直到2不成立退出循环。 
*/
