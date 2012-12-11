//
// RPCService.cpp
//
// Copyright (c) 2004-2010 The OpenVanilla Project (http://openvanilla.org)
// All rights reserved.
// 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

#include "BaseIMEServer.h"
#include "ManagedConnector.h"
#include "SharedUI.h"
#include "BPMFUserPhraseHelper.h"
#include "Minotaur.h"
#pragma warning(push)
#pragma warning(disable:4091)
#include <msclr\lock.h>
#pragma warning(pop)
#include <TrackerMaker.h>

// @todo in deployment, make this better
extern "C" {
extern char VendorMotcle[];
extern size_t VendorMotcleSize;
};

#pragma managed
#include "BIServerRPCInterface.h"
#include "BIClientRPCInterface.h"

using namespace System::Windows::Forms;
using namespace std;
using namespace OpenVanilla;
using namespace LFPlatform;
using namespace BaseIMEServer;
using namespace Manjusri;
using namespace Minotaur;
using namespace Takao;

static bool BISServerDuringUpdate = false;

boolean BISOpenConnection( 
    /* [ref][out] */ BISContext *contextPtr)
{
	Logger logger("RPC endpoint");
	logger.debug("BISOpenConnection");
	
	if (!IMEServer::SharedLoader()) {
        *contextPtr = 0;
        return FALSE;
    }

    // if UI is not ready, fail
	if (SharedUI::InputBuffer == nullptr || SharedUI::CandidatePanel == nullptr || SharedUI::StatusBar == nullptr || SharedUI::ToolTip == nullptr) {
        *contextPtr = 0;
        return FALSE;
    }
    	
    PVLoaderContext* context = IMEServer::SharedLoader()->createContext();
    if (!context)
        return  FALSE;
        
    *contextPtr = (BISContext)context;
    return TRUE;
}

void BISCloseConnection( 
    /* [ref][out][in] */ BISContext *contextPtr)
{
	Logger logger("RPC endpoint");
	logger.debug("BISCloseConnection");    
	
    if (!*contextPtr)
        return;
    
    PVLoaderContext* context = (PVLoaderContext*)*contextPtr;
    delete context;
    *contextPtr = NULL;	
}

void BISPing( 
    /* [in] */ BISContext paramContext)
{
	Logger logger("RPC endpoint");
	logger.debug("BISPing");
}

void BISActivate( 
    /* [in] */ BISContext paramContext,
    /* [in] */ boolean stopServerSideUI, 
    /* [out, string, size_is(BISMaxStringLength)] */ wchar_t *currentInputMethodName,
    /* [out] */ int *statusBarPosX, 
    /* [out] */ int *statusBarPosY)
{
	// Logger logger("BISActivate");
	// logger.debug("locking");

    if (BISServerDuringUpdate)
        return;

	msclr::lock lock(ServerLock::SharedLock);

	IMEServer::SharedIMEServer()->syncUserCannedMessages();
    IMEServer::SharedIMEServer()->syncUserOneKeyData();

	PVLoaderContext* context = (PVLoaderContext*)paramContext;
    context->activate();
    IMEServer::SharedIMEServer()->setActiveContext(context);

	String^ imname = SharedUI::StatusBar->LocalizedNameOfCurrentInputMethod();
    pin_ptr<const wchar_t> wchptr = PtrToStringChars(imname);
	wcscpy_s(currentInputMethodName, BISMaxStringLength, wchptr);
	
    IMEServer::SharedIMEServer()->setStopUsingServerSideUI(!!stopServerSideUI);

    *statusBarPosX = SharedUI::StatusBar->Location.X;
    *statusBarPosY = SharedUI::StatusBar->Location.Y;    
}

void BISDeactivate( 
    /* [in] */ BISContext paramContext,
    /* [size_is][string][out] */ wchar_t *residueString)
{
	wmemset(residueString, 0, BISMaxStringLength);
	Logger logger("BISDeactivate");
	// logger.debug("locking");

    if (BISServerDuringUpdate)
        return;

	msclr::lock lock(ServerLock::SharedLock);

	PVLoaderContext* context = (PVLoaderContext*)paramContext;
	string res = context->residueComposingTextBeforeDeactivation();
	wstring wres = OVUTF16::FromUTF8(res);
	wcscpy_s(residueString, BISMaxStringLength, wres.c_str());

    context->clear();
    context->deactivate();
    IMEServer::SharedLoaderService()->resetState();
    
    if (context == IMEServer::SharedIMEServer()->activeContext())    
        IMEServer::SharedIMEServer()->setActiveContext(0); 

	SharedUI::CandidatePanel->Hide();
    SharedUI::InputBuffer->Hide();    
}

void BISShowStatusBar( 
    /* [in] */ BISContext paramContext)
{
    if (BISServerDuringUpdate)
        return;

	Logger logger("RPC endpoint");
	logger.debug("BISShowStatusBar");
	
	// if (!SharedUI::StatusBar->Visible)
   	SharedUI::StatusBar->Show();	
}

void BISHideStatusBar( 
    /* [in] */ BISContext paramContext)
{
    if (BISServerDuringUpdate)
        return;

	Logger logger("RPC endpoint");
	logger.debug("BISHideStatusBar");

	PVLoaderContext* context = (PVLoaderContext*)paramContext;
    if (context == IMEServer::SharedIMEServer()->activeContext()) {
       	SharedUI::StatusBar->Hide();
	}
}

void BISShowInputBuffer( 
    /* [in] */ BISContext paramContext)
{
    if (BISServerDuringUpdate)
        return;

	Logger logger("RPC endpoint");
	logger.debug("BISShowInputBuffer");

	// if (!SharedUI::InputBuffer->Visible)
   	SharedUI::InputBuffer->Show();
}

void BISHideInputBuffer( 
    /* [in] */ BISContext paramContext)
{
    if (BISServerDuringUpdate)
        return;

	Logger logger("RPC endpoint");
	logger.debug("BISHideInputBuffer");

    // if prompt text has something, we know the fake input buffer needs to be on nonetheless
    // we need this trick because WM_IME_ENDCOMPOSITION from BaseIME will always call BISHideInputBuffer, 
    // and we don't want to rock the boat
	if (!IMEServer::SharedLoaderService()->prompt().size()) {
		if (SharedUI::InputBuffer->Visible)
        	SharedUI::InputBuffer->Hide();
	}
}

void BISSetInputBufferPosition( 
    /* [in] */ BISContext paramContext,
    /* [in] */ int cursorPosX,
    /* [in] */ int cursorPosY)
{
    if (BISServerDuringUpdate)
        return;

	Logger logger("RPC endpoint");
	logger.debug("BISSetInputBufferPosition");

	string promptText = IMEServer::SharedLoaderService()->prompt();
	if(promptText.size())
		cursorPosY -= 24;
	if(cursorPosY < 0)
		cursorPosY = 0;

    SharedUI::InputBuffer->SetLocation(cursorPosX, cursorPosY);
}

void BISSetInputBufferTraits( 
    /* [in] */ BISContext paramContext,
    /* [in] */ long fontSize)
{
    if (BISServerDuringUpdate)
        return;

	Logger logger("RPC endpoint");
	logger.debug("BISSetInputBufferTraits: %i", fontSize);
	
    SharedUI::InputBuffer->SetFontHeight(fontSize);
	SharedUI::CandidatePanel->SetInputBufferHeightInPixel(fontSize);
	SharedUI::ToolTip->SetInputBufferHeightInPixel(fontSize);
}

