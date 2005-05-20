/* March 12, 2005

	PLplot driver for AquaTerm and Mac OS X.

	Copyright (C) Per Persson
	Copyright (C) 2005 Hazen Babcock

	This file is part of PLplot.

	PLplot is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Library Public License as published
	by the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	PLplot is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Library General Public License for more details.

	You should have received a copy of the GNU Library General Public License
	along with PLplot; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
   	
*/

//---------------------------------------------
// Header files, defines and local variables
//---------------------------------------------

/* OS X specific header files */

#import <Foundation/Foundation.h>
#import <aquaterm/AQTAdapter.h>

/* PLplot header files */

#include "plplotP.h"
#include "drivers.h"

/* constants */

#define SCALE			0.1
#define AQT_Default_X	720
#define AQT_Default_Y	540
#define DPI             72.0

#define MAX_STRING_LEN	1000
#define FCI 			0x10000000

/* local variables */

static NSAutoreleasePool *arpool;   // Objective-C autorelease pool
static id adapter;					// Adapter object

char* plD_DEVICE_INFO_aqt = "aqt:AquaTerm (Mac OS X):1:aqt:50:aqt";

static int currentPlot = 0;
static int maxWindows = 30;
static int windowXSize = 0;
static int windowYSize = 0;

/* font stuff */

/*
	AquaTerm font look-up table

	The table is initialized with lowest commong denominator truetype 
	fonts that (I hope) most Macs will have.
*/

#define AQT_N_FontLookup 30
static FCI_to_FontName_Table AQT_FontLookup[AQT_N_FontLookup] = {
	{PL_FCI_MARK | 0x000, "Helvetica"},
	{PL_FCI_MARK | 0x001, "Times-Roman"},
	{PL_FCI_MARK | 0x002, "Courier"},
	{PL_FCI_MARK | 0x003, "Times-Roman"},
	{PL_FCI_MARK | 0x004, "LucidaGrande Regular"},
	{PL_FCI_MARK | 0x010, "Helvetica-Oblique"},
	{PL_FCI_MARK | 0x011, "Times-Italic"},
	{PL_FCI_MARK | 0x012, "Courier-Oblique"},
	{PL_FCI_MARK | 0x013, "Times-Italic"},
	{PL_FCI_MARK | 0x014, "LucidaGrande Regular"},
	{PL_FCI_MARK | 0x020, "Helvetica-Oblique"},
	{PL_FCI_MARK | 0x021, "Times-Italic"},
	{PL_FCI_MARK | 0x022, "Courier-Oblique"},
	{PL_FCI_MARK | 0x023, "Times-Italic"},
	{PL_FCI_MARK | 0x024, "LucidaGrande Regular"},
	{PL_FCI_MARK | 0x100, "Helvetica-Bold"},
	{PL_FCI_MARK | 0x101, "Times-Bold"},
	{PL_FCI_MARK | 0x102, "Courier-Bold"},
	{PL_FCI_MARK | 0x103, "Times-Bold"},
	{PL_FCI_MARK | 0x104, "LucidaGrande Regular"},
	{PL_FCI_MARK | 0x110, "Helvetica-BoldOblique"},
	{PL_FCI_MARK | 0x111, "Times-BoldItalic"},
	{PL_FCI_MARK | 0x112, "Courier-BoldOblique"},
	{PL_FCI_MARK | 0x113, "Times-BoldItalic"},
	{PL_FCI_MARK | 0x114, "LucidaGrande Regular"},
	{PL_FCI_MARK | 0x120, "Helvetica-BoldOblique"},
	{PL_FCI_MARK | 0x121, "Times-BoldItalic"},
	{PL_FCI_MARK | 0x122, "Courier-BoldOblique"},
	{PL_FCI_MARK | 0x123, "Times-BoldItalic"},
	{PL_FCI_MARK | 0x124, "LucidaGrande Regular"}
};

/*
	AquaTerm font environment variables

	When the driver is initialized it will check to see if 
	the user has opted to overide one of the above fonts by 
	setting one of the environment variables below.

	This list must be in the same order with the same number of
	elements as the above list
	
	These are the same environment variable names as would be used
	on a linux system, but they have a slightly different meaning.
	Since AquaTerm will find the font for us (if it can) given
	just the font name, you should only set the environment
	variable to the font name. You don't need to provide
	a path. If you installed the font using Font Book, AquaTerm
	should not have any trouble finding it.
	
	FIXME: Would it be better to use different environment variable 
		names then plfreetype.c? If not, then it probably isn't
		ideal to have two different copies of the same list of
		environment variable names.
*/

