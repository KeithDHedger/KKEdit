/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * globals.cpp
 *
*/

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>

#include "config.h"
#ifdef BUILDDOCVIEWER
#include <webkit/webkit.h>
#endif

#ifdef _ASPELL_
#include <aspell.h>
#endif

#include "globals.h"
#include "searchcallbacks.h"
#include "callbacks.h"

GtkWidget*		window=NULL;
GtkNotebook*	notebook=NULL;
GtkWidget*		menubar=NULL;
GtkWidget*		menufile;
GtkWidget*		menuedit;
GtkWidget*		menufunc;
GtkWidget*		menunav;
GtkWidget*		menutools;
GtkWidget*		menuhelp;
GtkWidget*		menumanpage;

GtkWidget*		menubookmark;
GtkWidget*		menubookmarksub;
GtkWidget*		menuprint;
GtkWidget*		menuclose;
GtkWidget*		menucloseall;
GtkWidget*		menusaveall;
GtkWidget*		menurevert;

GtkWidget*		redoMenu;
GtkWidget*		undoMenu;
GtkWidget*		saveMenu;
GtkWidget*		saveAsMenu;

GtkWidget*		lineNumberWidget;
GtkWidget*		findApiWidget;
GtkWidget*		findDefWidget;
GtkWidget*		liveSearchWidget;

int				currentPage=0;

GtkWidget*		prefswin;
bool			indent;
bool			lineNumbers;
bool			lineWrap;
bool			highLight;
bool			singleUse;
int				tabWidth;
char*			fontAndSize;
char*			terminalCommand;
int				depth;
bool			onExitSaveSession;
bool			restoreBookmarks;
bool			showJumpToLine;
bool			showFindAPI;
bool			showFindDef;
bool			showLiveSearch;

GtkWidget*		fontBox;
GtkWidget*		terminalBox;
bool			tmpIndent;
bool			tmpLineNumbers;
bool			tmpLineWrap;
bool			tmpHighLight;
bool			tmpSingleUse;
int				tmpTabWidth;
int				tmpDepth;
bool			tmpSaveSessionOnExit;
bool			tmpRestoreBookmarks;
bool			tmpShowJumpToLine;
bool			tmpShowFindAPI;
bool			tmpShowFindDef;
bool			tmpShowLiveSearch;

GtkWidget*		toolNameWidget;
GtkWidget*		commandLineWidget;
GtkWidget*		inTermWidget;
GtkWidget*		inPopupWidget;
GtkWidget*		alwaysPopupWidget;
GtkWidget*		syncWidget;
GtkWidget*		ignoreWidget;
GtkWidget*		pasteWidget;
GtkWidget*		replaceWidget;
GtkWidget*		showDocWidget;
GtkWidget*		toolSelect;

char*			selectedToolPath=NULL;
GList*			toolsList=NULL;

GtkWidget*		restoreBMs;

bool			inTerm=false;
bool			inPopup=false;
bool			alwaysPopup=false;
bool			runSync=true;
bool			ignoreOut=true;
bool			pasteOut=false;
bool			replaceOut=false;
bool			showDoc=false;
bool			editTool=false;

int				windowWidth;
int				windowHeight;
int				windowX=-1;
int				windowY=-1;
bool			wrapSearch;
bool			insensitiveSearch;
bool			useRegex;
bool			replaceAll;
bool			findInAllFiles;

int				currentTabNumber;
int 			untitledNumber=1;

GtkToolItem*	newButton;
GtkToolItem*	openButton;
GtkToolItem*	saveButton;
GtkToolItem*	saveasButton;
GtkToolItem*	closeButton;
GtkToolItem*	redoButton;
GtkToolItem*	undoButton;
GtkToolItem*	sourceFormatButton;

GtkWidget*		findReplaceDialog;
GtkWidget*		findBox;
GtkWidget*		replaceBox;

char*			functionSearchText=NULL;
char*			thePage=NULL;
char*			htmlFile=NULL;
char*			htmlURI=NULL;

#ifdef BUILDDOCVIEWER
GtkWidget*		docView;
WebKitWebView*	webView;
#endif

//spellcheck
GtkWidget*		spellCheckWord;
GtkWidget*		wordListDropbox;
char*			badWord=NULL;
char*			goodWord=NULL;
#ifdef _ASPELL_
AspellConfig*	aspellConfig;
AspellSpeller*	spellChecker=0;
#endif


void scrollToIterInPane(pageStruct* page,GtkTextIter* iter)
{
	if(page->inTop==true)
		gtk_text_view_scroll_to_iter((GtkTextView*)page->view,iter,0,true,0,0.5);
	else
		gtk_text_view_scroll_to_iter((GtkTextView*)page->view2,iter,0,true,0,0.5);
}

