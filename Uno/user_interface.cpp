#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "uno.h"
#include "utilities.h"

extern int main_state;
extern STATE game_state;

extern CARDSET CARDS[5];
extern CARDSET cards_to_play;

extern HANDLE inputMutex, messageListMutex;
extern MSGLISTNODEPTR msgListPtr, msgListTailPtr;

void addMsg(char * msg)
{
	WaitForSingleObject(messageListMutex, INFINITE);

	MSGLISTNODEPTR newPtr;

	newPtr = (MSGLISTNODEPTR)malloc(sizeof(MSGLISTNODE));
	newPtr->msg = (char *)malloc(sizeof(char) * strlen(msg));
	strcpy(newPtr->msg, msg);
	newPtr->nextPtr = NULL;

	if (msgListPtr == NULL)
	{
		msgListPtr = newPtr;
		msgListTailPtr = msgListPtr;
	}
	else
	{
		msgListTailPtr->nextPtr = newPtr;
		msgListTailPtr = newPtr;
	}

	ReleaseMutex(messageListMutex);
}

void delMsg()
{
	if (msgListPtr != NULL)
	{
		while (msgListPtr != NULL)
		{
			MSGLISTNODEPTR tmp;

			tmp = msgListPtr;
			msgListPtr = msgListPtr->nextPtr;
			tmp->nextPtr = NULL;
			free(tmp);
		}
	}

}

void printMsg()
{
	WaitForSingleObject(messageListMutex, INFINITE);
	MSGLISTNODEPTR currentPtr = msgListPtr, tmp;
	
	printf("��Ϣ�б�:\n");
	while (currentPtr != NULL)
	{
		printf("Msg: %s\n", currentPtr->msg);
		currentPtr = currentPtr->nextPtr;
		
	}
	printf("----------------\n");

	ReleaseMutex(messageListMutex);
}

void printUI()
{
	
	printGameState(game_state);
	printf("��������б���\n");
	printCardset(CARDS[game_state.player]);
	if(cards_to_play.size > 0)
	{
		printf("����Գ������ƣ�\n");
		printCardset(cards_to_play);
	}
}

DWORD WINAPI userInterfaceThread(LPVOID pM)
{
	while (main_state != GAME_END)
	{
		system("cls");
		
		switch (main_state)
		{
			case ROUND_START:
				printUI();
				Sleep(100);
				while(main_state != PLAY_CARD);
				break;
		
			case PLAY_CARD:
				printUI();
				Sleep(1000);
				
				WaitForSingleObject(inputMutex, INFINITE);
				break;
		}
	}

	return 0;
}