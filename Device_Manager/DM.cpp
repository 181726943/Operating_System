#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include<string.h>
#include <cstdlib>
#include <cstdlib>
#include <iomanip>

using namespace std;

/* 进程控制块 */
struct PCB
{
    string processid;  /* 进程ID */
    string device_name;  /* 申请设备名称 */
    string coid;    /* 进程所用控制器名称 */
    string chid;    /* 进程所用通道名称 */
};

/* 设备控制表 */
struct DCT
{
    char type;    /* 设备类型 I/O */
    string Dname;    /* 设备名称 */
    int state;    /* 设备状态1/0 */
    string coid;    /* 连接相应控制器的id */
};

/* 系统设备表 */
struct SDT
{
    char type;    /* 设备类型I/O */
    string Dname;    /* 设备名称 */
    DCT DCT;    /* 绑定的相应的设备 */
};

/* 控制器控制表 */
struct COCT
{
    string controllerid;    /* 控制器id */
    int state;    /* 控制器状态1/0 */
    string Chid;    /* 与控制器相连的通道的id */
    // COCT(): controllerid(controllerid) , state(state) , Chid(Chid){}
    // COCT(string controllerid,int state,string Chid) : controllerid(controllerid) , state(state) , Chid(Chid) {}
};

/* 通道控制表 */
struct CHCT
{
    string channelid;  /* 通道id */
    int state;  /* 通道状态1/0 */
    vector <string> COid;   /* 与通道相连的控制器的id */
};

queue <PCB> ch_block;  /* 通道阻塞队列 */
queue <PCB> co_block;  /* 控制器阻塞队列 */
vector <PCB> run_p;    /* 申请成功的队列 */
vector <SDT> sdt(20);   /* 系统设备表队列 */
vector <COCT> coct(20);  /* 控制器表队列 */
vector <CHCT> chct(2);  /* 通道控制表队列 */

void show();    /* 显示设备信息 */
void init();    /* 初始化 */
void Add();  /* 添加设备 */
void Distribute();  /* 分配设备 */
void Recycle();  /* 回收设备 */
void Delete();  /* 删除设备 */

/* 初始化 */
void init(){
    /* 初始化SDT和DCT */

    sdt[0].Dname = "display1";
    sdt[0].type = 'O';
    sdt[0].DCT.Dname = sdt[0].Dname;
    sdt[0].DCT.type = sdt[0].type;
    sdt[0].DCT.state = 0;
    sdt[0].DCT.coid = "co1";
    sdt[1].Dname = "keyboard1";
    sdt[1].type = 'I';
    sdt[1].DCT.Dname = sdt[1].Dname;
    sdt[1].DCT.type = sdt[1].type;
    sdt[1].DCT.state = 0;
    sdt[1].DCT.coid = "co2";
    sdt[2].Dname = "mouse1";
    sdt[2].type = 'I';
    sdt[2].DCT.Dname = sdt[2].Dname;
    sdt[2].DCT.type = sdt[2].type;
    sdt[2].DCT.state = 0;
    sdt[2].DCT.coid = "co3";
    sdt[3].Dname = "printer1";
    sdt[3].type = 'O';
    sdt[3].DCT.Dname = sdt[3].Dname;
    sdt[3].DCT.type = sdt[3].type;
    sdt[3].DCT.state = 0;
    sdt[3].DCT.coid = "co4";
    

    /* 初始化控制器控制表 */
    // coct[0]("col",0,"ch1");
    // strcpy((char*)&coct[0].controllerid,"col");
    coct[0].controllerid = string("co1");
    coct[0].state = 0;
    coct[0].Chid = string("ch1");
    coct[1].controllerid = string("co2");
    coct[1].state = 0;
    coct[1].Chid = string("ch1");
    coct[2].controllerid = string("co3");
    coct[2].state = 0;
    coct[2].Chid = string("ch2");
    coct[3].controllerid = string("co4");
    coct[3].state = 0;
    coct[3].Chid = string("ch2");

    /* 初始化通道控制表 */
    chct[0].channelid = string("ch1");
    chct[0].state = 0;
    chct[0].COid.push_back("co1");
    chct[0].COid.push_back("co2");
    chct[1].channelid = string("ch2");
    chct[1].state = 0;
    chct[1].COid.push_back("co3");
    chct[1].COid.push_back("co4");
}

