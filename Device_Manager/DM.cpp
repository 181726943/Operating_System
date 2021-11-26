#include <iostream>
#include <string>
#include <vector>
#include <queue>

using namespace std;

/* 进程控制块 */
struct PCB
{
    string process;  /* 进程ID */
    string device_name;  /* 申请设备名称 */
    string coid;    /* 进程所用控制器名称 */
    string chid;    /* 进程所用通道名称 */
};


/* 设备控制表 */
struct DCT
{
    string Dname;    /* 设备名称 */
    char type;    /* 设备类型 I/O */
    int state;    /* 设备状态1/0 */
    string coid;    /* 连接相应控制器的id */
};

/* 系统设备表 */
struct SDT
{
    string Dname;    /* 设备名称 */
    char type;    /* 设备类型I/O */
    DCT DCT;    /* 绑定的相应的设备 */
};

/* 控制器控制表 */
struct COCT
{
    string controllerid;    /* 控制器id */
    int state;    /* 控制器状态1/0 */
    string Chid;    /* 与控制器相连的通道的id */
};

/* 通道控制表 */
struct CHCT
{
    string channelid;  /* 通道id */
    int state;  /* 通道状态1/0 */
    vector <string> COid;   /* 与通道相连的控制器的id */
};

queue <DCT> block;  /* 阻塞队列 */
vector <SDT> sdt;
vector <COCT> coct;
vector <CHCT> chct(2);

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
    coct[0].controllerid = "co1";
    coct[0].state = 0;
    coct[0].Chid = "ch1";
    coct[1].controllerid = "co2";
    coct[1].state = 0;
    coct[1].Chid = "ch1";
    coct[2].controllerid = "co3";
    coct[2].state = 0;
    coct[2].Chid = "ch2";
    coct[3].controllerid = "co4";
    coct[3].state = 0;
    coct[3].Chid = "ch2";

    /* 初始化通道控制表 */
    chct[0].channelid = "ch1";
    chct[0].state = 0;
    chct[0].COid[0] = "co1";
    chct[0].COid[1] = "co2";
    chct[1].channelid = "ch2";
    chct[1].state = 0;
    chct[1].COid[2] = "co3";
    chct[1].COid[3] = "co4";
}

/* 添加设备 */
void Add(){
    SDT newDevice;  /* 新添加设备的结构体 */
    int addcon;  /* 是否添加控制器的标志 */
    COCT newco;

    if(sdt.size() >= 50){
        cout<<"设备数量已达上限，无法添加!!!"<<endl;
    }
    else{
        /* 设定设备名称 */
        cout<<"请输入设备名称:";    /* 输入添加设备的名称 */
        cin>>newDevice.Dname;
        for(int i=0; i<sdt.size(); i++){    /* 判定添加的设备是否已存在 */
            if(sdt[i].Dname.compare(newDevice.Dname) == 0){  /* 如果存在则退出，并提示添加失败 */
                cout<<"设备已存在，添加失败!!!"<<endl;
                return;
            }
        }
        /* 设定设备类型 */
        cout<<"请输入设备类型(I/O):";
        cin>>newDevice.type;
        while(newDevice.type != 'I' || newDevice.type != 'O'){    /* 判断输入值是否符合要求，只能是I/O */
            cout<<"输入不合法，请重新输入!!!"<<endl;
            cin>>newDevice.type;
        }
        newDevice.DCT.state = 0;    /* 新添加设备的状态 */
        /* 新设备控制器设置 */
        cout<<"是否添加新的控制器(0.否 1.是)"<<endl;
        cin>>addcon;
        while(addcon != 0 || addcon != 1){
            cout<<"输入不合法，请重新输入!"<<endl;
            cin>>addcon;
        }
        if(addcon == 0)  /* 不添加新的控制器 */
        {
            cout<<"请选择已有设备控制器(ch1,ch2,ch3,ch4):";
            cin>>newDevice.DCT.coid;
            while(newDevice.DCT.coid != "ch1" || newDevice.DCT.coid != "ch2" || newDevice.DCT.coid != "ch3" || newDevice.DCT.coid != "ch4"){
                cout<<"输入不合法，请重新输入!!!"<<endl;
                cin>>newDevice.DCT.coid;
            }
        }
        else{   /* 添加新的的控制器 */
            cout<<"请输入添加控制器的id：";
            cin>>newco.controllerid;
            for(int k=0; k<coct.size(); k++){
                if(newco.controllerid.compare(coct[k].controllerid) == 0){
                    k = 0;
                    cout<<"控制器已存在,请重新添加"<<endl;
                    cin>>newco.controllerid;
                }
            }
            newDevice.DCT.coid = newco.controllerid;  /* 将新设备与添加的控制器连接起来 */
            cout<<"请输入控制器所连通道的id(ch1,ch2)：";
            cin>>newco.Chid;
            while(newco.Chid.compare("ch1") != 0 || newco.Chid.compare("ch2") != 0){
                cout<<"输入不合法，请重新输入!!!"<<endl;
                cin>>newco.Chid;
            }
            /* 在通道控制表中加入新添加的控制器 */
            if(newco.Chid.compare("ch1") == 0)  /* 说明新添加的控制器与通道1相连 */
                chct[0].COid.push_back(newco.controllerid);
            else  /* 说明新添加的控制器与通道2相连 */
                chct[1].COid.push_back(newco.controllerid);
            coct.push_back(newco);
        }
        cout<<"设备添加成功！"<<endl;
        sdt.push_back(newDevice);   /* 设备添加完成，将新设备添加到SDT队列 */
    }
}

/* 分配设备 */
void Distribute(){
    string D_name;
    cout<<"请输入请求分配设备的ID:";
    cin>>D_name;
    for(int i=0; i<sdt.size(); i++){
        if (sdt[i].Dname.compare(D_name) == 0)  /* 申请设备存在 */
        {
            if(sdt[i].DCT.state == 1){  /* 设备被占用 */
                cout<<"设备被占用,申请失败!"<<endl;
                return;
            }
            else if(sdt[i].DCT.state == 0){  /* 设备空闲 */
                sdt[i].DCT.state = 1;
                for(int k=0; k<coct.size(); k++){  /* 查找控制该设备的控制器 */
                    if(sdt[i].DCT.coid.compare(coct[k].controllerid) == 0){
                        if(coct[k].state == 0){  /* 控制器空闲 */
                            coct[k].state = 1;
                            if(chct[0].state == 0){  /* 通道1空闲 */
                                chct[0].state = 1;  /* 将通道1状态置1(忙碌) */
                                cout<<"分配设备成功"<<endl;
                                return;
                            }
                            else if (chct[1].state == 0){   /* 通道2空闲 */
                                chct[1].state = 1;   /* 将通道2状态置1(忙碌) */
                                cout<<"分配设备成功"<<endl;
                                return;
                            }
                            else{   /* 通道忙碌 */
                                cout<<"通道阻塞，已将请求加入通道阻塞队列"<<endl;
                                return;
                            }
                        }
                    }
                }
                cout<<"控制器阻塞，已将控制器加入控制器阻塞队列"<<endl;
                return;
            }
        }
    }
}

int main(){
}