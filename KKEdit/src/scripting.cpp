/*
 *
 * ©K. D. Hedger. Thu  6 Oct 12:10:09 BST 2016 kdhedger68713@gmail.com

 * This file (scripting.cpp) is part of KKEdit.

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

#include <sys/msg.h>

#include "kkedit-includes.h"

int			queueID=-1;
msgStruct	message;
int			msgType=1;
int			receiveType=IPC_NOWAIT;
bool		waitForFinish=false;
bool		waitForUserContinue=false;

int sendRealMsg(int msglen)
{
	if((msgsnd(queueID,&message,msglen,0))==-1)
		{
			fprintf(stderr,"Can't send message :(\n");
			return(NOSENDMSG);
		}
	return(ALLOK);
}

void sendMsg(const char *msg)
{
	unsigned 	segstart=0;
	unsigned	slen=strlen(msg);

	message.mType=MSGSEND;
	if(slen>=MAX_MSG_SIZE)
		{
			while(slen-segstart>MAX_MSG_SIZE)
				{
					snprintf(message.mText,MAX_MSG_SIZE-1,"%s",msg);
					if(sendRealMsg(strlen(message.mText))!=ALLOK)
						return;
					msg+=(MAX_MSG_SIZE-2);
					segstart+=(MAX_MSG_SIZE-2);
				}
			if(strlen(msg)>0)
				{
					snprintf(message.mText,strlen(msg)+1,"%s",msg);
					if(sendRealMsg(strlen(msg)+1)!=ALLOK)
						return;
				}
		}
	else
		{
			snprintf(message.mText,MAX_MSG_SIZE-1,"%s",msg);
			if(sendRealMsg(strlen(message.mText))!=ALLOK)
				return;
	}
}

void sendOK(void)
{
	message.mType=MSGSEND;
	snprintf(message.mText,MAX_MSG_SIZE-1,"%s","ok");
	sendRealMsg(strlen(message.mText));
	waitForFinish=false;
}

void sendContinue(void)
{
	message.mType=MSGSEND;
	snprintf(message.mText,MAX_MSG_SIZE-1,"%s","continue");
	sendRealMsg(strlen(message.mText));
	waitForUserContinue=false;
	gtk_widget_hide(continueMenu);
}

GtkWidget* activateMenuInBar(GtkWidget *parent,const gchar *name)
{
	GtkWidget	*widg=NULL;
	if (GTK_IS_CONTAINER(parent))
		{
			GList *children=gtk_container_get_children(GTK_CONTAINER(parent));
			while(children!=NULL)
				{
				GtkWidget* widget=NULL;
				 widget=(GtkWidget*)children->data;
				if(widget!=NULL)
					{
						holdWidget=NULL;
						widg=findMenu(gtk_menu_item_get_submenu((GtkMenuItem*)widget),name);
						if(holdWidget!=NULL)
							{
								gtk_menu_item_activate((GtkMenuItem*)holdWidget);
								g_list_free(children);
								return(NULL);
							}
					}
					children=g_list_next(children);
				}
			g_list_free(children);
		}
	return NULL;
}

const char	*commandList[]={"Quit","GotoLine","SearchDef","SelectTab","Bookmark","CloseTab","SetMark","UnsetMark","ActivateMenu","MoveTo","Paste","InsertText","InsertNL","InsertFile","PrintFiles","WaitForKKEdit","ShowContinue","RunTool","RestoreSession",NULL};

enum {SQUIT=0,SGOTOLINE,SSEARCHDEF,SSELECTTAB,SBOOKMARK,SCLOSETAB,SSETMARK,SUNSETMARK,SACTIVATEMENU,SMOVETO,SPASTE,SINSERTTEXT,SINSERTNL,SINSERTFILE,SPRINTFILES,SWAITFORKKEDIT,SSHOWCONTINUE,SRUNTOOL,SRESTORESESSION};

const char	*commandArgsArray[10]={NULL,};
unsigned	commandArgsCnt=0;
unsigned	commandNumber=666;

void runKKCommand(void)
{
	pageStruct	*page=getPageStructByIDFromPage(-1);
	GtkTextIter	iter;
	char		buffer[256];
	char		*syscommand;
	GString		*str;
	FILE		*fp;

	switch(commandNumber)
		{
//quit app
			case SQUIT:
				doShutdown(NULL,NULL,NULL);
				break;
//goto line on current page
			case SGOTOLINE:
				{
					long line=0;
					if(commandArgsCnt==0)
						return;
							line=atoi(commandArgsArray[0]);
							if(fromGOpen==true)
								gtk_widget_show_all((GtkWidget*)(GtkTextView*)page->view);
							gotoLine(NULL,(void*)line);
				}
				break;
//search for define
			case SSEARCHDEF:
				if(commandArgsCnt==0)
					return;
				defSearchFromBar((GtkWidget*)commandArgsArray[0],NULL);
				break;
//switch to tab by name
			case SSELECTTAB:
				if(commandArgsCnt==0)
					return;
				for(int j=-0;j<gtk_notebook_get_n_pages(mainNotebook);j++)
					{
						page=getPageStructByIDFromPage(j);
						if(strcmp(page->fileName,commandArgsArray[0])==0)
							{
								gtk_notebook_set_current_page(mainNotebook,j);
								break;
							}
					}
				break;
//toggle bookmark
			case SBOOKMARK:
				toggleBookmark(NULL,NULL);
				break;
//close current tab
			case SCLOSETAB:
				closeTab(NULL,(void*)-1);
				break;
//set user mark
			case SSETMARK:
				if(commandArgsCnt==0)
					return;
				setUserMark(commandArgsArray[0]);
				break;
//unset user mark
			case SUNSETMARK:
				if(commandArgsCnt==0)
					return;
				removeUserMark(commandArgsArray[0]);
				break;
//select menu
			case SACTIVATEMENU:
				if(commandArgsCnt==0)
					return;
				holdWidget=NULL;
				activateMenuInBar((GtkWidget*)menuBar,commandArgsArray[0]);
				break;
//goto offset at current line
			case SMOVETO:
				if(commandArgsCnt==0)
					return;
				gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&iter,gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer));
				gtk_text_iter_set_line_offset(&iter,atoi(commandArgsArray[0]));
				gtk_text_buffer_place_cursor((GtkTextBuffer*)page->buffer,&iter);
				break;
//paste clipboard at current pos
			case SPASTE:
				pasteFromClip(NULL,NULL);
				break;
//insert text at current pos
			case SINSERTTEXT:
				if(commandArgsCnt==0)
					return;
				gtk_text_buffer_insert_at_cursor((GtkTextBuffer*)page->buffer,(const gchar*)commandArgsArray[0],-1);
				break;
//insert newlines at current pos
			case SINSERTNL:
				if(commandArgsCnt==0)
					return;
				sprintf(buffer,"%c",'\n');
				for(int j=0;j<atoi(commandArgsArray[0]);j++)
					gtk_text_buffer_insert_at_cursor((GtkTextBuffer*)page->buffer,(const gchar*)buffer,-1);
				break;			
//insert file at current pos
			case SINSERTFILE:
				if(commandArgsCnt==0)
					return;
				str=g_string_new(NULL);
				sinkReturn=asprintf(&syscommand,"cat %s",commandArgsArray[0]);
				fp=popen(syscommand,"r");
				if(fp!=NULL)
					{
						while(fgets(buffer,256,fp))
							g_string_append_printf(str,"%s",buffer);
						gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(page->buffer),str->str,str->len);
						pclose(fp);
					}
				debugFree(&syscommand);
				g_string_free(str,true);
				break;
//print list of open files
			case SPRINTFILES:
				{
					char			*strarray=NULL;
					unsigned int	buffersize=1000;
					char			*pagepath=NULL;
					FILE			*savestdout;
					char			nl[2]={'\n',0};
					char			*com=NULL;
					strarray=(char*)calloc(buffersize,1);
					for(int j=0;j<gtk_notebook_get_n_pages(mainNotebook);j++)
						{
							pagepath=(char*)(pageStruct*)(getPageStructByIDFromPage(j))->filePath;
							if(pagepath!=NULL)
								{
									if(buffersize<(strlen(strarray)+strlen(pagepath)+2))
										{
											buffersize+=1000;
											strarray=(char*)realloc(strarray,buffersize);
										}
									if((pagepath!=NULL) && (strlen(pagepath)>0))
										{
											strcat(strarray,pagepath);
											strcat(strarray,nl);
										}
								}
						}
					if(strarray!=NULL)
						{
							sendMsg(strarray);
							free(strarray);
						}
				}
				break;
//wait for this instanace to finish what its doing ( open files etc )
			case SWAITFORKKEDIT:
				waitForFinish=true;
				break;
//wait for user to continue
			case SSHOWCONTINUE:
				waitForUserContinue=true;
				gtk_widget_show(continueMenu);
				break;
//run external tool by menu name
			case SRUNTOOL:
				{
					GList		*tools=toolsList;
					const char	*label;
					if(commandArgsCnt==0)
						return;
					while(tools!=NULL)
						{
							label=((toolStruct*)(tools->data))->menuName;
							if(strcmp(label,commandArgsArray[0])==0)
								{
									externalTool(NULL,tools->data);
									break;
								}
							tools=tools->next;
						}
				}
				break;
//restore session by filename
			case SRESTORESESSION:
				if(commandArgsArray[0]==NULL)
					restoreSession(NULL,NULL);
				else
					restoreSession(NULL,(gpointer)commandArgsArray[0]);
				break;
		}
}

void doKKCommand(void)
{
	char		*command;
	char		*token=NULL;
	unsigned	commandnumber=0;

	commandnumber=0;
	command=message.mText;
	token=strtok(command,":");

	for(int j=0;j<10;j++)
		commandArgsArray[j]=NULL;
	commandArgsCnt=0;
		
	while(token!=NULL)
		{
			while(commandList[commandnumber]!=NULL)
				{
					if(strcmp(commandList[commandnumber],token)==0)
						{
							//printf("found command=%s ",commandList[commandnumber]);
							commandNumber=commandnumber;
							token=strtok(NULL,":");
							while(token!=NULL)
								{
									commandArgsArray[commandArgsCnt]=token;
									commandArgsCnt++;
									//printf("args=%s ",token);
									token=strtok(NULL,":");
								}
							//printf("\n");
							runKKCommand();
							return;
						}
					commandnumber++;
				}
			token=strtok(NULL,":");
		}
}

void getMsg(void)
{
	int retcode=1;
	retcode=msgrcv(queueID,&message,MAX_MSG_SIZE,MSGRECEIVE,receiveType);

	while(retcode>1)
		{
			doKKCommand();
			retcode=msgrcv(queueID,&message,MAX_MSG_SIZE,MSGRECEIVE,receiveType|MSG_NOERROR);
		}
}

void createQueue(void)
{
	if((queueID=msgget(sessionID,IPC_CREAT|0660))==-1)
		{
			fprintf(stderr,"Can't create message queue, scripting wont work :( ...\n");
		}
}



