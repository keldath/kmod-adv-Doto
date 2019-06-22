/*  <advc.127> Cut and pasted from CvDLLInterfaceIFaceBase.h; this function
	is getting too large for an inline definition. */

#include "CvGameCoreDLL.h"
#include "CvDLLInterfaceIFaceBase.h" // </advc.127>
#include "CvGameAI.h"
#include "CvPlayerAI.h"

void CvDLLInterfaceIFaceBase::addHumanMessage(PlayerTypes ePlayer, bool bForce,
		int iLength, CvWString szString, LPCTSTR pszSound,
		InterfaceMessageTypes eType, LPCSTR pszIcon, ColorTypes eFlashColor,
		int iFlashX, int iFlashY, bool bShowOffScreenArrows,
		bool bShowOnScreenArrows) {

	CvPlayer& pl = GET_PLAYER(ePlayer);
	if(pl.isHuman() ||
		/*  <advc.700> Want message archive to be available when human
			takes over. AI messages expire just like human messages. */
			(GC.getGameINLINE().isOption(GAMEOPTION_RISE_FALL) &&
			!pl.isHumanDisabled() &&
			GC.getGameINLINE().getRiseFall().isDeliverMessages(ePlayer))) {
			// </advc.700>
		addMessage(ePlayer, bForce, iLength, szString, pszSound, eType,
				pszIcon, eFlashColor, iFlashX, iFlashY, bShowOffScreenArrows,
				bShowOnScreenArrows);
	}
	//else if (GC.getGameINLINE().getActivePlayer() == ePlayer)
	// advc.700: Replacing the above
	else if(!pl.isHuman() && pl.isHumanDisabled()) {
		// this means ePlayer is human, but currently using auto-play (K-Mod)
		if(eType == MESSAGE_TYPE_MAJOR_EVENT || eType == MESSAGE_TYPE_CHAT ||
				eType == MESSAGE_TYPE_MAJOR_EVENT_LOG_ONLY) { // advc.106b
			addMessage(ePlayer,
				/*  advc.127: bForce=true causes the event to be announced
					immediately, whereas bForce=false seems to delay the
					announcement until the start of the next turn (until the
					preceding event has been on display long enough?).
					bForce=true should make Auto Play easier to follow.
					Also set pszSound to NULL; no sounds during Auto Play. */
					true, iLength, szString, NULL, eType, NULL,
					//NO_COLOR, -1, -1, // (K-Mod)
					/*  advc.127: Don't want stuff to flash during Auto Play,
						but pszIcon=NULL is enough to prevent this. No need to
						discard the color and coordinates. */
					eFlashColor, iFlashX, iFlashY,
					false, false);
		}
	}
}
