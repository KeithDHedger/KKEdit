/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
 * callbacks.cpp
 *
*/

#include <gtk/gtk.h>
#include <gtksourceview/gtksourceiter.h>
#include <unique/unique.h>
#include <gtksourceview/gtksourceprintcompositor.h>

#include "config.h"
#ifdef BUILDDOCVIEWER
#include <webkit/webkit.h>
#endif

#include "globals.h"
#include "files.h"
#include "guis.h"
#include "navcallbacks.h"
#include "searchcallbacks.h"

GtkWidget*			tabMenu;
char				defineText[1024];
GtkPrintSettings*	settings=NULL;

void doOpenFile(GtkWidget* widget,gpointer data)
{
	GtkWidget*	dialog;
	char*		filename;

	dialog=gtk_file_chooser_dialog_new("Open File",NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,NULL);
	if (gtk_dialog_run(GTK_DIALOG (dialog))==GTK_RESPONSE_ACCEPT)
		{
			filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			openFile(filename,0);
			g_free (filename);
		}
	gtk_widget_destroy (dialog);
	gtk_widget_show_all(window);
}

int show_question(char* filename)
{
	GtkWidget*	dialog;
	gint		result;
	char*		message;

	asprintf(&message,"Save file %s before closing?",filename);
	dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_NONE,message);

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_SAVE,GTK_RESPONSE_YES,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_NO,GTK_RESPONSE_NO,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),"Warning unsaved data!");

	result=gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_destroy(dialog);
	g_free(message);
	return(result);
}

void setSensitive(void)
{
	pageStruct*		page=getPageStructPtr(currentTabNumber);
	const gchar*	text;
	char*			newlabel;
	int				offset=0;

	if(page==NULL)
		{
//toolbar
			gtk_widget_set_sensitive((GtkWidget*)undoButton,false);
			gtk_widget_set_sensitive((GtkWidget*)redoButton,false);
			gtk_widget_set_sensitive((GtkWidget*)saveButton,false);
//menu
			gtk_widget_set_sensitive((GtkWidget*)undoMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)redoMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)saveMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)saveAsMenu,false);
			gtk_widget_set_sensitive((GtkWidget*)menubookmark,false);
			gtk_widget_set_sensitive((GtkWidget*)menufunc,false);
			gtk_widget_set_sensitive((GtkWidget*)menunav,false);
			gtk_widget_set_sensitive((GtkWidget*)menuprint,false);
			gtk_widget_set_sensitive((GtkWidget*)menuclose,false);
			gtk_widget_set_sensitive((GtkWidget*)menucloseall,false);
			gtk_widget_set_sensitive((GtkWidget*)menusaveall,false);
			gtk_widget_set_sensitive((GtkWidget*)menurevert,false);
		}
	else
		{
			text=gtk_label_get_text((GtkLabel*)page->tabName);
//toolbar
			gtk_widget_set_sensitive((GtkWidget*)undoButton,gtk_source_buffer_can_undo(page->buffer));
			gtk_widget_set_sensitive((GtkWidget*)redoButton,gtk_source_buffer_can_redo(page->buffer));
			gtk_widget_set_sensitive((GtkWidget*)saveButton,gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer)));

//menu
			gtk_widget_set_sensitive((GtkWidget*)undoMenu,gtk_source_buffer_can_undo(page->buffer));
			gtk_widget_set_sensitive((GtkWidget*)redoMenu,gtk_source_buffer_can_redo(page->buffer));
			gtk_widget_set_sensitive((GtkWidget*)saveMenu,gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer)));
//tab
			gtk_widget_set_sensitive((GtkWidget*)saveButton,gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer)));
			if(text[0]=='*')
				offset=1;

			if(gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer))==true)
				asprintf(&newlabel,"*%s",&text[offset]);
			else
				asprintf(&newlabel,"%s",&text[offset]);

			gtk_label_set_text((GtkLabel*)page->tabName,(const gchar*)newlabel);
			g_free(newlabel);
			gtk_widget_set_sensitive((GtkWidget*)menubookmark,true);
			gtk_widget_set_sensitive((GtkWidget*)menunav,true);
			gtk_widget_set_sensitive((GtkWidget*)saveAsMenu,true);
			gtk_widget_set_sensitive((GtkWidget*)menuprint,true);
			gtk_widget_set_sensitive((GtkWidget*)menuclose,true);
			gtk_widget_set_sensitive((GtkWidget*)menucloseall,true);
			gtk_widget_set_sensitive((GtkWidget*)menusaveall,true);
			gtk_widget_set_sensitive((GtkWidget*)menurevert,true);
			gtk_widget_show_all(page->tabName);
		}
}

