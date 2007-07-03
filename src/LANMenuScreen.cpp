/*
  Copyright (C) 2007 Bradley Arsenault

  Copyright (C) 2001-2004 Stephane Magnenat & Luc-Olivier de Charrière
  for any question or comment contact us at <stephane at magnenat dot net> or <NuageBleu at gmail dot com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "LANMenuScreen.h"
#include "GlobalContainer.h"
#include <GUIButton.h>
#include <GUIText.h>
#include <GraphicContext.h>
#include <Toolkit.h>
#include <StringTable.h>
#include "LANFindScreen.h"

LANMenuScreen::LANMenuScreen()
{
	addWidget(new TextButton(0,  70, 300, 40, ALIGN_CENTERED, ALIGN_SCREEN_CENTERED, "menu", Toolkit::getStringTable()->getString("[host]"), HOST));
	addWidget(new TextButton(0,  130, 300, 40, ALIGN_CENTERED, ALIGN_SCREEN_CENTERED,  "menu", Toolkit::getStringTable()->getString("[join a game]"), JOIN));
	addWidget(new TextButton(0, 415, 300, 40, ALIGN_CENTERED, ALIGN_SCREEN_CENTERED,  "menu", Toolkit::getStringTable()->getString("[goto main menu]"), QUIT, 27));
	addWidget(new Text(0, 18, ALIGN_FILL, ALIGN_SCREEN_CENTERED, "menu", Toolkit::getStringTable()->getString("[lan]")));
}

LANMenuScreen::~LANMenuScreen()
{
	/*delete font;
	delete arch;*/
}

void LANMenuScreen::onAction(Widget *source, Action action, int par1, int par2)
{
	if ((action==BUTTON_RELEASED) || (action==BUTTON_SHORTCUT))
	{
		if(par1 == JOIN)
		{
			LANFindScreen lanfs;
			int rc = lanfs.execute(globalContainer->gfx, 40);
			if(rc==-1)
				endExecute(-1);
			else
				endExecute(JoinedGame);
		}
		else if(par1 == HOST)
		{
			endExecute(HostedGame);
		}
		else if(par1 == QUIT)
		{
			endExecute(QuitMenu);
		}
	}
}

void LANMenuScreen::paint(int x, int y, int w, int h)
{
	gfx->drawFilledRect(x, y, w, h, 0, 0, 0);
	//gfxCtx->drawSprite(0, 0, arch, 0);
}

int LANMenuScreen::menu(void)
{
	return LANMenuScreen().execute(globalContainer->gfx, 30);
}
