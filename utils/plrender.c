/* $Id$
   $Log$
   Revision 1.9  1993/02/25 18:33:09  mjl
   Fixed an inconsistency in reading the metafile page headers.

 * Revision 1.8  1993/02/23  05:54:25  mjl
 * A couple of minor documentation and code changes.
 *
 * Revision 1.7  1993/02/23  05:35:47  mjl
 * Converted to new plplot command-line handling functions, resulting in
 * a considerable reduction in the amount of actual code.  Miscellaneous bugs in
 * file seeking fixed.  Extraneous page printed on a -p command eliminated.
 * Many other small improvements.
 *
 * Revision 1.6  1993/01/23  06:16:08  mjl
 * Formatting changes only to pltek.  plrender changes include: support for
 * polylines (even converts connected lines while reading into polylines for
 * better response), new color model support, event handler support.  New
 * events recognized allow seeking to arbitrary locations in the file (absolute
 * or relative), and backward.  Some old capabilities (no longer useful)
 * eliminated.
 *
 * Revision 1.5  1992/11/07  08:08:55  mjl
 * Fixed orientation code, previously it rotated plot in the wrong direction.
 * It now supports 3 different rotations (-ori 1, -ori 2, -ori 3).
 * Also eliminated some redundant code.
 *
 * Revision 1.4  1992/10/29  15:56:16  mjl
 * Gave plrender an ID tag.
 *
 * Revision 1.3  1992/10/12  17:12:58  mjl
 * Rearranged order of header file inclusion.
 * #include "plplot.h" must come first!!
 *
 * Revision 1.2  1992/09/29  04:46:46  furnish
 * Massive clean up effort to remove support for garbage compilers (K&R).
 *
 * Revision 1.1  1992/05/20  21:35:59  furnish
 * Initial checkin of the whole PLPLOT project.
 *
*/

/*
    plrender.c

    Copyright 1991, 1992, 1993
    Geoffrey Furnish
    Maurice LeBrun

    This software may be freely copied, modified and redistributed without
    fee provided that this copyright notice is preserved intact on all
    copies and modified copies.

    There is no warranty or other guarantee of fitness of this software.
    It is provided solely "as is". The author(s) disclaim(s) all
    responsibility and liability with respect to this software's usage or
    its effect upon hardware or computer systems.

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

    This file contains the code to render a PLPLOT metafile, written by
    the metafile driver, plmeta.c.
*/

char ident[] = "@(#) $Id$";

#include "plplot.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "plevent.h"
#include "metadefs.h"
#include "pdf.h"

static char *program_name = "plrender";
static char *program_version = PLMETA_VERSION;

/* Static function prototypes. */
/* INDENT OFF */
/* These handle the command loop */

static void	process_next	(U_CHAR c);
static void	plr_init	(U_CHAR c);
static void	plr_line	(U_CHAR c);
static void	plr_clr		(U_CHAR c);
static void	plr_page	(U_CHAR c);
static void	plr_color	(U_CHAR c);
static void	plr_switch	(U_CHAR c);
static void	plr_width	(U_CHAR c);
static void	plr_esc		(U_CHAR c);
static void	plresc_rgb	(void);
static void	plresc_ancol	(void);

/* Support functions */

static U_CHAR	getcommand	(void);
static void	ungetcommand	(U_CHAR);
static void	get_ncoords	(PLFLT *x, PLFLT *y, PLINT n);
static void	NextFamilyFile	(U_CHAR *);
static void	ReadPageHeader	(void);
static void	plr_KeyEH	(PLKey *, void *, int *);
static void	SeekToPage	(long);
static void	check_alignment (FILE *);

/* Initialization functions */

static void	Init		(int, char **);
static void	OpenMetaFile	(int *, char **);
static int	ReadFileHeader	(void);
static void	Help		(void);
static void 	Usage		(char *);

/* Option handlers */

static int HandleOption_h	(char *, char *);
static int HandleOption_v	(char *, char *);
static int HandleOption_i	(char *, char *);
static int HandleOption_f	(char *, char *);
static int HandleOption_b	(char *, char *);
static int HandleOption_e	(char *, char *);
static int HandleOption_p	(char *, char *);
static int HandleOption_a	(char *, char *);
static int HandleOption_mar	(char *, char *);
static int HandleOption_ori	(char *, char *);
static int HandleOption_jx	(char *, char *);
static int HandleOption_jy	(char *, char *);

/* Global variables */

/* Page info */

static PLINT	page_begin=1;	/* Where to start plotting */
static PLINT	page_end=-1;	/* Where to stop (0 to disable) */
static PLINT	curpage=1;	/* Current page number */
static PLINT	cursub=0;	/* Current subpage */
static PLINT	nsubx;		/* subpages in x */
static PLINT	nsuby;		/* subpages in y */
static PLINT	target_page;	/* Page we are seeking to */
static U_LONG	prevpage_loc;	/* Byte position of previous page header */
static U_LONG	curpage_loc;	/* Byte position of current page header */
static U_LONG	nextpage_loc;	/* Byte position of next page header */
static int	no_pagelinks;	/* Set if metafile doesn't have page links */
static int	end_of_page;	/* Set when we're at the end of a page */

/* File info */