void BISSetCandidatePanelPosition( 
    /* [in] */ BISContext paramContext,
    /* [in] */ int cursorPosX,
    /* [in] */ int cursorPosY)
{
    if (BISServerDuringUpdate)
        return;

	if (SharedUI::InputBuffer->Visible == true) {
		System::Drawing::Point^ newPoint = SharedUI::InputBuffer->CursorPosition;
		SharedUI::CandidatePanel->SetLocation(newPoint->X, newPoint->Y);
		SharedUI::CandidatePanel->SetInputBufferHeightInPixel(SharedUI::InputBuffer->BufferHeight);
		SharedUI::ToolTip->SetLocation(newPoint->X, newPoint->Y);
		return;
	}
	cursorPosY += 5;
    SharedUI::CandidatePanel->SetLocation(cursorPosX, cursorPosY);
	SharedUI::ToolTip->SetLocation(cursorPosX, cursorPosY);
	SharedUI::StatusBar->SetKeyboardFormLocation(cursorPosX, cursorPosY);
}

void BISShowCandidatePanel( 
    /* [in] */ BISContext paramContext)
{
    if (BISServerDuringUpdate)
        return;

	Logger logger("RPC endpoint");
	logger.debug("BISShowCandidatePanel");
	// if (!SharedUI::CandidatePanel->Visible)
   	SharedUI::CandidatePanel->Show();
}

void BISHideCandidatePanel( 
    /* [in] */ BISContext paramContext)
{
    if (BISServerDuringUpdate)
        return;

	Logger logger("RPC endpoint");
	logger.debug("BISHideCandidatePanel");

    if (SharedUI::CandidatePanel->Visible)   
	    SharedUI::CandidatePanel->Hide();
}

void BISHideToolTip( 
    /* [in] */ BISContext paramContext)
{
    if (BISServerDuringUpdate)
        return;

	Logger logger("RPC endpoint");
	logger.debug("BISHideToolTip");
	
	if (SharedUI::ToolTip->Visible)
    	SharedUI::ToolTip->Hide();
}

void BISClearContextState( 
    /* [in] */ BISContext paramContext)
{
    if (BISServerDuringUpdate)
        return;
    
	Logger logger("RPC endpoint");
	logger.debug("BISClearContextState");
	
	PVLoaderContext* context = (PVLoaderContext*)paramContext;
    context->clear();
    IMEServer::SharedLoaderService()->resetState();
    
    SharedUI::CandidatePanel->Hide();
    SharedUI::InputBuffer->Hide();    
}

System::String^ UTF8toCSString(string UTF8string)
{
	wstring wString = OVUTF16::FromUTF8(UTF8string);
	System::String^ CSString = gcnew System::String(wString.c_str());
	return CSString;
}