const char *aqt_font_env_names[AQT_N_FontLookup] = {
	"PLPLOT_FREETYPE_SANS_FONT",
	"PLPLOT_FREETYPE_SERIF_FONT",
	"PLPLOT_FREETYPE_MONO_FONT",
	"PLPLOT_FREETYPE_SCRIPT_FONT",
	"PLPLOT_FREETYPE_SYMBOL_FONT",
	"PLPLOT_FREETYPE_SANS_ITALIC_FONT",
	"PLPLOT_FREETYPE_SERIF_ITALIC_FONT",
	"PLPLOT_FREETYPE_MONO_ITALIC_FONT",
	"PLPLOT_FREETYPE_SCRIPT_ITALIC_FONT",
	"PLPLOT_FREETYPE_SYMBOL_ITALIC_FONT",
	"PLPLOT_FREETYPE_SANS_OBLIQUE_FONT",
	"PLPLOT_FREETYPE_SERIF_OBLIQUE_FONT",
	"PLPLOT_FREETYPE_MONO_OBLIQUE_FONT",
	"PLPLOT_FREETYPE_SCRIPT_OBLIQUE_FONT",
	"PLPLOT_FREETYPE_SYMBOL_OBLIQUE_FONT",
	"PLPLOT_FREETYPE_SANS_BOLD_FONT",
	"PLPLOT_FREETYPE_SERIF_BOLD_FONT",
	"PLPLOT_FREETYPE_MONO_BOLD_FONT",
	"PLPLOT_FREETYPE_SCRIPT_BOLD_FONT",
	"PLPLOT_FREETYPE_SYMBOL_BOLD_FONT",
	"PLPLOT_FREETYPE_SANS_BOLD_ITALIC_FONT",
	"PLPLOT_FREETYPE_SERIF_BOLD_ITALIC_FONT",
	"PLPLOT_FREETYPE_MONO_BOLD_ITALIC_FONT",
	"PLPLOT_FREETYPE_SCRIPT_BOLD_ITALIC_FONT",
	"PLPLOT_FREETYPE_SYMBOL_BOLD_ITALIC_FONT",
	"PLPLOT_FREETYPE_SANS_BOLD_OBLIQUE_FONT",
	"PLPLOT_FREETYPE_SERIF_BOLD_OBLIQUE_FONT",
	"PLPLOT_FREETYPE_MONO_BOLD_OBLIQUE_FONT",
	"PLPLOT_FREETYPE_SCRIPT_BOLD_OBLIQUE_FONT",
	"PLPLOT_FREETYPE_SYMBOL_BOLD_OBLIQUE_FONT"
};

/* Debugging extras */

static inline void NOOP_(id x, ...) {;}

#ifdef LOGGING
#define LOG  NSLog
#else
#define LOG  NOOP_
#endif  /* LOGGING */

//-----------------------------------------------
// function declarations
//-----------------------------------------------

/* helper functions */

void get_cursor(PLGraphicsIn *);
void proc_str (PLStream *, EscText *);
NSMutableAttributedString * create_string(const PLUNICODE *, int, PLFLT);
void set_font_and_size(NSMutableAttributedString *, PLUNICODE, PLFLT, int);
char * UCS4_to_UTF8(const PLUNICODE);
void check_font_environment_variables(void);

/* PLplot interface functions */

void plD_dispatch_init_aqt      (PLDispatchTable *pdt);
void plD_init_aqt               (PLStream *);
void plD_line_aqt               (PLStream *, short, short, short, short);
void plD_polyline_aqt   		(PLStream *, short *, short *, PLINT);
void plD_eop_aqt                (PLStream *);
void plD_bop_aqt                (PLStream *);
void plD_tidy_aqt               (PLStream *);
void plD_state_aqt              (PLStream *, PLINT);
void plD_esc_aqt                (PLStream *, PLINT, void *);

//---------------------------------------------------------------------
//   dispatch_init_init()
//
//   Initialize device dispatch table
//----------------------------------------------------------------------

