/*------------------------------------------------------*/
/*                      头文件加载                       */
/*======================================================*/
#include "stdio.h"
#include "scope.h"
#include "string.h"
#include "stdint.h"
#include "funcinter.h"
#include "SEEKFREE_IPS114_SPI.h"
/*------------------------------------------------------*/
/*                       类型定义                       */
/*======================================================*/
typedef struct ListNode_t
{
    void *p; // 指向值所在地址的指针
    size_t len;
    char *name;
    bool isInteger;
    struct ListNode *next;
} ListNode_t;

enum SCOPE_UART_Enum
{
    UART_N = UART_2,         // 用到的UART模块
    SCOPE_RX = UART2_RX_C05, // UART接收引脚
    SCOPE_TX = UART2_TX_C04, // UART发送引脚
    SCOPE_BAUD = 115200,     // UART波特率
};

#define FLOAT_FORMAT "%.3f"
/*------------------------------------------------------*/
/*                       函数声明                       */
/*======================================================*/
ListNode_t *ListNode(void);
/*------------------------------------------------------*/
/*                       变量定义                       */
/*======================================================*/
ListNode_t *listHead;             // 链表头
ListNode_t *listEnd;              // 链表尾
static size_t listLength = 0;     // 链表长度
static size_t nameLength = 0;     // 数据名长度
static size_t lastListLength = 0; // 最后一次记录的链表长度
/*------------------------------------------------------*/
/*                       函数定义                       */
/*======================================================*/
/*------------------------------*/
/*         初始化示波器         */
/*==============================*/
void scopeInit(void)
{
    listHead = ListNode();
    listEnd = listHead;
    uartInit(SCOPE_BAUD, SCOPE_TX, SCOPE_RX);
    // scopePushValue(&value, sizeof(value), "int", true);
    // scopePushValue(&value2, sizeof(value2), "double", false);
}
/*------------------------------*/
/*        添加示波器显示        */
/*==============================*/
void scopePushValue(void *p, size_t length, char *name, bool isInteger)
{
    listEnd->next = ListNode();
    listEnd = listEnd->next;
    listEnd->p = p;
    listEnd->len = length;
    listEnd->name = name;
    listEnd->isInteger = isInteger;
    listLength++;                   // 维护链表长度
    nameLength += strlen(name) + 1; // 维护数据名长度
}
/*------------------------------*/
/*         发送数据列表         */
/*==============================*/
void scopeSendList(void)
{
    ListNode_t *iter;
    iter = listHead;
    // 构造发送数据
    char *list = "SCOPELIST";
    uartPutBuff(list, strlen(list));
    while (iter->next != NULL)
    {
        iter = iter->next;
        uartPutBuff(",", 1);
        uartPutBuff(iter->name, strlen(iter->name));
    }
    uartPutBuff("\n", 1);
}

void scopeSendValue(void)
{
    if (lastListLength != listLength)
    { // 检测是否新添加发送数据
        lastListLength = listLength;
        scopeSendList(); // 更新数据列表
    }
    ListNode_t *iter;
    iter = listHead;
    // 构造发送数据
    char *list = "SCOPEVALUE";
    uartPutBuff(list, strlen(list));
    while (iter->next != NULL)
    {
        iter = iter->next;
        if (iter->isInteger == true)
        {
            long value;
            memcpy(&value, iter->p, iter->len);
            char input[20];
            sprintf(input, "%ld", value);
            uartPutBuff(",", 1);
            uartPutBuff(input, strlen(input));
        }
        else
        {
            double value;
            if (iter->len == sizeof(float)) // float转double需要额外转换
            {
                float transform;
                memcpy(&transform, iter->p, iter->len);
                value = (double)transform;
            }
            else
            {
                memcpy(&value, iter->p, iter->len);
            }
            char input[20];
            sprintf(input, FLOAT_FORMAT, value);
            uartPutBuff(",", 1);
            uartPutBuff(input, strlen(input));
        }
    }
    uartPutBuff("\n", 1);
}

void scope(void)
{
    static uint8_t count = 0;
    count++;
    if (count >= 25)
    {
        count = 0;
        scopeSendList();
    }
    rt_thread_mdelay(1);
    scopeSendValue();
}

static void _errorHandler(void)
{
    while (1)
    {
    };
}
/*------------------------------*/
/*           申请节点           */
/*==============================*/
ListNode_t *ListNode(void)
{
    ListNode_t *pointer;
    pointer = (ListNode_t *)malloc(sizeof(ListNode_t)); // 注意，申请太多空间没有释放的话内存会溢出
    if (pointer == NULL)
    {
        _errorHandler();
    }
    return pointer;
}