boolean BISHandleKey( 
    /* [in] */ BISContext paramContext,
    /* [in] */ unsigned int keyCode,
    /* [in] */ unsigned int keyModifiers,
    /* [size_is][string][out] */ wchar_t *committedString,
    /* [size_is][string][out] */ wchar_t *combinedCompositionString,
    /* [size_is][string][out] */ wchar_t *combinedCompositionStringAttribute,
    /* [size_is][string][out] */ wchar_t *compositionString,
    /* [size_is][string][out] */ wchar_t *readingString,
    /* [size_is][string][out] */ wchar_t *candidatePageAsString,
	/* [size_is][string][out] */ wchar_t *candidatePageKeyString,
    /* [size_is][string][out] */ wchar_t *actionVerb,
    /* [size_is][string][out] */ wchar_t *actionParameter,
    /* [out] */ unsigned int *cursorPosition,
    /* [out] */ boolean *isCandidatePanelVisible,
    /* [out] */ boolean *shouldUpdateCandidatePanel,
    /* [out] */ unsigned int *highlightedCandidateIndex)
{
    if (BISServerDuringUpdate)
        return FALSE;
    
	Logger logger("RPC endpoint");
    logger.debug("BISHandleKey, keyCode = 0x%08x (%d), keyModifiers = 0x%08x", keyCode, keyCode, keyModifiers);
    
    #define WCLEAR(s) wmemset(s, 0, BISMaxStringLength)
    WCLEAR(committedString);
    WCLEAR(combinedCompositionString);
    WCLEAR(combinedCompositionStringAttribute);
    WCLEAR(readingString);
    WCLEAR(compositionString);
    WCLEAR(candidatePageAsString);
	WCLEAR(candidatePageKeyString);
    WCLEAR(actionVerb);
    WCLEAR(actionParameter);
    *cursorPosition = 0;
    *isCandidatePanelVisible = FALSE;
    *shouldUpdateCandidatePanel = FALSE;
    *highlightedCandidateIndex = 0;
    
    OVKeyValueMap loaderConfig = IMEServer::SharedLoader()->configKeyValueMap();
    
    PVLoaderContext* context = (PVLoaderContext*)paramContext;
    bool isHandled = false;
    bool settingPunctuationKeyboardFirstTime = false;
    bool punctionaryKeyboardHandled = false;
    
    bool isAlphanumericMode = IMEServer::SharedIMEServer()->isAlphanumericMode();
    bool ignoreShiftAsAlphanumericModeToggleKey = false;
    bool enablesCapsLockAsAlphanumericModeToggle = false;
    
    bool serverSideUISuppressed = IMEServer::SharedIMEServer()->serverSideUINotUsed();
    
    if (loaderConfig.hasKey("EnablesCapsLockAsAlphanumericModeToggle")) {
        enablesCapsLockAsAlphanumericModeToggle = loaderConfig.isKeyTrue("EnablesCapsLockAsAlphanumericModeToggle");
    }
    
    if (enablesCapsLockAsAlphanumericModeToggle) {
        ignoreShiftAsAlphanumericModeToggleKey = true;
    }
    else {
        ignoreShiftAsAlphanumericModeToggleKey = loaderConfig.isKeyTrue("IgnoreShiftAsAlphanumericModeToggleKey");
    }
    
    if (enablesCapsLockAsAlphanumericModeToggle && (keyModifiers & OVKeyMask::CapsLock) && keyModifiers != BASEIME_FAKEKEY_MODIFIER) {
        // reverse the case, remove the caps lock mask
        keyModifiers = keyModifiers & (~OVKeyMask::CapsLock);
        
        if (keyCode >= 'A' && keyCode <= 'Z') {
            keyCode = keyCode - 'A' + 'a';
        }
        else if (keyCode >= 'a' && keyCode <= 'z') {
            keyCode = keyCode - 'a' + 'A';
        }
        
        isAlphanumericMode = true;
    }
    
    string chineseConverterToggleKey = loaderConfig.stringValueForKey("ChineseConverterToggleKey");
    char chineseConverterToggleKeyCode = chineseConverterToggleKey.size() ? chineseConverterToggleKey[0] : 0;
    string repeatLastCommitTextKey = loaderConfig.stringValueForKey("RepeatLastCommitTextKey");
    char repeatLastCommitTextKeyCode = repeatLastCommitTextKey.size() ? repeatLastCommitTextKey[0] : 0;

    if (keyCode == BASEIME_FAKEKEY_CODE && keyModifiers == BASEIME_FAKEKEY_MODIFIER) {
        char directChar = IMEServer::SharedIMEServer()->directCharToClient();

        if (directChar) {
            IMEServer::SharedIMEServer()->clearDirectCharToClient();
            PVKeyImpl* keyImpl = new PVKeyImpl(directChar, 0);
    		OVKey translatedKey = OVKey(keyImpl);
    		isHandled = context->handleKeyEvent(&translatedKey);	            
        }
        else {

			vector<string> cps = context->composingText()->codePoints();
			vector<string> front, back;
			front.insert(front.end(), cps.begin(), cps.begin() + context->composingText()->cursorPosition());
			back.insert(back.end(), cps.begin() + context->composingText()->cursorPosition(), cps.end());
            context->clear();
            IMEServer::SharedLoaderService()->resetState();
			context->composingText()->finishCommit();
			context->composingText()->setText(OVStringHelper::Join(front) + IMEServer::SharedIMEServer()->directTextToClient() + OVStringHelper::Join(back));
            context->composingText()->commit();
            IMEServer::SharedIMEServer()->clearDirectTextToClient();
            isHandled = true;
        }
    }
    else if (keyCode == OVKeyCode::LeftShift && !ignoreShiftAsAlphanumericModeToggleKey && !context->candidateService()->accessVerticalCandidatePanel()->isVisible()) {
        // shift toggles alphanumeric mode, also filters stuff
        logger.debug("toggles alphanumeric mode with left SHIFT");
		SharedUI::StatusBar->ToggleAlphanumericMode();

        // clear everything to ensure no candidate panel etc. is open
        // if (context->readingText()->isEmpty() && context->composingText()->isEmpty())
        //     context->clear();
            
        // returns false, otherwise the next SHIFT will never arrive here...
        isHandled = false;
    }
    else if (keyCode == OVKeyCode::Space && (keyModifiers & OVKeyMask::Ctrl)) {
        // ignore
        logger.debug("ignores CTRL-SPACE");
        isHandled = false;
    }
    else if (keyCode == OVKeyCode::Space && keyModifiers == OVKeyMask::Shift) {
        logger.debug("toggles full-width mode with SHIFT-SPACE");
        SharedUI::StatusBar->ToggleFullWidthCharacterMode();
        isHandled = true;
    }
	else if (keyCode == ',' && (keyModifiers & OVKeyMask::Ctrl) && (keyModifiers & OVKeyMask::Alt)) {
		logger.debug("toggles symbol keypad with CTRL-ALT-,");
		
		if (serverSideUISuppressed) {
            IMEServer::SharedLoaderService()->beep();                
		}
		else if (!SharedUI::CandidatePanel->Visible && !settingPunctuationKeyboardFirstTime) {
			SharedUI::StatusBar->KeyboardFormVisible = true;
			IMEServer::SharedIMEServer()->setWaitingForPunctuationKeyboardKey(true);
			settingPunctuationKeyboardFirstTime = true;
		}
		isHandled = true;
	}
	else if (chineseConverterToggleKeyCode && keyCode == chineseConverterToggleKeyCode && (keyModifiers & OVKeyMask::Ctrl) && (keyModifiers & OVKeyMask::Alt)) {
		logger.debug("toggles converter with CTRL-ALT-s");
		SharedUI::StatusBar->ToggleChineseCharacterConverter();
	}	
    else if (repeatLastCommitTextKeyCode && keyCode == repeatLastCommitTextKeyCode && (keyModifiers & OVKeyMask::Ctrl) && (keyModifiers & OVKeyMask::Alt)) {
        
        if (!context->composingText()->isEmpty()) {
            IMEServer::SharedLoaderService()->beep();
        }
        else {
            context->composingText()->commit(IMEServer::SharedIMEServer()->lastCommittedText());
        }
        
        isHandled = true;
    }
    else if (keyCode == '\\' && keyModifiers == OVKeyMask::Ctrl && loaderConfig.isKeyTrue("ToggleInputMethodWithControlBackslash")) {
		if (serverSideUISuppressed) {
            IMEServer::SharedLoaderService()->beep();                
		}
        else {
            SharedUI::StatusBar->UseNextInputMethod();
        
            string commitText = context->composingText()->composedText();
            context->clear();
        
            if (commitText.length()) {
                 context->composingText()->appendText(commitText);
                 context->composingText()->commit();
            }
        }
        
        isHandled = true;
    }
    else {
        PVKeyImpl* keyImpl;
        
        if (IMEServer::SharedIMEServer()->isWaitingForPunctuationKeyboardKey()) {
            string directText = IMEServer::SharedIMEServer()->lookupPunctuationKeyboardSymbol(tolower(keyCode));
            
            if (directText.length()) {
                keyImpl = new PVKeyImpl(directText, keyCode, OVKeyMask::DirectText | (keyModifiers & ~(OVKeyMask::Shift | OVKeyMask::CapsLock | OVKeyMask::NumLock)));
            }
            else {
                keyImpl = new PVKeyImpl(0, 0);
                IMEServer::SharedLoaderService()->beep();                
            }

            punctionaryKeyboardHandled = true;
        }
        // if we're in alphanumeric mode, we take the key a pass-thru key (just like any non-ASCII string)
        // or, if it's a numeric lock key, we do the same
        else if (isAlphanumericMode || (keyModifiers & OVKeyMask::NumLock)) {
            
            logger.debug("alphanumeric mode, sending direct text with key code: %c", keyCode);

			// WE MUST DISCOUNT 127 (DEL) which is actually a non-printable char

			string directText = (keyCode >= 32 && keyCode < 127) ?  IMEServer::SharedIMEServer()->fullWidthCharacterStringIfActivated(string(1, keyCode)) : string();

            if (keyCode >= 32 && keyCode < 127)
                keyImpl = new PVKeyImpl(directText, keyCode, OVKeyMask::DirectText | (keyModifiers & ~(OVKeyMask::Shift | OVKeyMask::CapsLock | OVKeyMask::NumLock)));
            else
                keyImpl = new PVKeyImpl(keyCode, (keyModifiers & ~(OVKeyMask::Shift | OVKeyMask::CapsLock | OVKeyMask::NumLock)));
        }
        else if ((keyModifiers & OVKeyMask::Shift) && ((keyCode >= 'a' && keyCode <= 'z') || (keyCode >= 'A' && keyCode <= 'Z'))) {
            // if it's alpha, SHIFT key is ON but not CAPS LOCK, we reverse the case, and send a pass-thru key to the app
            
            unsigned int newKeyCode;
            
            if (keyCode >= 'a' && keyCode <= 'z')
                newKeyCode = toupper(keyCode);
            else
                newKeyCode = tolower(keyCode);

            logger.debug("shift on, sending direct text with key code: %c (old: %c)", newKeyCode, keyCode);

            string directText = IMEServer::SharedIMEServer()->fullWidthCharacterStringIfActivated(string(1, (char)newKeyCode));            
            keyImpl = new PVKeyImpl(directText, newKeyCode, OVKeyMask::DirectText | (keyModifiers & ~(OVKeyMask::Shift | OVKeyMask::CapsLock | OVKeyMask::NumLock)));
        }
        else {
            // otherwise, we remove the CAPS LOCK mask, and always use the lower case (Windows mode), same here, ignore 127

            string directText = (keyCode >= 32 && keyCode < 127) ?  IMEServer::SharedIMEServer()->fullWidthCharacterStringIfActivated(string(1, (char)tolower(keyCode))) : string();

            keyImpl = new PVKeyImpl(directText, tolower(keyCode), keyModifiers & (~OVKeyMask::CapsLock));
        }

		OVKey translatedKey = OVKey(keyImpl);
		
		if (punctionaryKeyboardHandled) {
		    context->handleKeyEvent(&translatedKey);
            isHandled = true;
		}
		else {
		    isHandled = context->handleKeyEvent(&translatedKey);	
		}
	}

    bool hasOtherMasks = !!(keyModifiers & (~(OVKeyMask::Shift | OVKeyMask::CapsLock | OVKeyMask::NumLock)));
    if (isHandled && !hasOtherMasks) {
        // if reading buffer, composing buffer are both empty, and the commited string equals keyCode, then we make it a pass thru -- if it's not in EnablesCapsLockAsAlphanumericModeToggle mode (because the case is reversed)
        
        if (context->composingText()->isCommitted() && context->composingText()->isEmpty() && context->readingText()->isEmpty()) {
            if (context->composingText()->composedCommittedText() == string(1, keyCode) && !enablesCapsLockAsAlphanumericModeToggle) {
                
                logger.debug("Is a pass thru key");
                isHandled = false;
            }
        }
    }        
    
    if (IMEServer::SharedIMEServer()->isWaitingForPunctuationKeyboardKey() && !settingPunctuationKeyboardFirstTime) {
		SharedUI::StatusBar->HideKeyboardForm();
        IMEServer::SharedIMEServer()->setWaitingForPunctuationKeyboardKey(false);
        
        if (!punctionaryKeyboardHandled) {
            IMEServer::SharedLoaderService()->beep();
        }
    }    

    if (context->composingText()->isCommitted()) {
        IMEServer::SharedIMEServer()->setLastCommittedText(context->composingText()->composedCommittedText());
		wcscpy_s(committedString, BISMaxStringLength, OVUTF16::FromUTF8(context->composingText()->composedCommittedText()).c_str());
        context->composingText()->finishCommit();
    }
    

    // combine composingText and readingText
    PVCombinedUTF16TextBuffer combinedBuffer(*(context->composingText()), *(context->readingText()));

	string promptText = IMEServer::SharedLoaderService()->prompt();
	PVTextBuffer promptBuffer;
	if (promptText.size()) {
		string logText = IMEServer::SharedLoaderService()->log();
		promptBuffer.setText(logText);
		promptBuffer.setCursorPosition(OVUTF8Helper::SplitStringByCodePoint(logText).size());
		promptBuffer.updateDisplay();

		PVTextBuffer newBuffer;
		newBuffer.setText(combinedBuffer.composedText());
		newBuffer.setCursorPosition(combinedBuffer.cursorPosition());

		PVCombinedUTF16TextBuffer::SegmentPairVector seg = combinedBuffer.wideSegmentPairs();
		for(size_t i = 0; i< seg.size(); i++) {
			if(seg[i].second == PVCombinedUTF16TextBuffer::Highlight)
			{
				OVTextBuffer::RangePair highlight = seg[i].first;
				newBuffer.setHighlightMark(OVTextBuffer::RangePair(highlight.first, highlight.second));
				break;
			}
		};

		newBuffer.updateDisplay();

		combinedBuffer = PVCombinedUTF16TextBuffer(promptBuffer, newBuffer);
	} 

    wcscpy_s(combinedCompositionString, BISMaxStringLength, combinedBuffer.wideComposedText().c_str());    
    *cursorPosition = combinedBuffer.cursorPosition();  // not wideCursorPosition(), we use the codepoint count

	// copy attributes: n (normal), h (highlight), r (reading in combined composition string)
	PVCombinedUTF16TextBuffer::SegmentPairVector segpairs = combinedBuffer.wideSegmentPairs();
	wmemset(combinedCompositionStringAttribute, 'n', wcslen(combinedCompositionString));

    for (PVCombinedUTF16TextBuffer::SegmentPairVector::const_iterator iter = segpairs.begin() ; iter != segpairs.end() ; ++iter) {
        OVTextBuffer::RangePair range = (*iter).first;
        PVCombinedUTF16TextBuffer::SegmentType type = (*iter).second;
        wchar_t fillchar = 'n';

        switch(type) {
        case PVCombinedUTF16TextBuffer::Highlight:
			fillchar = 'h';
            break;
		case PVCombinedUTF16TextBuffer::Reading:
			fillchar = 'r';
			break;
        default:
			fillchar = 'n';
            break;
        }

		for (size_t ai = range.first ; ai < range.first + range.second ; ++ai)
			combinedCompositionStringAttribute[ai] = fillchar;
	}

    // copy reading text alone
    wcscpy_s(readingString, BISMaxStringLength, OVUTF16::FromUTF8(context->readingText()->composedText()).c_str());
    wcscpy_s(compositionString, BISMaxStringLength, OVUTF16::FromUTF8(context->composingText()->composedText()).c_str());

    if (context->composingText()->shouldUpdate() || context->readingText()->shouldUpdate()) {
        context->composingText()->finishUpdate();
        context->readingText()->finishUpdate();
    }

    PVVerticalCandidatePanel* panel = context->candidateService()->accessVerticalCandidatePanel();
    *isCandidatePanelVisible = (panel->isVisible() == true);
    *shouldUpdateCandidatePanel = (panel->shouldUpdate() == true);
    
    // updates the candidate panel
    if (panel->shouldUpdate()) {        
        size_t fromIndex = panel->currentPage() * panel->candidatesPerPage();
        size_t index;
        size_t count = panel->currentPageCandidateCount();
        size_t highlightedIndex = panel->currentHightlightIndex();
        *highlightedCandidateIndex = highlightedIndex;
        OVCandidateList* list = panel->candidateList();
        
		array<String^>^ stringArray = gcnew array<String^>(count);
		array<String^>^ selectionKeyArray = gcnew array<String^>(count);		
        size_t candidatePageStringLength = 0;
		
        for (index = 0; index < count; index++) {
            string candidate = list->candidateAtIndex(fromIndex + index);
            wstring wCandidate = OVUTF16::FromUTF8(candidate);

            if (wCandidate.length() + candidatePageStringLength + 1 < BISMaxStringLength) {                
                wcscpy_s(candidatePageAsString + candidatePageStringLength, BISMaxStringLength - candidatePageStringLength - 1, wCandidate.c_str());
                candidatePageAsString[candidatePageStringLength + wCandidate.length()] = 0xffff;
                candidatePageStringLength += (wCandidate.length() + 1);
            };
            
			stringArray[index] = gcnew String(wCandidate.c_str());
			wstring wKey = OVUTF16::FromUTF8(panel->candidateKeyAtIndex(index).receivedString());
			selectionKeyArray[index] = gcnew String(wKey.c_str());
        }
        
        // finishes our strange combination of the candidate string
        candidatePageAsString[candidatePageStringLength] = 0;

		// fills in the keys
		size_t perPage = panel->candidatesPerPage();
		string cks;
		for (size_t pi = 0 ; pi < perPage ; ++pi) {
			OVKey k = panel->candidateKeyAtIndex(pi);
			if (k.keyCode()) {
				cks += string(1, k.keyCode());
			}
			else {
				cks += " ";
			}
		}
		wstring wcks = OVUTF16::FromUTF8(cks);
		wcscpy_s(candidatePageKeyString, BISMaxStringLength, wcks.c_str());
        
        // filling out the settings
		SharedUI::CandidatePanel->SetItemsPerPage((int)panel->candidatesPerPage());
		SharedUI::CandidatePanel->SetPage((int)panel->currentPage() + 1, (int)panel->pageCount());
		SharedUI::CandidatePanel->SetInControl((bool)panel->isInControl());
		
        String^ promptText = gcnew String(OVUTF16::FromUTF8(panel->prompt()).c_str());
		SharedUI::CandidatePanel->SetPrompt(promptText);
        delete promptText;
        
		SharedUI::CandidatePanel->SetHighlight((int)highlightedIndex);

		if(selectionKeyArray->Length > 0)
			SharedUI::CandidatePanel->SetSelectionKeys(selectionKeyArray);
			
		if(stringArray->Length > 0)
            SharedUI::CandidatePanel->SetCandidates(stringArray);
                        
        SharedUI::CandidatePanel->Refresh();
		context->candidateService()->accessVerticalCandidatePanel()->finishUpdate();

        // reclaim memory
        delete stringArray;
        delete selectionKeyArray;
    }
    
    // update the input buffer if it's visible    
	if (combinedBuffer.wideComposedText().length() || promptText.size()) {
		System::String^ csString = gcnew System::String(combinedBuffer.wideComposedText().c_str());
		// zonble
		//SharedUI::InputBuffer->SetBuffer(csString);
		//SharedUI::InputBuffer->SetCursorIndex(*cursorPosition);
		SharedUI::InputBuffer->SetBufferAndCursorIndex(csString, *cursorPosition);
		SharedUI::InputBuffer->ClearUnderlines();
        delete csString;

		if (promptText.size()) {
			SharedUI::InputBuffer->SetPrompt(UTF8toCSString(promptText));
			string promptDescription = IMEServer::SharedLoaderService()->promptDescription();
			if (promptDescription.size())
				SharedUI::InputBuffer->SetPromptDescription(UTF8toCSString(promptDescription));
			else
				SharedUI::InputBuffer->CleatPromptDescription();
		} else {
			SharedUI::InputBuffer->ClearPrompt();
		}
			
		PVCombinedUTF16TextBuffer::SegmentPairVector seg = combinedBuffer.wideSegmentPairs();
		for (size_t i = 0; i< seg.size(); i++) {
			if(seg[i].second == PVCombinedUTF16TextBuffer::Highlight)
			{
				OVTextBuffer::RangePair highlight = seg[i].first;
				SharedUI::InputBuffer->AddUnderline(highlight.first, highlight.second);
			}
		};

		if (promptText.size()) {
		 	SharedUI::InputBuffer->SetReading((int)promptBuffer.cursorPosition(), (int)context->composingText()->codePointCount());

			// Fix location;
			if (!SharedUI::InputBuffer->Visible) {
				int X = SharedUI::InputBuffer->Left;
				int Y = SharedUI::InputBuffer->Top;
				Y = Y - 24;
				SharedUI::InputBuffer->SetLocation(X, Y);
			}
            // @trying force input buffer to show even in IME-aware apps
            SharedUI::InputBuffer->Show();

            // @trying and supresses sending stuff back to the client!
            WCLEAR(committedString);
            WCLEAR(combinedCompositionString);
            WCLEAR(combinedCompositionStringAttribute);
            WCLEAR(readingString);
            WCLEAR(compositionString);
		}
		else {
		 	SharedUI::InputBuffer->SetReading((int)context->composingText()->cursorPosition(), (int)context->readingText()->codePointCount());
		}
	 	
		if (SharedUI::InputBuffer->Visible) {
			//SharedUI::CandidatePanel->SetLoocation(SharedUI::InputBuffer->CursorPosition);
			//SharedUI::CandidatePanel->SetInputBufferHeightInPixel(SharedUI::InputBuffer->BufferHeight);
	 	    SharedUI::InputBuffer->Refresh();
		}
	}
	else {
		SharedUI::InputBuffer->SetBufferAndCursorIndex("", 0);
		if (SharedUI::InputBuffer->Visible)		
            SharedUI::InputBuffer->Hide();
	}

    // Search in the default dictionary
	if (IMEServer::SharedLoaderService()->dicitonaryKeyword().length()) {
		SharedUI::StatusBar->SearchDictionary(UTF8toCSString(IMEServer::SharedLoaderService()->dicitonaryKeyword()));
	}
	
	// Open URL in client if requested
	if (IMEServer::SharedLoaderService()->URLToOpen().length()) {
        wcscpy_s(actionVerb, BISMaxStringLength, L"open");
        wcscpy_s(actionParameter, BISMaxStringLength, OVUTF16::FromUTF8(IMEServer::SharedLoaderService()->URLToOpen()).c_str());
	}

	// Send "tracker" URL request if requested
	if (IMEServer::SharedLoaderService()->loaderFeatureKey().length()) {
		string key = IMEServer::SharedLoaderService()->loaderFeatureKey();
		string value = IMEServer::SharedLoaderService()->loaderFeatureValue();

		if (key == "SendTrackerRequest") {
			//TrackerMaker tm;
			//wstring wurl = OVUTF16::FromUTF8(tm.trackerURLString(value));
			//String ^csurl = gcnew String(wurl.c_str());
			//Tracker::SendTrackingURL(csurl);
		}
		else if (key == "LaunchApp") {
			wcscpy_s(actionVerb, BISMaxStringLength, L"launchApp");
			wcscpy_s(actionParameter, BISMaxStringLength, OVUTF16::FromUTF8(value).c_str());
		}
	}

    if (!serverSideUISuppressed) {
    	if (context->composingText()->toolTipText().size() && !panel->isVisible()) {
    		Drawing::Point^ p = SharedUI::InputBuffer->CursorPosition;
    		SharedUI::ToolTip->SetLocation(p->X, p->Y);
    		SharedUI::ToolTip->SetText(UTF8toCSString(context->composingText()->toolTipText()));
    		SharedUI::ToolTip->Refresh();
    		SharedUI::ToolTip->Show();
    		context->composingText()->clearToolTip();
    	}
    	else {
    		if (SharedUI::ToolTip) {
    			if (SharedUI::ToolTip->Visible)	
    				SharedUI::ToolTip->Hide();
    		}
    	}
	}
	
	if (SharedUI::StatusBar->KeyboardFormVisible) {
		if (SharedUI::InputBuffer->Visible) {
			Drawing::Point^ p = SharedUI::InputBuffer->CursorPosition;
			SharedUI::StatusBar->SetKeyboardFormLocation(p->X, p->Y);			
		}
		SharedUI::StatusBar->ShowKeyboardForm();
	}
	else {
		SharedUI::StatusBar->HideKeyboardForm();
	}

    if (!serverSideUISuppressed) {
    	// Pops up notifications
    	if (IMEServer::SharedLoaderService()->notifyMessage().size()) {
    		vector<string> messages = IMEServer::SharedLoaderService()->notifyMessage();
    		for (vector<string>::iterator iter = messages.begin() ; iter != messages.end() ; ++iter) {
    			string message = *iter;
    			wstring wMessage = OVUTF16::FromUTF8(message);
			
                String^ notifyString = gcnew String(wMessage.c_str());
    			SharedUI::StatusBar->Notify(notifyString);
                delete notifyString;
    		}
    	}
	}

    // Beep
    if (IMEServer::SharedLoaderService()->shouldBeep() && loaderConfig.isKeyTrue("ShouldPlaySoundOnTypingError")) {
        String^ beepFilename = gcnew String(OVUTF16::FromUTF8(loaderConfig.stringValueForKey("SoundFilename")).c_str());
        SharedUI::StatusBar->Beep(beepFilename);
        delete beepFilename;
    }

    string savedPrompt = IMEServer::SharedLoaderService()->prompt();
	string savedPromptDescrption = IMEServer::SharedLoaderService()->promptDescription();
    string savedLog = IMEServer::SharedLoaderService()->log();
	IMEServer::SharedLoaderService()->resetState();
    IMEServer::SharedLoaderService()->setPrompt(savedPrompt);
	IMEServer::SharedLoaderService()->setPromptDescription(savedPromptDescrption);
    IMEServer::SharedLoaderService()->setLog(savedLog);

	SharedUI::StatusBar->ResetIdleCount();

    return isHandled == true;
    
    #undef WCLEAR    
}

