// Constants....

#ifndef _CONSTANTS_H
#define _CONSTANTS_H

const size_t NEWNEWS_SIZE = 32760;

//Main Window
const uint32 GET_NEWS  	  		= 'NGgn';
const uint32 SEND_NEWS    		= 'NGsn';
const uint32 BOTH_NEWS 	  		= 'NGbn';
const uint32 EDIT_SERVERS 		= 'NGes';
const uint32 EXPIRE_NEWS  		= 'NGen';
const uint32 REFRESH_LIST 		= 'NGrl';
const uint32 NEXT_ARTICLE		= 'NGnm';
const uint32 NEXT_UNREAD  		= 'NGur';
const uint32 THREAD_READ	  	= 'NGtr';
const uint32 GROUP_READ   		= 'NGgr';
const uint32 NEXT_GROUP         = 'NGng';
const uint32 SAVE_ARTICLE 		= 'NGsa'; 
const uint32 REPLY_ARTICLE 		= 'NGra'; 
const uint32 NEW_ARTICLE 		= 'NGna'; 
const uint32 FORWARD_ARTICLE 	= 'NGfa'; 
const uint32 DISPLAY_ARTICLE	= 'NGda';
const uint32 CURRENT_ARTICLE    = 'NGca';
const uint32 OPEN_ARTWIN		= 'NGoa';
const uint32 ARTWIN_CLOSED		= 'NGwc';
const uint32 SET_TITLE          = 'NGst';
const uint32 FOCUS_ON_TEXT      = 'NGsf';
const uint32 DECODE_ARTICLE     = 'NGua';
const uint32 DECODE_MIME	    = 'NGdm';
const uint32 FIX_GROUPS         = 'NGfg';
const uint32 OPTION_WINDOW		= 'NGow';
const uint32 SCRIPT_SERVER       = 'NGrs';


//Reply Window

const uint32 ARTICLE_SEND 		= 'NGas';
const uint32 ARTICLE_CANCEL 	= 'NGac';

//NNTP and TransferWindow

const uint32 LOG_DATA  			= 'NGld';

// Drag and Drop
const uint32 DRAG_TO_TRACKER 	= 'DDdt';

//Option Window
const uint32 TRANSLATION_ENUM	= 'TWtv';
const uint32 OPTION_SAVE        = 'OWos';

//SCRIPT commands
const uint32 SCRIPT_ASKS         = 'REas';


#endif
