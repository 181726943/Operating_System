#include <iostream>
#include <Windows.h>
#include <queue>
#include <stack>
#include <vector>
#include <iomanip>
using namespace std;

int *page = new int(-1);   //页面访问序列(用容器存储)
int content;               //内存块数
int *memory = new int(-1); //装入内存块中的页面(定义内存中可以存放四个页面)
float lost = 0;            //缺页次数
queue<int> q_memory;       //FIFO算法内存块
stack<int> s_memory;       //LRU算法内存块
stack<int> s_copy;         //副栈，用来交换数据

bool isInside(int num); //页面是否在内存中
void OPT(int num);      //最佳置换
void FIFIO(int num);    //先进先出置换
void LRU(int num);      //最近最久未使用

bool isInside(int num)
{
    for (int i = 0; i < 4; i++)
    {
        if (page[num] == memory[i])
            return true;
    }
    return false;
}

/* 最优置换算法 */
void OPT(int num, int capcity)
{                     //此处的num表示将要访问的页面在页面序列中的位置
    int next_max = 0; //内存中的页面据下一次出现的距离
    int change;       //内存中所要换出的页面的位置
    int m;
    bool inavailable = true; //内存不可用标记，true表示不可用，false表示可用
    if (isInside(num))
    { //所访问的页面在内存中
        cout << "访问成功!" << endl;
        cout << "此时内存中的页面为：";
        for (int i = 0; i < capcity; i++)
        {
            if (memory[i] != -1)
                cout << memory[i] << "  ";
            else
                break;
        }
        cout << endl<< endl;
    }
    else
    { //表示访问失败，即所访问的页面不在内存中
        cout << "缺页" << endl;
        lost++; //访问失败数加一
        for (int j = 0; j < capcity; j++)
        {
            if (memory[j] == -1)
            { //寻找内存块中是否还有空间
                memory[j] = page[num];
                inavailable = false;
                cout << "内存中尚有空间，无需置换" << endl;
                cout << "此时内存中的页面为：";
                for (int i = 0; i < capcity; i++)
                {
                    if (memory[i] != -1)
                        cout << memory[i] << "  ";
                }
                cout << endl<< endl;
                break;
            }
        }
        if (inavailable)
        { //表示内存中已经没有空间了，需要换出页面
            //下面的循环寻找内存中的页面将来第一次访问距现在最远的页面
            for (int j = 0; j < capcity; j++)
            {
                for (m = num; m < 20; m++)
                    /* 此处不要将循环嵌套，因为会出现一种情况，就是内存中的某个页面永久不再访问
                那么m就等于20，但是下面的条件默认会再次访问，因此就会出现 next_max 和 
                change 无法赋值，程序出现问题 */
                    if (memory[j] == page[m]) //寻找当前内存中的页面在随后再次被访问的位置
                        break;
                if (m > next_max)
                {                 //此处大于表示内存中的这个页面到目前为止是最晚被访问的
                    next_max = m; //m表示在这个地方会再次出现给定页面
                    change = j;   //内存中要被交换出去的页面的索引号即 memory[change]
                }
            }
            cout << "置换memory[" << change << "]=" << memory[change] << "处页面:" << endl;
            memory[change] = page[num];
            cout << "此时内存中的页面为：";
            for (int i = 0; i < capcity; i++)
            {
                if (memory[i] != -1)
                    cout << memory[i] << "  ";
            }
            cout << endl<< endl;
        }
    }
}