void BISNextAvailableClientRPCEndpointName( 
    /* [in] */ BISContext paramContext,
    /* [size_is][string][out] */ wchar_t *endpointName)
{
    if (BISServerDuringUpdate)
        return;

	Logger logger("RPC endpoint");
	logger.debug("BISNextAvailableClientRPCEndpointName");

    wstring wName = OVUTF16::FromUTF8(IMEServer::SharedIMEServer()->nextAvailableClientRPCEndpointName());
    wcscpy_s(endpointName, BISMaxStringLength, wName.c_str());
}

void BISSetCurrentClientRPCEndpoint( 
    /* [in] */ BISContext paramContext,
    /* [size_is][string][in] */ const wchar_t *endpointName)
{
    if (BISServerDuringUpdate)
        return;

	Logger logger("RPC endpoint");
	logger.debug("BISSetCurrentClientRPCEndpoint");

	PVLoaderContext* context = (PVLoaderContext*)paramContext;
    if (context == IMEServer::SharedIMEServer()->activeContext())    	
        IMEServer::SharedIMEServer()->setCurrentClientRPCEndpointName(OVUTF8::FromUTF16(endpointName));
}

void BISEndCurrentClientRPCEndpoint( 
    /* [in] */ BISContext paramContext)
{
    if (BISServerDuringUpdate)
        return;

	Logger logger("RPC endpoint");
	logger.debug("BISEndCurrentClientRPCEndpoint");

	PVLoaderContext* context = (PVLoaderContext*)paramContext;
    if (context == IMEServer::SharedIMEServer()->activeContext())    		
        IMEServer::SharedIMEServer()->setCurrentClientRPCEndpointName("");
}

