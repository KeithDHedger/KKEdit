/************************************************************************
*									*
* SysV message queue send/receive					*
*									*
* K.D.Hedger 2011							*
*									*
* kdhedger@yahoo.co.uk							*
*									*
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <getopt.h>

#define APPNAME "kkeditmsg"
#define VERSION "0.2.0"

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

void printHelp()
{
	printf("Usage: %s [OPTION] [TEXT]\n"
	       "A CLI application\n"
	       " -s, --send	Send message [TEXT] (defaults to receive)\n"
	       " -r, --receive	Print all messages in queue to stdout\n"
	       " -f, --flush	Flush message queue quietly\n"
	       " -k, --key	Use key [INTEGER] instead of generated one\n"
	       " -w, --wait	Wait for message's to arrive (blocking)\n"
	       " -W, --wait-first	Wait for first message to arrive (blocking) then continue\n"
	       " -a, --activate	Activate kkedit\n"
	       " -R, --remove	Remove Queue\n"
	       " -v, --version	output version information and exit\n"
	       " -h, -?, --help	print this help\n\n"
	       "Report bugs to kdhedger@yahoo.co.uk\n"
	       ,APPNAME);
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

int main(int argc, char **argv)
{
	int	c;
	int	key;
	int	retcode;
	const char *gg="G";
	
	buffer.mType=MSGRECEIVE;
	buffer.mText[0]=0;
	key=0xdeadbeef;

	while (1)
		{
			int option_index = 0;
			c = getopt_long (argc, argv, "v?hdwWfarRs:k:",long_options, &option_index);
			if (c == -1)
				break;

			switch (c)
				{
				case 's':
					strcpy(buffer.mText,optarg);
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
					break;

				case 'a':
					doActivateKKEdit=true;
					break;

				case 'R':
					doRemove=true;
					break;

				case 'v':
					printf("kkeditmsg %s\n",VERSION);
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

	if(action==true)
		sendMsg();
	else
		readMsg();

	if(doActivateKKEdit==true)
		{
			char *com;
			asprintf(&com,"kkedit -i %i",key);
			system(com);
			free(com);
		}
	return(ALLOK);

}