/* 先进先出算法 */
void FIFO(int num, int capcity)
{
    if (!q_memory.empty())
    { //判断队列不为空
        for (int i = 0; i < q_memory.size(); i++)
        { //将队列中的页面复制到内存数组memory中
            memory[i] = q_memory.front();
            q_memory.pop();
            q_memory.push(memory[i]);
        }
        if (isInside(num))
        { //判断所访问的页面是否在内存中
            cout << "访问成功!" << endl;
            cout << "此时内存中的页面为：";
            for (int i = 0; i < q_memory.size(); i++)
            {
                cout << memory[i] << "  ";
            }
            cout << endl<< endl;
        }
        else
        { //访问的页面不在内存中
            cout << "缺页" << endl;
            lost++;                        //缺页次数加一
            if (q_memory.size() < capcity) //内存是否已满，队列长度小于4表示内存未满
                q_memory.push(page[num]);  //直接将页面加入内存
            else
            { //内存已满，需要先换出一个页面再将访问的页面添加进去
                q_memory.pop();
                q_memory.push(page[num]);
            }
            cout << "此时内存中的页面为："; //输出当前内存中的页面
            for (int i = 0; i < q_memory.size(); i++)
            {
                cout << q_memory.front() << "  ";
                q_memory.push(q_memory.front());
                q_memory.pop();
            }
            cout << endl<< endl;
        }
    }
    else
    { //队列为空，内存中当前没有页面
        cout << "缺页" << endl;
        lost++;                   //缺页数加一
        q_memory.push(page[num]); //将页面添加到内存中
        cout << "此时内存中的页面为：";
        cout << q_memory.front();
        cout << endl << endl;
    }
}

/* 最近最久未使用算法 */
void LRU(int num, int capcity)
{
    if (!s_memory.empty())
    { //内存栈不为空
        //访问的页面在内存中,此时这个算法就涉及到一个问题，需要将该页面置于栈顶
        int top = 0;
        for (int i = s_memory.size(); i > 0; i--)
        {
            if (s_memory.top() != page[num])
            {
                s_copy.push(s_memory.top());
                s_memory.pop();
            }
            else
            {
                top = s_memory.top();
                s_memory.pop();
            }
        }
        for (int i = s_copy.size(); i > 0; i--)
        {
            s_memory.push(s_copy.top());
            s_copy.pop();
        }
        if (top)
            s_memory.push(top);
        if (s_memory.top() == page[num])
        {
            cout << "访问成功" << endl;
            cout << "此时内存中的页面为：";
            for (int i = s_memory.size(); i > 0; i--)
            { //输出内存中的页面
                cout << s_memory.top() << "  ";
                s_copy.push(s_memory.top());
                s_memory.pop();
            }
            cout << endl<< endl;
            for (int i = s_copy.size(); i > 0; i--)
            { //将副栈中的页面放回内存栈
                s_memory.push(s_copy.top());
                s_copy.pop();
            }
        }
        else
        { //访问的页面不在内存中
            cout << "缺页" << endl;
            lost++;
            if (s_memory.size() < capcity)
            { //内存未满
                s_memory.push(page[num]);
                cout << "此时内存中的页面为：";
                for (int i = s_memory.size(); i > 0; i--)
                {
                    cout << s_memory.top() << "  ";
                    s_copy.push(s_memory.top());
                    s_memory.pop();
                }
                cout << endl<< endl;
                for (int i = s_copy.size(); i > 0; i--)
                { //将副栈中的页面放回内存栈
                    s_memory.push(s_copy.top());
                    s_copy.pop();
                }
            }
            else
            { //内存已满,需要将栈底的页面换出，因此要将栈倒置
                for (int i = s_memory.size(); i > 0; i--)
                { //将内存栈中的页面放入副栈
                    if (s_memory.size() != 1)
                    { //不是栈底页面就放入副栈
                        s_copy.push(s_memory.top());
                        s_memory.pop();
                    }
                    else
                    { //栈底页面直接换出无需存入副栈
                        s_memory.pop();
                    }
                }
                for (int i = s_copy.size(); i > 0; i--)
                { //再把页面放入内存栈
                    s_memory.push(s_copy.top());
                    s_copy.pop();
                }
                s_memory.push(page[num]);
                cout << "此时内存中的页面为：";
                for (int i = s_memory.size(); i > 0; i--)
                {
                    cout << s_memory.top() << "  ";
                    s_copy.push(s_memory.top());
                    s_memory.pop();
                }
                cout << endl<< endl;
                for (int i = s_copy.size(); i > 0; i--)
                { //将副栈中的页面放回内存栈
                    s_memory.push(s_copy.top());
                    s_copy.pop();
                }
            }
        }
    }
    else
    {                             //内存栈为空，表示可以直接将页面调入内存
        lost++;                   //内存栈为空，访问的页面不在内存，缺页
        s_memory.push(page[num]); //将页面存入内存
        cout << "此时内存中的页面为：";
        cout << s_memory.top();
        cout << endl
             << endl;
    }
}