static FILE	*MetaFile;
static char	BaseName[70] = "", FamilyName[80] = "", FileName[90] = "";
static PLINT	is_family, member=1, is_filter;
static char	mf_magic[40], mf_version[40];

/* Dummy vars for reading stuff that is to be thrown away */

static U_CHAR	dum_uchar;
static U_SHORT	dum_ushort;
static char	dum_char80[80];
static float	dum_float;

/* Plot parameters */

static int	orient;
static float	aspect;
static int	orientset, aspectset;
static PLFLT	mar=0.0, jx=0.5, jy=0.5;
static U_SHORT	lpbpxmi, lpbpxma, lpbpymi, lpbpyma;

/* Plot dimensions */

static U_SHORT	xmin = 0;
static U_SHORT	xmax = PLMETA_X_OLD;
static U_SHORT	ymin = 0;
static U_SHORT	ymax = PLMETA_Y_OLD;
static PLINT	xlen, ylen;

static float	pxlx = PIXEL_RES_X_OLD;
static float	pxly = PIXEL_RES_Y_OLD;

static PLFLT	dev_xpmm, dev_ypmm;
static PLINT	dev_xmin, dev_xmax, dev_ymin, dev_ymax;
static PLFLT	vpxmin, vpxmax, vpxlen, vpymin, vpymax, vpylen;

/* Miscellaneous */

static U_CHAR 	c_old, c1;
static U_SHORT	npts;
static int	direction_flag, isanum;
static char	num_buffer[20];
static PLFLT	x[PL_MAXPOLYLINE], y[PL_MAXPOLYLINE];

/* Exit codes */

#define	EX_SUCCESS	0		/* success! */
#define	EX_ARGSBAD	1		/* invalid args */
#define	EX_BADFILE	2		/* invalid filename or contents */

/*----------------------------------------------------------------------*\
* Options data structure definition.
\*----------------------------------------------------------------------*/

static PLOptionTable option_table[] = {
{
    "h",			/* Help */
    HandleOption_h,
    0,
    "-h",
    "Print out this message" },
{
    "v",			/* Version */
    HandleOption_v,
    0,
    "-v",
    "Print out the plrender version number" },
{
    "i",			/* Input file */
    HandleOption_i,
    PL_PARSE_ARG,
    "-i name",
    "Input filename" },
{
    "f",			/* Filter option */
    HandleOption_f,
    PL_PARSE_ARG,
    "-f",
    "Filter option -- equivalent to \"-i - -o -\"" },
{
    "b",			/* Beginning page number */
    HandleOption_b,
    PL_PARSE_ARG,
    "-b number",
    "Beginning page number" },
{
    "e",			/* End page number */
    HandleOption_e,
    PL_PARSE_ARG,
    "-e number",
    "End page number" },
{
    "p",			/* Specified page only */
    HandleOption_p,
    PL_PARSE_ARG,
    "-p page",
    "Plot given page only" },
{
    "a",			/* Aspect ratio */
    HandleOption_a,
    PL_PARSE_ARG,
    "-a aspect",
    "Plot aspect ratio" },
{
    "mar",			/* Margin */
    HandleOption_mar,
    PL_PARSE_ARG,
    "-mar margin",
    "Total fraction of page to reserve for margins" },
{
    "ori",			/* Orientation */
    HandleOption_ori,
    PL_PARSE_ARG,
    "-ori orient",
    "Plot orientation (0,2=landscape, 1,3=portrait)" },
{
    "jx",			/* Justification in x */
    HandleOption_jx,
    PL_PARSE_ARG,
    "-jx number",
    "Justification of plot on page in x (0.0 to 1.0)" },
{
    "jy",			/* Justification in y */
    HandleOption_jy,
    PL_PARSE_ARG,
    "-jy number",
    "Justification of plot on page in y (0.0 to 1.0)" },
{
    NULL,
    NULL,
    0,
    NULL,
    NULL }
};

static char *notes[] = {
"All parameters must be white-space delimited, and plrender options override",
"any plplot options of the same name.  If the \"-i\" flag is omitted the",
"filename parameter must come last.  Specifying \"-\" for the input or output",
"filename means use stdin or stdout, respectively.  Not all options valid",
"with all drivers.  Please see the man pages for more detail.",
NULL};

/* INDENT ON */
/*----------------------------------------------------------------------*\
* main()
*
* plrender -- render a PLPLOT metafile.
\*----------------------------------------------------------------------*/

main(int argc, char *argv[])
{
    static U_CHAR c;

/* Initialize */

    Init(argc, argv);

/*
* Read & process metafile commands.
* If familying is turned on, the end of one member file is just treated as
* a page break assuming the next member file exists.
*/
    for (;;) {
	c_old = c;
	c = getcommand();

	if (c == CLOSE) {
	    if (is_family)
		NextFamilyFile(&c);
	    if (!is_family)
		break;
	}

	if ((c == PAGE || c == ADVANCE) && curpage == page_end)
	    break;

	process_next(c);
    }

/* Done */

    (void) fclose(MetaFile);
    if (strcmp(mf_version, "1993a") < 0) 
	grclr();

    grtidy();
    exit(EX_SUCCESS);
}