/* 添加设备 */
void Add(){
    SDT newDevice;  /* 新添加设备的结构体 */
    int addcon;  /* 是否添加控制器的标志 */
    COCT newco;

    if(sdt.size() >= 50){
        std::cout<<"设备数量已达上限，无法添加!!!"<<endl;
    }
    else{
        /* 设定设备名称 */
        std::cout<<"请输入设备名称:";    /* 输入添加设备的名称 */
        std::cin>>newDevice.Dname;
        for(int i=0; i<sdt.size(); i++){    /* 判定添加的设备是否已存在 */
            if(sdt[i].Dname.compare(newDevice.Dname) == 0){  /* 如果存在则退出，并提示添加失败 */
                std::cout<<"设备已存在，添加失败!!!"<<endl<<endl;
                return;
            }
        }
        /* 设定设备类型 */
        std::cout<<"请输入设备类型(I/O):";
        std::cin>>newDevice.type;
        while(newDevice.type != 'I' && newDevice.type != 'O'){    /* 判断输入值是否符合要求，只能是I/O */
            std::cout<<"输入不合法，请重新输入!!!"<<endl;
            std::cin>>newDevice.type;
        }
        newDevice.DCT.state = 0;    /* 新添加设备的状态 */
        /* 新设备控制器设置 */
        std::cout<<"是否添加新的控制器(0.否 1.是):";
        std::cin>>addcon;
        while(addcon != 0 && addcon != 1){
            std::cout<<"输入不合法，请重新输入!"<<endl;
            std::cin>>addcon;
        }
        if(addcon == 0)  /* 不添加新的控制器 */
        {
            std::cout<<"请选择已有设备控制器(co1,co2,co3,co4):";
            std::cin>>newDevice.DCT.coid;
            while(newDevice.DCT.coid != "co1" && newDevice.DCT.coid != "co2" && newDevice.DCT.coid != "co3" && newDevice.DCT.coid != "co4"){
                std::cout<<"输入不合法，请重新输入!!!"<<endl;
                std::cin>>newDevice.DCT.coid;
            }

        }
        else{   /* 添加新的的控制器 */
            std::cout<<"请输入添加控制器的id：";
            std::cin>>newco.controllerid;
            for(int k=0; k<coct.size(); k++){
                if(newco.controllerid == coct[k].controllerid){
                    k = 0;
                    std::cout<<"控制器已存在,请重新添加"<<endl<<endl;
                    std::cin>>newco.controllerid;
                }
            }
            newDevice.DCT.coid = newco.controllerid;  /* 将新设备与添加的控制器连接起来 */
            std::cout<<"请输入控制器所连通道的id(ch1,ch2)：";
            std::cin>>newco.Chid;
            while(newco.Chid != "ch1" && newco.Chid !="ch2"){
                std::cout<<"输入不合法，请重新输入!!!"<<endl;
                std::cin>>newco.Chid;
            }
            /* 在通道控制表中加入新添加的控制器 */
            if(newco.Chid == "ch1")  /* 说明新添加的控制器与通道1相连 */
                chct[0].COid.push_back(newco.controllerid);
            else  /* 说明新添加的控制器与通道2相连 */
                chct[1].COid.push_back(newco.controllerid);
            coct.push_back(newco);
        }
        std::cout<<"设备添加成功！"<<endl<<endl;
        sdt.push_back(newDevice);   /* 设备添加完成，将新设备添加到SDT队列 */
    }
}

