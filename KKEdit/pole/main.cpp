/*
 *
 * K.D.Hedger 2012 <kdhedger68713@gmail.com>
 *
 */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

#include "config.h"
#include "../src/kkedit-includes.h"

GtkWidget*	progressWindow;
GtkWidget*	progressBar;
bool		doPulse=true;
int			percent;
char		*path=NULL;

enum {QUIT,PULSE,PERCENT};

GtkWidget* createNewBox(int orient,bool homog,int spacing)
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

void shutdown(GtkWidget* widget,gpointer data)
{
	gtk_main_quit();
}

void killBarberPole(void)
{
	gtk_widget_destroy(progressWindow);
	progressBar=NULL;
	remove(path);
	shutdown(NULL,NULL);
}

gboolean idleScroll(gpointer data)
{
	FILE*	fp;
	char	line[256];
	int		dowhat=0;
	char*	command;
	int		perc;

	asprintf(&command,"cat %s",(char*)data);

	fp=popen(command, "r");
	while(fgets(line,256,fp))
		{
			dowhat=PERCENT;
			if(strncasecmp(line,"quit",4)==0)
				dowhat=QUIT;
			if(strncasecmp(line,"pulse",5)==0)
				dowhat=PULSE;
		}
	pclose(fp);

	gtk_main_iteration_do(false);
	if(progressBar!=NULL)
		{
			switch(dowhat)
				{
					case QUIT:
						killBarberPole();
						return(false);
						break;
					case PULSE:
						doPulse=true;
						break;
					default:
						doPulse=false;
						percent=atoi(line);
						break;
				}
			if(doPulse==true)
				gtk_progress_bar_pulse((GtkProgressBar*)progressBar);
			else
				{
					perc=(int)atof(line);
					if(perc>100)
						gtk_progress_bar_set_fraction((GtkProgressBar*)progressBar,1.0);
					else if(perc<0)
						gtk_progress_bar_set_fraction((GtkProgressBar*)progressBar,0);
					else
						gtk_progress_bar_set_fraction((GtkProgressBar*)progressBar,(float)perc/100);
				}
			return(true);
		}
	else
		return(false);
}


void showBarberPole(const char* title,char* filepath)
{
	GtkWidget*		vbox;

	progressWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(progressWindow,400,40);
	gtk_window_set_type_hint((GtkWindow*)progressWindow,GDK_WINDOW_TYPE_HINT_NORMAL);
	gtk_window_set_title((GtkWindow*)progressWindow,title);
	//vbox=gtk_vbox_new(FALSE,0);
	vbox=createNewBox(NEWVBOX,false,0);
	progressBar=gtk_progress_bar_new();
	gtk_progress_bar_set_fraction((GtkProgressBar*)progressBar,0);

//TODO//
#ifndef _USEGTK3_
	gtk_progress_bar_set_orientation((GtkProgressBar*)progressBar,GTK_PROGRESS_LEFT_TO_RIGHT);
#endif

	gtk_box_pack_start(GTK_BOX(vbox),progressBar,false,false,8);
	gtk_container_add(GTK_CONTAINER(progressWindow),vbox);

	gtk_window_set_keep_above((GtkWindow*)progressWindow,true);
	gtk_window_set_deletable((GtkWindow*)progressWindow,false);
	gtk_window_set_resizable((GtkWindow*)progressWindow,false);
	gtk_window_set_skip_taskbar_hint((GtkWindow*)progressWindow,true);
	gtk_window_set_skip_pager_hint((GtkWindow*)progressWindow,true);

	gtk_widget_show_all(progressWindow);
	g_timeout_add(100,idleScroll,(void*)filepath);
}

int main(int argc,char **argv)
{
	char*	command;
	path=argv[2];

	doPulse=false;
	if(argc>3)
		asprintf(&command,"/bin/echo \"%s\" > \"%s\"",argv[3],argv[2]);
	else
		asprintf(&command,"/bin/echo 0 > \"%s\"",argv[2]);
	system(command);
	free(command);
	gtk_init(&argc,&argv);
	showBarberPole(argv[1],argv[2]);
	gtk_main();
}