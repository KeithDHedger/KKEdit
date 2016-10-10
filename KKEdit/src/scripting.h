/*
 *
 * ©K. D. Hedger. Thu  6 Oct 12:10:04 BST 2016 kdhedger68713@gmail.com

 * This file (scripting.h) is part of KKEdit.

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

#ifndef _SCRIPTING_
#define _SCRIPTING_

#define MAX_MSG_SIZE 1024

#define ALLOK 0
#define UNKNOWNARG 1
#define NOMAKEQUEUE 2
#define NOSENDMSG 3
#define WAIT_MSG 0

#define MSGANY 0
#define MSGSEND 2
#define MSGRECEIVE 1

struct msgStruct
{
	long mType;
	char mText[MAX_MSG_SIZE];
};

extern bool	waitForFinish;
extern bool	waitForUserContinue;
extern int			queueID;
extern const char	*commandList[];

void createQueue(void);
void getMsg(void);
void sendOK(void);
void sendContinue(void);

#endif