/* 分配设备 */
void Distribute(){
    string D_name;
    PCB process;
    std::cout<<"请输入申请进程的ID:";
    std::cin>>process.processid;
    std::cout<<"请输入请求分配设备的ID:";
    std::cin>>D_name;
    for(int i=0; i<sdt.size(); i++){
        // std::cout<<sdt[i].Dname;
        const char * temp = sdt[i].Dname.c_str();
        if (strcmp(temp,D_name.c_str()) == 0)  /* 申请设备存在 */
        {
            process.device_name = D_name;
            if(sdt[i].DCT.state == 1){  /* 设备被占用 */
                std::cout<<"设备被占用,申请失败!"<<endl;
                process.device_name = "";
                return;
            }
            else if(sdt[i].DCT.state == 0){  /* 设备空闲 */
                sdt[i].DCT.state = 1;
                for(int k=0; k<coct.size(); k++){  /* 查找控制该设备的控制器 */
                    if(sdt[i].DCT.coid == coct[k].controllerid){
                        if(coct[k].state == 0){  /* 控制器空闲 */
                            process.coid = coct[k].controllerid;    /* 将空闲的控制器交给申请进程使用 */
                            coct[k].state = 1;
                            if(coct[k].Chid == "ch1" && chct[0].state == 0){  /* 表明控制器与通道1相连且通道1空闲 */
                                process.chid = "ch1";   /* 该进程占用通道1 */
                                chct[0].state = 1;  /* 将通道1状态置1(忙碌) */
                                run_p.push_back(process);
                                std::cout<<"分配设备成功"<<endl;
                                return;
                            }
                            else if (coct[k].Chid == "ch2" && chct[1].state == 0){   /* 表明控制器与通道2相连且通道2空闲 */
                                process.chid = "ch2";   /* 该进程占用通道2 */
                                chct[1].state = 1;   /* 将通道2状态置1(忙碌) */
                                run_p.push_back(process);
                                std::cout<<"分配设备成功"<<endl;
                                return;
                            }
                            else{   /* 通道忙碌 */
                                ch_block.push(process);  /* 将该进程加入通道阻塞队列 */
                                std::cout<<"通道阻塞，已将请求加入通道阻塞队列"<<endl;
                                return;
                            }
                        }
                        else{
                            co_block.push(process);
                            std::cout<<"控制器阻塞，已将请求加入控制器阻塞队列"<<endl;
                            return;
                        }
                    }
                }
            }
        }
    }
    std::cout<<"申请设备不存在,请添加设备!!!"<<endl;
    return;
}

/* 回收设备 */
void Recycle(){
    string Deviceid;  /* 待回收设备id */
    string controlid;   /* 待回收设备所用控制器id */
    string channelid;   /* 带回收设备所用通道id */
    std::cout<<"请输入回收设备id：";
    std::cin>>Deviceid;
    for(int i=0; i<sdt.size(); i++){
        if(sdt[i].Dname == Deviceid){    /* 寻找待回收设备 */
            if(sdt[i].DCT.state == 1){  /* 如果待回收设备正在使用中 */
                sdt[i].DCT.state = 0;   /* 设备状态置为空闲 */
                for(int k=0; k<run_p.size(); k++)
                    if(run_p[k].device_name == Deviceid){    /* 查找使用该设备的进程 */
                        controlid.assign(run_p[k].coid);   /* 记录进程占用的控制器 */
                        channelid.assign(run_p[k].chid);   /* 记录进程占用通道 */
                        run_p.erase(run_p.begin()+k);   /* 释放该进程 */
                    }
                for(int j=0; j<coct.size(); j++)   /* 查找设备占用的控制器 */
                    if(coct[j].controllerid == controlid)
                        coct[j].state = 0;  /* 找到后将控制器状态置为空闲 */
                for(int channel=0; channel<chct.size(); channel++)  /* 查找设备占用通道 */
                    if(chct[channel].channelid == channelid)
                        chct[channel].state = 0;    /* 找到后将通道状态置为空闲 */
                std::cout<<"设备成功回收"<<endl<<endl;
                return;
            }
            else{
                std::cout<<"设备未被占用，无需回收"<<endl<<endl;
                return;
            }
        }
    }
    std::cout<<"该设备不存在，请检查设备名称是否正确"<<endl<<endl;
}

