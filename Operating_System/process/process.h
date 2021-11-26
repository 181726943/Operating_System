#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<queue>
#include<windows.h>
#include <iostream>
using namespace std;
struct jincheng_type{   //进程状态定义 
	int pid;          //进程 
	int priority;     //进程优先级 
	int capcity;      //进程大小 
	int state;   //标志进程状态，0为不在内存，1为在内存,2，3为挂起 
	char info[20];   //进程内容 
}; 