int main()
{
    system("chcp 65001");
    int k = 0;
    int page_len; //页面数
    int order;    //指令号
    bool loop = true;
    cout << "输入页面访问序列(至多20个，多于20的将自动舍去)：";
    while (cin >> page[k] && k < 19)
    {
        k++;
    }
    page_len = k + 1;
    cout << "输入内存块数：";
    cin >> content;
    while (loop)
    {
        cout << "*------------------------------------------------*" << endl;
        cout << "|**************Virtual Page Manager**************|" << endl;
        cout << "|------------------------------------------------|" << endl;
        cout << "| [1].OPT算法               [2].FIFO算法         |" << endl;
        cout << "| [3].LRU算法               [4].更新页面访问序列 |" << endl;
        cout << "| [5].重新分配内存块        [6].退出系统         |" << endl;
        cout << "*------------------------------------------------*" << endl;
        cout << "请输入指令号(1~6): ";
        cin >> order;
        switch (order)
        {
        case 1:
        {
            cout << "当前执行OPT算法" << endl;
            lost = 0; //重置缺页数
            for (int i = 0; i < content; i++)
            { //初始化内存块
                memory[i] = -1;
            }
            for (int opt = 0; opt < page_len; opt++)
            { //opt:页面访问序列的索引
                cout << "读取page[" << opt << "]=" << page[opt] << endl;
                OPT(opt, content);
            }
            cout << "页面总数为:" << page_len << endl;
            cout << "缺页次数为:" << lost << endl;
            cout << "命中率为  :" << (1 - lost / page_len) << endl;
        }
        break;
        case 2:
        {
            cout << "当前执行FIFO算法" << endl;
            lost = 0;
            for (int i = 0; i < content; i++)
            { //初始化内存块
                memory[i] = -1;
            }
            for (int fifo = 0; fifo < page_len; fifo++)
            {
                cout << "读取page[" << fifo << "]=" << page[fifo] << endl;
                FIFO(fifo, content);
            }
            cout << "页面总数为:" << page_len << endl;
            cout << "缺页次数为:" << lost << endl;
            // double res = 1 - (double)(lost / page_len);
            cout << "命中率为  :" << (1 - lost / page_len) << endl;
            // printf("%f", res);
            // cout <<float(res) << endl;
        }
        break;
        case 3:
        {
            cout << "当前执行LRU算法" << endl;
            lost = 0;
            for (int i = 0; i < content; i++)
            { //初始化内存块
                memory[i] = -1;
            }
            for (int lru = 0; lru < page_len; lru++)
            {
                cout << "读取page[" << lru << "]=" << page[lru] << endl;
                LRU(lru, content);
            }

            cout << "页面总数为:" << page_len << endl;
            cout << "缺页次数为:" << lost << endl;
            cout << "命中率为  :" << (1 - lost / page_len) << endl;
        }
        break;
        case 4:
        {
            k = 0;
            cout << "输入页面访问序列(至多20个，多于20的将自动舍去)：";
            while (cin >> page[k] && k < 19)
            {
                k++;
            }
            page_len = k + 1;
        }
        break;
        case 5:
        {
            cout << "请输入新内存块的大小： ";
            cin >> content;
            for (int i = 0; i < content; i++)
            { //初始化内存块
                memory[i] = -1;
            }
        }
        break;
        case 6:
            cout << "已退出系统!" << endl;
            exit(0);
        default:
            cout << "输入错误，请检查输入(只能从1~6中选取)！！！";
            break;
        }
    }
    delete [] page;
    delete [] memory;
    system("pause");
    return 0;
}