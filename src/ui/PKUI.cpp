//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKUI.h"
#include "PKStr.h"

//
// Keyboard Utilities
//

#ifdef WIN32

void PKUIComputeKeyState(WPARAM		   osCode, 
						 unsigned int *modifier,
						 PKKey		  *key,
						 unsigned int *virtualKeyCode)
{
	if(modifier       == NULL ||
	   key            == NULL ||
	   virtualKeyCode == NULL)
	{
		return;
	} 

	SHORT shiftDown = GetKeyState(VK_SHIFT);
	SHORT ctrlDown  = GetKeyState(VK_CONTROL);

	// Alt cannot be pressed otherwise
	// we would be getting a WM_SYSKEY*
	// message instead.

	if(shiftDown == -127 || shiftDown == -128)
	{
		*modifier |= PK_MOD_SHIFT;
	}

	if(ctrlDown == -127 || ctrlDown == -128)
	{
		*modifier |= PK_MOD_CONTROL;
	}
	
	*virtualKeyCode = (unsigned int) osCode;

	switch(osCode)
	{
	case '0':
		*key = PK_KEY_0;
		break;

	case '1':
		*key = PK_KEY_1;
		break;

	case '2':
		*key = PK_KEY_2;
		break;

	case '3':
		*key = PK_KEY_3;
		break;

	case '4':
		*key = PK_KEY_4;
		break;

	case '5':
		*key = PK_KEY_5;
		break;

	case '6':
		*key = PK_KEY_6;
		break;

	case '7':
		*key = PK_KEY_7;
		break;

	case '8':
		*key = PK_KEY_8;
		break;

	case '9':
		*key = PK_KEY_9;
		break;

	case 'A':
		*key = PK_KEY_A;
		break;

	case 'B':
		*key = PK_KEY_B;
		break;

	case 'C':
		*key = PK_KEY_C;
		break;

	case 'D':
		*key = PK_KEY_D;
		break;

	case 'E':
		*key = PK_KEY_E;
		break;

	case 'F':
		*key = PK_KEY_F;
		break;

	case 'G':
		*key = PK_KEY_G;
		break;

	case 'H':
		*key = PK_KEY_H;
		break;

	case 'I':
		*key = PK_KEY_I;
		break;

	case 'J':
		*key = PK_KEY_J;
		break;

	case 'K':
		*key = PK_KEY_K;
		break;

	case 'L':
		*key = PK_KEY_L;
		break;

	case 'M':
		*key = PK_KEY_M;
		break;

	case 'N':
		*key = PK_KEY_N;
		break;

	case 'O':
		*key = PK_KEY_O;
		break;

	case 'P':
		*key = PK_KEY_P;
		break;

	case 'Q':
		*key = PK_KEY_Q;
		break;

	case 'R':
		*key = PK_KEY_R;
		break;

	case 'S':
		*key = PK_KEY_S;
		break;

	case 'T':
		*key = PK_KEY_T;
		break;

	case 'U':
		*key = PK_KEY_U;
		break;

	case 'V':
		*key = PK_KEY_V;
		break;

	case 'W':
		*key = PK_KEY_W;
		break;

	case 'X':
		*key = PK_KEY_X;
		break;

	case 'Y':
		*key = PK_KEY_Y;
		break;

	case 'Z':
		*key = PK_KEY_Z;
		break;

	case VK_LEFT:
		*key = PK_KEY_LEFT;
		break;

	case VK_RIGHT:
		*key = PK_KEY_RIGHT;
		break;

	case VK_UP:
		*key = PK_KEY_UP;
		break;

	case VK_DOWN:
		*key = PK_KEY_DOWN;
		break;

	case VK_RETURN:
		*key = PK_KEY_ENTER;
		break;

	case VK_ESCAPE:
		*key = PK_KEY_ESCAPE;
		break;

	case VK_BACK:
		*key = PK_KEY_BACKSPACE;
		break;

	case VK_DELETE:
		*key = PK_KEY_DELETE;
		break;

	case VK_TAB:
		*key = PK_KEY_TAB;
		break;

	case VK_SPACE:
		*key = PK_KEY_SPACE;
		break;

	case VK_HOME:
		*key = PK_KEY_HOME;
		break;

	case VK_INSERT:
		*key = PK_KEY_INSERT;
		break;

	case VK_PRIOR:
		*key = PK_KEY_PAGEUP;
		break;

	case VK_NEXT:
		*key = PK_KEY_PAGEDOWN;
		break;

	case VK_END:
		*key = PK_KEY_END;
		break;

	case VK_F1:
		*key = PK_KEY_F1;
		break;

	case VK_F2:
		*key = PK_KEY_F2;
		break;

	case VK_F3:
		*key = PK_KEY_F3;
		break;

	case VK_F4:
		*key = PK_KEY_F4;
		break;

	case VK_F5:
		*key = PK_KEY_F5;
		break;

	case VK_F6:
		*key = PK_KEY_F6;
		break;

	case VK_F7:
		*key = PK_KEY_F7;
		break;

	case VK_F8:
		*key = PK_KEY_F8;
		break;

	case VK_F9:
		*key = PK_KEY_F9;
		break;

	case VK_F10:
		*key = PK_KEY_F10;
		break;

	case VK_F11:
		*key = PK_KEY_F11;
		break;

	case VK_F12:
		*key = PK_KEY_F12;
		break;
	}
	
}

