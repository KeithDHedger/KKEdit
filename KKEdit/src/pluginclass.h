/*
 *
 * ©K. D. Hedger. Fri  6 May 10:50:05 BST 2016 kdhedger68713@gmail.com

 * This file (pluginclass.h) is part of KKEdit.

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

#include <gmodule.h>
#include <stdarg.h>

#include "kkedit-includes.h"

#ifndef _PLUGINCLASS_
#define _PLUGINCLASS_

#ifndef _PLUGINDATA_
#define _PLUGINDATA_

struct moduleData
{
	char*		name;
	bool		enabled;
	GModule*	module;
	bool		loaded;
	char*		path;
	bool		unload;
};
#endif

class PluginClass
{
	public:
		GList*			plugins;
		char*			plugFolderPaths[2];
		plugData*		globalPlugData;
		int				plugCount;
		bool			doLoadPlugs;

		PluginClass(bool loadPlugs);
		~PluginClass();
		void			loadPlugins(void);
		void			setPlugFolder(void);
		void			appendToWhiteList(char* name);
		void			deleteWhiteList();
		moduleData*		getPluginByName(char* name);
		char*			getPluginPathByName(char* name);
		int				runPlugFunction(moduleData* pdata,const char* func);
		bool			enablePlugin(char* name,bool wanttounload);
		bool			checkForFunction(char* name,const char* func);

		void			setUserData(const char *fmt,...);

	private:
		GList*			plugEnabledList;

		bool			checkForEnabled(char* plugname);
		void			getEnabledList(void);
		char*			getNameFromModule(GModule* module);
		char*			getNameFromPath(char* path);
};

#endif