/*----------------------------------------------------------------------*\
* 			Process the command loop
\*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*\
* process_next()
*
* Process a command.
* Typically plrender issues commands to plplot much like an application
* program would.  This results in a more robust and flexible API.  On
* the other hand, it is sometimes necessary to directly call low-level
* plplot routines to achieve certain special effects.  This is probably
* tolerable if used sparingly (and points to areas where the plplot API
* needs improvement!).
\*----------------------------------------------------------------------*/

static void
process_next(U_CHAR c)
{
    switch ((int) c) {

      case INITIALIZE:
	plr_init(c);
	break;

      case LINE:
      case LINETO:
      case POLYLINE:
	plr_line(c);
	break;

      case CLEAR:
	plr_clr(c);
	break;

      case PAGE:
	plr_page(c);
	break;

      case ADVANCE:
	plr_clr(c);
	plr_page(c);
	break;

      case NEW_COLOR0:
      case NEW_COLOR1:
	plr_color(c);
	break;

      case SWITCH_TO_TEXT:
      case SWITCH_TO_GRAPH:
	plr_switch(c);
	break;

      case NEW_WIDTH:
	plr_width(c);
	break;

      case ESCAPE:
	plr_esc(c);
	break;

      default:
	plwarn("process_next: Unrecognized command\n");
    }
}

/*----------------------------------------------------------------------*\
* void plr_init()
*
* Handle initialization.
\*----------------------------------------------------------------------*/

static void
plr_init(U_CHAR c)
{
    float dev_aspect, ratio;

/* Register event handler */

    plsKeyEH(plr_KeyEH, NULL);

/* Start up plplot */

    plinit();
    gsub(&nsubx, &nsuby, &cursub);

/*
* Aspect ratio scaling
*
* If the user has not set the aspect ratio in the code via plsasp() it will
* be zero, and is set to the natural ratio of the metafile coordinate system.
* The aspect ratio set from the command line overrides this.
*/
    if (aspect == 0.0)
	aspect = ((float) (ymax - ymin) / pxly) /
	    ((float) (xmax - xmin) / pxlx);

    if (aspect <= 0.)
	fprintf(stderr,
		"Error in aspect ratio setting, aspect = %f\n", aspect);

    if (orient == 1 || orient == 3)
	aspect = 1.0 / aspect;

/* Aspect ratio of output device */

    gphy(&dev_xmin, &dev_xmax, &dev_ymin, &dev_ymax);
    gpixmm(&dev_xpmm, &dev_ypmm);

    dev_aspect = ((dev_ymax - dev_ymin) / dev_ypmm) /
	((dev_xmax - dev_xmin) / dev_xpmm);

    if (dev_aspect <= 0.)
	fprintf(stderr,
	      "Error in aspect ratio setting, dev_aspect = %f\n", dev_aspect);

    ratio = aspect / dev_aspect;

/* This is the default case; come back to here if things mess up */

    vpxlen = 1.0;
    vpylen = 1.0;
    vpxmin = 0.5 - vpxlen / 2.;
    vpymin = 0.5 - vpylen / 2.;
    vpxmax = vpxmin + vpxlen;
    vpymax = vpymin + vpylen;

    xlen = xmax - xmin;
    ylen = ymax - ymin;

/*
* If ratio < 1, you are requesting an aspect ratio (y/x) less than the
* natural aspect ratio of the output device, and you will need to reduce the
* length in y correspondingly.  Similarly, for ratio > 1, x must be reduced.
* 
* Note that unless the user overrides, the default is to *preserve* the
* aspect ratio of the original device (plmeta output file).  Thus you
* automatically get all physical coordinate plots to come out correctly.
*/
    if (ratio <= 0)
	fprintf(stderr, "Error in aspect ratio setting, ratio = %f\n", ratio);
    else if (ratio < 1)
	vpylen = ratio;
    else
	vpxlen = 1. / ratio;

    if (mar > 0.0 && mar < 1.0) {
	vpxlen *= (1.0 - mar);
	vpylen *= (1.0 - mar);
    }

    vpxmin = MAX(0., jx - vpxlen / 2.0);
    vpxmax = MIN(1., vpxmin + vpxlen);
    vpxmin = vpxmax - vpxlen;

    vpymin = MAX(0., jy - vpylen / 2.0);
    vpymax = MIN(1., vpymin + vpylen);
    vpymin = vpymax - vpylen;
}

/*----------------------------------------------------------------------*\
* plr_line()
*
* Draw a line or polyline.
\*----------------------------------------------------------------------*/

static void
plr_line(U_CHAR c)
{
    npts = 1;
    switch ((int) c) {

      case LINE:
	get_ncoords(x, y, 1);

      case LINETO:
	for (;;) {
	    get_ncoords(x + npts, y + npts, 1);

	    npts++;
	    if (npts == PL_MAXPOLYLINE)
		break;

	    c1 = getcommand();
	    if (c1 != LINETO) {
		ungetcommand(c1);
		break;
	    }
	}
	break;

      case POLYLINE:
	plm_rd(read_2bytes(MetaFile, &npts));
	get_ncoords(x, y, npts);
	break;
    }

    plline(npts, x, y);

    x[0] = x[npts - 1];
    y[0] = y[npts - 1];
}

