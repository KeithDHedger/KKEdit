/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * globals.cpp
 *
*/

#include "kkedit-includes.h"


//app
GApplication*	mainApp;
bool			autoSelected=false;

//main mainWindow
GtkWidget*		mainWindow=NULL;
GtkWidget*		menuBar=NULL;
GtkToolbar*		toolBar;
GtkWidget*		statusBarMenu=NULL;
GtkNotebook*	mainNotebook=NULL;

//file menu
GtkWidget*		fileMenu;
GtkWidget*		saveMenu;
GtkWidget*		saveAsMenu;
GtkWidget*		saveAllMenu;
GtkWidget*		closeMenu;
GtkWidget*		closeAllMenu;
GtkWidget*		revertMenu;
GtkWidget*		printMenu;
GtkWidget		*sortTabsMenu=NULL;
//edit menu
GtkWidget*		editMenu;
GtkWidget*		redoMenu;
GtkWidget*		undoMenu;
GtkWidget*		redoAllMenu;
GtkWidget*		undoAllMenu;
GtkWidget*		cutMenu;
GtkWidget*		copyMenu;
GtkWidget*		pasteMenu;
//view menu
GtkWidget*		viewMenu=NULL;
//nav menu
GtkWidget*		navMenu;
GtkWidget*		goBackMenu;
//func menu
GtkWidget*		funcMenu;
//bookmark menu
GtkWidget*		bookMarkMenu;
GtkWidget*		bookMarkSubMenu;
//tools menu
GtkWidget*		toolsMenu;
GtkWidget*		toolOutMenu=NULL;
//plug menu
GtkWidget*		globalPlugMenu=NULL;
//help menu
GtkWidget*		helpMenu;

//status bar message
char*			statusMessage=NULL;

GtkAccelGroup*	accgroup=NULL;


GtkWidget*		menuItemOpen=NULL;


VISIBLE GList*	newBookMarksList=NULL;
char*			highlightColour;
char*			tmpHighlightColour;
bool			showBMBar;
GtkWidget*		bmHighlightBox;
int				bmMarkNumber=0;

char*			toolBarLayout=NULL;
GtkWidget*		toolBarBox;




GtkWidget*		lineNumberWidget;
GtkWidget*		findApiWidget;
GtkWidget*		findQtApiWidget;
GtkWidget*		findDefWidget;
GtkWidget*		liveSearchWidget;

int				currentPage=0;
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
GtkWidget*		prefswin;
bool			indent;
bool			lineNumbers;
bool			lineWrap;
bool			highLight;
bool			noSyntax;
bool			singleUse;
unsigned int	tabWidth;
char*			fontAndSize;
char*			terminalCommand;
char*			rootCommand;
unsigned int	depth;
bool			onExitSaveSession;
bool			restoreBookmarks;
char*			styleName=NULL;
bool			noDuplicates;
bool			noWarnings;
bool			readLinkFirst;
bool			autoShowComps;
bool			autoCheck;
unsigned int	autoShowMinChars;
bool			useGlobalPlugMenu;
unsigned int	maxTabChars=20;
unsigned int	maxBMChars;
unsigned int	maxFuncDefs;

GtkWidget*		fontButton;
GtkWidget*		terminalBox;
GtkWidget*		rootCommandBox;
GtkWidget*		defaultBrowserBox;
char*			browserCommand=strdup("firefox");

//key short cuts
GtkWidget*		keysWindow=NULL;

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
GtkWidget*		useBarWidget;

toolStruct*		selectedToolFromDrop=NULL;

//view tool output
GtkWidget*		mainVPane=NULL;
bool			showToolOutWin=false;
GtkWidget*		toolOutputView=NULL;
GtkTextBuffer*	toolOutputBuffer=NULL;
GtkWidget*		toolOutVBox=NULL;
bool			showStatus;
GtkWidget*		statusWidget=NULL;

GList*			toolsList=NULL;

GtkWidget*		restoreBMs;

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

GtkWidget*		mainWindowScrollbox=NULL;

GtkWidget*		mainWindowVBox=NULL;
GtkWidget*		mainTopUserVBox=NULL;
GtkWidget*		mainWindowHBox=NULL;
GtkWidget*		mainLeftUserVBox=NULL;
GtkWidget*		mainNotebookVBox=NULL;
GtkWidget*		mainRightUserVBox=NULL;
GtkWidget*		mainBottomUserVBox=NULL;