void PKUIParseAccelerator(std::string accel,
                          BYTE       *modifier,
                          WORD       *key)
{
    if(modifier == NULL || key == NULL)
    {
        return;
    }

    std::vector<std::string> items;
    PKStr::explode(accel, '+', &items);

    *modifier = 0;
    *key      = 0;

    uint32_t count = items.size();

    for(uint32_t i = 0; i < count; i++)
    {
        std::string item = items[i];
        
        if(PKStr::toLower(item) == "ctrl")
        {
            *modifier |= FCONTROL;
        }

        if(PKStr::toLower(item) == "alt")
        {
            *modifier |= FALT;
        }

        if(PKStr::toLower(item) == "shift")
        {
            *modifier |= FSHIFT;
        }

        if(item.size() == 1)
        {
            *modifier |= FVIRTKEY;
            *key = item[0];
        }
        else if((item[0] == 'f') || (item[0] == 'F'))
        {
            // F1 to F12
            *modifier |= FVIRTKEY;

            if(PKStr::toLower(item) == "f1")
            {
                *key = VK_F1;
            }

            if(PKStr::toLower(item) == "f2")
            {
                *key = VK_F2;
            }

            if(PKStr::toLower(item) == "f3")
            {
                *key = VK_F3;
            }

            if(PKStr::toLower(item) == "f4")
            {
                *key = VK_F4;
            }

            if(PKStr::toLower(item) == "f5")
            {
                *key = VK_F5;
            }

            if(PKStr::toLower(item) == "f6")
            {
                *key = VK_F6;
            }

            if(PKStr::toLower(item) == "f7")
            {
                *key = VK_F7;
            }

            if(PKStr::toLower(item) == "f8")
            {
                *key = VK_F8;
            }

            if(PKStr::toLower(item) == "f9")
            {
                *key = VK_F9;
            }

            if(PKStr::toLower(item) == "f10")
            {
                *key = VK_F10;
            }

            if(PKStr::toLower(item) == "f11")
            {
                *key = VK_F11;
            }

            if(PKStr::toLower(item) == "f12")
            {
                *key = VK_F12;
            }
        }
    }
}

#endif

#ifdef MACOSX

#include <Carbon/Carbon.h>

enum {
  kHomeKey = 1,
  kEnterKey = 0x3,
  kHelpKey = 5,
  kDeleteKey = 8,
  kTabKey = 9,
  kPageUpKey = 0x0b,
  kPageDownKey,
  kReturnKey = 0x0d,
  kEscKey = 0x1B,
  kLeftArrowKey,
  kRightArrowKey,
  kUpArrowKey,
  kDownArrowKey,
  kSpaceKey,
  kFDeleteKey = 0x7f 
};