void BISOpenPreferences( 
    /* [in] */ BISContext paramContext)
{
    if (BISServerDuringUpdate)
        return;

	Logger logger("RPC endpoint");
	logger.debug("BISOpenPreferences");
    
    // launch the preferences app
    SharedUI::StatusBar->ShowPreferences();
}

void BISClosePreferences( 
    /* [in] */ BISContext paramContext)
{
    if (BISServerDuringUpdate)
        return;

	Logger logger("RPC endpoint");
	logger.debug("BISClosePreferences");
    
    // launch the preferences app
    // SharedUI::StatusBar->HidePreferences();
}

void BISShutdownServer( 
    /* [in] */ BISContext paramContext)
{
	Logger logger("RPC endpoint");
	logger.debug("Received server shutdown request");
    
    Application::Exit();
}

void BISShutdownServerForDatabaseUpdate(/* [in] */ BISContext paramContext)
{
	msclr::lock lock(ServerLock::SharedLock);
    
	Logger logger("RPC endpoint");
	logger.debug("BISShutdownServerForDatabaseUpdate");
    IMEServer::SharedIMEServer()->shutdownServerForDatabaseUpdate();
    
    BISServerDuringUpdate = true;
    Sleep(1000);
}

void BISRestartServerAndCompleteDatabaseUpdate(/* [in] */ BISContext paramContext)
{
	Logger logger("RPC endpoint");
	logger.debug("BISRestartServerAndCompleteDatabaseUpdate");	
    IMEServer::SharedIMEServer()->restartServerAndCompleteDatabaseUpdate();
    
    BISServerDuringUpdate = false;

	SharedUI::StatusBar->Reload();
}

