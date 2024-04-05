#include <reg52.h>

// 按键
// k0 增
// k1 减
// k2 整数
// k3 小数
// k4 停/重置
char kStatus[5] = {0};
char kStatusNew[5] = {0};

// 时间记录
char time[4] = {0};

// 工作状态
// -1 倒计时
// 0 停止
// 1 正计时
char countStatus = 0;

// 蜂鸣器
sbit buzzer = P2 ^ 3;

// 按键
sbit kLine0 = P3 ^ 3;
sbit kLine1 = P3 ^ 4;
sbit kLine2 = P3 ^ 5;
sbit kLine3 = P3 ^ 6;
sbit kLine4 = P2 ^ 7;

// 数码管显示
sbit dle = P2 ^ 6;
sbit wle = P2 ^ 7;

unsigned char order[] = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf};
unsigned char num[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};

void delay(unsigned int);
void showNum(unsigned char number, bit dp, unsigned char choose);
char *kDetect();

void main()
{
    // 初始状态
    // 蜂鸣器
    buzzer = 1;
    // 按键

    while (1)
    {
        showNum(1, 1, 1);
        kDetect();
    }
}

void showNum(unsigned char number, bit dp, unsigned char choose)
{
    P0 = order[choose - 1];
    wle = 1;
    wle = 0;
    P0 = num[number] + (dp ? 8 : 0);
    dle = 1;
    dle = 0;
    P0 = 0x00;
    delay(1);
}

char *kDetect()
{
    // 检测k4
    kLine0 = 1;
    kLine1 = 1;
    kLine2 = 1;
    kLine3 = 1;
    kLine4 = 1;
    if (0 == kLine4)
    {
        kStatusNew[4] = 1;
    }
    // 检测k3
    kLine1 = 1;
    kLine2 = 1;
    kLine3 = 1;
    kLine4 = 0;
    if (0 == kLine0)
    {
        kStatusNew[3] = 1;
    }
    // 检测k2
    kLine1 = 1;
    kLine2 = 1;
    kLine3 = 0;
    kLine4 = 1;
    if (0 == kLine0)
    {
        kStatusNew[2] = 1;
    }
    // 检测k1
    kLine1 = 1;
    kLine2 = 0;
    kLine3 = 1;
    kLine4 = 1;
    if (0 == kLine0)
    {
        kStatusNew[1] = 1;
    }
    // 检测k0
    kLine1 = 0;
    kLine2 = 1;
    kLine3 = 1;
    kLine4 = 1;
    if (0 == kLine0)
    {
        kStatusNew[0] = 1;
    }

    return kStatusNew;
}

void timeCount(char direction)
{
    if (1 == direction)
    {
        // 加
        time[3]++;
        if (time[3] > 9)
        {
            time[3] = 0;
            time[2]++;
            if (time[2] > 9)
            {
                time[2] = 0;
                time[1]++;
                if (time[1] > 9)
                {
                    time[1] = 0;
                    time[0]++;
                    if (time[0] > 5)
                    {
                        // 超出范围
                        time[0] = 0;
                    }
                }
            }
        }
    }
    else if (-1 == direction)
    {
        // 减
        time[3]--;
        if (time[3] < 0)
        {
            time[3] = 9;
            time[2]--;
            if (time[2] < 0)
            {
                time[2] = 9;
                time[1]--;
                if (time[1] < 0)
                {
                    time[1] = 9;
                    time[0]--;
                    if (time[0] < 0)
                    {
                        // 超出范围
                        time[0] = 5;
                    }
                }
            }
        }
    }
    return;
}

void delay(unsigned int xms)
{
    unsigned int i, j;
    for (i = xms; i > 0; i--)
        for (j = 110; j > 0; j--)
            ;
}