/*----------------------------------------------------------------------*\
* get_ncoords()
*
* Read n coordinate vectors and properly orient.
\*----------------------------------------------------------------------*/

static void
get_ncoords(PLFLT *x, PLFLT *y, PLINT n)
{
    PLINT i;
    PLSHORT xs[PL_MAXPOLYLINE], ys[PL_MAXPOLYLINE];

    plm_rd(read_2nbytes(MetaFile, (U_SHORT *) xs, n));
    plm_rd(read_2nbytes(MetaFile, (U_SHORT *) ys, n));

    switch (orient) {

      case 3:
	for (i = 0; i < n; i++) {
	    x[i] = xmin + (ymax - ys[i]) * xlen / ylen;
	    y[i] = ymin + (xs[i] - xmin) * ylen / xlen;
	}
	return;

      case 1:
	for (i = 0; i < n; i++) {
	    x[i] = xmax - (ymax - ys[i]) * xlen / ylen;
	    y[i] = ymax - (xs[i] - xmin) * ylen / xlen;
	}
	return;

      case 2:
	for (i = 0; i < n; i++) {
	    x[i] = xmin + (xmax - xs[i]);
	    y[i] = ymin + (ymax - ys[i]);
	}
	return;

      default:
	for (i = 0; i < n; i++) {
	    x[i] = xs[i];
	    y[i] = ys[i];
	}
    }
}

/*----------------------------------------------------------------------*\
* plr_clr()
*
* Clear screen.
\*----------------------------------------------------------------------*/

static void
plr_clr(U_CHAR c)
{
    PLINT cursub, nsubx, nsuby;

    gsub(&nsubx, &nsuby, &cursub);
    if (cursub == nsubx * nsuby) {
	grclr();
	end_of_page = 1;
    }
}

/*----------------------------------------------------------------------*\
* plr_page()
*
* Page/subpage advancement.
\*----------------------------------------------------------------------*/

static void
plr_page(U_CHAR c)
{
    cursub++;
    if (cursub > nsubx * nsuby) {
	cursub = 1;
	curpage++;
    }
    ungetcommand(c);
    ReadPageHeader();

/* On startup, seek to starting page if other than 1 */

    if (curpage == 1) {
	if (page_begin > 1) {
	    if (no_pagelinks) 
		plwarn("plrender: Metafile does not support page seeks\n");
	    else 
		SeekToPage(page_begin);
	}
    }

/* Advance and setup the page or subpage */

    if (end_of_page) {
	grpage();
	end_of_page = 0;
    }

    ssub(nsubx, nsuby, cursub);
    setsub();

    plvpor(vpxmin, vpxmax, vpymin, vpymax);
    plwind((PLFLT) xmin, (PLFLT) xmax, (PLFLT) ymin, (PLFLT) ymax);
}

/*----------------------------------------------------------------------*\
* plr_color()
*
* Change color.
\*----------------------------------------------------------------------*/

static void
plr_color(U_CHAR c)
{
    U_SHORT icol;
    U_CHAR icol0, r, g, b;

    if (c == NEW_COLOR1) {
	plwarn("No support for cmap 1 yet");
	return;
    }
    if (strcmp(mf_version, "1993a") >= 0) {
	plm_rd(read_1byte(MetaFile, &icol0));

	if (icol0 == PL_RGB_COLOR) {
	    plm_rd(read_1byte(MetaFile, &r));
	    plm_rd(read_1byte(MetaFile, &g));
	    plm_rd(read_1byte(MetaFile, &b));
	    plrgb1(r, g, b);
	}
	else {
	    plcol(icol0);
	}
    }
    else {
	plm_rd(read_2bytes(MetaFile, &icol));
	plcol(icol);
    }
}

/*----------------------------------------------------------------------*\
* plr_switch()
*
* Switch between graphics/text modes.
\*----------------------------------------------------------------------*/

static void
plr_switch(U_CHAR c)
{
    if (c == SWITCH_TO_TEXT)
	pltext();

    else if (c == SWITCH_TO_GRAPH)
	plgra();
}

/*----------------------------------------------------------------------*\
* plr_width()
*
* Change pen width.
\*----------------------------------------------------------------------*/

static void
plr_width(U_CHAR c)
{
    U_SHORT width;

    plm_rd(read_2bytes(MetaFile, &width));

    plwid(width);
}

/*----------------------------------------------------------------------*\
* plr_esc()
*
* Handle all escape functions.
\*----------------------------------------------------------------------*/

static void
plr_esc(U_CHAR c)
{
    U_CHAR op;

    plm_rd(read_1byte(MetaFile, &op));
    switch (op) {

      case PL_SET_RGB:
	plresc_rgb();
	return;

      case PL_ALLOC_NCOL:
	plresc_ancol();
	return;

      case PL_SET_LPB:
	plm_rd(read_2bytes(MetaFile, &lpbpxmi));
	plm_rd(read_2bytes(MetaFile, &lpbpxma));
	plm_rd(read_2bytes(MetaFile, &lpbpymi));
	plm_rd(read_2bytes(MetaFile, &lpbpyma));
	return;
    }
}

