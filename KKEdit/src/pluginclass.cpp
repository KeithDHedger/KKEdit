/*
 *
 * ©K. D. Hedger. Sun 25 Oct 14:51:48 GMT 2015 kdhedger68713@gmail.com

 * This file(pluginclass.cpp) is part of KKEdit.

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

#include "sliceclass.h"
#include "pluginclass.h"

PluginClass::PluginClass(bool loadPlugs)
{
	ERRDATA
	plugEnabledList=NULL;
	plugins=NULL;
	plugCount=-1;

	sinkReturn=asprintf(&plugFolderPaths[GLOBALPLUGS],"%s",GPLUGSFOLDER);
	sinkReturn=asprintf(&plugFolderPaths[LOCALPLUGS],"%s/%s/plugins-gtk",getenv("HOME"),APPFOLDENAME);

	this->doLoadPlugs=loadPlugs;
	this->loadPlugins();

	g_mkdir_with_parents(plugFolderPaths[LOCALPLUGS],493);
	ERRDATA
}

PluginClass::~PluginClass()
{
	ERRDATA debugFree(&plugFolderPaths[LOCALPLUGS]);
}

void PluginClass::setPlugFolder(void)
{
	ERRDATA
}

char *PluginClass::getNameFromPath(char *path)
{
	ERRDATA
	char			*name;
	StringSlice	*slice=new StringSlice;
	char			*base;

	slice->setReturnDupString(true);	
	base=strdup(path);
	name=basename(base);
	name=slice->sliceBetween(name,(char*)"lib",(char*)".so");
	ERRDATA delete slice;
	ERRDATA debugFree(&base);
	ERRDATA return(name);
}

char *PluginClass::getNameFromModule(GModule *module)
{
	ERRDATA
	char		*name;
	StringSlice	*slice=new StringSlice;

	slice->setReturnDupString(true);	
	name=slice->sliceBetween((char*)g_module_name(module),(char*)"lib",(char*)".so");
	ERRDATA delete slice;
	ERRDATA return(name);
}

bool PluginClass::checkForEnabled(char *plugname)
{
	ERRDATA
	FILE	*fd=NULL;
	char	*filename;
	char	buffer[1024];
	char	name[256];

	if(this->doLoadPlugs==false)
		{
			ERRDATA return(false);
		}

	sinkReturn=asprintf(&filename,"%s/%s/pluglist",getenv("HOME"),APPFOLDENAME);
	fd=fopen(filename,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					name[0]=0;
					sinkReturnStr=fgets(buffer,1024,fd);
					sscanf(buffer,"%s",(char*)&name);

					if((strlen(name)>0) &&(strcasecmp(name,plugname)==0))
						{
							fclose(fd);
							ERRDATA debugFree(&filename);
							ERRDATA return(true);
						}
				}
			fclose(fd);
		}
	ERRDATA debugFree(&filename);
	ERRDATA return(false);
}

gint compareName(gpointer data,gpointer user_data)
{
	ERRDATA
	moduleData *pd=(moduleData*)data;

	if(strcmp(pd->name,(char*)user_data)==0)
		{
			ERRDATA return(0);
		}

	ERRDATA return(1);
}

moduleData *PluginClass::getPluginByName(char *name)
{
	ERRDATA
	GList		*pl=NULL;
	moduleData	*pd=NULL;

	pl=g_list_find_custom(this->plugins,(gconstpointer )name,(GCompareFunc)compareName);
	if(pl!=NULL)
		pd=(moduleData*)pl->data;

	ERRDATA return(pd);
}

void PluginClass::deleteWhiteList()
{
	ERRDATA
	char	*command;

	sinkReturn=asprintf(&command,"rm %s/%s/pluglist 2>/dev/null",getenv("HOME"),APPFOLDENAME);
	sinkReturn=system(command);
	ERRDATA debugFree(&command);
}

void PluginClass::appendToWhiteList(char *name)
{
	ERRDATA
	char	*command;

	sinkReturn=asprintf(&command,"echo %s >> %s/%s/pluglist",name,getenv("HOME"),APPFOLDENAME);
	sinkReturn=system(command);
	ERRDATA debugFree(&command);
}

void PluginClass::loadPlugins(void)
{
	ERRDATA
	FILE		*pf;
	char		buffer[4096];
	GModule		*module=NULL;
	char		*command;
	moduleData	*pdata;
	char		*testname;

	if(!g_module_supported())
		{
			perror("modules not supported");
		}
	else
		{
			this->plugCount=0;
		for(int j=0;j<2;j++)
				{
#ifdef _INBSD_
					sinkReturn=asprintf(&command,"find %s -follow -iname \"*.so\" | sort",plugFolderPaths[j]);
#else
					sinkReturn=asprintf(&command,"find %s -follow -iname \"*.so\" -printf '%%f/%%p\n' 2>/dev/null| sort -n -t /|cut -f2- -d/",plugFolderPaths[j]);
#endif
					pf=popen(command,"r");
					if(pf!=NULL)
						{
							while(fgets(buffer,4096,pf))
								{
									buffer[strlen(buffer)-1]=0;

									testname=this->getNameFromPath(buffer);
									if(this->getPluginByName(testname)==NULL)
										{
											pdata=(moduleData*)malloc(sizeof(moduleData));
											pdata->name=testname;
											pdata->path=strdup(buffer);

											if(this->checkForEnabled(testname))
												{
													module=g_module_open(buffer,G_MODULE_BIND_LAZY);
													if(module!= NULL)
														{
															pdata->module=module;
															pdata->loaded=true;
															pdata->enabled=true;
														}
													else
														{
															pdata->module=NULL;
															pdata->loaded=false;
															pdata->enabled=false;
															printf("Can't open: %s\n",buffer);
														}
												}
											else
												{
													pdata->module=NULL;
													pdata->loaded=false;
													pdata->enabled=false;
												}
											this->plugins=g_list_append(this->plugins,pdata);
											this->plugCount++;
										}
								}
							pclose(pf);
						}
					ERRDATA debugFree(&command);
				}
		}
	ERRDATA
}

void PluginClass::getEnabledList(void)
{
	ERRDATA
}

int PluginClass::runPlugFunction(moduleData *pdata,const char *func)
{
	ERRDATA
	int retval=-1;

	int	(*module_plug_function)(gpointer globaldata);
	if(pdata->module!=NULL)
		{
			this->globalPlugData->modData=pdata;
			ERRDATA if(g_module_symbol(pdata->module,func,(gpointer*)&module_plug_function))
						retval=module_plug_function((void*)this->globalPlugData);
		}
	ERRDATA return(retval);
}

bool PluginClass::enablePlugin(char *name,bool wanttounload)
{	
	moduleData	*pd=NULL;
	pd=getPluginByName(name);
	if(pd!=NULL)
		{
			pd->unload=wanttounload;
			ERRDATA return((bool)this->runPlugFunction(pd,"enablePlug"));
		}
	ERRDATA return(false);
}

char *PluginClass::getPluginPathByName(char *name)
{
	ERRDATA
	FILE		*pf;
	char		buffer[4096];
	char		*command;
	char		*testname;

	for(int j=0;j<2;j++)
		{
			sinkReturn=asprintf(&command,"find %s -follow -iname \"*.so\" 2>/dev/null",plugFolderPaths[j]);
			pf=popen(command,"r");
			if(pf!=NULL)
				{
					while(fgets(buffer,4096,pf))
						{
							buffer[strlen(buffer)-1]=0;
							testname=this->getNameFromPath(buffer);
							if(strcasecmp(name,testname)==0)
								{
									pclose(pf);
									ERRDATA debugFree(&testname);
									ERRDATA return(strdup(buffer));
								}
							else
								ERRDATA debugFree(&testname);
						}
					pclose(pf);
				}
		}
	ERRDATA return(NULL);
}

bool PluginClass::checkForFunction(char *name,const char *func)
{
	ERRDATA
	moduleData *mod=NULL;
	gpointer	symbol;

	mod=this->getPluginByName(name);
	if(mod->module!=NULL)
		{
			ERRDATA return(g_module_symbol(mod->module,func,&symbol));
		}

	ERRDATA return(false);
}

void PluginClass::setUserData(const char *fmt,...)
{
	va_list	ap;
	va_start(ap,fmt);

	while(*fmt)
		switch(*fmt++)
			{
				case 't':
					this->globalPlugData->userData.type=va_arg(ap,int);
					break;
				case 's':
					this->globalPlugData->userData.userStr=strdup(va_arg(ap,char *));
        			break;
				case 'S':
					this->globalPlugData->userData.userStr2=strdup(va_arg(ap,char *));
        			break;
				case 'd':
					this->globalPlugData->userData.userData=va_arg(ap,void*);
					break;
				case 'b':
					this->globalPlugData->userData.userStart=va_arg(ap,int);
					break;
				case 'e':
					this->globalPlugData->userData.userEnd=va_arg(ap,int);
					break;
				case 'i':
					this->globalPlugData->userData.userInt=va_arg(ap,int);
					break;
			}
	va_end(ap);
}