void PKUIComputeKeyState(EventRecord  *eventRecord, 
						 unsigned int *modifier,
						 PKKey		  *keyCode,
						 unsigned int *virtualKeyCode)
{
	if(eventRecord == NULL ||
	   modifier    == NULL ||
	   keyCode     == NULL ||
	   virtualKeyCode == NULL)
	{
		return;
	}
	
	if(eventRecord->modifiers & cmdKey)
	{
		*modifier |= PK_MOD_COMMAND;
	}

	if(eventRecord->modifiers & optionKey ||
	   eventRecord->modifiers & rightOptionKey)
	{
		*modifier |= PK_MOD_OPTION;
	}

	if(eventRecord->modifiers & shiftKey ||
	   eventRecord->modifiers & rightShiftKey)
	{
		*modifier |= PK_MOD_SHIFT;
	}

	if(eventRecord->modifiers & controlKey ||
	   eventRecord->modifiers & rightControlKey)
	{
		*modifier |= PK_MOD_CONTROL;
	}
	
	SInt16 theKey = (eventRecord->message & charCodeMask);
	SInt16 vcode  = ((eventRecord->message & keyCodeMask)>>8);
	
	*virtualKeyCode = vcode;
	
	switch(theKey)
	{
		case kEscKey:
		{
			*keyCode = PK_KEY_ESCAPE;
			return;
		}
		break;

		case kEnterKey:
		case kReturnKey:
		{
			*keyCode = PK_KEY_ENTER;
			return;
		}
		break;

		case kDeleteKey:
		{
			*keyCode = PK_KEY_BACKSPACE;
			return;
		}
		break;

		case kFDeleteKey:
		{
			*keyCode = PK_KEY_DELETE;
			return;
		}
		break;

		case kTabKey:
		{
			*keyCode = PK_KEY_TAB;
			return;
		}
		break;

		case kSpaceKey:
		{
			*keyCode = PK_KEY_SPACE;
			return;
		}
		break;

		case kLeftArrowKey:
		{
			*keyCode = PK_KEY_LEFT;
			return;
		}
		break;

		case kRightArrowKey:
		{
			*keyCode = PK_KEY_RIGHT;
			return;
		}
		break;

		case kDownArrowKey:
		{
			*keyCode = PK_KEY_DOWN;
			return;
		}
		break;

		case kUpArrowKey:
		{
			*keyCode = PK_KEY_UP;
			return;
		}
		break;
	}

	// If we got there, it's not an all-keyboard "system" key
	// and we must look at the virtual key code and translate
	// the key to be sure about what's been pressed...
	
	Ptr KCHRPtr  = (Ptr) GetScriptManagerVariable(smKCHRCache);
	UInt32 state = 0;
	
	UInt16 vkeyCode = (vcode & 0xFF); // no modifiers
	UInt16 shiftedVCode = (vcode & 0xFF) | (1 << shiftKeyBit); // shift modifier
	
	UInt32 characterCode = KeyTranslate(KCHRPtr, vkeyCode, &state);
	UInt32 shiftedCharacterCode = KeyTranslate(KCHRPtr, shiftedVCode, &state);

	// Now see what we got
	
	switch(shiftedCharacterCode)
	{
		case 'A':
		{
			*keyCode = PK_KEY_A;
		}
		break;
		case 'B':
		{
			*keyCode = PK_KEY_B;
		}
		break;
		case 'C':
		{
			*keyCode = PK_KEY_C;
		}
		break;
		case 'D':
		{
			*keyCode = PK_KEY_D;
		}
		break;
		case 'E':
		{
			*keyCode = PK_KEY_E;
		}
		break;
		case 'F':
		{
			*keyCode = PK_KEY_F;
		}
		break;
		case 'G':
		{
			*keyCode = PK_KEY_G;
		}
		break;
		case 'H':
		{
			*keyCode = PK_KEY_H;
		}
		break;
		case 'I':
		{
			*keyCode = PK_KEY_I;
		}
		break;
		case 'J':
		{
			*keyCode = PK_KEY_J;
		}
		break;
		case 'K':
		{
			*keyCode = PK_KEY_K;
		}
		break;
		case 'L':
		{
			*keyCode = PK_KEY_L;
		}
		break;
		case 'M':
		{
			*keyCode = PK_KEY_M;
		}
		break;
		case 'N':
		{
			*keyCode = PK_KEY_N;
		}
		break;
		case 'O':
		{
			*keyCode = PK_KEY_O;
		}
		break;
		case 'P':
		{
			*keyCode = PK_KEY_P;
		}
		break;
		case 'Q':
		{
			*keyCode = PK_KEY_Q;
		}
		break;
		case 'R':
		{
			*keyCode = PK_KEY_R;
		}
		break;
		case 'S':
		{
			*keyCode = PK_KEY_S;
		}
		break;
		case 'T':
		{
			*keyCode = PK_KEY_T;
		}
		break;
		case 'U':
		{
			*keyCode = PK_KEY_U;
		}
		break;
		case 'V':
		{
			*keyCode = PK_KEY_V;
		}
		break;
		case 'W':
		{
			*keyCode = PK_KEY_W;
		}
		break;
		case 'X':
		{
			*keyCode = PK_KEY_X;
		}
		break;
		case 'Y':
		{
			*keyCode = PK_KEY_Y;
		}
		break;
		case 'Z':
		{
			*keyCode = PK_KEY_Z;
		}
		break;
	}
	
	if(characterCode		== '0' ||
	   shiftedCharacterCode == '0')
	{
		*keyCode = PK_KEY_0;
	}

	if(characterCode		== '1' ||
	   shiftedCharacterCode == '1')
	{
		*keyCode = PK_KEY_1;
	}

	if(characterCode		== '2' ||
	   shiftedCharacterCode == '2')
	{
		*keyCode = PK_KEY_2;
	}

	if(characterCode		== '3' ||
	   shiftedCharacterCode == '3')
	{
		*keyCode = PK_KEY_3;
	}

	if(characterCode		== '4' ||
	   shiftedCharacterCode == '4')
	{
		*keyCode = PK_KEY_4;
	}

	if(characterCode		== '5' ||
	   shiftedCharacterCode == '5')
	{
		*keyCode = PK_KEY_5;
	}

	if(characterCode		== '6' ||
	   shiftedCharacterCode == '6')
	{
		*keyCode = PK_KEY_6;
	}

	if(characterCode		== '7' ||
	   shiftedCharacterCode == '7')
	{
		*keyCode = PK_KEY_7;
	}

	if(characterCode		== '8' ||
	   shiftedCharacterCode == '8')
	{
		*keyCode = PK_KEY_8;
	}

	if(characterCode		== '9' ||
	   shiftedCharacterCode == '9')
	{
		*keyCode = PK_KEY_9;
	}

}

