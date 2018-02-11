/*
 *
 * ©K. D. Hedger. Wed 19 Oct 20:25:49 BST 2016 kdhedger68713@gmail.com

 * This file (main.cpp) is part of KKEdit.

 * KKEdit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEdit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEdit.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <getopt.h>

#include "../../config.h"

#define APPNAME "kkeditmsg"
#define MSGVERSION "0.2.0"

#define MAX_MSG_SIZE 4096

#define ALLOK 0
#define UNKNOWNARG 1
#define NOMAKEQUEUE 2
#define NOSENDMSG 3
#define WAIT_MSG 0

#define MSGANY 0
#define MSGSEND 1
#define MSGRECEIVE 2

struct option long_options[] =
{
	{"send",1,0,'s'},
	{"all",0,0,'a'},
	{"key",1,0,'k'},
	{"wait",0,0,'w'},
	{"wait-first",0,0,'W'},
	{"block",0,0,'b'},
	{"flush",0,0,'f'},
	{"activate",0,0,'A'},
	{"version",0,0,'v'},
	{"help",0,0,'?'},
	{0, 0, 0, 0}
};

struct msgBuffer
{
	long mType;
	char mText[MAX_MSG_SIZE];
};

int			queueID;
msgBuffer	buffer;
bool		action=false;
int			receiveType=IPC_NOWAIT;
bool		printAll=false;
bool		allDone=false;
bool		flushQueue=false;
bool		doActivateKKEdit=false;
bool		doRemove=false;
bool		waitFirst=false;
int			sinkReturn;

const char	*commandList[]={"Quit","GotoLine","SearchDef","SelectTab","SelectTabByPath","Bookmark","CloseTab","SetMark","UnsetMark","MoveTo","SelectBetween","Paste","Copy","Cut","InsertText","InsertNL","InsertFile","PrintFiles","WaitForKKEdit","ShowContinue","RunTool","RestoreSession","ActivateMenuNamed","ActivateMenuLabeled","SendPosData","SendSelectedText",NULL};

void printHelp()
{
	unsigned cnt=0;
	printf("Usage: %s [OPTION] [TEXT]\n"
	       "A CLI application\n"
	       " -s, --send	Send message [TEXT] (defaults to receive)\n"
	       " -r, --receive	Print all messages in queue to stdout\n"
	       " -f, --flush	Flush message queue quietly\n"
	       " -k, --key	Use key [INTEGER] instead of generated one\n"
	       " -w, --wait	Wait for message's to arrive (blocking)\n"
	       " -W, --wait-first	Wait for first message to arrive (blocking) then continue\n"
	       " -b, --block	Wait for first message to arrive (blocking) then continue receved message is discarded\n"
	       " -a, --activate	Activate kkedit\n"
	       " -R, --remove	Remove Queue\n"
	       " -v, --version	output version information and exit\n"
	       " -h, -?, --help	print this help\n\n"
	       "Report bugs to kdhedger@yahoo.co.uk\n"
	       ,APPNAME _EXECSUFFIX_);

	printf("\nCommands recognised by KKEdit:\n");
	while(commandList[cnt]!=NULL)
		{
			printf("%s ",commandList[cnt]);
			cnt++;
		}
	printf("\n");
}

void sendMsg()
{
	buffer.mType=MSGSEND;
	if((msgsnd(queueID,&buffer,strlen(buffer.mText)+1,0))==-1)
		{
			fprintf(stderr,"Can't send message :(\n");
			exit(NOSENDMSG);
		}
}

void readMsg()
{
	int retcode;

	retcode=msgrcv(queueID,&buffer,MAX_MSG_SIZE,MSGRECEIVE,receiveType);

	if(retcode>1)
		printf("%s",buffer.mText);
	else
		allDone=true;
}

void block(void)
{
	int retcode;

	retcode=msgrcv(queueID,&buffer,MAX_MSG_SIZE,MSGRECEIVE,WAIT_MSG);
}

int main(int argc, char **argv)
{
	int	c;
	int	key;
	int	retcode;
	const char *gg="G";
	bool	doblock=false;
	
	buffer.mType=MSGRECEIVE;
	buffer.mText[0]=0;
	key=0xdeadbeef;

	while (1)
		{
			int option_index = 0;
			c = getopt_long (argc, argv, "v?hdbwWfarRs:k:",long_options, &option_index);
			if (c == -1)
				break;

			switch (c)
				{
				case 's':
					strcpy(buffer.mText,optarg);
					sendMsg();

					action=true;
					break;

				case 'r':
					printAll=true;
					break;

				case 'f':
					flushQueue=true;
					break;

				case 'w':
					receiveType=WAIT_MSG;
					break;

				case 'W':
					waitFirst=true;
					break;

				case 'k':
					key=atoi(optarg);
	if((queueID=msgget(key,IPC_CREAT|0660))==-1)
		{
			fprintf(stderr,"Can't create message queue\n");
			exit(NOMAKEQUEUE);
		}
					break;

				case 'a':
					doActivateKKEdit=true;
					break;

				case 'R':
					doRemove=true;
					break;

				case 'b':
					doblock=true;
					break;

				case 'v':
					printf("kkeditmsg %s\n",MSGVERSION);
					return ALLOK;
					break;

				case '?':
				case 'h':
					printHelp();
					return ALLOK;
					break;

				default:
					fprintf(stderr,"?? Unknown argument ??\n");
					return UNKNOWNARG;
					break;
				}
		}

	if((queueID=msgget(key,IPC_CREAT|0660))==-1)
		{
			fprintf(stderr,"Can't create message queue\n");
			exit(NOMAKEQUEUE);
		}

	if (flushQueue==true)
		{
			allDone=false;
			while(allDone==false)
				{
					retcode=msgrcv(queueID,&buffer,MAX_MSG_SIZE,MSGANY,receiveType);
					if(retcode<=1)
						allDone=true;
				}
			return(ALLOK);
		}

	if(doActivateKKEdit==true)
		{
			char *com;
			sinkReturn=asprintf(&com,"kkedit" _EXECSUFFIX_ " -i %i",key);
			sinkReturn=system(com);
			free(com);
		}

	if(doblock==true)
		{
			block();
			return(ALLOK);
		}

	if (printAll==true)
		{
			if(waitFirst==true)
				{
					receiveType=WAIT_MSG;
					readMsg();
					receiveType=IPC_NOWAIT;
				}
			while(allDone==false)
				readMsg();
			return(ALLOK);
		}

	if(doRemove==true)
		{
			queueID=msgget(key,IPC_CREAT|0660);
			msgctl(queueID,IPC_RMID,NULL);
		}


	if(action==false)
		readMsg();

	return(ALLOK);

}

