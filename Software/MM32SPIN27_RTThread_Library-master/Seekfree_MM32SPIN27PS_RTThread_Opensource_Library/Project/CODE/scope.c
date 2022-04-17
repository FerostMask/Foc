/*------------------------------------------------------*/
/*                      头文件加载                       */
/*======================================================*/
#include "scope.h"
#include "string.h"
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
    bool isInt;
    struct ListNode *next;
} ListNode_t;

typedef enum SCOPE_UART_Enum
{
    UART_N = UART_2,         // 用到的UART模块
    SCOPE_RX = UART2_RX_C05, // UART接收引脚
    SCOPE_TX = UART2_TX_C04, // UART发送引脚
    SCOPE_BAUD = 115200,     // UART波特率
} SCOPE_UART_Enum;

#define FIXLEN 11
/*------------------------------------------------------*/
/*                       函数声明                       */
/*======================================================*/
ListNode_t *ListNode(void);
/*------------------------------------------------------*/
/*                       变量定义                       */
/*======================================================*/
ListNode_t *listHead;         // 链表头
ListNode_t *listEnd;          // 链表尾
static size_t listLength = 0; // 链表长度
static size_t nameLength = 0; // 数据名长度
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
}
/*------------------------------*/
/*        添加示波器显示        */
/*==============================*/
void scopeAddValue(void *p, size_t length, char *name, bool isInt)
{
    listEnd->next = ListNode();
    listEnd = listEnd->next;
    listEnd->p = p;
    listEnd->len = length;
    listEnd->name = name;
    listEnd->isInt = isInt;
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
    char *list = (char *)malloc(nameLength + FIXLEN);
    strcpy(list, "LISTSCOPE,");
    while (iter->next != NULL)
    {
        iter = iter->next;
        strcat(list, iter->name); // 加入数据名
        strcat(list, ",");        // 分隔符号
    }
    list[nameLength + FIXLEN - 2] = '\n';
    ips114_showstr(0, 0, list);
}

// void test()
// {
//     int value = 10;
//     scopeAddValue(&value, sizeof(value), "test1", false);
//     scopeAddValue(&value, sizeof(value), "test2", false);
//     scopeSendList();
// }

// void test()
// {
//     char *name = "value";
//     scopeAddValue(&value, sizeof(value), name, false);
//     ListNode_t *iter;
//     iter = listHead;
//     while (iter->next != 0)
//     {
//         iter = iter->next;
//         if (iter->isInt == true)
//         {
//             long showValue = 0;
//             memcpy(&showValue, iter->p, iter->len);
//             ips114_showint16(0, 1, showValue);
//         }
//         else
//         {
//             double showValue = 0;
//             memcpy(&showValue, iter->p, iter->len);
//             ips114_showfloat(0, 0, showValue, 3, 3);
//         }
//     }
// }

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
    pointer = (ListNode_t *)malloc(sizeof(ListNode_t));
    if (pointer == NULL)
    {
        _errorHandler();
    }
    return pointer;
}
