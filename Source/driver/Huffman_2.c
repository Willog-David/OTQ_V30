/*
 * Huffman_2.c
 *
 *  Created on: 2021. 11. 19.
 *      Author: Administrator
 */


#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include "struct.h"
#include "string.h"

#define INT_MAX 800


typedef struct NODE
{
   char v;
   uint16 cnt;
   struct NODE*next;
   struct NODE*left;
   struct NODE*right;
}NODE;

NODE *head;

uint16 STACK[100];
sint16 top = -1;

void push(uint16 n)
{
   top += 1;
   STACK[top] = n;
}

uint16 pop()
{
   uint16 result = STACK[top];
   top -= 1;
   return result;
}

void addToSLL(NODE* cur)
{
   if (head == 0)
   {
      head = cur;
      return;
   }

   NODE*temp = head;
   while (temp->next != 0)
   {
      temp = temp->next;
   }
   temp->next = cur;
}



void createVertex(char _v)
{
	static uint16  count=0;
   NODE*newOne = (NODE*)calloc(sizeof(NODE),1);
   newOne->v = _v;
   newOne->cnt = 1;
   newOne->left = newOne->right = newOne->next = 0;

   addToSLL(newOne);
   count++;
}

NODE* findVertex(char v)
{
   NODE*temp = head;
   while (temp != 0)
   {
      if (temp->v == v)
      {
         return temp;
      }
      temp = temp->next;
   }
   return 0;
}

void cntCharacterNum(char* _buf)
{
   char v;
   for (int i = 0; i < 425; i++)
   {
      //if (_buf[i] == '\0')
      //{
         //return;
      //}
      //else
      //{
          v = _buf[i];
          if (findVertex(v) == 0)
          {
             createVertex(v);
          }
          else
          {
             NODE* find = findVertex(v);
             find->cnt += 1;
          }
      //}
   }
}

NODE* findSmallest()
{

   int short_num = INT_MAX;
   NODE*short_node = head;

   if (head == 0)
   {
      return 0;
   }

   NODE*temp = head;
   while (temp != 0)
   {
      if (temp->cnt < short_num)
      {
         short_node = temp;
         short_num = temp->cnt;
      }
      temp = temp->next;
   }

   if (short_node == head)
   {
      head = head->next;
   }
   else
   {
      NODE*prev = head;
      while (prev->next != short_node)
      {
         prev = prev->next;
      }
      prev->next = short_node->next;
   }

   short_node->next = 0;
   return short_node;
}

NODE* MakeNewNode(void)
{
	NODE*newNode = (NODE*)malloc(sizeof(NODE));
	memset(newNode,0x00,sizeof(NODE));
	return newNode;
}

NODE* buildHuffmanTree( )
{
	uint16 cnt;
   while (1)
   {
      NODE* first = findSmallest();   // 빈도�일  것을 뽑는
      NODE* second = findSmallest();

      if (second == 0)   // 마�막에 �리�뽑힌
      {
         return first;
      }

      NODE*newOne = MakeNewNode();

      newOne->next = 0;
      newOne->left = first;
      newOne->right = second;
      newOne->cnt = first->cnt + second->cnt;

      addToSLL(newOne);   // �리 �체�구성�으롣는
   }

}


void showAll()
{
   NODE*temp = head;
   while (temp != 0)
   {
      printf("%c : %d\n", temp->v, temp->cnt);
      temp = temp->next;
   }
}

void showCode()
{
   for (int i = top-1; i >= 0; i--)
   {
      printf("%d", STACK[i]);
   }
   //printf("\n");
}

void inorderTraversal(NODE* cur)
{
   if (cur == 0)
   {
      pop();
      return;
   }
   push(0);
   inorderTraversal(cur->left);
   if (cur->left == 0 && cur->right == 0)
   {
      printf("% : ",cur->v);
      showCode();
   }
   push(1);
   inorderTraversal(cur->right);
   pop();
}

int Huffman_Encode(uint8 *Huffbuf, uint16 len)
{
   //FILE*f = 0;
   //f = fopen("alice.txt", "rt");

	for(int i=0;i<len;i++)
	{
		if(Huffbuf[i]==0x00)Huffbuf[i] = 0xfe;
	}
   for (int i=0;i<1;i++)
   {
      //char buf[100];

      //fgets(buf, 99, f);

      // buf�긴 �용글�씩 �으면서
      // �character개수를다.

      //  buf[i] != '\0'
      // 마�막에 �는 것� 99개까지 못읽�다. null characet가 �닌 �만 개수른야 �다.

      cntCharacterNum(Huffbuf);
   }

   //showAll();

   NODE*tree = buildHuffmanTree();

   inorderTraversal(tree);

   return 0;
}
