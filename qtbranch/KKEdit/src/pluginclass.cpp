/******************************************************
*
*     ©keithhedger Tue  8 Jul 19:39:52 BST 2014
*     kdhedger68713@gmail.com
*
*     pluginclass.cpp
* 
******************************************************/

#include "sliceclass.h"
#include "pluginclass.h"
#if 0

PluginClass::PluginClass(bool loadPlugs)
{
#if 0
	plugEnabledList=NULL;
	plugins=NULL;
	plugFolderPaths[GLOBALPLUGS]=(char*)DATADIR "/plugins";
	asprintf(&plugFolderPaths[LOCALPLUGS],"%s/.KKEdit/plugins",getenv("HOME"));
	plugCount=-1;
	this->doLoadPlugs=loadPlugs;
	this->loadPlugins();
#endif
}

PluginClass::~PluginClass()
{
#if 0
	debugFree(plugFolderPaths[LOCALPLUGS],"~PluginClass plugFolderPaths");
#endif
}

void PluginClass::setPlugFolder(void)
{
#if 0
#endif
}

char* PluginClass::getNameFromPath(char* path)
{
#if 0
	char*			name;
	StringSlice*	slice=new StringSlice;
	char*			base;

	slice->setReturnDupString(true);	
	base=strdup(path);
	name=basename(base);
	name=slice->sliceBetween(name,(char*)"lib",(char*)".so");
	delete slice;
	debugFree(base,"PluginClass::getNameFromPath base");
	return(name);
#endif
}

char* PluginClass::getNameFromModule(GModule* module)
{
#if 0
	char*		name;
	StringSlice*	slice=new StringSlice;

	slice->setReturnDupString(true);	
	name=slice->sliceBetween((char*)g_module_name(module),(char*)"lib",(char*)".so");
	delete slice;
	return(name);
#endif
}

bool PluginClass::checkForEnabled(char* plugname)
{
#if 0
	FILE*	fd=NULL;
	char*	filename;
	char	buffer[1024];
	char	name[256];

	if(this->doLoadPlugs==false)
		return(false);

	asprintf(&filename,"%s/.KKEdit/pluglist",getenv("HOME"));
	fd=fopen(filename,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					name[0]=0;
					fgets(buffer,1024,fd);
					sscanf(buffer,"%s",(char*)&name);

					if((strlen(name)>0) && (strcasecmp(name,plugname)==0))
						{
							fclose(fd);
							debugFree(filename,"PluginClass::checkForEnabled filename");
							return(false);
						}
				}
			fclose(fd);
		}
	debugFree(filename,"PluginClass::checkForEnabled filename");
	return(true);
#endif
}

gint compareName(gpointer data,gpointer user_data)
{
#if 0
	moduleData* pd=(moduleData*)data;

	if(strcmp(pd->name,(char*)user_data)==0)
		return(0);

	return(1);
#endif
}

moduleData* PluginClass::getPluginByName(char* name)
{
#if 0
	GList*		pl=NULL;
	moduleData*	pd=NULL;

	pl=g_list_find_custom(this->plugins,(gconstpointer )name,(GCompareFunc)compareName);
	if(pl!=NULL)
		pd=(moduleData*)pl->data;

	return(pd);
#endif
}

void PluginClass::deleteBlackList()
{
#if 0
	char*	command;

	asprintf(&command,"rm %s/.KKEdit/pluglist 2>/dev/null",getenv("HOME"));
	system(command);
	debugFree(command," PluginClass::deleteBlackList command");
#endif
}

void PluginClass::appendToBlackList(char* name)
{
#if 0
	char*	command;

	asprintf(&command,"echo %s >> %s/.KKEdit/pluglist",name,getenv("HOME"));
	system(command);
	debugFree(command," PluginClass::appendToBlackList command");

#endif
}

void PluginClass::loadPlugins(void)
{
#if 0
	FILE*		pf;
	char		buffer[4096];
	GModule*	module=NULL;
	char*		command;
	moduleData*	pdata;
	char*		testname;

	if(!g_module_supported())
		{
			perror ("modules not supported");
		}
	else
		{
			this->plugCount=0;
			for(int j=0;j<2;j++)
				{
					asprintf(&command,"find %s -follow -iname \"*.so\" -printf '%%f/%%p\n'| sort -n -t /|cut -f2- -d/",plugFolderPaths[j]);
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
					debugFree(command,"PluginClass::loadPlugins command");
				}
		}
#endif
}

void PluginClass::getEnabledList(void)
{
#if 0
#endif
}

int PluginClass::runPlugFunction(moduleData* pdata,const char* func)
{
#if 0
	int retval=-1;

	int	(*module_plug_function)(gpointer globaldata);
	if(pdata->module!=NULL)
		{
			this->globalPlugData->modData=pdata;
			if(g_module_symbol(pdata->module,func,(gpointer*)&module_plug_function))
				retval=module_plug_function((void*)this->globalPlugData);
		}
	return(retval);
#endif
}

bool PluginClass::enablePlugin(char* name,bool wanttounload)
{
#if 0
	moduleData*	pd=NULL;
	pd=getPluginByName(name);
	if(pd!=NULL)
		{
			pd->unload=wanttounload;
			return((bool)this->runPlugFunction(pd,"enablePlug"));
		}
	return(false);
#endif
}

char* PluginClass::getPluginPathByName(char* name)
{
#if 0
	FILE*		pf;
	char		buffer[4096];
	char*		command;
	char*		testname;

	for(int j=0;j<2;j++)
		{
			asprintf(&command,"find %s -follow -iname \"*.so\" 2>/dev/null",plugFolderPaths[j]);
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
									debugFree(testname,"PluginClass::getPluginPathByName testname");
									return(strdup(buffer));
								}
							else
								debugFree(testname,"PluginClass::getPluginPathByName testname");
						}
					pclose(pf);
				}
		}
	return(NULL);
#endif
}

bool PluginClass::checkForFunction(char* name,const char* func)
{
#if 0
	moduleData* mod=NULL;
	gpointer	symbol;

	mod=this->getPluginByName(name);
	if(mod->module!=NULL)
		return(g_module_symbol(mod->module,func,&symbol));

	return(false);
#endif
}

#endif