/*----------------------------------------------------------------------*\
* plresc_rgb()
*
* Process escape function for RGB color selection.
* Note that RGB color selection is no longer handled this way by
* plplot but we must handle it here for old metafiles.
\*----------------------------------------------------------------------*/

static void
plresc_rgb(void)
{
    float red, green, blue;
    U_SHORT ired, igreen, iblue;

    plm_rd(read_2bytes(MetaFile, &ired));
    plm_rd(read_2bytes(MetaFile, &igreen));
    plm_rd(read_2bytes(MetaFile, &iblue));

    red = (double) ired / 65535.;
    green = (double) igreen / 65535.;
    blue = (double) iblue / 65535.;

    plrgb((PLFLT) red, (PLFLT) green, (PLFLT) blue);
}

/*----------------------------------------------------------------------*\
* plresc_ancol()
*
* Process escape function for named color table allocation.
* OBSOLETE -- just read the info and move on.
\*----------------------------------------------------------------------*/

static void
plresc_ancol(void)
{
    U_CHAR icolor;
    char name[80];

    plm_rd(read_1byte(MetaFile, &icolor));
    plm_rd(read_header(MetaFile, name));
}

/*----------------------------------------------------------------------*\
* 			Support routines
\*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*\
* NextFamilyFile()
*
* Start the next family if it exists.
\*----------------------------------------------------------------------*/

static void
NextFamilyFile(U_CHAR *c)
{
    (void) fclose(MetaFile);
    member++;
    (void) sprintf(FileName, "%s.%i", FamilyName, member);

    if ((MetaFile = fopen(FileName, BINARY_READ)) == NULL) {
	is_family = 0;
	return;
    }
    if (ReadFileHeader()) {
	is_family = 0;
	return;
    }

/*
* If the family file was created correctly, the first instruction in the
* file MUST be an INITIALIZE.  We throw this away and put a page advance in
* its place.
*/
    *c = getcommand();
    if (*c != INITIALIZE)
	fprintf(stderr,
		"Warning, first instruction in member file not an INIT\n");
    else
	*c = ADVANCE;
}

/*----------------------------------------------------------------------*\
* getcommand()
*
* Read & return the next command
\*----------------------------------------------------------------------*/

static U_CHAR
getcommand(void)
{
    int c;

    c = getc(MetaFile);
    if (c == EOF)
	plexit("getcommand: Unable to read from MetaFile");

    return (U_CHAR) c;
}

/*----------------------------------------------------------------------*\
* ungetcommand()
*
* Push back the last command read.
\*----------------------------------------------------------------------*/

static void
ungetcommand(U_CHAR c)
{
    if (ungetc(c, MetaFile) == EOF)
	plexit("ungetcommand: Unable to push back character");
}

/*----------------------------------------------------------------------*\
* plr_KeyEH()
*
* Keyboard event handler.  For mapping keyboard sequences to commands
* not usually supported by plplot, such as seeking around in the
* metafile.  Recognized commands:
*
* <Backspace>	|
* <Delete>	| Back page
* <Page up>	|
*
* +<num><CR>	Seek forward <num> pages.
* -<num><CR>	Seek backward <num> pages.
*
* <num><CR>	Seek to page <num>.
*
* Both <BS> and <DEL> are recognized for a back-page since the target
* system may use either as its erase key.  <Page Up> is present on some
* keyboards (different from keypad key).
*
* No user data is passed in this case, although a test case is
* illustrated. 
*
* Illegal input is ignored.
\*----------------------------------------------------------------------*/

static void
plr_KeyEH(PLKey *key, void *user_data, int *p_exit_eventloop)
{
    char *tst = (char *) user_data;
    int input_num;

/* TEST */

    if (tst != NULL) {
	pltext();
	printf("tst string: %s\n", tst);
	plgra();
    }

/* The rest deals with seeking only; so we return if it is disabled */

    if (no_pagelinks) 
	return;

/* Forward (+) or backward (-) */

    if (key->string[0] == '+')
	direction_flag = 1;

    else if (key->string[0] == '-')
	direction_flag = -1;

/* If a number, store into num_buffer */

    if (isdigit(key->string[0])) {
	isanum = TRUE;
	(void) strncat(num_buffer, key->string, (20-strlen(num_buffer)));
    }

/* 
* Seek to specified page, or page advance.
* Not done until user hits <return>.
* Need to check for both <LF> and <CR> for portability.
*/
    if (key->code == PLK_Return ||
	key->code == PLK_Linefeed ||
	key->code == PLK_Next)
    {
	if (isanum) {
	    input_num = atoi(num_buffer);
	    if (input_num == 0) {
		if (strcmp(num_buffer, "0"))
		    input_num = -1;
	    }
	    if (input_num >= 0) {
		if (direction_flag == 0)
		    target_page = input_num;
		else if (direction_flag > 0)
		    target_page = curpage + input_num;
		else if (direction_flag < 0)
		    target_page = curpage - input_num;

		SeekToPage(target_page);
	    }
	}
	*p_exit_eventloop = TRUE;
    }

/* Page backward */

    if (key->code == PLK_BackSpace ||
	key->code == PLK_Delete ||
	key->code == PLK_Prior) 
    {
	*p_exit_eventloop = TRUE;
	target_page = curpage - 1;
	SeekToPage(target_page);
    }

/* DEBUG */

#ifdef DEBUG
    pltext();
    printf("key->code = %x, target_page = %d, page = %d,\
           exit_eventloop = %d\n", key->code, target_page, curpage,
	   *p_exit_eventloop);
    plgra();
#endif

/* Cleanup */

    if (*p_exit_eventloop == TRUE) {
	num_buffer[0] = '\0';
	direction_flag = 0;
	isanum = 0;
    }
}