void plD_dispatch_init_aqt( PLDispatchTable *pdt )
{
   pdt->pl_MenuStr  = "AquaTerm - Mac OS X";
   pdt->pl_DevName  = "aqt";
   pdt->pl_type     = plDevType_Interactive;
   pdt->pl_seq      = 1;
   pdt->pl_init     = (plD_init_fp)     plD_init_aqt;
   pdt->pl_line     = (plD_line_fp)     plD_line_aqt;
   pdt->pl_polyline = (plD_polyline_fp) plD_polyline_aqt;
   pdt->pl_eop      = (plD_eop_fp)      plD_eop_aqt;
   pdt->pl_bop      = (plD_bop_fp)      plD_bop_aqt;
   pdt->pl_tidy     = (plD_tidy_fp)     plD_tidy_aqt;
   pdt->pl_state    = (plD_state_fp)    plD_state_aqt;
   pdt->pl_esc      = (plD_esc_fp)      plD_esc_aqt;
}

//---------------------------------------------------------------------
//   aqt_init()
//
//   Initialize device
//----------------------------------------------------------------------

void plD_init_aqt(PLStream *pls)
{
	
   if (arpool == NULL)   /* Make sure we don't leak mem by allocating every time */
   {
      arpool = [[NSAutoreleasePool alloc] init];
      adapter = [[AQTAdapter alloc] init];
   }
   [adapter setBackgroundColorRed:0.5 green:0.5 blue:0.5];

   pls->termin = 1;			/* interactive device */
   pls->color = 1;			/* supports color */
   pls->width = 1;
   pls->verbose = 1;
   pls->bytecnt = 0;
   pls->debug = 1;
   pls->dev_text = 1;		/* handles text */
   pls->dev_unicode = 1; 	/* wants text as unicode */
   pls->page = 0;
   pls->dev_fill0 = 1;		/* supports hardware solid fills */
   pls->dev_fill1 = 1;

   pls->graphx = GRAPHICS_MODE;

   if (!pls->colorset)
      pls->color = 1; 

   /* Set up device parameters */
   
   plP_setpxl(DPI/25.4/SCALE, DPI/25.4/SCALE);           /* Pixels/mm. */
   
   /* Set the bounds for plotting.  default is AQT_Default_X x AQT_Default_Y unless otherwise specified. */
   
   if (pls->xlength <= 0 || pls->ylength <= 0){
      windowXSize = AQT_Default_X;
      windowYSize = AQT_Default_Y;
      plP_setphy((PLINT) 0, (PLINT) (AQT_Default_X/SCALE), (PLINT) 0, (PLINT) (AQT_Default_Y/SCALE));
   } else {
      windowXSize = pls->xlength;
      windowYSize = pls->ylength;
      plP_setphy((PLINT) 0, (PLINT) (pls->xlength/SCALE), (PLINT) 0, (PLINT) (pls->ylength/SCALE));
   }   

	/* check font environment variables & update font table as necessary */
	
	check_font_environment_variables();
}

//----------------------------------------------------------------------
//   aqt_bop()
//
//   Set up for the next page.
//----------------------------------------------------------------------

void plD_bop_aqt(PLStream *pls)
{
   currentPlot = currentPlot>=maxWindows?0:currentPlot;
   [adapter openPlotWithIndex:currentPlot++];
   [adapter setPlotSize:NSMakeSize(windowXSize, windowYSize)];
   [adapter setLinewidth:1.0];
   [adapter setColorRed:(float)(pls->curcolor.r/255.)
   				  green:(float)(pls->curcolor.g/255.)
				   blue:(float)(pls->curcolor.b/255.)];

   pls->page++;
}

//---------------------------------------------------------------------
//   aqt_line()
//
//   Draw a line in the current color from (x1,y1) to (x2,y2).
//----------------------------------------------------------------------

void plD_line_aqt(PLStream *pls, short x1a, short y1a, short x2a, short y2a)
{
   [adapter moveToPoint:NSMakePoint((float)x1a*SCALE, (float)y1a*SCALE)];
   [adapter addLineToPoint:NSMakePoint((float)x2a*SCALE, (float)y2a*SCALE)];
}

//---------------------------------------------------------------------
//  aqt_polyline()
//
// Draw a polyline in the current color.
//---------------------------------------------------------------------

void plD_polyline_aqt(PLStream *pls, short *xa, short *ya, PLINT npts)
{
   int i;

   for (i = 0; i < npts - 1; i++)
      plD_line_aqt(pls, xa[i], ya[i], xa[i + 1], ya[i + 1]);
}

//---------------------------------------------------------------------
//   aqt_eop()
//
//   End of page
//---------------------------------------------------------------------

void plD_eop_aqt(PLStream *pls)
{
   [adapter renderPlot];
}

