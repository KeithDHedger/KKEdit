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
#include <gtksourceview/gtksourcestyleschememanager.h>
#include <gdk/gdkkeysyms.h>

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
#include "navcallbacks.h"

GtkWidget*		window=NULL;
GtkAccelGroup*	accgroup=NULL;
GtkNotebook*	notebook=NULL;
GtkWidget*		menubar=NULL;
GtkWidget*		menufile;
GtkWidget*		menuedit;
GtkWidget*		menufunc;
GtkWidget*		menunav;
GtkWidget*		menutools;
GtkWidget*		menuhelp;
GtkWidget*		menumanpage;
GtkWidget*		menuItemOpen=NULL;
GtkWidget*		menuView=NULL;
GtkWidget*		menuToolOut=NULL;
GtkWidget*		menuStatusBar=NULL;

GList*			newBookMarksList=NULL;
GtkWidget*		menuBookMark;
GtkWidget*		menuBookMarkSubMenu;
char*			highlightColour;
char*			tmpHighlightColour;
bool			showBMBar;
GtkWidget*		bmHighlightBox;
int				bmMarkNumber=0;

char*			toolBarLayout=NULL;
char*			tmpToolBarLayout=NULL;
GtkToolbar*		toolBar;
GtkWidget*		toolBarBox;

GtkWidget*		menuprint;
GtkWidget*		menuclose;
GtkWidget*		menucloseall;
GtkWidget*		menusaveall;
GtkWidget*		menurevert;

GtkWidget*		redoMenu;
GtkWidget*		undoMenu;
GtkWidget*		redoAllMenu;
GtkWidget*		undoAllMenu;
GtkWidget*		cutMenu;
GtkWidget*		copyMenu;
GtkWidget*		pasteMenu;

GtkWidget*		saveMenu;
GtkWidget*		saveAsMenu;

GtkWidget*		lineNumberWidget;
GtkWidget*		findApiWidget;
GtkWidget*		findQtApiWidget;
GtkWidget*		findDefWidget;
GtkWidget*		liveSearchWidget;

int				currentPage=0;
bool			nagScreen;
bool			tmpNagScreen;

GtkWidget*		prefswin;
bool			indent;
bool			lineNumbers;
bool			lineWrap;
bool			highLight;
bool			singleUse;
int				tabWidth;
char*			fontAndSize;
char*			terminalCommand;
char*			rootCommand;
int				depth;
bool			onExitSaveSession;
bool			restoreBookmarks;
char*			styleName=NULL;
bool			noDuplicates;
bool			tmpNoDuplicates;
bool			noWarnings;
bool			tmpNoWarnings;
bool			readLinkFirst;
bool			tmpReadLinkFirst;

GtkWidget*		fontButton;
GtkWidget*		terminalBox;
GtkWidget*		rootCommandBox;

//key short cuts
GtkWidget*		keysWindow=NULL;

bool			tmpIndent;
bool			tmpLineNumbers;
bool			tmpLineWrap;
bool			tmpHighLight;
bool			tmpSingleUse;
int				tmpTabWidth;
int				tmpDepth;
bool			tmpSaveSessionOnExit;
bool			tmpRestoreBookmarks;
char*			tmpStyleName=NULL;

GtkWidget*		toolNameWidget;
GtkWidget*		commandLineWidget;
GtkWidget*		keyWidget;
GtkWidget*		commentWidget;
GtkWidget*		inTermWidget;
GtkWidget*		inPopupWidget;
GtkWidget*		alwaysPopupWidget;
GtkWidget*		syncWidget;
GtkWidget*		ignoreWidget;
GtkWidget*		pasteWidget;
GtkWidget*		replaceWidget;
GtkWidget*		outputWidget;
GtkWidget*		showDocWidget;
GtkWidget*		toolSelect;
GtkWidget*		clearViewWidget;
GtkWidget*		runAsRootWidget;

//view tool output
GtkWidget*		mainVPane=NULL;
bool			showToolOutWin=false;
GtkWidget*		toolOutputView=NULL;
GtkTextBuffer*	toolOutputBuffer=NULL;
GtkWidget*		toolOutVBox=NULL;
bool			showStatus;
GtkWidget*		statusWidget=NULL;

char*			selectedToolPath=NULL;
GList*			toolsList=NULL;