/*----------------------------------------------------------------------*\
* SeekToPage()
*
* Seek to 'target_page'.
\*----------------------------------------------------------------------*/

static void
SeekToPage(long target_page)
{
    long delta;

/*
* The first seek we have to handle specially, since we're actually at the end
* of a page, and haven't yet read the page header for the next page.  So the
* first thing to do is find the next page header in the direction we want to
* go, and make sure the subpage and page counters are updated appropriately.
*/
    if (target_page > curpage) {
	if (nextpage_loc == 0) 
	    return;
	fseek(MetaFile, nextpage_loc, 0);
	cursub++;
	if (cursub > nsubx * nsuby) {
	    cursub = 1;
	    curpage++;
	}
    }
    else {
	if (curpage_loc == 0) 
	    return;
	fseek(MetaFile, curpage_loc, 0);
    }
    delta = target_page - curpage;

/* Now loop until we arrive at the target page */

    while (delta != 0) {
	ReadPageHeader();
	if (delta > 0) {
	    if (nextpage_loc == 0) 
		break;
	    fseek(MetaFile, nextpage_loc, 0);
	    cursub++;
	    if (cursub > nsubx * nsuby) {
		cursub = 1;
		curpage++;
	    }
	}
	else {
	    if (prevpage_loc == 0) 
		break;
	    fseek(MetaFile, prevpage_loc, 0);
	    cursub--;
	    if (cursub < 1) {
		cursub = nsubx * nsuby;
		curpage--;
	    }
	}
	delta = target_page - curpage;
    }
}

/*----------------------------------------------------------------------*\
* ReadPageHeader()
*
* Reads the metafile, processing page header info.
* Assumes the file pointer is positioned immediately before a PAGE.
\*----------------------------------------------------------------------*/

static void
ReadPageHeader(void)
{
    U_CHAR c;
    U_SHORT page;

/* Read page header */

    curpage_loc = ftell(MetaFile);
    c = getcommand();
    if (c != PAGE && c != ADVANCE) 
	plexit("plrender: page advance expected; none found\n");

    if (strcmp(mf_version, "1992a") >= 0) {
	if (strcmp(mf_version, "1993a") >= 0) {
	    plm_rd(read_2bytes(MetaFile, &page));
	    plm_rd(read_4bytes(MetaFile, &prevpage_loc));
	    plm_rd(read_4bytes(MetaFile, &nextpage_loc));
	}
	else {
	    plm_rd(read_2bytes(MetaFile, &dum_ushort));
	    plm_rd(read_2bytes(MetaFile, &dum_ushort));
	}
    }
}

/*----------------------------------------------------------------------*\
* Init()
*
* Do initialization for main().
\*----------------------------------------------------------------------*/

static void
Init(int argc, char **argv)
{
    int i, mode, status;

/* First process plrender command line options */

    mode = PL_PARSE_PARTIAL;
    status = plParseOpts(&argc, argv, mode, option_table, Usage);

/*
* We want the plplot command line options to override their possible
* counterparts in the metafile header.  So we defer parsing the
* rest of the command line until after we process the metafile header.
*/
    OpenMetaFile(&argc, argv);
    if (ReadFileHeader())
	exit(EX_BADFILE);

/* Finally, give the rest of the command line to plplot to process. */

    status = plParseInternalOpts(&argc, argv, mode);

/* 
* At this point the only remaining argument in argv should be the program
* name (which should be first).  Anything else is an error.  Handle illegal
* flags first.
*/
    for (i = 1; i < argc; i++) {
	if ((argv)[i][0] == '-')
	    Usage(argv[i]);
    }

/* These must be extraneous file names */

    if (i > 1) {
	fprintf(stderr, "Only one filename argument accepted.\n");
	Usage("");
    }
}

/*----------------------------------------------------------------------*\
* OpenMetaFile()
*
* Attempts to open the named file.
* If the output file isn't already determined via the -i or -f flags, 
* we assume it's the last argument in argv[]. 
\*----------------------------------------------------------------------*/

