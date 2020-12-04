/************************************************
 * 故障点双向链表使用说明
 * 
 * 1. SelectDLink(struct eq_list* head, eq_huanqiu_t data)
 * 查找数据，返回 data 所在链表指针，没找到返回 NULL
 * 
 * 2. InsertDLink(struct eq_list* head, eq_huanqiu_t data)
 * 插入数据，如果链表未建立则初始化链表，返回链表首地址
 * 如果数据重复则返回重复数据所在元素地址，不重复返回创建元素地址
 * 出错返回NULL
 * 
 * 3. DeleteDLink(struct eq_list* head, eq_huanqiu_t deleteData)
 * 删除数据，删除链表中与 deleteData loop , point 对应的元素
 * 删除成功返回head地址
 * 如果链表里只有一个元素并删除了这个元素，则返回NULL
 * 
 * 4. DestoryDLink(struct eq_list* head)
 * 销毁链表，删除链表中所有元素，计数清零并返回NULL
 * 
 * 5. DisplayDLink(struct eq_list* head)
 * 打印链表元素点位号，Debug时使用
 * 
 * 注：使用 malloc 函数注意当前系统
 * 如果在FREERTOS里需要使用 pvPortMalloc 从 TOTAL_HEAP_SIZE 中申请内存
 *
 * 作者：Joseph Zhang
 * 日期：2020-12-04
 * **********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

uint8_t AbEquipQTY = 0;

typedef struct
{
    uint8_t CID;
    uint8_t dev;
    uint8_t loop;
    uint8_t point;
    uint8_t subpoint;
    uint8_t type;
    uint8_t status;
}eq_huanqiu_t;

struct eq_list
{
    struct eq_list* prior;
    eq_huanqiu_t eqvalue;
    struct eq_list* next;
};

/* 初始化双向链表 */
struct eq_list* InitDLink(eq_huanqiu_t data)
{
    struct eq_list* head = (struct eq_list*)malloc(sizeof(struct eq_list));
    if (head != NULL) 
    {
        head->eqvalue = data;
        head->next = head;
        head->prior = head;
        return head;
    }
    return(NULL);
    
}

/* 输出数据 */
void DisplayDLink(struct eq_list* head)
{
    uint8_t index = 1;
    if (head == NULL)
    {
        printf("\r\n**********本列表中有%d个故障点**********\r\n", AbEquipQTY);
        printf("双向链表=NULL\r\n");
        return;
    }
    struct eq_list* pNode = head;
    printf("\r\n**********本列表中有%d个故障点**********\r\n", AbEquipQTY);
    while (pNode->next != head)
    {
        printf("序号: %d	回路: %d	点号: %d	内存地址:%p\r\n", index, pNode->eqvalue.loop, pNode->eqvalue.point, pNode);
        pNode = pNode->next;
        index++;
    }// while结束后pNode是最后一个元素
    printf("序号: %d	回路: %d	点号: %d	内存地址:%p\r\n", index, pNode->eqvalue.loop, pNode->eqvalue.point, pNode);
}

struct eq_list*SelectDLink(struct eq_list* head, eq_huanqiu_t data)
{
    if (head == NULL)
    {
        printf("链表未初始化\r\n");
        return (NULL);
    }
    struct eq_list* pNode = head;
    // 第一个元素
    if (pNode->eqvalue.loop == data.loop && pNode->eqvalue.point == data.point)
    {
        return(pNode);
    }
    // 中间元素
    pNode = pNode->next;
    while (pNode != head)
    {
        if (pNode->eqvalue.loop == data.loop && pNode->eqvalue.point == data.point)
        {
            break;
        }
        pNode = pNode->next;
    }
    if (pNode == head) // 如果再次指向第一元素
    {
        // printf("没有找到需要的数据\n");
        return(NULL);
    }
    else
    {
        return(pNode);
    }
}