//---------------------------------------------------------------------
// aqt_tidy()
//
// Close graphics file or otherwise clean up.
//---------------------------------------------------------------------

void plD_tidy_aqt(PLStream *pls)
{
   [adapter closePlot];
}
//---------------------------------------------------------------------
//  plD_state_aqt()
//
//  Handle change in PLStream state (color, pen width, fill attribute, etc).
//---------------------------------------------------------------------

void plD_state_aqt(PLStream *pls, PLINT op)
{
   int i;
   float r,g,b;

   switch (op)
   {
      case PLSTATE_WIDTH:
         [adapter setLinewidth:(float)pls->width];
         break;

      case PLSTATE_COLOR0:	// this seems to work, but that isn't to say that it is done right...
         [adapter setBackgroundColorRed:(float)(plsc->cmap0[0].r/255.0) 
	                              green:(float)(plsc->cmap0[0].g/255.0)
	                               blue:(float)(plsc->cmap0[0].b/255.0)];
      case PLSTATE_COLOR1:
      case PLSTATE_FILL:
		 [adapter setColorRed:(float)(pls->curcolor.r/255.)
   		 		   	    green:(float)(pls->curcolor.g/255.)
		 		         blue:(float)(pls->curcolor.b/255.)];
         break;

      case PLSTATE_CMAP0:
         break;

      case PLSTATE_CMAP1:
         break;
   }
}

//---------------------------------------------------------------------
// aqt_esc()
//
// Escape function.
//
// Functions:
//
//      PLESC_EH        Handle pending events
//      PLESC_EXPOSE    Force an expose
//      PLESC_FILL      Fill polygon
//      PLESC_FLUSH     Flush X event buffer
//      PLESC_GETC      Get coordinates upon mouse click
//      PLESC_REDRAW    Force a redraw
//      PLESC_RESIZE    Force a resize
//---------------------------------------------------------------------
//
void plD_esc_aqt(PLStream *pls, PLINT op, void *ptr)
{
   int     i;
   switch (op)
   {
      case PLESC_EXPOSE:              // handle window expose
         break;
      case PLESC_RESIZE:              // handle window resize
         break;
      case PLESC_REDRAW:              // handle window redraw
         break;
      case PLESC_TEXT:                // switch to text screen
         break;
      case PLESC_GRAPH:               // switch to graphics screen
         break;
      case PLESC_FILL:                // fill polygon	 
         [adapter moveToVertexPoint:NSMakePoint(pls->dev_x[0]*SCALE, pls->dev_y[0]*SCALE)];
         for (i = 1; i < pls->dev_npts ; i++)
         {
            [adapter addEdgeToVertexPoint:NSMakePoint(pls->dev_x[i]*SCALE, pls->dev_y[i]*SCALE)];
         };
         break;
      case PLESC_DI:                  // handle DI command
         break;
      case PLESC_FLUSH:               // flush output
         break;
      case PLESC_EH:                  // handle Window events
         break;
      case PLESC_GETC:                // get cursor position
         [adapter renderPlot]; // needed to give the user something to click on
         get_cursor((PLGraphicsIn*)ptr);
         break;
      case PLESC_SWIN:                // set window parameters
         break;
      case PLESC_HAS_TEXT:
         proc_str(pls, (EscText *)ptr);
         break;
   }
}

//---------------------------------------------------------------------
// get_cursor()
//
// returns the location of the next mouse click
//---------------------------------------------------------------------

void get_cursor(PLGraphicsIn *gin){
	int scanned, x, y, button;
    NSString *temp;

	temp = [adapter waitNextEvent];
	scanned = sscanf([temp cString],"1:{%d, %d}:%d", &x, &y, &button);

	if(scanned == 3){	// check that we did actually get a reasonable event string
		gin->button = button;
		gin->pX = x;
		gin->pY = y;
		gin->dX = (PLFLT)x;
		gin->dY = (PLFLT)y;
	} else {	// just return zeroes if we did not
		gin->button = 0;
		gin->pX = 0;
		gin->pY = 0;
		gin->dX = 0.0;
		gin->dY = 0.0;	
	}
}

//---------------------------------------------------------------------
// proc_str()
//
// Processes strings for display. The actual parsing of the unicode
// string is handled by the sub-routine create_string.
//---------------------------------------------------------------------