void PKUIParseAccelerator(std::string accel,
						  UInt8      *modifier,
						  UInt16     *key)
{
    if(modifier == NULL || key == NULL)
    {
        return;
    }
	
    std::vector<std::string> items;
    PKStr::explode(accel, '+', &items);
	
    *modifier = kMenuNoCommandModifier;
    *key      = 0;
	
    uint32_t count = items.size();
	
    for(uint32_t i = 0; i < count; i++)
    {
        std::string item = items[i];
        
        if(PKStr::toLower(item) == "ctrl")
        {
            *modifier |= kMenuControlModifier;
        }
		
        if(PKStr::toLower(item) == "option")
        {
            *modifier |= kMenuOptionModifier;
        }
		
        if(PKStr::toLower(item) == "shift")
        {
            *modifier |= kMenuShiftModifier;
        }
		
		if(PKStr::toLower(item) == "command")
        {
            *modifier &= ~kMenuNoCommandModifier;
        }
		
        if(item.size() == 1)
        {
            *key = item[0];
        }
        // TODO? F1 -> F12
    }	
}

#endif

#ifdef LINUX

#include <gdk/gdkkeysyms.h>

void PKUIParseAccelerator(std::string      accel,
						  GdkModifierType *modifier,
						  guint           *key)						  
{
    if(modifier == NULL || key == NULL)
    {
        return;
    }
	
    std::vector<std::string> items;
    PKStr::explode(accel, '+', &items);
	
    *modifier = (GdkModifierType) 0;
    *key      = 0;
	
    uint32_t count = items.size();
	
    for(uint32_t i = 0; i < count; i++)
    {
        std::string item = items[i];
        
        if(PKStr::toLower(item) == "ctrl")
        {
            *modifier = (GdkModifierType)((int)(*modifier) | GDK_CONTROL_MASK);
        }
				
        if(PKStr::toLower(item) == "shift")
        {
            *modifier = (GdkModifierType)((int)(*modifier) | GDK_SHIFT_MASK);
        }
		
		if(PKStr::toLower(item) == "alt")
        {
            *modifier = (GdkModifierType)((int)(*modifier) | GDK_MOD1_MASK);
        }
		
        if(item.size() == 1)
        {
            *key = item[0];
        }
        else if((item[0] == 'f') || (item[0] == 'F'))
        {
            // F1 to F12

            if(PKStr::toLower(item) == "f1")
            {
                *key = GDK_F1;
            }

            if(PKStr::toLower(item) == "f2")
            {
                *key = GDK_F2;
            }

            if(PKStr::toLower(item) == "f3")
            {
                *key = GDK_F3;
            }

            if(PKStr::toLower(item) == "f4")
            {
                *key = GDK_F4;
            }

            if(PKStr::toLower(item) == "f5")
            {
                *key = GDK_F5;
            }

            if(PKStr::toLower(item) == "f6")
            {
                *key = GDK_F6;
            }

            if(PKStr::toLower(item) == "f7")
            {
                *key = GDK_F7;
            }

            if(PKStr::toLower(item) == "f8")
            {
                *key = GDK_F8;
            }

            if(PKStr::toLower(item) == "f9")
            {
                *key = GDK_F9;
            }

            if(PKStr::toLower(item) == "f10")
            {
                *key = GDK_F10;
            }

            if(PKStr::toLower(item) == "f11")
            {
                *key = GDK_F11;
            }

            if(PKStr::toLower(item) == "f12")
            {
                *key = GDK_F12;
            }
        }
    }	
}	