bool closingAll=false;

void closeTab(GtkWidget* widget,gpointer data)
{
	long		thispage;
	int			result;
	pageStruct*	page;

	if(closingAll==true)
		thispage=0;
	else
		{
			if(data==NULL)
				thispage=gtk_notebook_get_current_page(notebook);
			else
				thispage=gtk_notebook_page_num(notebook,(GtkWidget *)data);
		}

	closingAll=false;
	page=getPageStructPtr(thispage);
	if(page==NULL)
		return;

	if(gtk_text_buffer_get_modified((GtkTextBuffer*)page->buffer))
		{
			result=show_question(page->fileName);
			switch(result)
				{
					case GTK_RESPONSE_YES:
						if(saveFile(NULL,NULL)==false)
							return;
						break;
					case GTK_RESPONSE_NO:
						break;
					default:
						return;
						break;
				}
		}

	if(page->filePath!=NULL)
		g_free(page->filePath);
	if(page->fileName!=NULL)
		g_free(page->fileName);
	if(page->gFile!=NULL)
		g_object_unref(page->gFile);
	if(page->monitor!=NULL)
		g_object_unref(page->monitor);
	if(page!=NULL)
		g_free(page);
	currentPage--;
	gtk_notebook_remove_page(notebook,thispage);
	setSensitive();
}

void closeAllTabs(GtkWidget* widget,gpointer data)
{
	int	numtabs=gtk_notebook_get_n_pages(notebook);
	GtkWidget*	menuitem;

	for(int loop=0;loop<numtabs;loop++)
		{
			closingAll=true;
			closeTab(NULL,0);
		}

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menubookmark),NULL);
	menubookmarksub=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menubookmark),menubookmarksub);
	menuitem=gtk_menu_item_new_with_label("Add Bookmark");
	gtk_menu_shell_append(GTK_MENU_SHELL(menubookmarksub),menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(addBookmark),NULL);
	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menubookmarksub),menuitem);
	gtk_widget_show_all(menubookmark);
}

void switchPage(GtkNotebook *notebook,gpointer arg1,guint thispage,gpointer user_data)
{
	pageStruct*	page=getPageStructPtr(thispage);
	char*		functions=NULL;
	GtkWidget*	menuitem;
	int			linenum;
	char		tmpstr[1024];
	char*		lineptr;
	bool		onefunc=false;

	GtkWidget* submenu=gtk_menu_item_get_submenu((GtkMenuItem*)menufunc);
	if (submenu!=NULL)
		gtk_menu_item_set_submenu((GtkMenuItem*)menufunc,NULL);

	currentTabNumber=thispage;

	page->rebuildMenu=false;
	getRecursiveTagList(page->filePath,&functions);
	lineptr=functions;

	page->isFirst=true;

	page->navSubMenu=(GtkMenuItem*)gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menufunc),(GtkWidget*)page->navSubMenu);

	while (lineptr!=NULL)
		{
			tmpstr[0]=0;
			sscanf (lineptr,"%*s %*s %i %*s %"VALIDCHARS"s",&linenum,tmpstr);
			lineptr=strchr(lineptr,'\n');
			if (lineptr!=NULL)
				lineptr++;
			if(strlen(tmpstr)>0)
				{
					onefunc=true;
					menuitem=gtk_image_menu_item_new_with_label(tmpstr);
					gtk_menu_shell_append(GTK_MENU_SHELL(page->navSubMenu),menuitem);
					gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(gotoLine),(void*)(long)linenum);
				}
		}
	gtk_window_set_title((GtkWindow*)window,page->fileName);
	gtk_widget_show_all(window);
	if(functions!=NULL)
		g_free(functions);

	gtk_widget_set_sensitive((GtkWidget*)menufunc,onefunc);
	setSensitive();
}

void copyToClip(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	gtk_text_buffer_copy_clipboard((GtkTextBuffer*)page->buffer,gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
}

void cutToClip(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	gtk_text_buffer_cut_clipboard((GtkTextBuffer*)page->buffer,gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),true);
	setSensitive();
}