/* 删除设备 */
void Delete(){
    string Device;  /* 待删除设备的id */
    string channel;  /* 设备所用通道id */
    string controller;  /* 设备所用控制器id */
    char confirm;   /* 是否确认删除设备的指令 */
    std::cout<<"请输入删除设备的名称:";
    std::cin>>Device;
    for(int i=0; i<sdt.size(); i++){    /* 定位将要删除的设备 */
        if(sdt[i].Dname == Device){  /* 找到要删除的设备 */
            dflag:if(sdt[i].DCT.state == 0){  /* 检查状态，设备未被占用 */
                    std::cout<<"是否删除设备"<<sdt[i].Dname<<"[y/n]:";
                    std::cin>>confirm;
                    confirm = tolower(confirm); /* 将输入的字母转化为小写 */
                    while(confirm != 'y' && confirm != 'n'){    /* 检测输入是否合法 */
                        std::cout<<"输入不合法，请重新输入!!!"<<endl;
                        std::cin>>confirm;
                        confirm = tolower(confirm);
                    }
                    if(confirm == 'n'){
                        std::cout<<"取消删除，删除失败"<<endl<<endl;
                        return;
                    }
                    else{
                        sdt.erase(sdt.begin()+i);   /* 将设备删除 */
                        std::cout<<"删除成功！"<<endl<<endl;
                        return;
                    }
            }
            else{   /* 如果设备正在使用中 */
                sdt[i].DCT.state = 0;
                for(int k=0; k<run_p.size(); k++)  /* 首先找到使用该设备的进程 */
                    if(run_p[k].device_name == Device){
                        controller = run_p[k].coid;  /* 记录控制器id */
                        channel = run_p[k].chid;    /* 记录通道id */
                        run_p.erase(run_p.begin()+k);   /* 释放该进程，即删除 */
                    }
                for(int j=0; j<coct.size(); j++)    /* 定位设备所使用控制器的位置 */
                    if(coct[j].controllerid == controller)   /* 找到该控制器，置0 */
                        coct[j].state = 0;
                if(channel == "ch1") /* 这两个判断语句寻找设备使用通道，找到置0 */
                    chct[0].state = 0;
                else
                    chct[1].state = 0;
                goto dflag;
            }
        }
    }
    std::cout<<"设备不存在，删除失败！！！"<<endl<<endl;
}

/* 显示设备详情 */
void show(){
    cout<<"--------------------------------------------------------------------"<<endl;
    cout<<"|   device   |   type   |   state   |   controller   |   channel   |"<<endl;
    cout<<"--------------------------------------------------------------------"<<endl;
    for (int i=0; i<sdt.size(); i++){
        cout<<setiosflags(ios::left)<<setw(14)<<sdt[i].Dname;
        cout<<setw(12)<<sdt[i].type;
        cout<<setw(13)<<sdt[i].DCT.state;
        cout<<setiosflags(ios::left)<<setw(18)<<sdt[i].DCT.coid;
        for(int k=0; k<coct.size(); k++){
            if(coct[k].controllerid.compare(sdt[i].DCT.coid) == 0)
                cout<<setw(15)<<coct[k].Chid<<endl;
        }
    }
    cout<<"--------------------------------------------------------------------"<<endl;
    cout<<endl;
}

/* 显示菜单 */
char menu(){
    char order;
    std::cout<<"****************************************"<<endl;
    std::cout<<"*        Virtual Device Manager        *"<<endl;
    std::cout<<"****************************************"<<endl;
    std::cout<<"* [a].添加设备            [u].分配设备 *"<<endl;
    std::cout<<"*                                      *"<<endl;
    std::cout<<"* [r].回收设备            [d].删除设备 *"<<endl;
    std::cout<<"*                                      *"<<endl;
    std::cout<<"* [l].查看设备            [q].退出     *"<<endl;
    std::cout<<"****************************************"<<endl;
    std::cout<<"请输入指令(不区分大小写):";
    std::cin>>order;
    order = tolower(order);  /* 将输入的指令转化为小写 */
    return order;
}

int main(){
    system("chcp 65001");
    char order; /* 输入指令 */
    init();
    while(1){
        order = menu();
        switch (order)
        {
            case 'a':
                Add();
                break;
            case 'u':
                Distribute();
                break;
            case 'r':
                Recycle();
                break;
            case 'd':
                Delete();
                break;
            case 'l':
                show();
                break;
            case 'q':
                std::cout<<"已退出系统"<<endl;
                exit(0);
            default:
                std::cout<<"输入有误，请检查指令后重新输入"<<endl<<endl;
                break;
        }
    }
    
    system("pause");
    return 0;
}