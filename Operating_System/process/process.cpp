#include "process.h"

queue<jincheng_type> block_q;  //阻塞队列，用于存放挂起的进程


struct jincheng_type process[20] = {0};
int count=0, ready_num = 0, guaqi=0, pid, flag=0;
 
//创建进程 
void create(){      
	int num ;  //num用来表示创建进程数量
	cout<<"请输入创建进程的个数(不可大于20)：";
	cin>>num;
	while(num+::count > 20){
		cout<<endl<<"创建进程数量过多,请重新输入，当前还可以创建的进程为"<<20-::count;
		cin>>num;
	}
	if(::count >= 20){
		cout<<endl<<"内存已满，无法创建进程";
	}
	else
		for(int i=0; i<20; i++)  {              //默认一次创建num个进程
			if(process[i].state == 0){//说明这个内存块可以运行进程3
				cout<<endl<<"请输入新进程pid"<<endl;
				cin>>process[i].pid;
				for(int j=0;j<i;j++){
					if(process[i].pid==process[j].pid){
						printf("\n该进程已存在,请重新输入进程ID\n");
						cin>>process[i].pid;
						j = 0;
					}
				}
				cout<<"请输入新进程优先级\n";
				cin>>process[i].priority; 
				cout<<"请输入新进程大小\n";
				cin>>process[i].capcity; 
				cout<<"请输入新进程内容\n";
				cin>>process[i].info; 
				//创建进程，使标记位为1
				process[i].state=1;
				cout<<"进程已成功创建！";
				num--;
				::count++;
				if(num == 0)
					break;
			}
		}
		::count += num;
}
 
//进程运行状态检测 
void run(){   
	printf("运行进程信息如下："); 
		for(int i=0;i<20;i++){
			if(::process[i].state == 1){
				//将执行队列中的进程及其属性值输出	
				printf("\n  pid=%d ",process[i].pid); 
				printf(" priority=%d  ",process[i].priority); 
				printf(" capcity=%d  ",process[i].capcity); 
				printf(" state=%d  ",process[i].state); 
				printf(" info=%s  ",process[i].info); 
				flag=1;
			}
		}
	if(!flag) 
		printf("\n当前没有运行进程！\n");
}
 
//进程换出，将执行队列中的进程换出到阻塞队列中
void come_out(){
	if(::count == 0){
		printf("当前没有运行进程！\n");
		return;
	}
	else if(::count != 0){
		cout<<endl<<"请输入换出进程的ID:";
		cin>>pid;
		for(int i=0; i<20; i++){
			//定位，从执行队列中找到要换出的进程，根据其状态进行相应处理
			if(pid==process[i].pid) {
				if(process[i].state==1){
					process[i].state=3;  //切换进程状态为阻塞状态
					block_q.push(process[i]);  //放入阻塞队列
					memset(&process[i],0,sizeof(jincheng_type));
					::count--;
					cout<<endl<<"已经成功换出进程\n";
				}
				else if(process[i].state==0)
					printf("\n要换出的进程不存在\n");
				else if(process[i].pid==3)
					printf("\n该进程已被换出\n");
				else
					cout<<endl<<"进程未进入内存，无法换出";
				flag=1;
				break;
			}
		}
		if(flag == 0)
			cout<<endl<<"进程不存在";
	}
} 

// 进程换入
void come_in(){
	if(block_q.empty()){ 
		printf("\n当前没有挂起进程\n");
		return;
	}
	else if(::count+1 > 20){
		cout<<"内存已满，无法换入";
		return;
	}
	else{
		cout<<"请输入换入内存的进程ID:";
		cin>>pid;
		int counter = block_q.size();
		while(block_q.front().pid != pid && counter >= 0){
			cout<<block_q.front().pid;
			block_q.push(block_q.front());
			block_q.pop();
			cout<<block_q.front().pid;
			counter--;
		}
		if(block_q.front().state == 3){
			block_q.front().state = 1;
			for(int i=0; i < 20; i++)
				if(process[i].state == 0){
					process[i].pid = block_q.front().pid;
					process[i].priority = block_q.front().priority;
					process[i].capcity = block_q.front().capcity;
					strcpy(process[i].info,block_q.front().info);
					process[i].state = block_q.front().state;
					::count++;
					cout<<"已成功换入进程";
					block_q.pop();
					flag = 1;
					break;
				}
			}
	}
	if(flag == 0)
		cout<<"要换入的进程不存在";
}
 
//结束（杀死）进程 
void kill(){
	if(!::count){
		printf("当前没有运行进程！\n");
		return;
	}
	printf("\n输入杀死进程的ID值");
	scanf("%d",&pid);
	for(int i=0;i<20;i++){
		//定位，找到所要杀死的进程，根据其状态做出相应处理
		if(pid==process[i].pid){
			memset(&process[i],0,sizeof(jincheng_type));
			::count--;
			printf("\n已经成功杀死进程\n");
			flag=1;
			break;
		}
	} 
	//找不到，则说明进程不存在
	if(!flag)
		printf("\n 要杀死的进程不存在\n");
} 
 
//唤醒进程
void wake_up(){
	if(block_q.empty()){ 
		printf("\n当前没有挂起进程\n");
		return;
	}

	if(::count<20 && !block_q.empty()){
		for(int i=0; i<20; i++)
			if(process[i].state == 0){
				block_q.front().state = 1;
				process[i].pid = block_q.front().pid;
				process[i].priority = block_q.front().priority;
				process[i].capcity = block_q.front().capcity;
				strcpy(process[i].info,block_q.front().info);
				process[i].state = block_q.front().state;
				::count++;
				flag = false;
				block_q.pop();
				cout<<endl<<"已成功唤醒进程";
				break;
			}
			
	}
	else{
		cout<<endl<<"内存已满，无法换入";
	}
	//找不到，则说明进程不存在
	if(flag) printf("\n要唤醒的进程不存在\n");
}
 
//主函数 
int main()
{
	system("chcp 65001");
    system("cls");
	int n = 1;
	int num;
	//一开始所有进程都不在内存中 
	for(int i=0;i<20;i++)
		process[i].state = 0;
	while(n){
		printf("\n******************************************");
		printf("\n*		进程演示系统	   	 *");
		printf("\n******************************************");
		printf("\n*1.创建新的进程		2.查看运行进程 	 *");
		printf("\n*3.换入某个进程		4.换出某个进程 	 *");
		cout<<endl<<"*5.杀死某个进程		6.唤醒某个进程 	 *";   
		printf("\n*7.退出系统		                 *");
		printf("\n******************************************");
		printf("\n请选择（1~7）\n");
		scanf("%d",&num);
		switch(num){
			case 1: create();break;
			case 2: run();break;
			case 3: come_in(); break;
			case 4: come_out();break;
			case 5: kill(); break;
			case 6: wake_up(); break;
			case 7: printf("已退出系统");exit(0);
			default: printf("请检查输入数值是否在系统功能中1~7");n=0;
		}
		flag = 0;//恢复标记  
	} 
	system("pause");
	return 0;
} 
 
 