GtkWidget*		restoreBMs;

//bool			inTerm=false;
//bool			inPopup=false;
//bool			alwaysPopup=false;
//bool			runSync=true;
//bool			ignoreOut=true;
//bool			pasteOut=false;
//bool			replaceOut=false;
//bool			viewOut=false;
//bool			showDoc=false;
//bool			editTool=false;
//bool			clearView=false;
//bool			runAsRoot=false;
//int				keyCode=0;

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

GtkToolItem*	newButton=NULL;
GtkToolItem*	openButton=NULL;
GtkToolItem*	saveButton=NULL;
GtkToolItem*	saveasButton=NULL;
GtkToolItem*	closeButton=NULL;
GtkToolItem*	redoButton=NULL;
GtkToolItem*	undoButton=NULL;
GtkToolItem*	cutButton=NULL;
GtkToolItem*	copyButton=NULL;
GtkToolItem*	pasteButton=NULL;
GtkToolItem*	findButton=NULL;
GtkToolItem*	gotoDefButton=NULL;
GtkToolItem*	backButton=NULL;
GtkToolItem*	gotoLineButton=NULL;
GtkToolItem*	findApiButton=NULL;
GtkToolItem*	findQtApiButton=NULL;
GtkToolItem*	findFuncDefButton=NULL;
GtkToolItem*	liveSearchButton=NULL;

//find replace
GtkWidget*		findReplaceDialog;
GtkWidget*		findBox;
GtkWidget*		replaceBox;
GtkWidget*		findDropBox=NULL;
GtkWidget*		replaceDropBox;
GSList*			findList=NULL;
GSList*			replaceList=NULL;

//custom toolbar
GtkWidget*		fromHBox;
GtkWidget*		iconViewBox;
bool			showToolBar=true;

int				listFunction=0;
GtkWidget*		funcListDrop=NULL;

char*			functionSearchText=NULL;
char*			thePage=NULL;
char*			htmlFile=NULL;
char*			htmlURI=NULL;

//runtime deps
int				gotManEditor;

GtkSourceStyleSchemeManager*	schemeManager;
GtkSourceStyleScheme*			styleScheme;

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

unsigned int	shortCuts[100][2]={
	{121,0},
	{63,1},
	{107,2},
	{72,3},
	{104,4},
	{68,5},
	{108,6},
	{109,7},
	{77,8},
	{93,9},
	{91,10},
	{39,11},
	{64,12}
};
//121 0 ^y Delete Current Line
//63 1 ^? Delete To End Of Line
//107 2 ^k Delete To Beginning Of Line
//72 3 ^H Select Word Under Cursor
//104 4 ^h Delete Word Under Cursor
//68 5 ^D Duplicate Current Line
//108 6 ^l Select Current Line
//109 7 ^m Move Current Line Up
//77 8 ^M Move Current Line Down
//93 9 ^] Select From Cursor To End Of Line
//91 10 ^[ Select From Beginning Of Line To Cursor
//39 11 ^' Move Selection Up
//64 12 ^@ Move Selection Down

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
	char*						command;
	char						line[1024];
	FILE*						fp;
	GFile*						gfile;
	GFileInfo*					gfileinfo;
	const char*					content_type;

	lm=gtk_source_language_manager_new();

	g_object_ref(lm);
	g_object_set_data_full(G_OBJECT(page->buffer),"languages-manager",lm,(GDestroyNotify)g_object_unref);
	lm=gtk_source_language_manager_get_default();

	gfile=g_file_new_for_path(page->filePath);
    gfileinfo=g_file_query_info(gfile,"standard::*",(GFileQueryInfoFlags)0,NULL,NULL);

    content_type=g_file_info_get_content_type(gfileinfo);
	mimetype=g_content_type_get_mime_type(content_type);

	if(mimetype==NULL)
		{
			char*	hold=NULL;

			mimetype=NULL;

			asprintf(&command,"xdg-mime query filetype \"%s\"",page->filePath);
			fp=popen(command,"r");
			fgets(line,1024,fp);
			hold=strdup(line);
			if(strchr(line,';')!=NULL)
				asprintf(&mimetype,"%s",sliceBetween(line,(char*)"",(char*)";"));
			else
				mimetype=strndup(line,strlen(line)-1);

			free(hold);
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
		else
			page->lang=NULL;

	g_object_unref(gfile);
	g_object_unref(gfileinfo);

	if(mimetype!=NULL)
		g_free(mimetype);
}