boolean BISSendCustomerServiceEmail( 
    /* [in] */ BISContext paramContext,
    /* [size_is][string][in] */ const wchar_t *email)
{
    return FALSE;
}

boolean BISImportUserPhraseDB( 
    /* [in] */ BISContext paramContext,
    /* [size_is][string][in] */ const wchar_t *filename)
{
    if (BISServerDuringUpdate)
        return FALSE;

    string ufn = OVUTF8::FromUTF16(filename);    
    OVPathInfo pathInfo = IMEServer::SharedLoaderPolicy()->modulePackagePathInfoFromPath("");
    OVSQLiteConnection* db = BPMFUserPhraseHelper::OpenUserPhraseDB(&pathInfo, IMEServer::SharedLoaderService());
    if (!db)
        return FALSE;
        
    bool result = BPMFUserPhraseHelper::Import(db, ufn);    
    delete db;
    
    // flush the config, thus flush its LM cache
    IMEServer::SharedLoader()->forceSyncModuleConfigForNextRound("SmartMandarin");
    
    return result == true;
}

boolean BISExportUserPhraseDB( 
    /* [in] */ BISContext paramContext,
    /* [size_is][string][in] */ const wchar_t *filename)
{
    if (BISServerDuringUpdate)
        return FALSE;

    string ufn = OVUTF8::FromUTF16(filename);    
    OVPathInfo pathInfo = IMEServer::SharedLoaderPolicy()->modulePackagePathInfoFromPath("");
    OVSQLiteConnection* db = BPMFUserPhraseHelper::OpenUserPhraseDB(&pathInfo, IMEServer::SharedLoaderService());
    if (!db)
        return FALSE;
        
    bool result = BPMFUserPhraseHelper::Export(db, ufn);    
    delete db;
    return result == true;    
}

boolean BISValidateFile( 
    /* [in] */ BISContext paramContext,
    /* [size_is][string][in] */ const wchar_t *packageFilename,
    /* [size_is][string][in] */ const wchar_t *signatureFilename)
{
    if (BISServerDuringUpdate)
        return FALSE;

    string upf = OVUTF8::FromUTF16(packageFilename);
    string usf = OVUTF8::FromUTF16(signatureFilename);

    pair<char*, size_t> sigfile = OVFileHelper::SlurpFile(usf);
    
    if (!sigfile.first)
        return FALSE;

    bool valid = false;
    
    if (Minos::ValidateFile(upf, sigfile, pair<char*, size_t>(VendorMotcle, VendorMotcleSize)))
        valid = true;

    free(sigfile.first);
    return valid == true;
}

void BISSendServiceMail( 
    /* [in] */ BISContext paramContext,
    /* [size_is][string][in] */ const wchar_t *email,
    /* [size_is][string][in] */ const wchar_t *description)
{
    // @todo call some Preference or StatusBar stuff here
    if (BISServerDuringUpdate)
        return;

}

#define IsFunction(x)   if (!wcscmp(function, x))
#define RETURN_STRING_RESULT(x)     wcscpy_s(output, BISMaxStringLength, OVUTF16::FromUTF8(x).c_str())
#define U8(x)       OVUTF8::FromUTF16(x)