void proc_str (PLStream *pls, EscText *args)
{
	PLFLT   	*t = args->xform;
	PLFLT   	a1, ft_ht, angle, shear;
	PLINT   	clxmin, clxmax, clymin, clymax;
	int     	i, jst, ref;
	NSMutableAttributedString *str;

	/* check that we got unicode, warning message and return if not */
	
	if(args->unicode_array_len == 0){
		printf("Non unicode string passed to AquaTerm driver, ignoring\n");
		return;
	}
	
	/* check that unicode string isn't longer then the max we allow */
	
	if(args->unicode_array_len >= MAX_STRING_LEN){
		printf("Sorry, the AquaTerm driver only handles strings of length < %d\n", MAX_STRING_LEN);
		return;
	}

	/* set the font height - the 1.2 factor was trial and error */

	ft_ht = 1.2 * pls->chrht * DPI/25.4; 	/* ft_ht in points. ht is in mm */

   	/* given transform, calculate rotation angle & shear angle */
	
	a1 = acos(t[0]) * 180. / PI;
	if (t[2] > 0.)
		angle = a1;
	else
		angle = 360. - a1;

	shear = 90.0 - acos(t[0]*t[1] + t[2]*t[3]) * 180.0 / PI;

	/* text justification, AquaTerm only supports 3 options, so we round appropriately */
	
	if (args->just < 0.33)
		jst = AQTAlignLeft;                             /* left */
	else if (args->just > 0.66)
		jst = AQTAlignRight;                            /* right */
	else
		jst = AQTAlignCenter;                           /* center */
	
	/* set the baseline of the string */
	
	if (args->base == 2)
		ref = AQTAlignTop;
	else if (args->base == 1)
		ref = AQTAlignBaseline;
	else
		ref = AQTAlignMiddle;

	/* create an appropriately formatted, etc... unicode string */
	
	str = create_string(args->unicode_array, args->unicode_array_len, ft_ht);

	/* display the string */
	
	[adapter setColorRed:(float)(pls->curcolor.r/255.)
                   green:(float)(pls->curcolor.g/255.)
                    blue:(float)(pls->curcolor.b/255.)];
    [adapter addLabel:str 
              atPoint:NSMakePoint((float)args->x*SCALE, (float)args->y*SCALE)
                angle:angle 
           shearAngle:shear 
                align:(jst | ref)];

    [str release];
}

//---------------------------------------------------------------------
// create_string()
//
// create a NSMutableAttributedString from the plplot ucs4 string
//
// assumptions :
//	1. font changes are unicode >= FCI
//  2. we'll never have to deal with a string longer then MAX_STRING_LEN characters
// 	3. <esc><esc> means we desired <esc> as a character & not actually as <esc>
//  4. there are no two character <esc> sequences... i.e. <esc>fn is now covered by fci
//
//---------------------------------------------------------------------

NSMutableAttributedString  * create_string(const PLUNICODE *ucs4, int ucs4_len, PLFLT font_height)
{
	PLUNICODE fci;
	char plplot_esc;
	int i;
	int cur_loc;
	int utf8_len;
	int updown;
	char dummy[MAX_STRING_LEN+1];
	char *font;
	unsigned char *utf8;
	NSMutableAttributedString *str;

	updown = 0;
	
	/* initialize the attributed string */

	for(i=0;i<MAX_STRING_LEN;i++) dummy[i] = 'i';
	dummy[MAX_STRING_LEN] = '\0';
	str = [[NSMutableAttributedString alloc] initWithString:[NSString stringWithCString:dummy]];

	/* get plplot escape character & current font */
	
	plgesc(&plplot_esc);
	plgfci(&fci);
	
	/* set the font for the string based on the current font & size */

	set_font_and_size(str, fci, font_height, 0);
	
	/* parse plplot ucs4 string */

	cur_loc = 0;
	i = 0;
	while (i < ucs4_len){
		if (ucs4[i] < FCI){	/* not a font change */
			if (ucs4[i] != (PLUNICODE)plplot_esc) {		/* a character to display */
				utf8 = UCS4_to_UTF8(ucs4[i]);
				[str replaceCharactersInRange:NSMakeRange(cur_loc, 1)
								   withString:[NSString stringWithUTF8String:utf8]];
				i++;
				cur_loc++;
				continue;
			}
			i++;
			if (ucs4[i] == (PLUNICODE)plplot_esc){
				utf8 = UCS4_to_UTF8(ucs4[i]);
				[str replaceCharactersInRange:NSMakeRange(cur_loc, 1)
								   withString:[NSString stringWithUTF8String:utf8]];
				i++;
				cur_loc++;
				continue;
			}
    		else {
    			if(ucs4[i] == (PLUNICODE)'f'){	// font change
    				i++;
    				printf("hmm, unicode string apparently not following fci convention...\n");
    			}
    			if(ucs4[i] == (PLUNICODE)'d'){	// Subscript
    				updown--;
    				[str addAttribute:@"NSSuperScript"
                				value:[NSNumber numberWithInt:updown]
                				range:NSMakeRange(cur_loc, (MAX_STRING_LEN - cur_loc))];
    			}
    			if(ucs4[i] == (PLUNICODE)'u'){	// Superscript
    				updown++;
    				[str addAttribute:@"NSSuperScript"
                				value:[NSNumber numberWithInt:updown]
                				range:NSMakeRange(cur_loc, (MAX_STRING_LEN - cur_loc))];
                }
           		i++;
    		}
    	}
    	else {	/* a font change */
    		set_font_and_size(str, ucs4[i], font_height, cur_loc);
        	i++;
    	}
	}
	
	/* trim string to appropriate final length */
	
	[str deleteCharactersInRange:NSMakeRange(cur_loc, (MAX_STRING_LEN - cur_loc))];
	
	return str;
}