pageStruct* getPageStructPtr(int pagenum)
{
	int			thispage;
	GtkWidget*	pageBox;

	if(pagenum==-1)
		thispage=gtk_notebook_get_current_page(notebook);
	else
		thispage=pagenum;

	pageBox=gtk_notebook_get_nth_page(notebook,thispage);
	if(pageBox==NULL)
		return(NULL);
	else
		return((pageStruct*)g_object_get_data((GObject*)pageBox,"pagedata"));
}

void getMimeType(char* filepath,void* ptr)
{
	char*	command;
	gchar	*stdout=NULL;
	gchar	*stderr=NULL;
	gint	retval=0;

	asprintf(&command,"file -b --mime-type %s",filepath);
	g_spawn_command_line_sync(command,&stdout,&stderr,&retval,NULL);
	if (retval==0)
		{
			stdout[strlen(stdout)-1]=0;
			*((char**)ptr)=strdup(stdout);
			g_free(stdout);
			g_free(stderr);
		}
}

void setLanguage(pageStruct* page)
{
	GtkSourceLanguage*			lang=NULL;
	GtkSourceLanguageManager*	lm=NULL;
	char*						mimetype;
	gboolean					result_uncertain;
	char*						command;
	char						line[1024];
	FILE*						fp;

	lm=gtk_source_language_manager_new();

	g_object_ref(lm);
	g_object_set_data_full(G_OBJECT(page->buffer),"languages-manager",lm,(GDestroyNotify)g_object_unref);
	lm=gtk_source_language_manager_get_default();
	mimetype=g_content_type_guess(page->filePath,NULL,0,&result_uncertain);

	if (result_uncertain)
		{
			g_free(mimetype);
			mimetype=NULL;

			asprintf(&command,"xdg-mime query filetype \"%s\"",page->filePath);
			fp=popen(command,"r");
			fgets(line,1024,fp);
			asprintf(&mimetype,"%s",sliceBetween(line,(char*)"",(char*)";"));
			pclose(fp);
		}

	lang=gtk_source_language_manager_guess_language(lm,page->filePath,mimetype);

	if (lang==NULL)
		{
			getMimeType((char*)page->filePath,&mimetype);
			lang=gtk_source_language_manager_guess_language(lm,page->filePath,mimetype);
			//g_print("Language: [%s]\n", gtk_source_language_get_name(lang));
			if (lang!=NULL)
				gtk_source_buffer_set_language(page->buffer,lang);
		}
	else
		{
			//g_print("Language: [%s]\n", gtk_source_language_get_name(lang));
			gtk_source_buffer_set_language(page->buffer,lang);
		}

	if(lang!=NULL)
		page->lang=gtk_source_language_get_name(lang);

	if(mimetype!=NULL)
		g_free(mimetype);
}


void runCommand(char* commandtorun,void* ptr,bool interm,int flags)
{
	char*	command;
	FILE*	fp=NULL;
	GString*	str=NULL;
	char		line[1024];

	if(interm==true)
		{
			asprintf(&command,"%s %s",terminalCommand,commandtorun);
			flags=8;
		}
	else
		command=strdup(commandtorun);

	if((flags & TOOL_ASYNC)==TOOL_ASYNC)
		{
			g_spawn_command_line_async(command,NULL);
		}
	else
		{
			fp=popen(command,"r");
			if(fp!=NULL)
				{
					str=g_string_new(NULL);
					while(fgets(line,1024,fp))
						 g_string_append(str,line);
					pclose(fp);
					if(ptr!=NULL)
						*((char**)ptr)=str->str;
					g_string_free(str,false);
				}
		}

	if(flags & TOOL_SHOW_DOC)
		showDocView(NULL,(void*)1);

	g_free(command);
}