GtkWidget*		mainWindowVPane=NULL;
GtkWidget*		secondWindowVPane=NULL;

GtkWidget*		mainWindowHPane=NULL;
GtkWidget*		secondWindowHPane=NULL;

VISIBLE int		currentTabNumber;
int 			untitledNumber=1;

GtkToolItem*	newButton=NULL;
GtkToolItem*	openButton=NULL;
GtkToolItem*	saveButton=NULL;
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
unsigned int	maxFRHistory=5;

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
char*			tmpFolderName=NULL;

//runtime deps
int				gotManEditor;
int				gotDoxygen;

GtkSourceStyleSchemeManager*	schemeManager;
GtkSourceStyleScheme*			styleScheme;

#ifdef _BUILDDOCVIEWER_
GtkWidget*		docView;
WebKitWebView*	webView;
bool			showHideDocviewer;
GtkWidget*		showDocViewWidget;
#endif

//spellcheck
GtkWidget*		spellCheckWord;
GtkWidget*		wordListDropbox;
GtkWidget*		wordListEntrybox;
char*			badWord=NULL;
char*			goodWord=NULL;
#ifdef _ASPELL_
AspellConfig*	aspellConfig;
AspellSpeller*	spellChecker=0;
#endif

const char*		localeLang;

HistoryClass*	history;
StringSlice*	globalSlice;

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

char*			shortCutStrings[NUMSHORTCUTS]={NULL,};

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

GtkWidget*		progressWindow;
GtkWidget*		progressBar;

PluginClass*	globalPlugins=NULL;

//save and load var lists
char*			windowAllocData=NULL;
char*			docWindowAllocData=NULL;

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
	{"singleuse",TYPEBOOL,&singleUse},
	{"noduplicates",TYPEBOOL,&noDuplicates},
	{"warning",TYPEBOOL,&noWarnings},
	{"savesessiononexit",TYPEBOOL,&onExitSaveSession},
	{"restorebookmarks",TYPEBOOL,&restoreBookmarks},
	{"nagscreen",TYPEBOOL,&nagScreen},
	{"readlink",TYPEBOOL,&readLinkFirst},
	{"autocomp",TYPEBOOL,&autoShowComps},
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
	{NULL,0,NULL}
};

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
char* truncateWithElipses(char* str,unsigned int maxlen)
{
	char*	retstr;
	char	*front,*back;
	int		sides;

	if(g_utf8_validate(str,-1,NULL)==true)
		{
			if(g_utf8_strlen(str,-1)>maxlen)
				{
					sides=(maxlen-5)/2;
					front=g_utf8_substring(str,0,sides);
					back=g_utf8_substring(str,g_utf8_strlen(str,-1)-sides,g_utf8_strlen(str,-1));
					asprintf(&retstr,"%s ... %s",front,back);
					debugFree(&front,"truncateWithElipses front");
					debugFree(&back,"truncateWithElipses back");
				}
			else
				retstr=strdup(str);
		}
	else
		retstr=strdup(str);

	return(retstr);
}

void plugRunFunction(gpointer data,gpointer funcname)
{
	globalPlugins->runPlugFunction((moduleData*)data,(const char*)funcname);
}

void scrollToIterInPane(pageStruct* page,GtkTextIter* iter)
{
	if(page->inTop==true)
		gtk_text_view_scroll_to_iter((GtkTextView*)page->view,iter,0,true,0,0.5);
	else
		gtk_text_view_scroll_to_iter((GtkTextView*)page->view2,iter,0,true,0,0.5);
}
/*
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
*/
VISIBLE pageStruct* getDocumentData(int pagenum)
{
	int			thispage;
	GtkWidget*	pageBox;

	if(pagenum==-1)
		thispage=gtk_notebook_get_current_page(mainNotebook);
	else
		thispage=pagenum;

	pageBox=gtk_notebook_get_nth_page(mainNotebook,thispage);
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
			debugFree(&stdout,"getMimeType stdout");
			debugFree(&stderr,"getMimeType stderr");
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
				asprintf(&mimetype,"%s",globalSlice->sliceBetween(line,NULL,(char*)";"));
			else
				mimetype=strndup(line,strlen(line)-1);

			debugFree(&hold,"setLanguage hold");
			pclose(fp);
		}

	lang=gtk_source_language_manager_guess_language(lm,page->filePath,mimetype);

	if (lang==NULL)
		{
			getMimeType((char*)page->filePath,&mimetype);
			lang=gtk_source_language_manager_guess_language(lm,page->filePath,mimetype);
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
		debugFree(&mimetype,"setLanguage mimetype");
}