//				if(strcmp(rootCommand,"")!=0)
//					asprintf(&command,"%s kkedit -m &>/dev/null &",rootCommand);
//				else
//					asprintf(&command,"%s sudo kkedit -m &>/dev/null &",terminalCommand);


void runCommand(char* commandtorun,void* ptr,bool interm,int flags,int useroot)
{
	char*		command;
	FILE*		fp=NULL;
	GString*	str=NULL;
	char		line[1024];
	GtkTextIter	iter;
	char*		asroot=strdup("");

	if(useroot==true)
		{
			if(strcmp(rootCommand,"")!=0)
				asprintf(&asroot,"%s ",rootCommand);
			else
				{
					asprintf(&asroot,"sudo ");
					interm=true;
				}
		}

	if(interm==true)
		{
			asprintf(&command,"%s %s sh -c \"%s\"",terminalCommand,asroot,commandtorun);
			flags=8;
		}
	else
		{
			asprintf(&command,"%ssh -c \"%s\"",asroot,commandtorun);
		}

	if((flags & TOOL_ASYNC)==TOOL_ASYNC)
		{
			g_spawn_command_line_async(command,NULL);
		}
	else
		{
			fp=popen(command,"r");
			if(fp!=NULL)
				{
					if((flags & TOOL_VIEW_OP)==TOOL_VIEW_OP)
						{
							showToolOutWin=true;
							gtk_widget_show(toolOutVBox);
							gtk_menu_item_set_label((GtkMenuItem*)menuToolOut,"Hide Tool Output");
							while(fgets(line,1024,fp))
								{
									gtk_text_buffer_insert_at_cursor(toolOutputBuffer,line,strlen(line));
									while (gtk_events_pending())
	 									gtk_main_iteration();
	 								gtk_text_buffer_get_end_iter(toolOutputBuffer,&iter);
	 								gtk_text_view_scroll_to_iter((GtkTextView*)toolOutputView,&iter,0,true,0,0);
								}
						}
					else
						{
							str=g_string_new(NULL);
							while(fgets(line,1024,fp))
								 g_string_append(str,line);
							if(ptr!=NULL)
								*((char**)ptr)=str->str;
							g_string_free(str,false);
						}
					pclose(fp);
				}
		}

	if(flags & TOOL_SHOW_DOC)
		showDocView(USEFILE,(char*)"");

	free(command);
	free(asroot);
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
											fdata->line=linenumber+1;
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
	char		line[2048];
	GString*	str=g_string_new(NULL);
	char*		command;
	char*		newstr=NULL;
	char*		sort=NULL;

	if(filepath==NULL)
		return;

	switch (listFunction)
		{
			case 0:
				asprintf(&sort,"sort -k 2rb,2rb -k 1b,1b");
				break;
			case 1:
				asprintf(&sort,"sort -k 2rb,2rb -k 3n,3n");
				break;
			case 2:
				asprintf(&sort,"sort -k 3n");
				break;
			case 4:
				asprintf(&sort,"sort -k 2rb,2rb -k 1b,1b");
				break;
			default:
				asprintf(&sort,"sort");
				break;
		}

	asprintf(&command,"find \"%s\" -maxdepth %i|ctags -L - -x|%s|sed 's@ \\+@ @g'",filepath,depth,sort);

	fp=popen(command, "r");
	while(fgets(line,2048,fp))
		{
			newstr=deleteSlice(line,filepath);
			g_string_append_printf(str,"%s",newstr);
			g_free(newstr);
		}
	pclose(fp);

	*((char**)ptr)=str->str;
	g_string_free(str,false);
	g_free(command);
	g_free(sort);
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

gint sortTools(gconstpointer a,gconstpointer b)
{
	return(strcasecmp(((toolStruct*)a)->menuName,((toolStruct*)b)->menuName));
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
	int				clearview=0;
	char*			commandarg=NULL;
	char*			commentarg=NULL;
	char*			menuname=NULL;
	int				rootarg=0;
	int				keycode=0;

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
									rootarg=0;
									clearview=0;
									keycode=0;
									menuname=NULL;
									commandarg=NULL;
									commentarg=NULL;

									while(fgets(buffer,4096,fd))
										{
											buffer[strlen(buffer)-1]=0;
											sscanf((char*)&buffer,"%s",(char*)&strarg);
											if(strcmp(strarg,"name")==0)
												asprintf(&menuname,"%.*s",(int)strlen(buffer)-5,(char*)&buffer[5]);
											if(strcmp(strarg,"command")==0)
												asprintf(&commandarg,"%.*s",(int)strlen(buffer)-8,(char*)&buffer[8]);
											if(strcmp(strarg,"comment")==0)
												asprintf(&commentarg,"%.*s",(int)strlen(buffer)-8,(char*)&buffer[8]);
											if(strcmp(strarg,"interm")==0)
												sscanf((char*)&buffer,"%*s %i",&intermarg);
											if(strcmp(strarg,"flags")==0)
												sscanf((char*)&buffer,"%*s %i",&flagsarg);
											if(strcmp(strarg,"inpopup")==0)
												sscanf((char*)&buffer,"%*s %i",&inpopup);
											if(strcmp(strarg,"alwayspopup")==0)
												sscanf((char*)&buffer,"%*s %i",&alwayspopup);
											if(strcmp(strarg,"clearview")==0)
												sscanf((char*)&buffer,"%*s %i",&clearview);
											if(strcmp(strarg,"runasroot")==0)
												sscanf((char*)&buffer,"%*s %i",&rootarg);
											if(strcmp(strarg,"shortcutkey")==0)
												sscanf((char*)&buffer,"%*s %i",&keycode);
										}

									if((menuname!=NULL) && (strlen(menuname)>0) && ( commandarg!=NULL))
										{
											tool=(toolStruct*)malloc(sizeof(toolStruct));
											tool->menuName=strdup(menuname);
											tool->command=strdup(commandarg);											
											tool->flags=flagsarg;
											tool->inTerminal=(bool)intermarg;
											tool->inPopUp=(bool)inpopup;
											tool->alwaysPopup=(bool)alwayspopup;
											tool->filePath=strdup(filepath);
											tool->clearView=(bool)clearview;
											tool->runAsRoot=(bool)rootarg;
											tool->keyCode=keycode;
											
											if(commentarg!=NULL)
												tool->comment=strdup(commentarg);
											else
												tool->comment=NULL;
											if(loop==0)
												tool->global=true;
											else
												tool->global=false;

											toolsList=g_list_prepend(toolsList,(gpointer)tool);
											free(menuname);
											free(commandarg);
											if(commentarg!=NULL)
												free(commentarg);
										}

									fclose(fd);
								}
							entry=g_dir_read_name(folder);
						}
				}
		}
	g_free(datafolder[0]);
	g_free(datafolder[1]);

	toolsList=g_list_sort(toolsList,sortTools);
}