void pasteFromClip(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);
	gtk_text_buffer_paste_clipboard((GtkTextBuffer*)page->buffer,gtk_clipboard_get(GDK_SELECTION_CLIPBOARD),NULL,true);
	setSensitive();
}

void undo(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);

	if(gtk_source_buffer_can_undo(page->buffer));
		{
			gtk_source_buffer_undo(page->buffer);
			page->isFirst=true;
			setSensitive();
		}
}

void redo(GtkWidget* widget,gpointer data)
{
	pageStruct*	page=getPageStructPtr(-1);

	if(gtk_source_buffer_can_redo(page->buffer));
		{
			gtk_source_buffer_redo(page->buffer);
			page->isFirst=true;
			setSensitive();
		}
}

void dropUri(GtkWidget *widget,GdkDragContext *context,gint x,gint y,GtkSelectionData *selection_data,guint info,guint32 time,gpointer user_data)
{
	gchar**	array=gtk_selection_data_get_uris(selection_data);
	int		cnt=g_strv_length(array);
	char*	filename;

	for(int j=0;j<cnt;j++)
		{
			filename=g_filename_from_uri(array[j],NULL,NULL);
			openFile(filename,0);
		}

	g_strfreev(array);
}

void externalTool(GtkWidget* widget,gpointer data)
{
	toolStruct*	tool=(toolStruct*)data;
	pageStruct*	page=getPageStructPtr(-1);
	char*		dirname;
	char*		text=NULL;
	GtkTextIter	start;
	GtkTextIter	end;
	char*		selection=NULL;
	const char*	vars[]={"%t","%f","%d","%i",NULL};
	char*		ptr;
	long		pos;
	int			loop=0;
	GString*	tempCommand;

	if(page==NULL || tool==NULL)
		return;

	const char*		varData[]={NULL,page->filePath,NULL,DATADIR};

	tempCommand=g_string_new(tool->command);

	if(page->filePath!=NULL)
		dirname=g_path_get_dirname(page->filePath);
	else
		asprintf(&dirname,"%s",getenv("HOME"));

	chdir(dirname);

	setenv("KKEDIT_CURRENTFILE",page->filePath,1);
	setenv("KKEDIT_CURRENTDIR",dirname,1);
	setenv("KKEDIT_DATADIR",DATADIR,1);
	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			setenv("KKEDIT_SELECTION",selection,1);
		}
	else
		asprintf(&selection,"%s","");

	varData[0]=selection;
	varData[2]=dirname;

	while(vars[loop]!=NULL)
		{
			ptr=strstr(tempCommand->str,vars[loop]);
			if(ptr!=NULL)
				{
					pos=(long)ptr-(long)tempCommand->str;
					tempCommand=g_string_erase(tempCommand,pos,2);
					tempCommand=g_string_insert(tempCommand,pos,varData[loop]);
				}
			loop++;
		}

	runCommand(tempCommand->str,&text,tool->inTerminal,tool->flags);
	g_free(selection);

	if(text!=NULL)
		{
			if(tool->flags & TOOL_REPLACE_OP)
				{
					gtk_text_buffer_get_bounds((GtkTextBuffer*)page->buffer,&start,&end);
					gtk_text_buffer_select_range((GtkTextBuffer*)page->buffer,&start,&end);
					gtk_text_buffer_delete_selection((GtkTextBuffer*)page->buffer,true,true);
					gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER (page->buffer),&start);
					gtk_text_buffer_insert(GTK_TEXT_BUFFER(page->buffer),&start,text,strlen(text));
				}

			if(tool->flags & TOOL_PASTE_OP)
				{
					if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
						gtk_text_buffer_delete_selection((GtkTextBuffer*)page->buffer,true,true);
					gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(page->buffer),text,strlen(text));
				}
		}

	unsetenv("KKEDIT_CURRENTFILE");
	unsetenv("KKEDIT_CURRENTDIR");
	unsetenv("KKEDIT_DATADIR");
	unsetenv("KKEDIT_SELECTION");
	g_free(text);
	g_free(dirname);
}