VISIBLE void runCommand(char* commandtorun,void* ptr,bool interm,int flags,int useroot,char* title)
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
							gtk_menu_item_set_label((GtkMenuItem*)toolOutMenu,gettext("Hide Tool Output"));
							while(fgets(line,1024,fp))
								{
									gtk_text_buffer_insert_at_cursor(toolOutputBuffer,line,strlen(line));
									while(gtk_events_pending())
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
		showDocView(USEFILE,(char*)"",title);

	debugFree(&command,"runCommand command");
	debugFree(&asroot,"runCommand asroot");
}

functionData* getFunctionByName(char* name,bool recurse)
{
	pageStruct*		page;
	int				numpages=gtk_notebook_get_n_pages(mainNotebook);
	char*			lineptr;
	char*			functions=NULL;
	char*			dirname;
	gchar*			stdout=NULL;
	char			function[1024];
	char			funcname[256];
	char			filepath[1024];
	int				linenumber;
	int				gotmatch;
	char*			possmatch=NULL;
	int				bestlen=-1;
	int				thislen;
	int				loophold=-1;
	bool			getfromfile;
	int				loop;
	int				maxpage=numpages;
	bool			whileflag=true;
	int				startpage;
	bool			checkthispage;
	functionData*	fdata;

	page=getDocumentData(-1);
	if(page==NULL)
		return(NULL);

	getfromfile=false;

	loop=gtk_notebook_get_current_page(mainNotebook);
	startpage=loop;
	checkthispage=true;

	while(whileflag==true)
		{
			page=getDocumentData(loop);
			if(page->filePath!=NULL)
				{
					getRecursiveTagList(page->filePath,&functions);
					lineptr=functions;

					while (lineptr!=NULL)
						{
							sscanf (lineptr,"%s",function);
							if(strlen(name)==strlen(function))
								{
									gotmatch=strncasecmp(function,name,strlen(function));
									thislen=strlen(name);
								}
							if((gotmatch==0) && (strlen(name)==strlen(function)))
								{
									fdata=(functionData*)malloc(sizeof(functionData));
									sscanf (lineptr,"%"VALIDFUNCTIONCHARS"s",function);
									fdata->name=strdup(function);
									sscanf (lineptr,"%*s %"VALIDFUNCTIONCHARS"s",function);
									fdata->type=strdup(function);
									sscanf (lineptr,"%*s %*s %i",&fdata->line);
									sscanf (lineptr,"%*s %*s %*i %"VALIDFILENAMECHARS"s",function);
									fdata->file=strdup(function);
									sscanf (lineptr,"%*s %*s %*i %*s %[^\n]s",function);
									fdata->define=strdup(function);
									fdata->intab=loop;
									return(fdata);
								}

							lineptr=strchr(lineptr,'\n');
							if (lineptr!=NULL)
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

	if(recurse==true)
		{
//not in any open files
//check ./ from all files
//dont do this from popup for speed reasons
			for(int loop=0; loop<numpages; loop++)
				{
					page=getDocumentData(loop);
					if(page->filePath!=NULL)
						{
							dirname=strdup(g_path_get_dirname(page->filePath));
							getRecursiveTagListFileName(dirname,&stdout);

							lineptr=stdout;
							while (lineptr!=NULL)
								{
									sscanf (lineptr,"%s",function);
									if(strlen(name)==strlen(function))
										{
											gotmatch=strncasecmp(function,name,strlen(function));
											thislen=strlen(name);
										}
									else
										{
											if(strlen(name)>strlen(function))
												{
													gotmatch=strncasecmp(function,name,strlen(function));
													thislen=strlen(function);
												}
											else
												{
													gotmatch=strncasecmp(name,function,strlen(name));
													thislen=strlen(name);
												}
										}

									if((gotmatch==0) && (strlen(name)==strlen(function)))
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
									if((gotmatch==0) && (bestlen<thislen))
										{
											possmatch=strdup(lineptr);
											bestlen=thislen;
											loophold=loop;
											getfromfile=true;
										}

									lineptr=strchr(lineptr,'\n');
									if (lineptr!=NULL)
										lineptr++;
								}
							if(stdout!=NULL)
								debugFree(&stdout,"getFunctionByName stdout");
							if(dirname!=NULL)
								debugFree(&dirname,"getFunctionByName dirname");
						}
				}
		}

	if(possmatch!=NULL)
		{
			if(getfromfile==false)
				{
					fdata=(functionData*)malloc(sizeof(functionData));
					sscanf (possmatch,"%"VALIDFUNCTIONCHARS"s",function);
					fdata->name=strdup(function);
					sscanf (possmatch,"%*s %"VALIDFUNCTIONCHARS"s",function);
					fdata->type=strdup(function);
					sscanf (possmatch,"%*s %*s %i",&fdata->line);
					sscanf (possmatch,"%*s %*s %*i %"VALIDFILENAMECHARS"s",function);
					fdata->file=strdup(function);
					sscanf (possmatch,"%*s %*s %*i %*s %[^\n]s",function);
					fdata->define=strdup(function);
					fdata->intab=loophold;
					return(fdata);
				}
			else
				{
					sscanf (possmatch, "%s\t%s\t%i",funcname,filepath,&linenumber);
					fdata=(functionData*)malloc(sizeof(functionData));
					fdata->name=strdup(funcname);
					fdata->file=strdup(filepath);
					fdata->line=linenumber+1;
					fdata->type=NULL;
					fdata->define=NULL;
					fdata->intab=-1;
					return(fdata);
				}
		}

	return(NULL);
}

void destroyData(functionData* fdata)
{
	if(fdata!=NULL)
		{
			if(fdata->name!=NULL)
				debugFree(&fdata->name,"destroyData name");
			if(fdata->type!=NULL)
				debugFree(&fdata->type,"destroyData type");
			if(fdata->file!=NULL)
				debugFree(&fdata->file,"destroyData file");
			if(fdata->define!=NULL)
				debugFree(&fdata->define,"destroyData define");
			debugFree((char**)&fdata,"destroyData fdata");
		}
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

	debugFree(&command,"getRecursiveTagListFileName command");
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
			newstr=globalSlice->deleteSlice(line,filepath);
			if(globalSlice->getResult()==NOERROR)
				{
					g_string_append_printf(str,"%s",newstr);
					debugFree(&newstr,"getRecursiveTagList newstr");
				}
		}
	pclose(fp);

	*((char**)ptr)=str->str;
	g_string_free(str,false);
	debugFree(&command,"getRecursiveTagList command");
	debugFree(&sort,"getRecursiveTagList sort");
}

void destroyTool(gpointer data)
{
	if(((toolStruct*)data)->menuName!=NULL)
		debugFree(&((toolStruct*)data)->menuName,"destroyTool menuName");
	if(((toolStruct*)data)->filePath!=NULL)
		debugFree(&((toolStruct*)data)->filePath,"destroyTool filePath");
	if(((toolStruct*)data)->command!=NULL)
		debugFree(&((toolStruct*)data)->command,"destroyTool command");
	debugFree((char**)&data,"destroyTool data");
}

gint sortTools(gconstpointer a,gconstpointer b)
{
	return(strcasecmp(((toolStruct*)a)->menuName,((toolStruct*)b)->menuName));
}

void buildToolsList(void)
{
	GDir*			folder;
	const gchar*	entry=NULL;
	char*			filepath;
	toolStruct*		tool;
	char*			datafolder[2];

	if(toolsList!=NULL)
		{
			g_list_free_full(toolsList,destroyTool);
			toolsList=NULL;
		}

	asprintf(&datafolder[0],"%s/tools/",DATADIR);
	asprintf(&datafolder[1],"%s/.KKEdit/tools/",getenv("HOME"));
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
							keycode=0;
							menuname=NULL;
							commandarg=NULL;
							commentarg=NULL;
							usebar=0;

							asprintf(&filepath,"%s%s",datafolder[loop],entry);
							loadVarsFromFile(filepath,tool_vars);

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
									debugFree(&menuname,"buildToolsList menuname");
									debugFree(&commandarg,"buildToolsList commandarg");
									if(commentarg!=NULL)
										debugFree(&commentarg,"buildToolsList commentarg");
								}

							debugFree(&filepath,"buildToolsList filepath");

							entry=g_dir_read_name(folder);
						}
				}
		}
	debugFree(&datafolder[0],"buildToolsList datafolder[0]");
	debugFree(&datafolder[1],"buildToolsList datafolder[1]");

	toolsList=g_list_sort(toolsList,sortTools);
}