functionData* getFunctionByName(char* name,bool recurse)
{
	pageStruct*	page;
	int			numpages=gtk_notebook_get_n_pages(notebook);
	char*		lineptr;
	char*		functions=NULL;
	char*		dirname;
	gchar*		stdout=NULL;
	char		function[1024];
	char		funcname[256];
	char		filepath[1024];
	int			linenumber;

	functionData* fdata;
	page=getPageStructPtr(-1);
	if(page==NULL)
		return(NULL);

	for(int loop=0;loop<numpages;loop++)
		{
			page=getPageStructPtr(loop);
			if(page->filePath!=NULL)
				{
					getRecursiveTagList(page->filePath,&functions);
					lineptr=functions;

					while (lineptr!=NULL)
						{
							sscanf (lineptr,"%s",function);
							if((strncasecmp(name,function,strlen(name))==0))
								{
									fdata=(functionData*)malloc(sizeof(functionData));
									sscanf (lineptr,"%"VALIDFUNCTIONCHARS"s",function);
									fdata->name=strdup(function);
									sscanf (lineptr,"%*s %"VALIDFUNCTIONCHARS"s",function);
									fdata->type=strdup(function);
									sscanf (lineptr,"%*s %*s %i",&fdata->line);
									sscanf (lineptr,"%*s %*s %*i %"VALIDFILENAMECHARS"s",function);
									fdata->file=strdup(function);
									sscanf (lineptr,"%*s %*s %*i %*s %"VALIDCHARS"s",function);
									fdata->define=strdup(function);
									fdata->intab=loop;
									return(fdata);
								}

							lineptr=strchr(lineptr,'\n');
							if (lineptr!=NULL)
								lineptr++;
						}
				}
		}
	if(recurse==true)
		{
//not in any open files
//check ./ from all files
//dont do this from popup for speed reasons
			for(int loop=0;loop<numpages;loop++)
				{
					page=getPageStructPtr(loop);
					if(page->filePath!=NULL)
						{
							dirname=strdup(g_path_get_dirname(page->filePath));
							getRecursiveTagListFileName(dirname,&stdout);

							lineptr=stdout;
							while (lineptr!=NULL)
								{
									sscanf (lineptr,"%s",function);
									if((strncasecmp(name,function,strlen(name))==0))
										{
											sscanf (lineptr, "%s\t%s\t%i",funcname,filepath,&linenumber);

											fdata=(functionData*)malloc(sizeof(functionData));
											fdata->name=strdup(funcname);
											fdata->file=strdup(filepath);
											fdata->line=linenumber;
											fdata->type=NULL;
											fdata->define=NULL;
											fdata->intab=-1;
											return(fdata);
										}

									lineptr=strchr(lineptr,'\n');
									if (lineptr!=NULL)
										lineptr++;
								}
							if(stdout!=NULL)
								g_free(stdout);
							if(dirname!=NULL)
								g_free(dirname);
						}
				}
		}

	return(NULL);
}

void destroyData(functionData* fdata)
{
	if(fdata!=NULL)
		{
			if(fdata->name!=NULL)
				g_free(fdata->name);
			if(fdata->type!=NULL)
				g_free(fdata->type);
			if(fdata->file!=NULL)
				g_free(fdata->file);
			if(fdata->define!=NULL)
				g_free(fdata->define);
			g_free(fdata);
		}
}

char* deleteSlice(char* srcstring,char* delstr)
{
	GString*	str=g_string_new(srcstring);
	char*		ptr;

	ptr=strstr(str->str,delstr);
	g_string_erase(str,(long)ptr-(long)str->str,strlen(delstr));
	return(g_string_free(str,false));
}

void getRecursiveTagListFileName(char* filepath,void* ptr)
{
	FILE*		fp;
	char		line[1024];
	GString*	str=g_string_new(NULL);
	char*		command;

	if(filepath==NULL)
		return;

	asprintf(&command,"find \"%s\" -maxdepth %i|ctags -L - --excmd=number --format=1 -f -",filepath,depth);
	fp=popen(command, "r");
	while(fgets(line,1024,fp))
		{
			g_string_append_printf(str,"%s",line);
		}
	pclose(fp);

	*((char**)ptr)=str->str;
	g_string_free(str,false);

	g_free(command);
}

void getRecursiveTagList(char* filepath,void* ptr)
{
	FILE*		fp;
	char		line[1024];
	GString*	str=g_string_new(NULL);
	char*		command;
	char*		newstr=NULL;

	if(filepath==NULL)
		return;

	asprintf(&command,"find \"%s\" -maxdepth %i|ctags -L - -x",filepath,depth);

	fp=popen(command, "r");
	while(fgets(line,1024,fp))
		{
			newstr=deleteSlice(line,filepath);
			g_string_append_printf(str,"%s",newstr);
			g_free(newstr);
		}
	pclose(fp);

	*((char**)ptr)=str->str;
	g_string_free(str,false);
	g_free(command);
}

//string slicing

char* slice(char* srcstring,int tmpstartchar,int tmpendchar)
{
	char*	dest;
	int		strsize;
	int		startchar=tmpstartchar;
	int		endchar=tmpendchar;

	if(tmpstartchar<0)
		startchar=0;

	if((tmpendchar<0) || (tmpendchar>(int)strlen(srcstring)))
		endchar=strlen(srcstring)-1;

	strsize=endchar-startchar+1;

	dest=(char*)malloc(strsize+1);
	strncpy(dest,(char*)&srcstring[startchar],strsize);
	dest[strsize]=0;

	return(dest);
}