static void
OpenMetaFile(int *p_argc, char **argv)
{
    if (is_filter)
	MetaFile = stdin;

    else if (!strcmp(FileName, "-"))
	MetaFile = stdin;

    else {
	if (*FileName == '\0') {
	    if (*p_argc > 1) {
		strncpy(FileName, argv[*p_argc-1], sizeof(FileName) - 1);
		FileName[sizeof(FileName) - 1] = '\0';
		argv[*p_argc] = NULL;
		(*p_argc)--;
	    }
	}
	if (*FileName == '\0') {
	    fprintf(stderr, "%s: No filename specified.\n", program_name);
	    Usage("");
	    exit(EX_ARGSBAD);
	}

/*
* Try to read named Metafile.  The following cases are checked in order:
*	<FileName>
*	<FileName>.1
*	<FileName>.plm
*	<FileName>.plm.1
*/
	strncpy(BaseName, FileName, sizeof(BaseName) - 1);
	BaseName[sizeof(BaseName) - 1] = '\0';

	if ((MetaFile = fopen(FileName, BINARY_READ)) != NULL) {
	    return;
	}

	(void) sprintf(FileName, "%s.%i", BaseName, member);
	if ((MetaFile = fopen(FileName, BINARY_READ)) != NULL) {
	    (void) sprintf(FamilyName, "%s", BaseName);
	    is_family = 1;
	    return;
	}

	(void) sprintf(FileName, "%s.plm", BaseName);
	if ((MetaFile = fopen(FileName, BINARY_READ)) != NULL) {
	    return;
	}

	(void) sprintf(FileName, "%s.plm.%i", BaseName, member);
	if ((MetaFile = fopen(FileName, BINARY_READ)) != NULL) {
	    (void) sprintf(FamilyName, "%s.plm", BaseName);
	    is_family = 1;
	    return;
	}

	fprintf(stderr, "Unable to open the requested metafile.\n");
	Usage("");
	exit(EX_BADFILE);
    }
}

/*----------------------------------------------------------------------*\
* ReadFileHeader()
*
* Checks file header.  Returns 1 if an error occured.
\*----------------------------------------------------------------------*/

static int
ReadFileHeader(void)
{
    char tag[80];

/* Read label field of header to make sure file is a PLPLOT metafile */

    plm_rd(read_header(MetaFile, mf_magic));
    if (strcmp(mf_magic, PLMETA_HEADER)) {
	fprintf(stderr, "Not a PLPLOT metafile!\n");
	return (1);
    }

/* Read version field of header.  We need to check that we can read the
   metafile, in case this is an old version of plrender. */

    plm_rd(read_header(MetaFile, mf_version));
    if (strcmp(mf_version, PLMETA_VERSION) > 0) {
	fprintf(stderr,
	    "Error: incapable of reading metafile version %s.\n", mf_version);
	fprintf(stderr, "Please obtain a newer copy of plrender.\n");
	return (1);
    }

/* Disable page seeking on versions without page links */

    if (strcmp(mf_version, "1993a") < 0) 
	no_pagelinks=1;

/* Return if metafile older than version 1992a (no tagged info). */

    if (strcmp(mf_version, "1992a") < 0) {
	return (0);
    }

/* Read tagged initialization info. */
/* This is an easy way to guarantee backward compatibility. */

    for (;;) {
	plm_rd(read_header(MetaFile, tag));
	if (*tag == '\0')
	    break;

	if (!strcmp(tag, "xmin")) {
	    plm_rd(read_2bytes(MetaFile, &xmin));
	    continue;
	}

	if (!strcmp(tag, "xmax")) {
	    plm_rd(read_2bytes(MetaFile, &xmax));
	    continue;
	}

	if (!strcmp(tag, "ymin")) {
	    plm_rd(read_2bytes(MetaFile, &ymin));
	    continue;
	}

	if (!strcmp(tag, "ymax")) {
	    plm_rd(read_2bytes(MetaFile, &ymax));
	    continue;
	}

	if (!strcmp(tag, "pxlx")) {
	    plm_rd(read_ieeef(MetaFile, &pxlx));
	    continue;
	}

	if (!strcmp(tag, "pxly")) {
	    plm_rd(read_ieeef(MetaFile, &pxly));
	    continue;
	}

	if (!strcmp(tag, "aspect")) {
	    plm_rd(read_ieeef(MetaFile, &dum_float));
	    if (!aspectset)
		aspect = dum_float;
	    continue;
	}

	if (!strcmp(tag, "width")) {
	    plm_rd(read_1byte(MetaFile, &dum_uchar));
	    plwid(dum_uchar);
	    continue;
	}

	if (!strcmp(tag, "orient")) {
	    plm_rd(read_1byte(MetaFile, &dum_uchar));
	    if (!orientset)
		orient = dum_uchar;
	    continue;
	}

	fprintf(stderr, "Unrecognized PLPLOT metafile header tag.\n");
	exit(EX_BADFILE);
    }

    return (0);
}

/*----------------------------------------------------------------------*\
* Help()
*
* Print long help message.
\*----------------------------------------------------------------------*/

static void
Help(void)
{
    PLOptionTable *opt;
    char **cpp;

    fprintf(stderr,
	    "\nUsage:\n        %s [%s options] [plplot options] [filename]\n",
	    program_name, program_name);

    fprintf(stderr, "\n%s options:\n", program_name);
    for (opt = option_table; opt->syntax; opt++) {
	fprintf(stderr, "    %-20s %s\n", opt->syntax, opt->desc);
    }

    plHelp();

    putc('\n', stderr);
    for (cpp = notes; *cpp; cpp++) {
	fputs(*cpp, stderr);
	putc('\n', stderr);
    }
    putc('\n', stderr);

    exit(1);
}

/*----------------------------------------------------------------------*\
* Usage()
*
* Print usage & syntax message.
\*----------------------------------------------------------------------*/