void openHelp(GtkWidget* widget,gpointer data)
{

#ifdef BUILDDOCVIEWER
	asprintf(&thePage,"file://%s/help/help.html",DATADIR);
	showDocView(NULL,(void*)1);
#else
	char*	runhelp;
	asprintf(&runhelp,"xdg-open %s/help/help.html",DATADIR);
	runCommand(runhelp,NULL,false,8);
	g_free(runhelp);
#endif

}

void copyToClipboard(GtkWidget* widget,gpointer data)
{
	GtkClipboard*	clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	gtk_clipboard_set_text(clipboard,(char*)data,-1);
}

void populatePopupMenu(GtkTextView *entry,GtkMenu *menu,gpointer user_data)
{
	pageStruct*	page=getPageStructPtr(-1);
	GtkTextIter	start;
	GtkTextIter	end;
	char*		selection=NULL;
	GtkWidget*	menuitem;
	GtkWidget*	image;

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);

	if(gtk_text_buffer_get_selection_bounds((GtkTextBuffer*)page->buffer,&start,&end))
		{
			selection=gtk_text_buffer_get_text((GtkTextBuffer*)page->buffer,&start,&end,false);
			if(selection!=NULL)
				{
					functionData* fdata=getFunctionByName(selection,false);
					if(fdata!=NULL)
						{
							sprintf((char*)&defineText,"%s",fdata->define);
							menuitem=gtk_menu_item_new_with_label(defineText);
							gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
							gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(copyToClipboard),(void*)defineText);
							destroyData(fdata);

							image=gtk_image_new_from_stock(GTK_STOCK_DIALOG_QUESTION,GTK_ICON_SIZE_MENU);
							menuitem=gtk_image_menu_item_new_with_label("Go To Definition");
							gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
							gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
							gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(goToDefinition),NULL);
						}
					menuitem=gtk_image_menu_item_new_with_label("Search In Gtk-Docs");
					image=gtk_image_new_from_stock(GTK_STOCK_FIND,GTK_ICON_SIZE_MENU);
					gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
					gtk_menu_shell_prepend(GTK_MENU_SHELL(menu),menuitem);
					gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(showDocView),NULL);
				}
		}
	gtk_widget_show_all((GtkWidget*)menu);
}

void doTabMenu(GtkWidget *widget,gpointer user_data)
{
	GtkClipboard*	clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

	gtk_clipboard_set_text(clipboard,(char*)user_data,-1);
	gtk_widget_destroy(tabMenu);
}

bool tabPopUp(GtkWidget *widget, GdkEventButton *event,gpointer user_data)
{
	pageStruct* page;
	GtkWidget*	menuitem;
	GtkWidget*	image;

	if(event->button==3 && event->type==GDK_BUTTON_PRESS)
	    {
			tabMenu=gtk_menu_new();
			page=(pageStruct*)user_data;

//copy filepath
			image=gtk_image_new_from_stock(GTK_STOCK_COPY,GTK_ICON_SIZE_MENU);
			menuitem=gtk_image_menu_item_new_with_label("Copy Filepath");
			gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
			gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doTabMenu),(void*)page->filePath);
//copy filename
			image=gtk_image_new_from_stock(GTK_STOCK_COPY,GTK_ICON_SIZE_MENU);
			menuitem=gtk_image_menu_item_new_with_label("Copy FileName");
			gtk_image_menu_item_set_image((GtkImageMenuItem*)menuitem,image);
			gtk_menu_shell_append(GTK_MENU_SHELL(tabMenu),menuitem);
			gtk_signal_connect(GTK_OBJECT(menuitem),"activate",G_CALLBACK(doTabMenu),(void*)page->fileName);

			gtk_menu_attach_to_widget(GTK_MENU(tabMenu),widget,NULL);
			gtk_menu_popup(GTK_MENU(tabMenu),NULL,NULL,NULL,NULL,event->button,event->time);
			gtk_widget_show_all((GtkWidget*)tabMenu);

			return(true);
		}
	else
		return(false);
}

void messageOpen(UniqueMessageData *message)
{
	int argc;

	gchar** uris=unique_message_data_get_uris(message);
	argc=g_strv_length(uris);

	for (int loop=1;loop<argc;loop++)
		openFile(uris[loop],1);
}

