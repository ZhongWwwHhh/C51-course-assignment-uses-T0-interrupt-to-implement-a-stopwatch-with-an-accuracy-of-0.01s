#include <reg52.h>

// 循环控制位
char i;

// 按键
// k0 增
// k1 减
// k2 整数
// k3 小数
// k4 停/重置
char kStatus[5] = {0};
char kStatusNew[5] = {0};
sbit kLine0 = P3 ^ 3;
sbit kLine1 = P3 ^ 4;
sbit kLine2 = P3 ^ 5;
sbit kLine3 = P3 ^ 6;
sbit kLine4 = P3 ^ 7;

// 时间记录
char time[4] = {0};

// 工作状态
// -2 暂停
// -1 倒计时
// 0 停止
// 1 正计时
char countStatus = 0;

// 蜂鸣器
sbit buzzer = P2 ^ 3;

// 数码管显示
sbit dle = P2 ^ 6;
sbit wle = P2 ^ 7;
unsigned char order[] = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf};
unsigned char num[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};

// 原函数
void delay(unsigned int);
void showNum(unsigned char number, bit dp, unsigned char choose);
void kDetect();
void timeCount(char direction);

void main()
{
    // 初始化
    // 蜂鸣器
    buzzer = 1;
    // 中断
    TMOD = 0x01;
    TH0 = (65536 - 9216) / 256;
    TL0 = (65536 - 9216) % 256;
    EA = 1;
    ET0 = 1;
    TR0 = 0;

    while (1)
    {
        // 探测按键
        kDetect();
        // 分辨新按下的按键
        for (i = 0; i < 5; i++)
        {
            if (0 == kStatus[i] && 1 == kStatusNew[i])
            {
                kStatus[i] = 1;
            }
            else if (1 == kStatus[i] && 1 == kStatusNew[i])
            {
                kStatusNew[i] = 0;
            }
            else if (1 == kStatus[i] && 0 == kStatusNew[i])
            {
                kStatus[i] = 0;
            }
        }
        // 识别按键操作
        if (0 == countStatus)
        {
            if (1 == kStatusNew[4])
            {
                // 开始计时
                if (0 == time[0] && 0 == time[1] && 0 == time[2] && 0 == time[3])
                {
                    // 正计时
                    countStatus = 1;
                    TR0 = 1;
                }
                else
                {
                    // 倒计时
                    countStatus = -1;
                    TR0 = 1;
                }
            }
            else if (1 == kStatus[2])
            {
                // 操作秒数
                if (1 == kStatusNew[0])
                {
                    // 秒增
                    time[1] += 1;
                    if (9 < time[1])
                    {
                        time[1] = 0;
                        time[0] += 1;
                        if (5 < time[0])
                        {
                            time[0] = 0;
                        }
                    }
                }
                else if (1 == kStatusNew[1])
                {
                    // 秒减
                    time[1] -= 1;
                    if (0 > time[1])
                    {
                        time[1] = 9;
                        time[0] -= 1;
                        if (0 > time[0])
                        {
                            time[0] = 5;
                        }
                    }
                }
            }
            else if (1 == kStatus[3])
            {
                // 操作小数秒
                if (1 == kStatusNew[0])
                {
                    // 小数秒增
                    timeCount(1);
                    buzzer = 1;
                }
                else if (1 == kStatusNew[1])
                {
                    // 小数秒减
                    timeCount(-1);
                    buzzer = 1;
                }
            }
        }
        else if (-2 == countStatus)
        {
            if (1 == kStatusNew[4])
            {
                // 归零停止
                for (i = 0; i < 4; i++)
                {
                    time[i] = 0;
                }
                countStatus = 0;
            }
        }
        else if (1 == countStatus || -1 == countStatus)
        {
            if (1 == kStatusNew[4])
            {
                // 暂停静音
                TR0 = 0;
                countStatus = -2;
                buzzer = 1;
                if (0 == time[0] && 0 == time[1] && 0 == time[2] && 0 == time[3])
                {
                    // 全为零进入停止
                    countStatus = 0;
                }
            }
        }

        // 显示
        showNum(time[0], 0, 1);
        showNum(time[1], 1, 2);
        showNum(time[2], 0, 3);
        showNum(time[3], 0, 4);
    }
}

void T0_time() interrupt 1
{
    // 重装初值
    TH0 = (65536 - 9216) / 256;
    TL0 = (65536 - 9216) % 256;
    // 走时
    if (1 == countStatus || -1 == countStatus)
    {
        timeCount(countStatus);
    }
}

void showNum(unsigned char number, bit dp, unsigned char choose)
{
    P0 = order[choose - 1];
    wle = 1;
    wle = 0;
    P0 = num[number] + (dp ? 0x80 : 0x00);
    dle = 1;
    dle = 0;
    P0 = 0x00;
    delay(1);
}

void kDetect()
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

    return;
}

void timeCount(char direction)
{
    if (1 == direction)
    {
        // 加
        time[3] += 1;
        if (time[3] > 9)
        {
            time[3] = 0;
            time[2] += 1;
            if (time[2] > 9)
            {
                time[2] = 0;
                time[1] += 1;
                if (time[1] > 9)
                {
                    time[1] = 0;
                    time[0] += 1;
                    if (time[0] > 5)
                    {
                        // 超出范围
                        time[0] = 5;
                        time[1] = 9;
                        time[2] = 9;
                        time[3] = 9;
                        // 正计时结束
                        TR0 = 0;
                        buzzer = 0;
                    }
                }
            }
        }
    }
    else if (-1 == direction)
    {
        // 减
        time[3] -= 1;
        if (time[3] < 0)
        {
            time[3] = 9;
            time[2] -= 1;
            if (time[2] < 0)
            {
                time[2] = 9;
                time[1] -= 1;
                if (time[1] < 0)
                {
                    time[1] = 9;
                    time[0] -= 1;
                    if (time[0] < 0)
                    {
                        // 超出范围
                        time[0] = 0;
                        time[1] = 0;
                        time[2] = 0;
                        time[3] = 0;
                        // 倒计时结束
                        TR0 = 0;
                        buzzer = 0;
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