static void
Usage(char *badOption)
{
    PLOptionTable *opt;
    int col, len;

    if (*badOption != '\0')
	fprintf(stderr, "\n%s:  bad command line option \"%s\"\r\n",
		program_name, badOption);

    fprintf(stderr,
	    "\nUsage:\n        %s [%s options] [plplot options] [filename]\n",
	    program_name, program_name);

    fprintf(stderr, "\n%s options:", program_name);

    col = 80;
    for (opt = option_table; opt->syntax; opt++) {
	len = 3 + strlen(opt->syntax);		/* space [ string ] */
	if (col + len > 79) {
	    fprintf(stderr, "\r\n   ");		/* 3 spaces */
	    col = 3;
	}
	fprintf(stderr, " [%s]", opt->syntax);
	col += len;
    }
    fprintf(stderr, "\r\n");

    plSyntax();

    fprintf(stderr, "\nType %s -h for a full description.\r\n\n",
	    program_name);

    exit(1);
}

/*----------------------------------------------------------------------*\
* Input handlers
\*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*\
* HandleOption_h()
*
* Performs appropriate action for option "h".
\*----------------------------------------------------------------------*/

static int
HandleOption_h(char *opt, char *optarg)
{

/* Help */

    Help();

    return(1);
}

/*----------------------------------------------------------------------*\
* HandleOption_v()
*
* Performs appropriate action for option "v".
\*----------------------------------------------------------------------*/

static int
HandleOption_v(char *opt, char *optarg)
{

/* Version */

    fprintf(stderr, "%s version: %s\n", program_name, program_version);
    return(1);
}

/*----------------------------------------------------------------------*\
* HandleOption_i()
*
* Performs appropriate action for option "i".
\*----------------------------------------------------------------------*/

static int
HandleOption_i(char *opt, char *optarg)
{

/* Input file */

    strncpy(FileName, optarg, sizeof(FileName) - 1);
    FileName[sizeof(FileName) - 1] = '\0';

    return(0);
}

/*----------------------------------------------------------------------*\
* HandleOption_f()
*
* Performs appropriate action for option "f".
\*----------------------------------------------------------------------*/

static int
HandleOption_f(char *opt, char *optarg)
{

/* Filter option */

    is_filter = 1;

    return(0);
}

/*----------------------------------------------------------------------*\
* HandleOption_b()
*
* Performs appropriate action for option "b".
\*----------------------------------------------------------------------*/

static int
HandleOption_b(char *opt, char *optarg)
{

/* Beginning page number */

    page_begin = atoi(optarg);

    return(0);
}

/*----------------------------------------------------------------------*\
* HandleOption_e()
*
* Performs appropriate action for option "e".
\*----------------------------------------------------------------------*/

static int
HandleOption_e(char *opt, char *optarg)
{

/* End page number */

    page_end = atoi(optarg);

    return(0);
}

/*----------------------------------------------------------------------*\
* HandleOption_p()
*
* Performs appropriate action for option "p".
\*----------------------------------------------------------------------*/

static int
HandleOption_p(char *opt, char *optarg)
{

/* Specified page only */

    page_begin = atoi(optarg);
    page_end = page_begin;

    return(0);
}

/*----------------------------------------------------------------------*\
* HandleOption_a()
*
* Performs appropriate action for option "a".
\*----------------------------------------------------------------------*/

static int
HandleOption_a(char *opt, char *optarg)
{

/* Aspect ratio */

    aspect = atof(optarg);
    aspectset = 1;

    return(0);
}

/*----------------------------------------------------------------------*\
* HandleOption_mar()
*
* Performs appropriate action for option "mar".
\*----------------------------------------------------------------------*/

static int
HandleOption_mar(char *opt, char *optarg)
{

/* Set margin factor -- total fraction of page to reserve at edge (includes
   contributions at both sides). */

    mar = atof(optarg);

    return(0);
}

/*----------------------------------------------------------------------*\
* HandleOption_ori()
*
* Performs appropriate action for option "ori".
\*----------------------------------------------------------------------*/

static int
HandleOption_ori(char *opt, char *optarg)
{

/* Orientation */

    orient = atoi(optarg);
    orientset = 1;

    return(0);
}

/*----------------------------------------------------------------------*\
* HandleOption_jx()
*
* Performs appropriate action for option "jx".
\*----------------------------------------------------------------------*/

static int
HandleOption_jx(char *opt, char *optarg)
{

/* Set justification in x (0.0 < jx < 1.0). jx = 0.5 (centered) is default */

    jx = atof(optarg);

    return(0);
}

/*----------------------------------------------------------------------*\
* HandleOption_jy()
*
* Performs appropriate action for option "jy".
\*----------------------------------------------------------------------*/

static int
HandleOption_jy(char *opt, char *optarg)
{

/* Set justification in y (0.0 < jy < 1.0). jy = 0.5 (centered) is default */

    jy = atof(optarg);

    return(0);
}

/*----------------------------------------------------------------------*\
* check_alignment()
*
* Reads the next byte and aborts if it is not an END_OF_HEADER.
* Currently unused.
\*----------------------------------------------------------------------*/

static void
check_alignment(FILE *file)
{
    U_CHAR c;

    plm_rd(read_1byte(file, &c));
    if (c != END_OF_FIELD)
	plexit("check_alignment: Metafile alignment problem");
}