/* 添加数据 */
struct eq_list* InsertDLink(struct eq_list* head, eq_huanqiu_t data)
{
    struct eq_list *ptr = head;
    if (head == NULL)
    {
        AbEquipQTY++;
        return(InitDLink(data));
    }
    if ((ptr = SelectDLink(head, data)) != NULL)
    {
        printf("重复数据\r\n");
        return(ptr);
    }
    struct eq_list* node = (struct eq_list*)malloc(sizeof(struct eq_list));
    if (node != NULL) 
    {
        node->next = head;
        node->prior = head->prior;
        node->eqvalue = data;
        head->prior->next = node;
        head->prior = node;
        AbEquipQTY++;
        return(node);
    }
    return(NULL);
}

/* 删除数据 */
struct eq_list* DeleteDLink(struct eq_list* head, eq_huanqiu_t deleteData)
{
    if (head == NULL)
    {
        printf("链表未初始化\n");
        return(head);
    }
    // 第一个元素
    struct eq_list* pNode = head;
    if (deleteData.loop == pNode->eqvalue.loop
        && deleteData.point == pNode->eqvalue.point)
    {
        if (AbEquipQTY == 1)
        {
            free(pNode);
            AbEquipQTY = 0;
            return(NULL);
        }
        else
        {
            struct eq_list* qNode = pNode->next;
            pNode->eqvalue = qNode->eqvalue;    // 1.第二个元素数据移动到第一个元素
            pNode->next = qNode->next;          // 2.第二个元素的指向（第三个元素地址）移动到第一个元素的next 
            pNode->next->prior = pNode;         // 3.第三个元素的prior更改为第一个元素的地址
            free(qNode);                        // 4.释放第二个元素
            AbEquipQTY--;
            return(head);
        }
    }
    // 中间元素
    pNode = pNode->next;
    while (pNode != head)
    {
        if (deleteData.loop == pNode->eqvalue.loop
            && deleteData.point == pNode->eqvalue.point)
        {
            pNode->prior->next = pNode->next;       // 1.修改前一个元素的后指向
            pNode->next->prior = pNode->prior;      // 2.修改后一个元素的前指向
            free(pNode);                            // 3.释放该元素
            AbEquipQTY--;
            return(head);
        }
        pNode = pNode->next;
    }     
    if (pNode == head)  // 如果再次指向第一元素
    {
        printf("\r\n没有找到需要删除的数据 回路:%d 点号: %d\n", deleteData.loop,deleteData.point);
        return(head);
    }
    else
    {
        return(head);
    }
}

/* 销毁双向链表 */
struct eq_list* DestoryDLink(struct eq_list* head)
{
    if (head == NULL)
    {
        printf("空链表\r\n");
        return(NULL);
    }
    struct eq_list* node = head->prior;
    struct eq_list* pNode = head;
    struct eq_list* qNode = head;
    while (pNode != node)
    {
        qNode = pNode;
        pNode = pNode->next;
        free(qNode);
    }
    free(node);
    AbEquipQTY = 0;
    return(NULL);
}

int main()
{
    eq_huanqiu_t eqtest;
    memset(&eqtest,0,sizeof(eqtest));
    eqtest.loop = 0x01;
    eqtest.point = 0x02;
    struct eq_list* head =NULL;
    if(AbEquipQTY == 0)
        head = InsertDLink(head, eqtest);
    //InsertDLink(head, eqtest);
    //eqtest.point = 0x03;
    //InsertDLink(head, eqtest);
    //eqtest.point = 0x04;
    //InsertDLink(head, eqtest);
    //DisplayDLink(head);
    //eqtest.point = 0x05;
    DisplayDLink(head);
    head = DeleteDLink(head, eqtest);
    DisplayDLink(head);
    if (AbEquipQTY == 0)
        head = InsertDLink(head, eqtest);
    eqtest.point = 0x03;
    InsertDLink(head, eqtest);
    DisplayDLink(head);
    head = DestoryDLink(head);
    DisplayDLink(head);
}
