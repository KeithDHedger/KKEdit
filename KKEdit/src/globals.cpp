/*
 *
 * ©K. D. Hedger. Sun 25 Oct 14:49:34 GMT 2015 kdhedger68713@gmail.com

 * This file(globals.cpp) is part of KKEdit.

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

#include "kkedit-includes.h"

GApplication	*mainApp;
bool			autoSeleced=false;
bool			fromGOpen=false;
char			*prefsFolder=NULL;

//app
//main mainWindow
GtkWidget		*mainWindow=NULL;
GtkWidget		*menuBar=NULL;
GtkToolbar		*toolBar;
GtkWidget		*statusBarMenu=NULL;
GtkNotebook		*mainNotebook=NULL;

//file menu
GtkWidget		*fileMenu;
GtkWidget		*menuItemOpen=NULL;
GtkWidget		*saveMenu;
GtkWidget		*saveAsMenu;
GtkWidget		*saveAllMenu;
GtkWidget		*closeMenu;
GtkWidget		*closeAllMenu;
GtkWidget		*revertMenu;
GtkWidget		*printMenu;
GtkWidget		*sortTabsMenu=NULL;
//edit menu
GtkWidget		*editMenu;
GtkWidget		*redoMenu;
GtkWidget		*undoMenu;
GtkWidget		*redoAllMenu;
GtkWidget		*undoAllMenu;
GtkWidget		*cutMenu;
GtkWidget		*copyMenu;
GtkWidget		*pasteMenu;
//view menu
GtkWidget		*viewMenu=NULL;
GtkWidget		*viewTabMenu=NULL;
GtkWidget		*viewTabSubMenu=NULL;

//nav menu
GtkWidget		*navMenu;
GtkWidget		*goBackMenu;
GtkWidget		*goForwardMenu;
GtkWidget		*gotoDefMenu;
GtkWidget		*searchInGtkDocsMenu;
GtkWidget		*searchInQTDocsMenu;
GtkWidget		*searchInDocsMenu;

//func menu
GtkWidget		*funcMenu;
//bookmark menu
GtkWidget		*bookMarkMenu;
GtkWidget		*bookMarkSubMenu;
//tools menu
GtkWidget		*toolsMenu;
GtkWidget		*toolOutMenu=NULL;
//plug menu
GtkWidget		*globalPlugMenu=NULL;
//help menu
GtkWidget		*helpMenu;

//status bar message
char			*statusMessage=NULL;

//barber pole
char			*barControl;

GtkAccelGroup	*accgroup=NULL;

VISIBLE GList	*newBookMarksList=NULL;
char			*highlightColour;
char			*tmpHighlightColour;
bool			showBMBar;
GtkWidget		*bmHighlightBox;
int				bmMarkNumber=0;
unsigned int	pageID=100;

char			*toolBarLayout=NULL;
GtkWidget		*toolBarBox;

GtkWidget		*lineNumberWidget;
GtkWidget		*findApiWidget;
GtkWidget		*findQtApiWidget;
GtkWidget		*findDefWidget;
GtkWidget		*liveSearchWidget;

int				currentPage=0;
int				sinkReturn;
char			*sinkReturnStr;

//nag
bool			nagScreen;
unsigned int	nagTime;
unsigned int	lastNagTime=0;
bool			timeToNag=true;
unsigned int	lastUpdate=0;
unsigned int	lastPlugUpdate=0;
unsigned int	updateWait=2678400;

//docview size and position
int				docWindowWidth;
int				docWindowHeight;
int				docWindowX=-1;
int				docWindowY=-1;

//prefs
GtkWidget		*prefswin;
bool			indent;
bool			lineNumbers;
bool			lineWrap;
bool			highLight;
bool			noSyntax;
bool			showWhiteSpace;
bool			singleUse;
unsigned int	tabWidth;
unsigned int	tabsSize;
char			*fontAndSize;
char			*terminalCommand;
char			*rootCommand;
unsigned int	depth;
bool			onExitSaveSession;
bool			restoreBookmarks;
char			*styleName=NULL;
bool			noDuplicates;
bool			noWarnings;
bool			readLinkFirst;
bool			autoShowComps;
bool			autoSearchDocs;
bool			autoCheck;
unsigned int	autoShowMinChars;
bool			useGlobalPlugMenu;
unsigned int	maxTabChars=20;
unsigned int	maxBMChars;
unsigned int	maxJumpHistory;
unsigned int	maxFuncDefs;

GtkWidget		*fontButton;
GtkWidget		*terminalBox;
GtkWidget		*rootCommandBox;
GtkWidget		*defaultBrowserBox;
char			*browserCommand=strdup("firefox");

#ifdef _USEGTK3_
GtkStyleProvider	*provider;
GtkStyleProvider	*tabBoxProvider;
#endif

//key short cuts
GtkWidget		*keysWindow=NULL;

char			*tmpStyleName=NULL;

GtkWidget		*toolNameWidget;
GtkWidget		*commandLineWidget;
GtkWidget		*keyWidget;
GtkWidget		*commentWidget;
GtkWidget		*inTermWidget;
GtkWidget		*inPopupWidget;
GtkWidget		*alwaysPopupWidget;
GtkWidget		*syncWidget;
GtkWidget		*ignoreWidget;
GtkWidget		*pasteWidget;
GtkWidget		*replaceWidget;
GtkWidget		*outputWidget;
GtkWidget		*showDocWidget;
GtkWidget		*toolSelect;
GtkWidget		*clearViewWidget;
GtkWidget		*runAsRootWidget;
GtkWidget		*useBarWidget;

toolStruct		*selectedToolFromDrop=NULL;

//view tool output
GtkWidget		*mainVPane=NULL;
bool			showToolOutWin=false;
GtkWidget		*toolOutputView=NULL;
GtkTextBuffer	*toolOutputBuffer=NULL;
GtkWidget		*toolOutVBox=NULL;
bool			showStatus;
GtkWidget		*statusWidget=NULL;

GList			*toolsList=NULL;

GtkWidget		*restoreBMs;

//main mainWindow
int				windowWidth;
int				windowHeight;
int				windowX=-1;
int				windowY=-1;
bool			wrapSearch;
bool			insensitiveSearch;
bool			useRegex;
bool			replaceAll;
bool			findInAllFiles;
bool			hightlightAll;
int				toolOutHeight=200;
int				bottomVPaneHite=200;
int				topVPaneHite=200;

GtkWidget		*mainWindowScrollbox=NULL;

GtkWidget		*mainWindowVBox=NULL;
GtkWidget		*mainTopUserVBox=NULL;
GtkWidget		*mainWindowHBox=NULL;
GtkWidget		*mainLeftUserVBox=NULL;
GtkWidget		*mainNotebookVBox=NULL;
GtkWidget		*mainRightUserVBox=NULL;
GtkWidget		*mainBottomUserVBox=NULL;

GtkWidget		*mainWindowVPane=NULL;
GtkWidget		*secondWindowVPane=NULL;

GtkWidget		*mainWindowHPane=NULL;
GtkWidget		*secondWindowHPane=NULL;

int				currentTabNumber;
int 			untitledNumber=1;

GtkToolItem		*newButton=NULL;
GtkToolItem		*openButton=NULL;
GtkToolItem		*saveButton=NULL;
GtkToolItem		*redoButton=NULL;
GtkToolItem		*undoButton=NULL;
GtkToolItem		*cutButton=NULL;
GtkToolItem		*copyButton=NULL;
GtkToolItem		*pasteButton=NULL;
GtkToolItem		*findButton=NULL;
GtkToolItem		*gotoDefButton=NULL;
GtkToolItem		*backButton=NULL;
GtkToolItem		*forwardButton=NULL;
GtkToolItem		*gotoLineButton=NULL;
GtkToolItem		*findApiButton=NULL;
GtkToolItem		*findQtApiButton=NULL;
GtkToolItem		*findFuncDefButton=NULL;
GtkToolItem		*liveSearchButton=NULL;

//back/forward history

//find replace
GtkWidget		*findReplaceDialog;
GtkWidget		*findBox;
GtkWidget		*replaceBox;
GtkWidget		*findDropBox=NULL;
GtkWidget		*replaceDropBox;
GSList			*findList=NULL;
GSList			*replaceList=NULL;
unsigned int	maxFRHistory=5;

//custom toolbar
GtkWidget		*fromHBox;
GtkWidget		*iconViewBox;
bool			showToolBar=true;

int				listFunction=0;
GtkWidget		*funcListDrop=NULL;

char			*functionSearchText=NULL;
char			*thePage=NULL;
char			*htmlFile=NULL;
char			*htmlURI=NULL;
char			*tmpFolderName=NULL;

//runtime deps
int				gotManEditor;
int				gotDoxygen;

GtkSourceStyleSchemeManager	*schemeManager;
GtkSourceStyleScheme			*styleScheme;

#ifdef _BUILDDOCVIEWER_
GtkWidget		*docView;
WebKitWebView	*webView;
bool			showHideDocviewer;
GtkWidget		*showDocViewWidget;
#endif

//spellcheck
GtkWidget		*spellCheckWord;
GtkWidget		*wordListDropbox;
GtkWidget		*wordListEntrybox;
char			*badWord=NULL;
char			*goodWord=NULL;
#ifdef _ASPELL_
AspellConfig	*aspellConfig;
AspellSpeller	*spellChecker=0;
#endif

const char		*localeLang;

//notebook
int				openInThisTab=-1;

//general app
bool			loadingSession;
HistoryClass	*globalHistory;
StringSlice		*globalSlice;
bool			doGoogleSearch;

unsigned int	shortCuts[NUMSHORTCUTS][2]=
{
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
	{64,12},
	{32,0}
};

char			*shortCutStrings[NUMSHORTCUTS]={NULL,};

args			keybindings_rc[]=
	{
//string
		{"deleteline",TYPESTRING,&shortCutStrings[0]},
		{"deletetoeol",TYPESTRING,&shortCutStrings[1]},
		{"deletetosol",TYPESTRING,&shortCutStrings[2]},
		{"selectword",TYPESTRING,&shortCutStrings[3]},
		{"deleteword",TYPESTRING,&shortCutStrings[4]},
		{"duplicateline",TYPESTRING,&shortCutStrings[5]},
		{"selectline",TYPESTRING,&shortCutStrings[6]},
		{"lineup",TYPESTRING,&shortCutStrings[7]},
		{"linedown",TYPESTRING,&shortCutStrings[8]},
		{"selecttoeol",TYPESTRING,&shortCutStrings[9]},
		{"selecttosol",TYPESTRING,&shortCutStrings[10]},
		{"selectionup",TYPESTRING,&shortCutStrings[11]},
		{"selectiondown",TYPESTRING,&shortCutStrings[12]},
		{"complete",TYPESTRING,&shortCutStrings[13]},
		{NULL,0,NULL}
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

GtkWidget		*progressWindow;
GtkWidget		*progressBar;

PluginClass	*globalPlugins=NULL;

//save and load var lists
char			*windowAllocData=NULL;
char			*docWindowAllocData=NULL;

args			kkedit_startup_vars[]=
{
	{"singleuse",TYPEBOOL,&singleUse},
	{NULL,0,NULL}
};

args			kkedit_window_rc[]=
{
	//bools
	{"insenssearch",TYPEBOOL,&insensitiveSearch},
	{"useregex",TYPEBOOL,&useRegex},
	{"wrapsearch",TYPEBOOL,&wrapSearch},
	{"replaceall",TYPEBOOL,&replaceAll},
	{"allfiles",TYPEBOOL,&findInAllFiles},
	{"showbmbar",TYPEBOOL,&showBMBar},
	{"showtoolbar",TYPEBOOL,&showToolBar},
	{"showstatusbar",TYPEBOOL,&showStatus},
	{"highlightall",TYPEBOOL,&hightlightAll},
	//strings
	{"windowsize",TYPESTRING,&windowAllocData},
	{"docwindowsize",TYPESTRING,&docWindowAllocData},
	//ints
	{"toolouthite",TYPEINT,&toolOutHeight},
	{"bottomvpanehite",TYPEINT,&bottomVPaneHite},
	{"topvpanehite",TYPEINT,&topVPaneHite},
	{"nagtime",TYPEINT,&lastNagTime},
	{"lastupdate",TYPEINT,&lastUpdate},
	{"lastplugupdate",TYPEINT,&lastPlugUpdate},
	//lists
	{"findlist",TYPELIST,&findList},
	{"replacelist",TYPELIST,&replaceList},
	{NULL,0,NULL}
};

args			kkedit_rc[]=
{
	//bools
	{"indentcode",TYPEBOOL,&indent},
	{"showlinenumbers",TYPEBOOL,&lineNumbers},
	{"wrapline",TYPEBOOL,&lineWrap},
	{"highlightcurrentline",TYPEBOOL,&highLight},
	{"nosyntax",TYPEBOOL,&noSyntax},
	{"showspaces",TYPEBOOL,&showWhiteSpace},
	{"singleuse",TYPEBOOL,&singleUse},
	{"noduplicates",TYPEBOOL,&noDuplicates},
	{"warning",TYPEBOOL,&noWarnings},
	{"savesessiononexit",TYPEBOOL,&onExitSaveSession},
	{"restorebookmarks",TYPEBOOL,&restoreBookmarks},
	{"nagscreen",TYPEBOOL,&nagScreen},
	{"readlink",TYPEBOOL,&readLinkFirst},
	{"autocomp",TYPEBOOL,&autoShowComps},
	{"autosearchdocs",TYPEBOOL,&autoSearchDocs},
	{"updatecheck",TYPEBOOL,&autoCheck},
	{"useplugmenu",TYPEBOOL,&useGlobalPlugMenu},
	//strings
	{"stylename",TYPESTRING,&styleName},
	{"higlightcolour",TYPESTRING,&highlightColour},
	{"toolbarlayout",TYPESTRING,&toolBarLayout},
	{"font",TYPESTRING,&fontAndSize},
	{"terminalcommand",TYPESTRING,&terminalCommand},
	{"rootcommand",TYPESTRING,&rootCommand},
	{"defaultbrowser",TYPESTRING,&browserCommand},
	//ints
	{"tabwidth",TYPEINT,&tabWidth},
	{"depth",TYPEINT,&depth},
	{"funcsort",TYPEINT,&listFunction},
	{"minautochars",TYPEINT,&autoShowMinChars},
	{"maxfrhistory",TYPEINT,&maxFRHistory},
	{"maxtabchars",TYPEINT,&maxTabChars},
	{"maxmenuchars",TYPEINT,&maxFuncDefs},
	{"maxbmchars",TYPEINT,&maxBMChars},
	{"tabssize",TYPEINT,&tabsSize},
	{"maxjumphistory",TYPEINT,&maxJumpHistory},
	{NULL,0,NULL}
};

int				intermarg=0;
int				flagsarg=0;
int				inpopup=0;
int				alwayspopup=0;
int				clearview=0;
char			*commandarg=NULL;
char			*commentarg=NULL;
char			*menuname=NULL;
int				rootarg=0;
int				keycode=0;
int				usebar=0;

args			tool_vars[]=
{
	//strings
	{"name",TYPESTRING,&menuname},
	{"command",TYPESTRING,&commandarg},
	{"comment",TYPESTRING,&commentarg},
	//ints
	{"interm",TYPEINT,&intermarg},
	{"flags",TYPEINT,&flagsarg},
	{"inpopup",TYPEINT,&inpopup},
	{"alwayspopup",TYPEINT,&alwayspopup},
	{"clearview",TYPEINT,&clearview},
	{"runasroot",TYPEINT,&rootarg},
	{"usebar",TYPEINT,&usebar},
	{"shortcutkey",TYPEINT,&keycode},
	{NULL,0,NULL}
};

//truncate tabname with elipses
char *truncateWithElipses(char *str,unsigned int maxlen)
{
	ERRDATA
	char	*retstr=NULL;
	char	*front,*back;
	int		sides;

	if(g_utf8_validate(str,-1,NULL)==true)
		{
			if(g_utf8_strlen(str,-1)>maxlen)
				{
					sides=(maxlen-5)/2;
					front=g_utf8_substring(str,0,sides);
					back=g_utf8_substring(str,g_utf8_strlen(str,-1)-sides,g_utf8_strlen(str,-1));
					sinkReturn=asprintf(&retstr,"%s ... %s",front,back);
					ERRDATA debugFree(&front);
					ERRDATA debugFree(&back);
				}
			else
				retstr=strdup(str);
		}
	else
		retstr=strdup(str);

	ERRDATA return(retstr);
}

void plugRunFunction(gpointer data,gpointer funcname)
{
	ERRDATA
	globalPlugins->runPlugFunction((moduleData*)data,(const char*)funcname);
	ERRDATA
}

void scrollToIterInPane(pageStruct *page,GtkTextIter *iter)
{
	ERRDATA
	gtk_text_view_scroll_to_iter((GtkTextView*)page->view,iter,0,true,0,0.5);
	ERRDATA
}

VISIBLE pageStruct *getPageStructPtr(int pagenum)
{
	ERRDATA
	int			thispage;
	GtkWidget	*pageBox=NULL;

	if(pagenum==-1)
		thispage=gtk_notebook_get_current_page(mainNotebook);
	else
		thispage=pagenum;

	if(thispage==-1)
		return(NULL);

	pageBox=gtk_notebook_get_nth_page(mainNotebook,thispage);
	if(pageBox==NULL)
		{
			ERRDATA return(NULL);
		}
	else
		{
			ERRDATA return((pageStruct*)g_object_get_data((GObject*)pageBox,"pagedata"));
		}
	ERRDATA
}

VISIBLE pageStruct *getPageStructByID(unsigned pageid)
{
	pageStruct	*page=NULL;

	for(int j=0;j<gtk_notebook_get_n_pages(mainNotebook);j++)
		{
			page=getPageStructPtr(j);
			if(page!=NULL)
				if(page->pageID==pageid)
					return(page);
		}
	return(NULL);
}

void getMimeType(char *filepath,void *ptr)
{
	ERRDATA
	char	*command;
	gchar	*stdout=NULL;
	gchar	*stderr=NULL;
	gint	retval=0;

	sinkReturn=asprintf(&command,"file -b --mime-type %s",filepath);
	g_spawn_command_line_sync(command,&stdout,&stderr,&retval,NULL);
	if(retval==0)
		{
			stdout[strlen(stdout)-1]=0;
			*((char**)ptr)=strdup(stdout);
			ERRDATA debugFree(&stdout);
			ERRDATA debugFree(&stderr);
		}
	ERRDATA
}

void setLanguage(pageStruct *page)
{
	ERRDATA
	GtkSourceLanguage			*lang=NULL;
	GtkSourceLanguageManager	*lm=NULL;
	char						*mimetype;
	char						*command;
	char						line[1024];
	FILE						*fp;
	GFile						*gfile;
	GFileInfo					*gfileinfo;
	const char					*content_type;

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
			char	*hold=NULL;

			mimetype=NULL;

			sinkReturn=asprintf(&command,"xdg-mime query filetype \"%s\"",page->filePath);
			fp=popen(command,"r");
			sinkReturnStr=fgets(line,1024,fp);
			hold=strdup(line);
			if(strchr(line,';')!=NULL)
				sinkReturn=asprintf(&mimetype,"%s",globalSlice->sliceBetween(line,NULL,(char*)";"));
			else
				mimetype=strndup(line,strlen(line)-1);

			ERRDATA debugFree(&hold);
			pclose(fp);
		}

	lang=gtk_source_language_manager_guess_language(lm,page->filePath,mimetype);

	if(lang==NULL)
		{
			getMimeType((char*)page->filePath,&mimetype);
			lang=gtk_source_language_manager_guess_language(lm,page->filePath,mimetype);
			if(lang!=NULL)
				gtk_source_buffer_set_language(page->buffer,lang);
		}
	else
		{
			gtk_source_buffer_set_language(page->buffer,lang);
		}

	if(lang!=NULL)
		page->lang=gtk_source_language_get_name(lang);
	else
		page->lang=NULL;

	g_object_unref(gfile);
	g_object_unref(gfileinfo);

	if(mimetype!=NULL)
		{
			ERRDATA debugFree(&mimetype);
		}
	ERRDATA
}

VISIBLE void runCommand(char *commandtorun,void *ptr,bool interm,int flags,int useroot,char *title)
{
	ERRDATA
	char		*command;
	FILE		*fp=NULL;
	GString	*str=NULL;
	char		line[1024];
	GtkTextIter	iter;
	char		*asroot=strdup("");

	if(useroot==true)
		{
			if(strcmp(rootCommand,"")!=0)
				sinkReturn=asprintf(&asroot,"%s ",rootCommand);
			else
				{
					sinkReturn=asprintf(&asroot,"sudo ");
					interm=true;
				}
		}

	if(interm==true)
		{
			sinkReturn=asprintf(&command,"%s %s sh -c \"%s\"",terminalCommand,asroot,commandtorun);
			flags=8;
		}
	else
		{
			sinkReturn=asprintf(&command,"%ssh -c \"%s\"",asroot,commandtorun);
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
							gtk_menu_item_set_label((GtkMenuItem*)toolOutMenu,MENU_HIDE_TOOL_OP_LABEL);
							while(fgets(line,1024,fp))
								{
									gtk_text_buffer_insert_at_cursor(toolOutputBuffer,line,strlen(line));
//									while(gtk_events_pending())
//										gtk_main_iteration();
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
		showDocView(USEFILE,(char*)"",title);

	ERRDATA debugFree(&command);
	ERRDATA debugFree(&asroot);
}

functionData *getFunctionByNameOpenFiles(char *name,bool casesensitive,bool wholefname)
{
	ERRDATA
	pageStruct		*page;
	int				numpages=gtk_notebook_get_n_pages(mainNotebook);
	char			*lineptr;
	char			*functions=NULL;
	gchar			*stdout=NULL;
	char			function[1024];
	int				gotmatch=-1;
	int				loop;
	int				maxpage=numpages;
	bool			whileflag;
	int				startpage;
	bool			checkthispage;
	functionData	*fdata;
	int				longest;

	page=getPageStructPtr(-1);
	if(page==NULL)
		ERRDATA return(NULL);

	loop=gtk_notebook_get_current_page(mainNotebook);
	startpage=loop;
	checkthispage=true;
	whileflag=true;
	while(whileflag==true)
		{
			page=getPageStructPtr(loop);
			if(page->filePath!=NULL)
				{
					getRecursiveTagList(page->filePath,&functions);
					lineptr=functions;

					while(lineptr!=NULL)
						{
							sscanf(lineptr,"%s",function);
							if(strlen(function)>=strlen(name))
								longest=strlen(function);
							else
								longest=strlen(name);

							if(casesensitive==true)
								{
									if(wholefname==true)
										gotmatch=strncmp(function,name,longest);
									else
										gotmatch=strncmp(function,name,(int)strlen(name));
								}
							else
								{
									if(wholefname==true)
										gotmatch=strncasecmp(function,name,longest);
									else
										{
											char *pstr=NULL;
											pstr=strcasestr(function,name);
											if(pstr!=NULL)
												gotmatch=0;
											else
												gotmatch=-1;
										}
							}

							if(gotmatch==0)
								{
									fdata=(functionData*)malloc(sizeof(functionData));
									sscanf(lineptr,"%"VALIDFUNCTIONCHARS"s",function);
									fdata->name=strdup(function);
									sscanf(lineptr,"%*s %"VALIDFUNCTIONCHARS"s",function);
									fdata->type=strdup(function);
									sscanf(lineptr,"%*s %*s %i",&fdata->line);
									sscanf(lineptr,"%*s %*s %*i %"VALIDFILENAMECHARS"s",function);
									fdata->file=strdup(function);
									sscanf(lineptr,"%*s %*s %*i %*s %[^\n]s",function);
									fdata->define=strdup(function);
									fdata->intab=loop;
									ERRDATA return(fdata);
								}

							lineptr=strchr(lineptr,'\n');
							if(lineptr!=NULL)
								lineptr++;
						}
				}

			if(checkthispage==true)
				{
					loop=-1;
					checkthispage=false;
				}

			loop++;
			if(loop==startpage)
				loop++;
			if(loop==maxpage)
				whileflag=false;
		}
	return(NULL);
}

functionData *getFunctionByName(char *name,bool recurse,bool casesensitive)
{
	ERRDATA
	pageStruct		*page;
	int				numpages=gtk_notebook_get_n_pages(mainNotebook);
	char			*lineptr;
	char			*dirname;
	gchar			*stdout=NULL;
	char			function[1024];
	char			funcname[256];
	char			filepath[1024];
	int				linenumber;
	int				gotmatch;
	char			*possmatch=NULL;
	int				bestlen=-1;
	int				thislen;
	int				loophold=-1;
	bool			getfromfile;
	functionData	*fdata;

	if(recurse==true)
		{
//not in any open files
//check ./ from all files
//dont do this from popup for speed reasons
			for(int loop=0; loop<numpages; loop++)
				{
					page=getPageStructPtr(loop);
					if(page->filePath!=NULL)
						{
							dirname=strdup(g_path_get_dirname(page->filePath));
							getRecursiveTagListFileName(dirname,&stdout);

							lineptr=stdout;
							while(lineptr!=NULL)
								{
									sscanf(lineptr,"%s",function);

									if(casesensitive==true)
										gotmatch=strncmp(function,name,(int)strlen(name));
									else
										gotmatch=strncasecmp(function,name,(int)strlen(name));

									if(gotmatch!=0)
										{
											char *p=NULL;
											p=strcasestr(function,name);
											if(p!=NULL)
												gotmatch=0;
										}
										
									if(strlen(name)==strlen(function))
										thislen=strlen(name);
									else
										{
											if(strlen(name)>strlen(function))
												thislen=strlen(function);
											else
												thislen=strlen(name);
										}

									if((gotmatch==0) &&(strlen(name)==strlen(function)))
										{
											sscanf(lineptr, "%s\t%s\t%i",funcname,filepath,&linenumber);
											fdata=(functionData*)malloc(sizeof(functionData));
											fdata->name=strdup(funcname);
											fdata->file=strdup(filepath);
											fdata->line=linenumber+1;
											fdata->type=NULL;
											fdata->define=NULL;
											fdata->intab=-1;
											ERRDATA return(fdata);
										}

									if((gotmatch==0) &&(bestlen<thislen))
										{
											possmatch=strdup(lineptr);
											bestlen=thislen;
											loophold=loop;
											getfromfile=true;
										}

									lineptr=strchr(lineptr,'\n');
									if(lineptr!=NULL)
										lineptr++;
								}
							if(stdout!=NULL)
								{
									ERRDATA debugFree(&stdout);
								}
							if(dirname!=NULL)
								{
									ERRDATA debugFree(&dirname);
								}
						}
				}
		}

	if(possmatch!=NULL)
		{
			if(getfromfile==false)
				{
					fdata=(functionData*)malloc(sizeof(functionData));
					sscanf(possmatch,"%"VALIDFUNCTIONCHARS"s",function);
					fdata->name=strdup(function);
					sscanf(possmatch,"%*s %"VALIDFUNCTIONCHARS"s",function);
					fdata->type=strdup(function);
					sscanf(possmatch,"%*s %*s %i",&fdata->line);
					sscanf(possmatch,"%*s %*s %*i %"VALIDFILENAMECHARS"s",function);
					fdata->file=strdup(function);
					sscanf(possmatch,"%*s %*s %*i %*s %[^\n]s",function);
					fdata->define=strdup(function);
					fdata->intab=loophold;
					ERRDATA return(fdata);
				}
			else
				{
					sscanf(possmatch, "%s\t%s\t%i",funcname,filepath,&linenumber);
					fdata=(functionData*)malloc(sizeof(functionData));
					fdata->name=strdup(funcname);
					fdata->file=strdup(filepath);
					fdata->line=linenumber+1;
					fdata->type=NULL;
					fdata->define=NULL;
					fdata->intab=-1;
					ERRDATA return(fdata);
				}
		}
	ERRDATA return(NULL);
}

void destroyData(functionData *fdata)
{
	ERRDATA
	if(fdata!=NULL)
		{
			if(fdata->name!=NULL)
				{
					ERRDATA debugFree(&fdata->name);
				}
			if(fdata->type!=NULL)
				{
					ERRDATA debugFree(&fdata->type);
				}
			if(fdata->file!=NULL)
				{
					ERRDATA debugFree(&fdata->file);
				}
			if(fdata->define!=NULL)
				{
					ERRDATA debugFree(&fdata->define);
				}
			ERRDATA debugFree((char**)&fdata);
		}
	ERRDATA
}

void getRecursiveTagListFileName(char *filepath,void *ptr)
{
	ERRDATA
	FILE		*fp;
	char		line[1024];
	GString	*str=g_string_new(NULL);
	char		*command;

	if(filepath==NULL)
		{
			ERRDATA return;
		}

	sinkReturn=asprintf(&command,"find \"%s\" -maxdepth %i|ctags -L - --excmd=number --format=1 -f -",filepath,depth);
	fp=popen(command, "r");
	while(fgets(line,1024,fp))
		{
			g_string_append_printf(str,"%s",line);
		}
	pclose(fp);

	*((char**)ptr)=str->str;
	g_string_free(str,false);

	ERRDATA debugFree(&command);
	ERRDATA
}

void getRecursiveTagList(char *filepath,void *ptr)
{
	ERRDATA
	FILE		*fp;
	char		line[2048];
	GString	*str=g_string_new(NULL);
	char		*command;
	char		*newstr=NULL;
	char		*sort=NULL;

	if(filepath==NULL)
		return;

	switch(listFunction)
		{
		case 0:
			sinkReturn=asprintf(&sort,"sort -k 2rb,2rb -k 1b,1b");
			break;
		case 1:
			sinkReturn=asprintf(&sort,"sort -k 2rb,2rb -k 3n,3n");
			break;
		case 2:
			sinkReturn=asprintf(&sort,"sort -k 3n");
			break;
		case 4:
			sinkReturn=asprintf(&sort,"sort -k 2rb,2rb -k 1b,1b");
			break;
		default:
			sinkReturn=asprintf(&sort,"sort");
			break;
		}

	sinkReturn=asprintf(&command,"find \"%s\" -maxdepth %i|ctags -L - -x|%s|sed 's@ \\+@ @g'",filepath,depth,sort);

	fp=popen(command, "r");
	while(fgets(line,2048,fp))
		{
			newstr=globalSlice->deleteSlice(line,filepath);
			if(globalSlice->getResult()==NOERROR)
				{
					g_string_append_printf(str,"%s",newstr);
					ERRDATA debugFree(&newstr);
				}
		}
	pclose(fp);

	*((char**)ptr)=str->str;
	g_string_free(str,false);
	ERRDATA debugFree(&command);
	ERRDATA debugFree(&sort);
}

void destroyTool(gpointer data)
{
	ERRDATA
	if(((toolStruct*)data)->menuName!=NULL)
		{
			ERRDATA debugFree(&((toolStruct*)data)->menuName);
		}
	if(((toolStruct*)data)->filePath!=NULL)
		{
			ERRDATA debugFree(&((toolStruct*)data)->filePath);
		}
	if(((toolStruct*)data)->command!=NULL)
		{
			ERRDATA debugFree(&((toolStruct*)data)->command);
		}
	ERRDATA debugFree((char**)&data);
}

gint sortTools(gconstpointer a,gconstpointer b)
{
	ERRDATA return(strcasecmp(((toolStruct*)a)->menuName,((toolStruct*)b)->menuName));
}

void buildToolsList(void)
{
	ERRDATA
	GDir			*folder;
	const gchar	*entry=NULL;
	char			*filepath;
	toolStruct		*tool;
	char			*datafolder[2];

	if(toolsList!=NULL)
		{
			g_list_free_full(toolsList,destroyTool);
			toolsList=NULL;
		}

	sinkReturn=asprintf(&datafolder[0],"%s/tools/",DATADIR);
	sinkReturn=asprintf(&datafolder[1],"%s/%s/tools/",getenv("HOME"),APPFOLDENAME);
	for(int loop=0; loop<2; loop++)
		{
			folder=g_dir_open(datafolder[loop],0,NULL);
			if(folder!=NULL)
				{
					entry=g_dir_read_name(folder);
					while(entry!=NULL)
						{
							intermarg=0;
							flagsarg=0;
							inpopup=0;
							alwayspopup=0;
							rootarg=0;
							clearview=0;
							keycode=GDK_KEY_VoidSymbol;
							menuname=NULL;
							commandarg=NULL;
							commentarg=NULL;
							usebar=0;

							sinkReturn=asprintf(&filepath,"%s%s",datafolder[loop],entry);
							loadVarsFromFile(filepath,tool_vars);

							if((menuname!=NULL) &&(strlen(menuname)>0) &&(commandarg!=NULL))
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
									tool->useBar=(bool)usebar;

									if(commentarg!=NULL)
										tool->comment=strdup(commentarg);
									else
										tool->comment=NULL;
									if(loop==0)
										tool->global=true;
									else
										tool->global=false;

									toolsList=g_list_prepend(toolsList,(gpointer)tool);
									ERRDATA debugFree(&menuname);
									ERRDATA debugFree(&commandarg);
									if(commentarg!=NULL)
										{
											ERRDATA debugFree(&commentarg);
										}
								}

							ERRDATA debugFree(&filepath);

							entry=g_dir_read_name(folder);
						}
				}
		}
	ERRDATA debugFree(&datafolder[0]);
	ERRDATA debugFree(&datafolder[1]);

	toolsList=g_list_sort(toolsList,sortTools);
	ERRDATA
}

void rebuildBookMarkMenu(void)
{
	ERRDATA
	GtkWidget	*menuitem;
	GtkWidget	*submenu;

	submenu=gtk_menu_item_get_submenu((GtkMenuItem*)bookMarkMenu);
	if(submenu!=NULL)
		gtk_menu_item_set_submenu((GtkMenuItem*)bookMarkMenu,NULL);

	bookMarkSubMenu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(bookMarkMenu),bookMarkSubMenu);

	menuitem=gtk_menu_item_new_with_label(MENU_BM_REMOVE_BMS);
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMarkSubMenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(removeAllBookmarks),NULL);

	menuitem=gtk_menu_item_new_with_label(MENU_BM_TOGGLE_BM_LABEL);
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMarkSubMenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(toggleBookmark),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMarkSubMenu),menuitem);
	ERRDATA
}

VISIBLE void navigateHistory(GtkWidget *widget,gpointer data)
{
	unsigned	tcnt=globalHistory->getSaveCnt();

	if((long)data==NAVLAST)
		{
			globalHistory->saveLastPos();
			globalHistory->goBack();
			globalHistory->goBack();
			globalHistory->goToPos();
		}
	else
		{
			if(globalHistory->getHistory(tcnt+1)->pageID==0)
				return;
			
			globalHistory->goForward();
			globalHistory->goToPos();
		}
	globalHistory->redoMenus();
}

void showBarberPole(const char *title)
{
	ERRDATA
	StringSlice	*slice=new StringSlice;
	char		*barcommand;

	sinkReturn=asprintf(&barControl,"%s/BarControl-%s",tmpFolderName,slice->randomName(6));
	sinkReturn=asprintf(&barcommand,POLEPATH " \"%s\" \"%s\" \"pulse\" &",title,barControl);
	sinkReturn=system(barcommand);
	ERRDATA debugFree(&barcommand);
	ERRDATA
}

void killBarberPole(void)
{
	ERRDATA
	char		*barcommand;

	usleep(100000);
	sinkReturn=asprintf(&barcommand,"/bin/echo quit > \"%s\"",barControl);
	sinkReturn=system(barcommand);
	ERRDATA debugFree(&barcommand);
	ERRDATA debugFree(&barControl);
}

VISIBLE void freeAndNull(char** ptr)
{
	if(*ptr!=NULL)
		free(*ptr);

	*ptr=NULL;
}

VISIBLE void debugFree(char** ptr)
{
#if _DEBUGLEVEL_ > DBG0 && _DEBUGLEVEL_ < DBG4
	FILE	*fp=stderr;

	if((_DEBUGLEVEL_ == DBG1) ||(_DEBUGLEVEL_ == DBG3))
		fprintf(stderr,"Free: File:%s ,Func:%s, Line:%i\n",errFile,errFunc,errLine);

	if(_DEBUGLEVEL_ == DBG2)
		{
			if(logFile!=NULL)
				{
					fp=fopen(logFile,"a");
					fprintf(fp,"Free: File:%s ,Func:%s, Line:%i\n",errFile,errFunc,errLine);
					fclose(fp);
				}
			else
				{
					fprintf(stderr,"Can't open log file\n");
					fprintf(stderr,"Free: File:%s ,Func:%s, Line:%i\n",errFile,errFunc,errLine);
				}
		}
#endif
	freeAndNull(ptr);
}

int			errLine;
const char	*errFile;
const char	*errFunc;
char		*logFile=NULL;

void catchSignal(int signal)
{
	FILE	*fp;

	if(_DEBUGLEVEL_ == DBG0)
		exit(EXIT_FAILURE);

	if(_DEBUGLEVEL_ == DBG1)
		fp=stderr;

	if((_DEBUGLEVEL_ == DBG2) ||(_DEBUGLEVEL_ == DBG3) ||(_DEBUGLEVEL_ == DBG4))
		{
			if(logFile!=NULL)
				fp=fopen(logFile,"a");
			else
				fp=stderr;
		}

	fprintf(fp,"Traped signal %s\n",strsignal(signal));
	fprintf(fp,"File: %s, Function: %s, Line: %i\n",errFile,errFunc,errLine);
	fprintf(stderr,"Traped signal %s\n",strsignal(signal));
	fprintf(stderr,"File: %s, Function: %s, Line: %i\n",errFile,errFunc,errLine);
	if(_DEBUGLEVEL_ == DBG2)
		fclose(fp);
    exit(EXIT_FAILURE);
}
 
GtkWidget *createNewBox(int orient,bool homog,int spacing)
{
	GtkWidget	*retwidg=NULL;

#ifdef _USEGTK3_
	if(orient==NEWVBOX)
		retwidg=gtk_box_new(GTK_ORIENTATION_VERTICAL,spacing);
	else
		retwidg=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,spacing);
	gtk_box_set_homogeneous((GtkBox*)retwidg,homog);
#else
	if(orient==NEWVBOX)
		retwidg=gtk_vbox_new(homog,spacing);
	else
		retwidg=gtk_hbox_new(homog,spacing);
#endif

	return(retwidg);
}

GtkWidget *createNewImageMenuItem(const char *stock,const char *label,bool useunderline)
{
	GtkWidget	*item;

#ifdef _USEGTK3_
	if(useunderline==true)
		item=gtk_menu_item_new_with_mnemonic(label);
	else
		item=gtk_menu_item_new_with_label(label);
#else
	GtkWidget	*image;
	if(useunderline==true)
		item=gtk_image_menu_item_new_with_mnemonic(label);
	else
		item=gtk_image_menu_item_new_with_label(label);
	image=gtk_image_new_from_stock(stock,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)item,image);
#endif

	return(item);
}

GtkWidget *createNewStockMenuItem(const char *stock,const char *label)
{
	GtkWidget	*item;

#ifdef _USEGTK3_
	item=gtk_menu_item_new_with_mnemonic(label);
#else
	item=gtk_image_menu_item_new_from_stock(stock,NULL);
#endif

	return(item);
}

GtkToolItem *createNewToolItem(const char *stock,const char *label)
{
	GtkToolItem	*button;
#ifdef _USEGTK3_
	GtkWidget	*image;
	image=gtk_image_new_from_icon_name(stock,GTK_ICON_SIZE_LARGE_TOOLBAR);
	button=gtk_tool_button_new(image,label);
#else
	button=gtk_tool_button_new_from_stock(stock);
	gtk_tool_button_set_label((GtkToolButton*)button,label);
#endif

	return(button);
}

GtkWidget *createNewStockButton(const char *stock,const char *label)
{
	GtkWidget	*button;
#ifdef _USEGTK3_
	button=gtk_button_new_with_mnemonic(label);
#else
	button=gtk_button_new_from_stock(stock);
#endif

	return(button);
}

GdkPixbuf *createNewIcon(const char *stock,GtkWidget *widg)
{
	GdkPixbuf	*icon;
#ifdef _USEGTK3_
	icon=gtk_icon_theme_load_icon(gtk_icon_theme_get_default(),stock,GTK_ICON_SIZE_LARGE_TOOLBAR,GTK_ICON_LOOKUP_NO_SVG,NULL);
#else
	icon=gtk_widget_render_icon(widg,stock,GTK_ICON_SIZE_LARGE_TOOLBAR,NULL);
#endif

	return(icon);
}

void setToobarWidgetsSensitive(pageStruct *page)
{
	ERRDATA
	bool		ismodified=false;
	bool		hasselection=false;

	if(page==NULL)
		{
			if(newButton!=NULL)
				gtk_widget_set_sensitive((GtkWidget*)newButton,true);
			if(openButton!=NULL)
				gtk_widget_set_sensitive((GtkWidget*)openButton,true);
			if(saveButton!=NULL)
				gtk_widget_set_sensitive((GtkWidget*)saveButton,false);
			if(cutButton!=NULL)
				gtk_widget_set_sensitive((GtkWidget*)cutButton,false);
			if(copyButton!=NULL)
				gtk_widget_set_sensitive((GtkWidget*)copyButton,false);
			if(pasteButton!=NULL)
				gtk_widget_set_sensitive((GtkWidget*)pasteButton,false);
			if(undoButton!=NULL)
				gtk_widget_set_sensitive((GtkWidget*)undoButton,false);
			if(redoButton!=NULL)
				gtk_widget_set_sensitive((GtkWidget*)redoButton,false);
			if(findButton!=NULL)
				gtk_widget_set_sensitive((GtkWidget*)findButton,false);
			if(gotoDefButton!=NULL)
				gtk_widget_set_sensitive((GtkWidget*)gotoDefButton,false);
			if(backButton!=NULL)
				gtk_widget_set_sensitive((GtkWidget*)backButton,false);
			if(forwardButton!=NULL)
				gtk_widget_set_sensitive((GtkWidget*)forwardButton,false);
			if(gotoLineButton!=NULL)
				gtk_widget_set_sensitive((GtkWidget*)gotoLineButton,false);
			if(findApiButton!=NULL)
				gtk_widget_set_sensitive((GtkWidget*)findApiButton,false);
			if(findQtApiButton!=NULL)
				gtk_widget_set_sensitive((GtkWidget*)findApiButton,false);
			if(findFuncDefButton!=NULL)
				gtk_widget_set_sensitive((GtkWidget*)findFuncDefButton,false);
			if(liveSearchButton!=NULL)
				gtk_widget_set_sensitive((GtkWidget*)liveSearchButton,false);
			return;
		}

	ismodified=gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer));
	hasselection=gtk_text_buffer_get_has_selection((GtkTextBuffer*)page->buffer);

	for(int j=0; j<(int)strlen(toolBarLayout); j++)
		{
			switch(toolBarLayout[j])
				{
//newnewButton
				case 'N':
					if(newButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)newButton,true);
					break;
//open+recent
				case 'O':
					if(openButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)openButton,true);
					break;
//save
				case 'S':
					if(saveButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)saveButton,ismodified);
					break;
//cut
				case 'X':
					if(cutButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)cutButton,hasselection);
					break;
//copy
				case 'C':
					if(copyButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)copyButton,hasselection);
					break;
//paste
				case 'P':
					if(pasteButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)pasteButton,true);
					break;
//undo
				case 'U':
					if(undoButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)undoButton,gtk_source_buffer_can_undo(page->buffer));
					break;
//redo
				case 'R':
					if(redoButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)redoButton,gtk_source_buffer_can_redo(page->buffer));
					break;
//find
				case 'F':
					if(findButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)findButton,true);
					break;
//navigation
				case 'G':
					if(gotoDefButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)gotoDefButton,hasselection);
					break;
//now handled from history class
//go back
//				case 'B':
//					if(backButton!=NULL)
//						gtk_widget_set_sensitive((GtkWidget*)backButton,globalHistory->canGoBack());
//					break;
////go forward
//				case 'W':
//					if(forwardButton!=NULL)
//						gtk_widget_set_sensitive((GtkWidget*)forwardButton,globalHistory->canGoForward());
//					break;
//
//go to line
				case '9':
					if(gotoLineButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)gotoLineButton,true);
					break;
//find in gtkdoc
				case 'A':
					if(findApiButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)findApiButton,true);
					break;
//find in qtdoc
				case 'Q':
					if(findQtApiButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)findQtApiButton,true);
					break;
//find in function def
				case 'D':
					if(findFuncDefButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)findFuncDefButton,true);
					break;
//livesearch
				case 'L':
					if(liveSearchButton!=NULL)
						gtk_widget_set_sensitive((GtkWidget*)liveSearchButton,true);
					break;
				}
		}
}

void setChangedSensitive(GtkTextBuffer *textbuffer,pageStruct *page)
{
	bool		ismodified;
	const gchar	*text=NULL;
	char		*newlabel=NULL;
	int			offset=0;
	bool		hasselection;

	if(loadingSession==true)
		return;

	if(page==NULL)
		return;

	setToobarWidgetsSensitive(page);

	gtk_widget_set_sensitive((GtkWidget*)undoMenu,page->canUndo);
	gtk_widget_set_sensitive((GtkWidget*)redoMenu,page->canRedo);
	gtk_widget_set_sensitive((GtkWidget*)undoAllMenu,page->canUndo);
	gtk_widget_set_sensitive((GtkWidget*)redoAllMenu,page->canRedo);

	ismodified=page->isDirty;
	gtk_widget_set_sensitive(saveMenu,ismodified);
	text=gtk_label_get_text((GtkLabel*)page->tabName);

	if(text[0]=='*')
		offset=1;

	if(ismodified==true)
		sinkReturn=asprintf(&newlabel,"*%s",&text[offset]);
	else
		sinkReturn=asprintf(&newlabel,"%s",&text[offset]);

	gtk_label_set_text((GtkLabel*)page->tabName,(const gchar*)newlabel);
	ERRDATA debugFree(&newlabel);

	hasselection=gtk_text_buffer_get_has_selection((GtkTextBuffer*)page->buffer);
	gtk_widget_set_sensitive((GtkWidget*)cutMenu,hasselection);
	gtk_widget_set_sensitive((GtkWidget*)copyMenu,hasselection);
	gtk_widget_set_sensitive((GtkWidget*)gotoDefMenu,hasselection);
	gtk_widget_set_sensitive((GtkWidget*)searchInGtkDocsMenu,hasselection);
	gtk_widget_set_sensitive((GtkWidget*)searchInQTDocsMenu,hasselection);
	if(gotDoxygen==0)
		gtk_widget_set_sensitive((GtkWidget*)searchInDocsMenu,hasselection);
	gtk_widget_set_sensitive((GtkWidget*)goBackMenu,globalHistory->canGoBack());
	gtk_widget_set_sensitive((GtkWidget*)goForwardMenu,globalHistory->canGoForward());

//do plugin sensitive
	globalPlugins->globalPlugData->page=page;
	g_list_foreach(globalPlugins->plugins,plugRunFunction,(gpointer)"setSensitive");
}

void resetSensitive(void)
{
	setToobarWidgetsSensitive(NULL);

	gtk_widget_set_sensitive((GtkWidget*)undoMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)redoMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)undoAllMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)redoAllMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)cutMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)copyMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)pasteMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)saveMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)gotoDefMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)saveAsMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)bookMarkMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)funcMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)navMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)printMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)closeMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)closeAllMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)saveAllMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)revertMenu,false);
	gtk_widget_set_sensitive((GtkWidget*)goBackMenu,false);
	gtk_statusbar_remove_all((GtkStatusbar*)statusWidget,0);
}

void setPageSensitive(void)
{
	if(gtk_notebook_get_n_pages(mainNotebook)==0)
		{
			resetSensitive();
			return;
		}
	setToobarWidgetsSensitive(NULL);
	gtk_widget_set_sensitive((GtkWidget*)saveAsMenu,true);
	gtk_widget_set_sensitive((GtkWidget*)saveAllMenu,true);
	gtk_widget_set_sensitive((GtkWidget*)closeAllMenu,true);
	gtk_widget_set_sensitive((GtkWidget*)closeMenu,true);
	gtk_widget_set_sensitive((GtkWidget*)pasteMenu,true);
	gtk_widget_set_sensitive((GtkWidget*)bookMarkMenu,true);
	gtk_widget_set_sensitive((GtkWidget*)navMenu,true);
	gtk_widget_set_sensitive((GtkWidget*)funcMenu,true);
	gtk_widget_set_sensitive((GtkWidget*)printMenu,true);
	gtk_widget_set_sensitive((GtkWidget*)revertMenu,true);
}

void getOldConfigs(const char *file,args *dataptr)
{
	char	*filename;
	int		retval=NOERR;

	sinkReturn=asprintf(&filename,"%s/%s/%s",getenv("HOME"),APPFOLDENAME,file);
	retval=loadVarsFromFile(filename,dataptr);
	ERRDATA debugFree(&filename);

	if(retval!=NOERR)
		{
			if(strcmp(APPFOLDENAME,".KKEdit-3")==0)
				{
					sinkReturn=asprintf(&filename,"%s/.KKEdit3/%s",getenv("HOME"),file);
					retval=loadVarsFromFile(filename,dataptr);
					ERRDATA debugFree(&filename);
					if(retval!=NOERR)
						{
							sinkReturn=asprintf(&filename,"%s/.KKEdit/%s",getenv("HOME"),file);
							retval=loadVarsFromFile(filename,dataptr);
							ERRDATA debugFree(&filename);
						}
				}
		}
}

void infoMessage(const char *message)
{
	GtkWidget *dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,GTK_BUTTONS_CLOSE,"%s",message);
	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
}

int gerrorMessage(GError **err)
{
	if(*err!=NULL)
		{
			infoMessage(((GError*)(*err))->message);
			g_error_free(*err);
			*err=NULL;
			return(1);
		}
	return(0);
}

#ifdef _USEGTK3_
void applyCSS (GtkWidget *widget, GtkStyleProvider *widgprovider)
{
	gtk_style_context_add_provider(gtk_widget_get_style_context (widget),widgprovider,GTK_STYLE_PROVIDER_PRIORITY_USER);
	if (GTK_IS_CONTAINER (widget))
		gtk_container_forall (GTK_CONTAINER(widget),(GtkCallback)applyCSS,widgprovider);
}
#endif