void BISStatelessFunctionVoid( 
    /* [size_is][string][in] */ const wchar_t *function)
{
    if (BISServerDuringUpdate)
        return;

    if (!wcscmp(function, BISFunctionSyncLoaderAndCurrentModuleConfig)) {
        IMEServer::SharedLoader()->syncLoaderConfig();
        IMEServer::SharedLoader()->syncSandwichConfig();  
		SharedUI::StatusBar->Reload(); 
    }
    else IsFunction(BISFunctionPhraseDBSave) {
        IMEServer::SharedIMEServer()->userPhraseDBSave();
    }
    else IsFunction(BISFunctionStopUsingSignedModules) {
        IMEServer::SharedIMEServer()->stopUsingSignedModules();
        SharedUI::StatusBar->Reload();    	
	}
    else IsFunction(BISFunctionResumeUsingSignedModules) { 
        IMEServer::SharedIMEServer()->resumeUsingSignedModules();
        SharedUI::StatusBar->Reload();    	
    }
}

int BISStatelessFunctionGetInt( 
    /* [size_is][string][in] */ const wchar_t *function)
{
    if (BISServerDuringUpdate)
        return 0;

    IsFunction(BISFunctionPhraseDBNumberOfRow) {
        return IMEServer::SharedIMEServer()->userPhraseDBNumberOfRow();
    }
    else IsFunction(BISFunctionGetNumberOfUnloadableSignedModules) {
        return (int)IMEServer::SharedIMEServer()->numberOfUnloadableSignedModules();
    }
    return 0;
}

boolean BISStatelessFunctionGetBool( 
    /* [size_is][string][in] */ const wchar_t *function)
{
    if (BISServerDuringUpdate)
        return FALSE;

    IsFunction(BISFunctionPhraseDBCanProvideService) {
        return IMEServer::SharedIMEServer()->userPhraseDBCanProvideService() == true;
    }

	return FALSE;
}

void BISStatelessFunctionGetString( 
    /* [size_is][string][in] */ const wchar_t *function,
    /* [size_is][string][out] */ wchar_t *output)
{    
    if (BISServerDuringUpdate)
        return;

    if (!wcscmp(function, BISFunctionGetDatabaseVersion)) {
        wcscpy_s(output, BISMaxStringLength, OVUTF16::FromUTF8(IMEServer::SharedIMEServer()->databaseVersion()).c_str());        
    }
    else if (!wcscmp(function, BISFunctionGetVersionInfoURL)) {
        wcscpy_s(output, BISMaxStringLength, OVUTF16::FromUTF8(IMEServer::SharedIMEServer()->versionInfoURL()).c_str());        
    }
    else if (!wcscmp(function, BISFunctionGetVersionInfoSignatureURL)) {        
        wcscpy_s(output, BISMaxStringLength, OVUTF16::FromUTF8(IMEServer::SharedIMEServer()->versionInfoSignatureURL()).c_str());
    }
    else if (!wcscmp(function, BISFunctionGetUserInfoForPOST)) {        
        stringstream sst;
        PVPlistValue* allPlists = IMEServer::SharedLoader()->loaderAndModulePropertyListsCombined();
        sst << *allPlists << endl;
        delete allPlists;
		
		wstring result = OVUTF16::FromUTF8(sst.str());
		const wchar_t *resultPtr = result.c_str();
		size_t length = wcslen(resultPtr);

		if (length > BISMaxStringLength - 1) {
			result = result.substr(0, BISMaxStringLength - 1);
		}

        wcscpy_s(output, BISMaxStringLength, result.c_str());
    }
    else if (!wcscmp(function, BISFunctionGetUpdateAction)) {        
        wcscpy_s(output, BISMaxStringLength, OVUTF16::FromUTF8(IMEServer::SharedVersionChecker()->updateAction()).c_str());    
    }
    else if (!wcscmp(function, BISFunctionGetActionURL)) {       
        wcscpy_s(output, BISMaxStringLength, OVUTF16::FromUTF8(IMEServer::SharedVersionChecker()->actionURL()).c_str());         
    }
    else if (!wcscmp(function, BISFunctionGetSignatureURL)) {  
        wcscpy_s(output, BISMaxStringLength, OVUTF16::FromUTF8(IMEServer::SharedVersionChecker()->signatureURL()).c_str());              
    }
    else if (!wcscmp(function, BISFunctionGetLocaleTaggedChangeLogURL)) {        
        string changeLogBaseURL = IMEServer::SharedVersionChecker()->changeLogBaseURL();
        string localeTag = IMEServer::SharedVersionChecker()->changeLogLocaleTagURL();
        string changeLogURL = OVStringHelper::StringByReplacingOccurrencesOfStringWithString(changeLogBaseURL, localeTag, IMEServer::SharedLoaderService()->locale());
        wstring ws = OVUTF16::FromUTF8(changeLogURL);
        wcscpy_s(output, BISMaxStringLength, ws.c_str());
    }
    else if (!wcscmp(function, BISFunctionGetPrimaryInputMethod)) {        
        wstring im = OVUTF16::FromUTF8(IMEServer::SharedLoader()->primaryInputMethod());        
        wcscpy_s(output, BISMaxStringLength, im.c_str());        
    }
    else if (!wcscmp(function, BISFunctionGetAllNonSystemGenericInputMethodsIDAndNames)) {        
        string combinedName;
        
    	vector<pair<string, string> > inputMethods = IMEServer::SharedLoader()->allInputMethodIdentifiersAndNames();
    	size_t size, i;
    	size = inputMethods.size();
    	for(i = 0; i < size; i++) {
    		pair<string, string> inputMethod = inputMethods[i];
    		string id = inputMethod.first;
    		string name = inputMethod.second;
    		
    		if (OVWildcard::Match(id, "Generic*") && !OVWildcard::Match(id, "Generic-cj-cin") && !OVWildcard::Match(id, "Generic-simplex-cin")) {
                string idName = id + string("\t") + name;
                
                if (combinedName.length() + idName.length() + 1 < BISMaxStringLength) {
                    if (combinedName.length()) {
                        combinedName += "\t";
                    }
                    
                    combinedName += idName;
                }
    		}
    	}

        wcscpy_s(output, BISMaxStringLength, OVUTF16::FromUTF8(combinedName).c_str());        
    }
    else IsFunction(BISFunctionGetServerLocale) {
        wstring wl = OVUTF16::FromUTF8(IMEServer::SharedLoaderService()->locale());
        wcscpy_s(output, BISMaxStringLength, wl.c_str());        
    }
    else IsFunction(BISFunctionGetUserFreeCannedMessagePath) {
        RETURN_STRING_RESULT(IMEServer::SharedIMEServer()->userFreeCannedMessagePath());
    }
}

void BISStatelessFunctionGetStringWithInt( 
    /* [size_is][string][in] */ const wchar_t *function,
    /* [size_is][string][out] */ wchar_t *output,
    int param1)
{
    if (BISServerDuringUpdate)
        return;

    IsFunction(BISFunctionPhraseDBDataAtRow) {
        RETURN_STRING_RESULT(IMEServer::SharedIMEServer()->userPhraseDBDataAtRow(param1));
    }
    else IsFunction(BISFunctionGetLocalizedNameOfUnloadableSignedModulesAtIndex) {
        RETURN_STRING_RESULT(IMEServer::SharedIMEServer()->localizedNameOfUnloadableSignedModulesAtIndex(param1));
    }
}