void PKUIComputeKeyState(GdkEventKey *event, 
						 unsigned int *modifier,
					     PKKey		  *keyCode,
					     unsigned int *virtualKeyCode)
{
	if(event       == NULL ||
	   modifier    == NULL ||
	   keyCode     == NULL ||
	   virtualKeyCode == NULL)
	{
		return;
	}

	// Modifiers

    if(event->state & GDK_SHIFT_MASK)
    {
        *modifier |= PK_MOD_SHIFT;
    }
        
    if(event->state & GDK_CONTROL_MASK)
    {
        *modifier |= PK_MOD_CONTROL;
    }

    if(event->state & GDK_MOD1_MASK)
    {
        *modifier |= PK_MOD_ALT;
    }

    // VK

    *virtualKeyCode = event->hardware_keycode;

    // Key

    switch(event->keyval)
    {
        case GDK_a:
        case GDK_A:
        {
            *keyCode = PK_KEY_A;
        }
        break;
        case GDK_b:
        case GDK_B:
        {
            *keyCode = PK_KEY_B;
        }
        break;
        case GDK_c:
        case GDK_C:
        {
            *keyCode = PK_KEY_C;
        }
        break;
        case GDK_d:
        case GDK_D:
        {
            *keyCode = PK_KEY_D;
        }
        break;
        case GDK_e:
        case GDK_E:
        {
            *keyCode = PK_KEY_E;
        }
        break;
        case GDK_f:
        case GDK_F:
        {
            *keyCode = PK_KEY_F;
        }
        break;
        case GDK_g:
        case GDK_G:
        {
            *keyCode = PK_KEY_G;
        }
        break;
        case GDK_h:
        case GDK_H:
        {
            *keyCode = PK_KEY_H;
        }
        break;
        case GDK_i:
        case GDK_I:
        {
            *keyCode = PK_KEY_I;
        }
        break;
        case GDK_j:
        case GDK_J:
        {
            *keyCode = PK_KEY_J;
        }
        break;
        case GDK_k:
        case GDK_K:
        {
            *keyCode = PK_KEY_K;
        }
        break;
        case GDK_l:
        case GDK_L:
        {
            *keyCode = PK_KEY_L;
        }
        break;
        case GDK_m:
        case GDK_M:
        {
            *keyCode = PK_KEY_M;
        }
        break;
        case GDK_n:
        case GDK_N:
        {
            *keyCode = PK_KEY_N;
        }
        break;
        case GDK_o:
        case GDK_O:
        {
            *keyCode = PK_KEY_O;
        }
        break;
        case GDK_p:
        case GDK_P:
        {
            *keyCode = PK_KEY_P;
        }
        break;
        case GDK_q:
        case GDK_Q:
        {
            *keyCode = PK_KEY_Q;
        }
        break;
        case GDK_r:
        case GDK_R:
        {
            *keyCode = PK_KEY_R;
        }
        break;
        case GDK_s:
        case GDK_S:
        {
            *keyCode = PK_KEY_S;
        }
        break;
        case GDK_t:
        case GDK_T:
        {
            *keyCode = PK_KEY_T;
        }
        break;
        case GDK_u:
        case GDK_U:
        {
            *keyCode = PK_KEY_U;
        }
        break;
        case GDK_v:
        case GDK_V:
        {
            *keyCode = PK_KEY_V;
        }
        break;
        case GDK_w:
        case GDK_W:
        {
            *keyCode = PK_KEY_W;
        }
        break;
        case GDK_x:
        case GDK_X:
        {
            *keyCode = PK_KEY_X;
        }
        break;
        case GDK_y:
        case GDK_Y:
        {
            *keyCode = PK_KEY_Y;
        }
        break;
        case GDK_z:
        case GDK_Z:
        {
            *keyCode = PK_KEY_Z;
        }
        break;

        case GDK_1:
        {
            *keyCode = PK_KEY_1;
        }
        break;
        case GDK_2:
        {
            *keyCode = PK_KEY_2;
        }
        break;
        case GDK_3:
        {
            *keyCode = PK_KEY_3;
        }
        break;
        case GDK_4:
        {
            *keyCode = PK_KEY_4;
        }
        break;
        case GDK_5:
        {
            *keyCode = PK_KEY_5;
        }
        break;
        case GDK_6:
        {
            *keyCode = PK_KEY_6;
        }
        break;
        case GDK_7:
        {
            *keyCode = PK_KEY_7;
        }
        break;
        case GDK_8:
        {
            *keyCode = PK_KEY_8;
        }
        break;
        case GDK_9:
        {
            *keyCode = PK_KEY_9;
        }
        break;
        case GDK_0:
        {
            *keyCode = PK_KEY_0;
        }
        break;

        case GDK_F1:
        {
            *keyCode = PK_KEY_F1;
        }
        break;
        case GDK_F2:
        {
            *keyCode = PK_KEY_F2;
        }
        break;
        case GDK_F3:
        {
            *keyCode = PK_KEY_F3;
        }
        break;
        case GDK_F4:
        {
            *keyCode = PK_KEY_F4;
        }
        break;
        case GDK_F5:
        {
            *keyCode = PK_KEY_F5;
        }
        break;
        case GDK_F6:
        {
            *keyCode = PK_KEY_F6;
        }
        break;
        case GDK_F7:
        {
            *keyCode = PK_KEY_F7;
        }
        break;
        case GDK_F8:
        {
            *keyCode = PK_KEY_F8;
        }
        break;
        case GDK_F9:
        {
            *keyCode = PK_KEY_F9;
        }
        break;
        case GDK_F10:
        {
            *keyCode = PK_KEY_F10;
        }
        break;
        case GDK_F11:
        {
            *keyCode = PK_KEY_F11;
        }
        break;
        case GDK_F12:
        {
            *keyCode = PK_KEY_F12;
        }
        break;

        case GDK_Tab:
        {
            *keyCode = PK_KEY_TAB;
        }
        break;
        case GDK_Escape:
        {
            *keyCode = PK_KEY_ESCAPE;
        }
        break;
        case GDK_space:
        {
            *keyCode = PK_KEY_SPACE;
        }
        break;
        case GDK_BackSpace:
        {
            *keyCode = PK_KEY_BACKSPACE;
        }
        break;
        case GDK_Return:
        {
            *keyCode = PK_KEY_ENTER;
        }
        break;
        case GDK_Delete:
        {
            *keyCode = PK_KEY_DELETE;
        }
        break;
        case GDK_Home:
        {
            *keyCode = PK_KEY_HOME;
        }
        break;
        case GDK_End:
        {
            *keyCode = PK_KEY_END;
        }
        break;
        case GDK_Insert:
        {
            *keyCode = PK_KEY_INSERT;
        }
        break;
        case GDK_Page_Up:
        {
            *keyCode = PK_KEY_PAGEUP;
        }
        break;
        case GDK_Page_Down:
        {
            *keyCode = PK_KEY_PAGEDOWN;
        }
        break;

        case GDK_Left:
        {
            *keyCode = PK_KEY_LEFT;
        }
        break;
        case GDK_Right:
        {
            *keyCode = PK_KEY_RIGHT;
        }
        break;
        case GDK_Up:
        {
            *keyCode = PK_KEY_UP;
        }
        break;
        case GDK_Down:
        {
            *keyCode = PK_KEY_DOWN;
        }
        break;
        
    }
}					     

#endif

