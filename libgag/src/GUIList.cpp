/*
  Copyright (C) 2001-2004 Stephane Magnenat & Luc-Olivier de Charrière
  for any question or comment contact us at nct@ysagoon.com or nuage@ysagoon.com

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

#include <GUIList.h>
#include <functional>
#include <algorithm>
#include <assert.h>
#include <Toolkit.h>
#include <GraphicContext.h>

using namespace GAGCore;

namespace GAGGUI
{
	List::List(int x, int y, int w, int h, Uint32 hAlign, Uint32 vAlign, const std::string &font)
	{
		this->x=x;
		this->y=y;
		this->w=w;
		this->h=h;
		this->hAlignFlag=hAlign;
		this->vAlignFlag=vAlign;
	
		this->font=font;
		nth=-1;
		disp=0;
		blockLength=0;
		blockPos=0;
	}
	
	List::~List()
	{
	
	}
	
	void List::clear(void)
	{
		strings.clear();
		nth=-1;
	}
	
	void List::onSDLEvent(SDL_Event *event)
	{
		int x, y, w, h;
		getScreenPos(&x, &y, &w, &h);
		
		HighlightableWidget::onSDLEvent(event);
	
		if (event->type==SDL_MOUSEBUTTONDOWN)
		{
			unsigned count = (h-4) / textHeight;
			unsigned wSel;
			if (strings.size() > count)
			{
				if (isPtInRect(event->button.x, event->button.y, x+w-21, y, 21, 21))
				{
					// we scroll one line up
					if (disp)
					{
						disp--;
					}
				}
				else if (isPtInRect(event->button.x, event->button.y, x+w-21, y+21, 21, blockPos))
				{
					// we one page up
					if (disp<count)
						disp=0;
					else
						disp-=count;
				}
				else if (isPtInRect(event->button.x, event->button.y, x+w-21, y+21+blockPos+blockLength, 21, h-42-blockPos-blockLength))
				{
					// we one page down
					disp+=count;
					if (disp>strings.size()-count)
						disp=strings.size()-count;
				}
				else if (isPtInRect(event->button.x, event->button.y, x+w-21, y+h-21, 21, 21))
				{
					// we scroll one line down
					if (disp<strings.size()-count)
					{
						disp++;
					}
				}
				wSel = w-20;
			}
			else
				wSel = w;
	
			if (isPtInRect(event->button.x, event->button.y, x, y, wSel, h))
			{
				if (event->button.button == SDL_BUTTON_LEFT)
				{
					int id=event->button.y-y-2;
					id/=textHeight;
					id+=disp;
					if ((id>=0) &&(id<(int)strings.size()))
					{
						if (this->nth != id) 
						{
							nth=id;
							this->selectionChanged();
						}
					}
				}
			}
			if (isPtInRect(event->button.x, event->button.y, x, y, w, h))
			{
				if (event->button.button == 4)
				{
					// we scroll one line up
					if (disp)
					{
						disp--;
					}
				}
				else if (event->button.button == 5)
				{
					// we scroll one line down
					if (disp<strings.size()-count)
					{
						disp++;
					}
				}
			}
		}
	}
	
	void List::selectionChanged()
	{
		this->parent->onAction(this, LIST_ELEMENT_SELECTED, this->nth, 0);
	}
	
	void List::init(void)
	{
		fontPtr = Toolkit::getFont(font.c_str());
		assert(fontPtr);
		textHeight = fontPtr->getStringHeight((const char *)NULL);
		assert(textHeight > 0);
	}
	
	void List::paint(void)
	{
		int x, y, w, h;
		getScreenPos(&x, &y, &w, &h);
		
		assert(parent);
		assert(parent->getSurface());
	
		int nextSize=textHeight;
		int yPos=y+2;
		int i=0;
		unsigned elementLength;
	
		HighlightableWidget::paint();
		parent->getSurface()->drawRect(x, y, w, h, 180, 180, 180);
	
		unsigned count = (h-4) / textHeight;
		if (strings.size() > count)
		{
			// draw line and arrows
			parent->getSurface()->drawVertLine(x+w-21, y, h, 180, 180, 180);
			parent->getSurface()->drawHorzLine(x+w-20, y+21, 19, 180, 180, 180);
			parent->getSurface()->drawHorzLine(x+w-20, y+h-21, 19, 180, 180, 180);
	
			unsigned j;
			int baseX = x+w-11;
			int baseY1 = y+11;
			int baseY2 = y+h-11;
			for (j=7; j>4; j--)
			{
				parent->getSurface()->drawLine(baseX-j, baseY1+j, baseX+j, baseY1+j, 255, 255, 255);
				parent->getSurface()->drawLine(baseX-j, baseY1+j, baseX, baseY1-j, 255, 255, 255);
				parent->getSurface()->drawLine(baseX, baseY1-j, baseX+j, baseY1+j, 255, 255, 255);
				parent->getSurface()->drawLine(baseX-j, baseY2-j, baseX+j, baseY2-j, 255, 255, 255);
				parent->getSurface()->drawLine(baseX-j, baseY2-j, baseX, baseY2+j, 255, 255, 255);
				parent->getSurface()->drawLine(baseX, baseY2+j, baseX+j, baseY2-j, 255, 255, 255);
			}
	
			// draw slider
			int leftSpace = h-43;
			if (leftSpace)
			{
				blockLength = (count * leftSpace) / strings.size();
				blockPos = (disp * (leftSpace - blockLength)) / (strings.size() - count);
				parent->getSurface()->drawRect(x+w-20, y+22+blockPos, 19, blockLength, 255, 255, 255);
			}
			else
			{
				blockLength=0;
				blockPos=0;
			}
	
			elementLength = w-22;
			parent->getSurface()->setClipRect(x+1, y+1, w-22, h-2);
		}
		else
		{
			disp = 0;
	
			elementLength = w-2;
			parent->getSurface()->setClipRect(x+1, y+1, w-2, h-2);
		}
	
		while ((nextSize<h-4) && ((size_t)i<strings.size()))
		{
			parent->getSurface()->drawString(x+2, yPos, fontPtr, (strings[i+disp]).c_str());
			if (i+static_cast<int>(disp) == nth)
				parent->getSurface()->drawRect(x+1, yPos-1, elementLength, textHeight, 170, 170, 240);
			nextSize+=textHeight;
			i++;
			yPos+=textHeight;
		}
		
		parent->getSurface()->setClipRect();
	}
	
	void List::addText(const std::string &text, size_t pos)
	{
		if (pos < strings.size())
		{
			strings.insert(strings.begin()+pos, text);
		}
	}
	
	void List::addText(const std::string &text)
	{
		strings.push_back(text);
	}
	
	void List::sort(void)
	{
		std::sort(strings.begin(), strings.end());
	}
	
	void List::removeText(size_t pos)
	{
		if (pos < strings.size())
		{
			strings.erase(strings.begin()+pos);
			if (static_cast<int>(pos) < nth)
				nth--;
		}
	}
	
	bool List::isText(const std::string &text) const
	{
		for (size_t i=0; i<strings.size(); i++)
		{
			if (strings[i] == text)
				return true;
		}
		return false;
	}
	
	const std::string &List::getText(size_t pos) const
	{
		if (pos < strings.size())
		{
			return strings[pos];
		}
		else
			assert(false);
	}
	
	const std::string &List::get(void) const
	{
		if (nth >= 0)
			return getText(static_cast<size_t>(nth));
		else
			assert(false);
	}
	
	int List::getSelectionIndex(void) const
	{
		return nth;
	}
	
	void List::setSelectionIndex(int index)
	{
		if ((index >= -1 ) && (index < static_cast<int>(strings.size())))
			this->nth = index;
	}
}