boolean BISStatelessFunctionGetBoolWithInt( 
    /* [size_is][string][in] */ const wchar_t *function,
    int param1)
{
    if (BISServerDuringUpdate)
        return FALSE;

    IsFunction(BISFunctionUnloadSignedModuleAtIndex) {
        bool status = IMEServer::SharedIMEServer()->unloadSignedModuleAtIndex((size_t)param1);
        // reset the UI
        SharedUI::StatusBar->Reload();    	
        return !!status;
    }
    
    return FALSE;
}

void BISStatelessFunctionSetStringAndInt( 
    /* [size_is][string][in] */ const wchar_t *function,
    /* [size_is][string][in] */ const wchar_t *param1,
    int param2)
{
    if (BISServerDuringUpdate)
        return;

    IsFunction(BISFunctionPhraseDBSetNewReadingForPhraseAtRow) {
        IMEServer::SharedIMEServer()->userPhraseDBSetNewReadingForPhraseAtRow(U8(param1), param2);
    }
    else IsFunction(BISFunctionPhraseDBSetPhraseAtRow) {
        IMEServer::SharedIMEServer()->userPhraseDBSetPhrase(U8(param1), param2);
    }
}

void BISStatelessFunctionSetInt( 
    /* [size_is][string][in] */ const wchar_t *function,
    int param1)
{
    if (BISServerDuringUpdate)
        return;

    IsFunction(BISFunctionPhraseDBDeleteRow) {
        IMEServer::SharedIMEServer()->userPhraseDBDeleteRow(param1);
    }
}

void BISStatelessFunctionSetString( 
    /* [size_is][string][in] */ const wchar_t *function,
    /* [size_is][string][in] */ const wchar_t *param1)
{
    if (BISServerDuringUpdate)
        return;

    IsFunction(BISFunctionPhraseDBAddNewRow) {
        IMEServer::SharedIMEServer()->userPhraseDBAddNewRow(U8(param1));
    }
    else IsFunction(BISFunctionForceSyncModuleConfig) {
        IMEServer::SharedLoader()->forceWriteModuleConfig(OVUTF8::FromUTF16(param1));            
    }
    else IsFunction(BISFunctionDeleteAllKeysAndValuesInModuleConfig) {
        PVPlistValue* pv = IMEServer::SharedLoader()->configDictionaryForModule(OVUTF8::FromUTF16(param1));
        if (pv) {
            pv->removeAllKeysAndValues();
        }            
    }
    else IsFunction(BISFunctionToggleAroundFilter) {
    	IMEServer::SharedLoader()->toggleAroundFilter(U8(param1));
    }
}


void BISStatelessFunctionGetStringWithString( 
    /* [size_is][string][in] */ const wchar_t *function,
    /* [size_is][string][out] */ wchar_t *output,
    /* [size_is][string][in] */ const wchar_t *param1)
{
    if (BISServerDuringUpdate)
        return;

    if (!wcscmp(function, BISFunctionGetTemporaryFilename)) {
        string ufn = OVUTF8::FromUTF16(param1);
        string result = OVPathHelper::PathCat(OVDirectoryHelper::TempDirectory(), ufn);
        wcscpy_s(output, BISMaxStringLength, OVUTF16::FromUTF8(result).c_str());
    }
    else IsFunction(BISFunctionPhraseDBReadingsForCharacter) {
        RETURN_STRING_RESULT(IMEServer::SharedIMEServer()->userPhraseDBReadingsForPhrase(U8(param1)));
    }
}

boolean BISStatelessFunctionGetBoolWithString( 
    /* [size_is][string][in] */ const wchar_t *function,
    /* [size_is][string][in] */ const wchar_t *param1)
{
    if (BISServerDuringUpdate)
        return FALSE;

    if (!wcscmp(function, BISFunctionShouldUpdate)) {        
        string ufn = OVUTF8::FromUTF16(param1);
        string loaderComponentName = IMEServer::SharedIMEServer()->loaderComponentName();
        string databaseComponentName = IMEServer::SharedIMEServer()->databaseComponentName();
        string moduleComponentName = IMEServer::SharedIMEServer()->vendorModuleComponentName();
        
    	if (!IMEServer::SharedVersionChecker()->loadVersionInfoXMLFile(ufn))
    		return FALSE;

        if (IMEServer::SharedVersionChecker()->componentNeedsUpdating(loaderComponentName)) {
            // now all update info is about loader component
            return TRUE;
        }

        if (IMEServer::SharedVersionChecker()->componentNeedsUpdating(databaseComponentName)) {
            return TRUE;
        }

        if (IMEServer::SharedVersionChecker()->componentNeedsUpdating(moduleComponentName)) {
            return TRUE;
        }

        return FALSE;
    }
    else IsFunction(BISFunctionIsAroundFilterEnabled) {
        return IMEServer::SharedLoader()->isAroundFilterActivated(U8(param1));
    }
    else IsFunction(BISFunctionModuleWithWildcardNameExists) {
    	OVWildcard exp(OVUTF8::FromUTF16(param1));

    	vector<pair<string, string> > mods = IMEServer::SharedLoader()->allModuleIdentifiersAndNames();
    	for (vector<pair<string, string> >::iterator mi = mods.begin() ; mi != mods.end() ; ++mi) {
    		if (exp.match((*mi).first)) {
    			return TRUE;
    		}
		}
		
        return FALSE;       
    }

	return FALSE;
}

extern "C" {
extern char VendorMotcle[];
extern size_t VendorMotcleSize;
};

boolean BISStatelessFunctionGetBoolWithStringAndString( 
    /* [size_is][string][in] */ const wchar_t *function,
    /* [size_is][string][in] */ const wchar_t *param1,
    /* [size_is][string][in] */ const wchar_t *param2)
{
    if (BISServerDuringUpdate)
        return FALSE;

    if (!wcscmp(function, BISFunctionValidateFile)) {        
        string upf = OVUTF8::FromUTF16(param1);
        string usf = OVUTF8::FromUTF16(param2);

        pair<char*, size_t> sigfile = OVFileHelper::SlurpFile(usf);

        if (!sigfile.first)
            return FALSE;

        bool valid = false;

        if (Minos::ValidateFile(upf, sigfile, pair<char*, size_t>(VendorMotcle, VendorMotcleSize)))
            valid = true;

        free(sigfile.first);
        return valid == true;
    }

	return FALSE;
}

void BISStatelessFunctionGetStringWithStringAndString( 
    /* [size_is][string][in] */ const wchar_t *function,
    /* [size_is][string][out] */ wchar_t *output,
    /* [size_is][string][in] */ const wchar_t *param1,
    /* [size_is][string][in] */ const wchar_t *param2)
{
    if (BISServerDuringUpdate)
        return;

    IsFunction(BISFunctionStringValueForConfigKeyOfModule) {
        string result;
        
        PVPlistValue* pv = IMEServer::SharedLoader()->configDictionaryForModule(OVUTF8::FromUTF16(param2));
        if (pv) {
            PVPlistValue* v = pv->valueForKey(OVUTF8::FromUTF16(param1));
            if (v) {
                result = v->stringValue();
            }
        }

        RETURN_STRING_RESULT(result);        
    }
}

void __RPC_USER BISContext_rundown( BISContext )
{
}