UniqueResponse messageReceived(UniqueApp *app,UniqueCommand command,UniqueMessageData *message,guint time,gpointer user_data)
{
	UniqueResponse	res;

	switch(command)
		{
			case UNIQUE_ACTIVATE:
				gtk_window_set_screen(GTK_WINDOW(window),unique_message_data_get_screen(message));
				gtk_window_present_with_time(GTK_WINDOW(window),time);
				res=UNIQUE_RESPONSE_OK;
				break;

			case UNIQUE_OPEN:
				messageOpen(message);
				gtk_window_set_screen(GTK_WINDOW(window),unique_message_data_get_screen(message));
				gtk_window_present_with_time(GTK_WINDOW(window),time);
   				res=UNIQUE_RESPONSE_OK;
				break;

			default:
				res=UNIQUE_RESPONSE_OK;
				break;
		}
	return(res);
}

void writeExitData(void)
{
	GtkAllocation	alloc;
	FILE*			fd=NULL;
	char*			filename;
	int				winx;
	int				winy;

	gtk_widget_get_allocation(window,&alloc);
	gtk_window_get_position((GtkWindow*)window,&winx,&winy);

	asprintf(&filename,"%s/.KKEdit",getenv("HOME"));
	g_mkdir_with_parents(filename,493);
	g_free(filename);
	asprintf(&filename,"%s/.KKEdit/kkedit.window.rc",getenv("HOME"));
	fd=fopen(filename,"w");
	if(fd!=NULL)
		{
			fprintf(fd,"windowsize	%i %i %i %i\n",alloc.width,alloc.height,winx,winy);
			fclose(fd);
		}
	g_free(filename);
}

void writeConfig(void)
{
	GtkAllocation	alloc;
	FILE*			fd=NULL;
	char*			filename;
	int				winx;
	int				winy;

	gtk_widget_get_allocation(window,&alloc);
	gtk_window_get_position((GtkWindow*)window,&winx,&winy);

	asprintf(&filename,"%s/.KKEdit",getenv("HOME"));
	g_mkdir_with_parents(filename,493);
	g_free(filename);
	asprintf(&filename,"%s/.KKEdit/kkedit.rc",getenv("HOME"));
	fd=fopen(filename,"w");
	if(fd!=NULL)
		{
			fprintf(fd,"indentcode	%i\n",(int)indent);
			fprintf(fd,"showlinenumbers	%i\n",(int)lineNumbers);
			fprintf(fd,"wrapline	%i\n",(int)lineWrap);
			fprintf(fd,"highlightcurrentline	%i\n",(int)highLight);
			fprintf(fd,"singleuse	%i\n",(int)singleUse);
			fprintf(fd,"insenssearch	%i\n",(int)insensitiveSearch);
			fprintf(fd,"wrapsearch	%i\n",(int)wrapSearch);
			fprintf(fd,"savesessiononexit	%i\n",(int)onExitSaveSession);
			fprintf(fd,"restorebookmarks	%i\n",(int)restoreBookmarks);

			fprintf(fd,"tabwidth	%i\n",tabWidth);
			fprintf(fd,"depth	%i\n",depth);
			fprintf(fd,"font	%s\n",fontAndSize);
			fprintf(fd,"terminalcommand	%s\n",terminalCommand);
			fclose(fd);
		}
	g_free(filename);
}

bool doSaveAll(GtkWidget* widget,gpointer data)
{
	int			numpages=gtk_notebook_get_n_pages(notebook);
	int			result;
	pageStruct*	page;

	for(int loop=0;loop<numpages;loop++)
		{
			page=getPageStructPtr(loop);
			if(gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(page->buffer)))
				{
					if((bool)data==true)
						{
							result=show_question(g_path_get_basename(page->fileName));
							switch(result)
								{
									case GTK_RESPONSE_YES:
										gtk_notebook_set_current_page(notebook,loop);
										saveFile(NULL,NULL);
										
										break;
									case GTK_RESPONSE_NO:
										break;
									default:
										return(false);;
										break;
								}
						}
					else
						{
							gtk_notebook_set_current_page(notebook,loop);
							saveFile(NULL,NULL);
						}
				}
		}
	return(true);
}

void doShutdown(GtkWidget* widget,gpointer data)
{
	if(doSaveAll(widget,(void*)true)==true)
		{
			if(onExitSaveSession)
				saveSession(NULL,NULL);
			writeExitData();
			gtk_main_quit();
		}
	system("rm /tmp/kkeditsearchfile.html &>/dev/null");
}