void rebuildBookMarkMenu(void)
{
	GtkWidget*	menuitem;
	GtkWidget*	submenu;

	submenu=gtk_menu_item_get_submenu((GtkMenuItem*)bookMarkMenu);
	if(submenu!=NULL)
		gtk_menu_item_set_submenu((GtkMenuItem*)bookMarkMenu,NULL);

	bookMarkSubMenu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(bookMarkMenu),bookMarkSubMenu);

	menuitem=gtk_menu_item_new_with_label(gettext("Remove All Bookmarks"));
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMarkSubMenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(removeAllBookmarks),NULL);

	menuitem=gtk_menu_item_new_with_label(gettext("Toggle Bookmark"));
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMarkSubMenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(toggleBookmark),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMarkSubMenu),menuitem);
}

VISIBLE void goBack(GtkWidget* widget,gpointer data)
{
	HistoryClass*	hist=new HistoryClass;

	GtkTextIter		iter;
	pageStruct*		page=history->getPage();
	TextBuffer*		buf;

	if(page==NULL)
		return;

	buf=history->getTextBuffer();

	if(gtk_notebook_get_current_page(mainNotebook)==history->getTabNumForPage())
		{
			buf->textBuffer=(GtkTextBuffer*)page->buffer;
			buf->getLineData();
			gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&iter,page->backMark);
			gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(page->buffer),&iter);
			scrollToIterInPane(page,&iter);
			gtk_text_buffer_move_mark_by_name((GtkTextBuffer*)page->buffer,"back-mark",&buf->cursorPos);
		}
	else
		{
			if(history->canGoBack()==false)
				{
					delete hist;
					setSensitive();
					return;
				}
		if(hist->savePosition()==true)
				{
					gtk_notebook_set_current_page(mainNotebook,history->getTabNumForPage());
					gtk_text_buffer_get_iter_at_mark((GtkTextBuffer*)page->buffer,&iter,page->backMark);
					gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(page->buffer),&iter);
					scrollToIterInPane(page,&iter);
					delete history;
					history=hist;
				}
			else
				{
					delete hist;
				}
		}
	setSensitive();
}