//---------------------------------------------------------------------
// set_font_and_size
//
// set the font & size of a attributable string object
//---------------------------------------------------------------------

void set_font_and_size(NSMutableAttributedString * str, PLUNICODE fci, PLFLT font_height, int cur_loc)
{
	char *font;

	font = plP_FCI2FontName(fci, AQT_FontLookup, AQT_N_FontLookup);
	
	// check whether that font exists & if not, use standard font instread
	
	if(font == NULL){
		printf("could not find font given by fci = 0x%x\n", fci);
		font = "Helvetica";
	}
	// font = "FreeSerif";	// force the font for debugging purposes
	// printf("Font at %d is : %s\n", cur_loc, font);

    [str addAttribute:@"AQTFontname"
                value:[NSString stringWithCString:font]
                range:NSMakeRange(cur_loc, (MAX_STRING_LEN - cur_loc))];
    [str addAttribute:@"AQTFontsize"
                value:[NSNumber numberWithFloat:font_height]
                range:NSMakeRange(cur_loc, (MAX_STRING_LEN - cur_loc))];
}

//---------------------------------------------------------------------
// UCS4_to_UTF8()
//
// convert PLplot UCS4 unicode character to UTF8 character for Mac
//---------------------------------------------------------------------

char * UCS4_to_UTF8(const PLUNICODE ucs4)
{
	int i,len;
	static unsigned char utf8[5];

	if (ucs4 < 0x80){
		utf8[0] = ucs4;
		utf8[1] = '\0';
		len = 1;
	}
	else if (ucs4 < 0x800) {
		utf8[0] = (0xC0 | ucs4 >> 6);
		utf8[1] = (0x80 | ucs4 & 0x3F);
		utf8[2] = '\0';
		len = 2;
	}
	else if (ucs4 < 0x10000) {
		utf8[0] = (0xE0 | ucs4 >> 12);
		utf8[1] = (0x80 | ucs4 >> 6 & 0x3F);
		utf8[2] = (0x80 | ucs4 & 0x3F);
		utf8[3] = '\0';
		len = 3;
	}
	else if (ucs4 < 0x200000) {
		utf8[0] = (0xF0 | ucs4 >> 18);
		utf8[1] = (0x80 | ucs4 >> 12 & 0x3F);
		utf8[2] = (0x80 | ucs4 >> 6 & 0x3F);
		utf8[3] = (0x80 | ucs4 & 0x3F);
		utf8[4] = '\0';
		len = 4;
	}

	// for debugging

/*	
	printf("ucs4 : %d\n", ucs4);
	printf("as utf8 : (%d) 0x", len);
	for(i=0;i<len;i++) printf("%x", utf8[i]);
	printf("\n");
*/

	return utf8;
}

//---------------------------------------------------------------------
// check_font_environment_variables
//
// Checks to see if any font environment variables are defined.
// If a font environment variable is defined, then the appropriate 
// element of the default font table is replaced with the font name
// string specified by the environment variable.
//---------------------------------------------------------------------


void check_font_environment_variables(void){
	int i;
	char *new_font;
	
	for(i=0;i<AQT_N_FontLookup;i++){
		if ((new_font = getenv(aqt_font_env_names[i])) != NULL){
			// printf("new font : %s\n", new_font);
			AQT_FontLookup[i].pfont = new_font;
		}
	}
}