char* sliceBetween(char* srcstring,char* startstr,char* endstr)
{
	int		startchar;
	int		endchar;
	char*	ptr;
	char*	dest=NULL;

	ptr=strstr(srcstring,startstr);
	if(ptr==NULL)
		return(NULL);
	startchar=(int)(long)ptr+strlen(startstr)-(long)srcstring;

	ptr=strstr((char*)&srcstring[startchar],endstr);
	if(ptr==NULL)
		return(NULL);
	endchar=(int)(long)ptr-(long)srcstring-1;

	dest=slice(srcstring,startchar,endchar);
	return(dest);
}

char* sliceLen(char* srcstring,int tmpstartchar,int len)
{
	char*	dest;
	int		strsize;
	int		startchar=tmpstartchar;
	int		endchar=len;

	if(tmpstartchar<0)
		startchar=0;

	if((len<0) || (len+startchar>(int)strlen(srcstring)))
		endchar=strlen(srcstring)-startchar;

	strsize=endchar;

	dest=(char*)malloc(strsize+1);
	strncpy(dest,(char*)&srcstring[startchar],endchar);
	dest[endchar]=0;

	return(dest);
}

char* sliceStrLen(char* srcstring,char* startstr,int len)
{
	char*	ptr;
	int		startchar;

	ptr=strstr(srcstring,startstr);
	if(ptr==NULL)
		return(NULL);
	startchar=(int)(long)ptr+strlen(startstr)-(long)srcstring;
	printf("%i\n",startchar);
	return(sliceLen(srcstring,startchar,len));
}

void destroyTool(gpointer data)
{
	if(((toolStruct*)data)->menuName!=NULL)
		g_free(((toolStruct*)data)->menuName);
	if(((toolStruct*)data)->filePath!=NULL)
		g_free(((toolStruct*)data)->filePath);
	if(((toolStruct*)data)->command!=NULL)
		g_free(((toolStruct*)data)->command);
	g_free(data);
}

void buildToolsList(void)
{
	GDir*			folder;
	const gchar*	entry=NULL;
	FILE*			fd=NULL;
	char*			filepath;
	char			buffer[4096];
	toolStruct*		tool;
	char*			datafolder[2];
	char			strarg[1024];
	
	int				intermarg=0;
	int				flagsarg=0;
	int				inpopup=0;
	int				alwayspopup=0;
	char*			commandarg=NULL;
	char*			menuname=NULL;

	if(toolsList!=NULL)
		{
			g_list_free_full(toolsList,destroyTool);
			toolsList=NULL;
		}

	asprintf(&datafolder[0],"%s/tools/",DATADIR);
	asprintf(&datafolder[1],"%s/.KKEdit/tools/",getenv("HOME"));
	for(int loop=0;loop<2;loop++)
		{
			folder=g_dir_open(datafolder[loop],0,NULL);
			if(folder!=NULL)
				{
					entry=g_dir_read_name(folder);
					while(entry!=NULL)
						{
							asprintf(&filepath,"%s%s",datafolder[loop],entry);
							fd=fopen(filepath,"r");
							if(fd!=NULL)
								{
									intermarg=0;
									flagsarg=0;
									inpopup=0;
									alwayspopup=0;

									while(fgets(buffer,4096,fd))
										{
											buffer[strlen(buffer)-1]=0;
											sscanf((char*)&buffer,"%s",(char*)&strarg);
											if(strcmp(strarg,"name")==0)
												asprintf(&menuname,"%.*s",(int)strlen(buffer)-5,(char*)&buffer[5]);
											if(strcmp(strarg,"command")==0)
												asprintf(&commandarg,"%.*s",(int)strlen(buffer)-8,(char*)&buffer[8]);
											if(strcmp(strarg,"interm")==0)
												sscanf((char*)&buffer,"%*s %i",&intermarg);
											if(strcmp(strarg,"flags")==0)
												sscanf((char*)&buffer,"%*s %i",&flagsarg);
											if(strcmp(strarg,"inpopup")==0)
												sscanf((char*)&buffer,"%*s %i",&inpopup);
											if(strcmp(strarg,"alwayspopup")==0)
												sscanf((char*)&buffer,"%*s %i",&alwayspopup);
										}

									if((menuname!=NULL) &&(strlen(menuname)>0))
										{
											tool=(toolStruct*)malloc(sizeof(toolStruct));
											tool->menuName=strdup(menuname);
											tool->command=strdup(commandarg);											
											tool->flags=flagsarg;
											tool->inTerminal=(bool)intermarg;
											tool->inPopUp=(bool)inpopup;
											tool->alwaysPopup=(bool)alwayspopup;
											tool->filePath=strdup(filepath);
											toolsList=g_list_prepend(toolsList,(gpointer)tool);
										}
									g_free(menuname);
									g_free(commandarg);
									menuname=NULL;
									commandarg=NULL;
									fclose(fd);
								}
							entry=g_dir_read_name(folder);
						}
				}
		}
	g_free(datafolder[0]);
	g_free(datafolder[1]);
}