char	*barControl;

void showBarberPole(const char* title)
{
	StringSlice	*slice=new StringSlice;
	char		*barcommand;

	asprintf(&barControl,"%s/BarControl-%s",tmpFolderName,slice->randomName(6));
	asprintf(&barcommand,POLEPATH " \"%s\" \"%s\" \"pulse\" &",title,barControl);
	system(barcommand);
	debugFree(&barcommand,"restore session barcommand");
}

void killBarberPole(void)
{
	char		*barcommand;

	usleep(100000);
	asprintf(&barcommand,"/bin/echo quit > \"%s\"",barControl);
	system(barcommand);
	debugFree(&barcommand,"restore session barcommand");
	debugFree(&barControl,"restore session barcontrol");
}

VISIBLE void debugFree(char** ptr,const char* message)
{
#ifdef _DEBUG_FREE_
	fprintf(stderr,"free :%s\n",message);
#endif

	if (*ptr!=NULL)
		free(*ptr);

	*ptr=NULL;

}

VISIBLE bool doUpdateWidgets=false;


VISIBLE void setWidgets(void)
{
	pageStruct* page;

	doUpdateWidgets=true;
	page=getDocumentData(-1);

	if(page!=NULL)
		switchPage(mainNotebook,page->tabVbox,currentTabNumber,NULL);

	setSensitive();
	refreshMainWindow();
	resetAllFilePrefs();
}



