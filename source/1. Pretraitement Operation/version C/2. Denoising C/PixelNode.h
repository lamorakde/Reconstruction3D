#ifndef PIXELNODE_H_INCLUDED
#define PIXELNODE_H_INCLUDED

typedef struct PixelNode
{
    int u;
    int v;
    struct PixelNode * next;
}PixelNode;

struct PixelNode *creat(PixelNode *head, int coordinate_u, int coordinate_v);
struct PixelNode *insertPixel(PixelNode *head, int coordinate_u, int coordinate_v);

struct PixelNode *creat(PixelNode *head, int coordinate_u, int coordinate_v)
{
    PixelNode *p1;
    p1=(struct PixelNode*)malloc(sizeof(struct PixelNode));//*Lnode等于L，对与*Lnode的分配空间相当与对主函数中的L分配空间。

    p1->u = coordinate_u;
    p1->v = coordinate_v;
    p1->next=NULL;

    if (head==NULL)
    {
        head=p1;
    }

    return head;
}

struct PixelNode *insertPixel(PixelNode *head, int coordinate_u, int coordinate_v)
{
    PixelNode *p1,*p2;
    p1=p2=(struct PixelNode*)malloc(sizeof(struct PixelNode));

    p1=head;
    int loop;

    for(loop=0;;loop++)
    {
        if((p1->next)!=NULL)
        {
            p1 = p1->next;
        }
        else
        {
            p2->u = coordinate_u;
            p2->v = coordinate_v;
            p2->next = NULL;
            p1->next = p2;
            break;
        }
    }
    return head;
};

// 以下是用main函数测试代码
/*
int main()
{
    PixelNode * head;
    head=NULL;
    int u = 100;
    int v = 111;
    head=creat(head,u,v);


    u=200;
    v=222;
    head=insertPixel(head,u,v);


    // 以下代码会依次从链表首部开始打印链表像素信息
    int test;
    PixelNode * testPtr;
    testPtr=head;
    for(test=1;;test++)
    {
        printf("Coordinate u: %d      Coordinate u: %d\n",testPtr->u,testPtr->v);

        if( (testPtr->next)!=NULL )
        {
            testPtr=testPtr->next;
        }
        else break;
    }
}
*/

#endif // PIXELNODE_H_INCLUDED
