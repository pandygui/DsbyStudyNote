/*文件*/
# include <stdio.h>
# include <stdlib.h>

int main (void)
{
    FILE * f1,* f2;
    
    system ("pause");
    return 0; 
} 
/*
     定义说明文件指针的一般形式为：
        FILE *指针变量标识符；


     文件的打开(fopen函数)
        fopen函数用来打开一个文件，其调用的一般形式为：
        文件指针名=fopen(文件名,使用文件方式);
        其中，
        "文件指针名"必须是被说明为FILE 类型的指针变量；
        "文件名"是被打开文件的文件名；
        "使用文件方式"是指文件的类型和操作要求。
        "文件名"是字符串常量或字符串数组。
        
    使用文件的方式共有12种，下面给出了它们的符号和意义。
     文件使用方式	              意义
        "rt"	    只读打开一个文本文件，只允许读数据
        "wt"	    只写打开或建立一个文本文件，只允许写数据
        "at"	    追加打开一个文本文件，并在文件末尾写数据
        "rb"	    只读打开一个二进制文件，只允许读数据
        "wb"	    只写打开或建立一个二进制文件，只允许写数据
        "ab"	    追加打开一个二进制文件，并在文件末尾写数据
        "rt+"	   读写打开一个文本文件，允许读和写
        "wt+"	   读写打开或建立一个文本文件，允许读写
        "at+"	   读写打开一个文本文件，允许读，或在文件末追加数据
        "rb+"	   读写打开一个二进制文件，允许读和写
        "wb+"	   读写打开或建立一个二进制文件，允许读和写
        "ab+"	   读写打开一个二进制文件，允许读，或在文件末追加数据

    文件关闭函数（fclose函数）
    文件一旦使用完毕，应用关闭文件函数把文件关闭，以避免文件的数据丢失等错误。
    fclose函数调用的一般形式是：
             fclose(文件指针)；

    文件的读写
    对文件的读和写是最常用的文件操作。在Ｃ语言中提供了多种文件读写的函数：
        ·字符读写函数  ：fgetc和fputc
        ·字符串读写函数：fgets和fputs
        ·数据块读写函数：freed和fwrite
        ·格式化读写函数：fscanf和fprinf
    使用以上函数都要求包含头文件stdio.h。

*/