void setPrefs(GtkWidget* widget,gpointer data)
{

	if(strcmp(gtk_widget_get_name(widget),"indent")==0)
		tmpIndent=gtk_toggle_button_get_active((GtkToggleButton*)data);
	if(strcmp(gtk_widget_get_name(widget),"show")==0)
		tmpLineNumbers=gtk_toggle_button_get_active((GtkToggleButton*)data);
	if(strcmp(gtk_widget_get_name(widget),"wrap")==0)
		tmpLineWrap=gtk_toggle_button_get_active((GtkToggleButton*)data);
	if(strcmp(gtk_widget_get_name(widget),"high")==0)
		tmpHighLight=gtk_toggle_button_get_active((GtkToggleButton*)data);
	if(strcmp(gtk_widget_get_name(widget),"single")==0)
		tmpSingleUse=gtk_toggle_button_get_active((GtkToggleButton*)data);
	if(strcmp(gtk_widget_get_name(widget),"save")==0)
		tmpSaveSessionOnExit=gtk_toggle_button_get_active((GtkToggleButton*)data);
	if(strcmp(gtk_widget_get_name(widget),"marks")==0)
		tmpRestoreBookmarks=gtk_toggle_button_get_active((GtkToggleButton*)data);

	gtk_widget_set_sensitive(restoreBMs,tmpSaveSessionOnExit);

	if(strcmp(gtk_widget_get_name(widget),"tabs")==0)
		tmpTabWidth=(int)gtk_spin_button_get_value((GtkSpinButton*)data);

	if(strcmp(gtk_widget_get_name(widget),"depth")==0)
		tmpDepth=(int)gtk_spin_button_get_value((GtkSpinButton*)data);

	if(strcmp(gtk_widget_get_name(widget),"cancel")==0)
		gtk_widget_destroy(prefswin);

	if(strcmp(gtk_widget_get_name(widget),"apply")==0)
		{
			indent=tmpIndent;
			lineNumbers=tmpLineNumbers;
			lineWrap=tmpLineWrap;
			highLight=tmpHighLight;
			singleUse=tmpSingleUse;
			onExitSaveSession=tmpSaveSessionOnExit;
			restoreBookmarks=tmpRestoreBookmarks;

			if(terminalCommand!=NULL)
				{
					g_free(terminalCommand);
					asprintf(&terminalCommand,"%s",gtk_entry_get_text((GtkEntry*)terminalBox));
				}

			if(fontAndSize!=NULL)
				{
					g_free(fontAndSize);
					asprintf(&fontAndSize,"%s",gtk_entry_get_text((GtkEntry*)fontBox));
				}

			tabWidth=tmpTabWidth;
			depth=tmpDepth;
			gtk_widget_destroy(prefswin);
			resetAllFilePrefs();
			writeConfig();
		}
}