void rebuildBookMarkMenu(void)
{
	GtkWidget*	menuitem;
	GtkWidget*	submenu;

	submenu=gtk_menu_item_get_submenu((GtkMenuItem*)menuBookMark);
	if(submenu!=NULL)
		gtk_menu_item_set_submenu((GtkMenuItem*)menuBookMark,NULL);

	menuBookMarkSubMenu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuBookMark),menuBookMarkSubMenu);

	menuitem=gtk_menu_item_new_with_label("Remove All Bookmarks");
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBookMarkSubMenu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(removeAllBookmarks),NULL);

	menuitem=gtk_menu_item_new_with_label("Toggle Bookmark");
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBookMarkSubMenu),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(toggleBookmark),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menuBookMarkSubMenu),menuitem);
}

void goBack(GtkWidget* widget,gpointer data)
{
	pageStruct*		page=getPageStructPtr(-1);
	GtkTextIter		iter;
	GtkTextIter		currentiter;
	GtkTextMark*	cursor;

	cursor=gtk_text_buffer_get_insert((GtkTextBuffer*)page->buffer);
	gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&currentiter,cursor);

	gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&iter,page->backMark);
	gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(page->buffer),&iter);
	scrollToIterInPane(page,&iter);

	gtk_text_buffer_move_mark_by_name((GtkTextBuffer*)page->buffer,"back-mark",&currentiter);
}