void setToolOptions(GtkWidget* widget,gpointer data)
{
	int		flags;
	char*	dirname;
	FILE*	fd=NULL;
	char	toolpath[2048];
	char*	text;

	if(strcmp(gtk_widget_get_name(widget),"interm")==0)
		inTerm=gtk_toggle_button_get_active((GtkToggleButton*)inTermWidget);
	if(strcmp(gtk_widget_get_name(widget),"sync")==0)
		runSync=gtk_toggle_button_get_active((GtkToggleButton*)syncWidget);
	if(strcmp(gtk_widget_get_name(widget),"ignore")==0)
		ignoreOut=gtk_toggle_button_get_active((GtkToggleButton*)ignoreWidget);
	if(strcmp(gtk_widget_get_name(widget),"paste")==0)
		pasteOut=gtk_toggle_button_get_active((GtkToggleButton*)pasteWidget);
	if(strcmp(gtk_widget_get_name(widget),"replace")==0)
		replaceOut=gtk_toggle_button_get_active((GtkToggleButton*)replaceWidget);

	if(runSync==false)
		{
			flags=TOOL_ASYNC;
			gtk_widget_set_sensitive(ignoreWidget,false);
			gtk_widget_set_sensitive(pasteWidget,false);
			gtk_widget_set_sensitive(replaceWidget,false);
		}
	else
		{
			gtk_widget_set_sensitive(ignoreWidget,true);
			gtk_widget_set_sensitive(pasteWidget,true);
			gtk_widget_set_sensitive(replaceWidget,true);

			if(ignoreOut==true)
				flags=TOOL_IGNORE_OP;
			if(pasteOut==true)
				flags=TOOL_PASTE_OP;
			if(replaceOut==true)
				flags=TOOL_REPLACE_OP;				
		}

	if(inTerm==true)
		{
			gtk_widget_set_sensitive(ignoreWidget,false);
			gtk_widget_set_sensitive(pasteWidget,false);
			gtk_widget_set_sensitive(replaceWidget,false);
		}

	if(strcmp(gtk_widget_get_name(widget),"apply")==0)
		{
			asprintf(&dirname,"%s/.KKEdit/tools",getenv("HOME"));
			asprintf(&text,"%s",gtk_entry_get_text((GtkEntry*)toolNameWidget));
			text=g_strdelimit(text," ",'-');
			sprintf((char*)&toolpath,"%s/.KKEdit/tools/%s",getenv("HOME"),text);

			fd=fopen(toolpath,"w");
			if(fd!=NULL)
				{
					fprintf(fd,"name\t%s\n",gtk_entry_get_text((GtkEntry*)toolNameWidget));
					fprintf(fd,"command\t%s\n",gtk_entry_get_text((GtkEntry*)commandLineWidget));
					fprintf(fd,"flags\t%i\n",flags);
					fprintf(fd,"interm\t%i\n",(int)inTerm);
					fclose(fd);
				}
			g_free(dirname);
			gtk_widget_destroy((GtkWidget*)data);

			buildTools();
			gtk_widget_show_all(menutools);
		}

	if(strcmp(gtk_widget_get_name(widget),"cancel")==0)
		gtk_widget_destroy((GtkWidget*)data);
}

void doAbout(GtkWidget* widget,gpointer data)
{
	const char*	authors[]={"K.D.Hedger <"MYEMAIL">",NULL};
	const char	copyright[] ="Copyright \xc2\xa9 2013 K.D.Hedger";
	const char*	aboutboxstring="KKEdit Code Text Editor";

	gtk_show_about_dialog(NULL,"authors",authors,"comments",aboutboxstring,"copyright",copyright,"version",KKEDIT_VERSION,"website",MYWEBSITE,"program-name","KKEdit","logo-icon-name","KKEdit",NULL); 
}

void drawPage(GtkPrintOperation *operation,GtkPrintContext *context,gint page_nr,gpointer user_data)
{
	GtkSourcePrintCompositor *compositor;

	compositor=GTK_SOURCE_PRINT_COMPOSITOR(user_data);
	gtk_source_print_compositor_draw_page(compositor,context,page_nr);
}

void beginPrint(GtkPrintOperation *operation,GtkPrintContext *context,gpointer user_data)
{
	GtkSourcePrintCompositor*	compositor;
	gint						n_pages;

    compositor=GTK_SOURCE_PRINT_COMPOSITOR (user_data);

    while(!gtk_source_print_compositor_paginate(compositor,context));

    n_pages=gtk_source_print_compositor_get_n_pages(compositor);
    gtk_print_operation_set_n_pages(operation,n_pages);
}

void printFile(GtkWidget* widget,gpointer data)
{
	pageStruct*					page=getPageStructPtr(-1);
	GtkSourcePrintCompositor*	printview=gtk_source_print_compositor_new_from_view(page->view);
	GtkPrintOperation*			print;
	GtkPrintOperationResult		result;

	print=gtk_print_operation_new ();
	if (settings != NULL)
		gtk_print_operation_set_print_settings(print,settings);
	g_signal_connect(print,"begin-print",G_CALLBACK(beginPrint),(void*)printview);
	g_signal_connect(print,"draw-page",G_CALLBACK (drawPage),(void*)printview);
	result=gtk_print_operation_run(print,GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,GTK_WINDOW(window),NULL);
	if (result==GTK_PRINT_OPERATION_RESULT_APPLY)
		{
			if (settings != NULL)
				g_object_unref(settings);
			settings=(GtkPrintSettings*)g_object_ref((gpointer)gtk_print_operation_get_print_settings(print));
		}
	g_object_unref(print);
	g_object_unref(printview);
}
