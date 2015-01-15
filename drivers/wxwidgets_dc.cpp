// Copyright (C) 2005  Werner Smekal, Sjaak Verdoold
// Copyright (C) 2005  Germain Carrera Corraleche
// Copyright (C) 1999  Frank Huebner
//
// This file is part of PLplot.
//
// PLplot is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as published
// by the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// PLplot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with PLplot; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
//

// TODO:
// - text clipping
// - implement AddToClipRegion for text correctly
//

// wxwidgets headers
#include <wx/wx.h>

#include "plDevs.h"

// plplot headers
#include "plplotP.h"

// std and driver headers
#include <cmath>
#include "wxwidgets.h"


//--------------------------------------------------------------------------
//  wxPLDevice::wxPLDevice( void )
//
//  Constructor of the standard wxWidgets device based on the wxPLDevBase
//  class. Only some initialisations are done.
//--------------------------------------------------------------------------
wxPLDevice::wxPLDevice( PLStream *pls, char * mfo, char * mfi, PLINT mfiSize, PLINT text, PLINT hrshsym )
{

    m_plstate_width  = false;
    m_plstate_color = false;

    //locate_mode = 0;
    //draw_xhair  = false;

    m_font     = NULL;
    m_lineSpacing = 1.0;
	m_underlined = false;

    m_dc       = NULL;

	if( mfo )
		strcpy(m_mfo, mfo);
	else
		//assume we will be outputting to the default
		//memory map until we are given a dc to draw to
		strcpy(m_mfo, "plplotMemoryMap");
	if( mfi )
		strcpy(m_mfi, mfi);
	else
		strcpy(m_mfi, "");
	m_inputSize = mfiSize;

	// be verbose and write out debug messages
#ifdef _DEBUG
    pls->verbose = 1;
    pls->debug   = 1;
#endif

    pls->color     = 1;             // Is a color device
    pls->dev_flush = 1;             // Handles flushes
    pls->dev_fill0 = 1;             // Can handle solid fills
    pls->dev_fill1 = 0;             // Can't handle pattern fills
    pls->dev_dash  = 0;
    pls->dev_clear = 1;             // driver supports clear
	pls->plbuf_write = 1;             // use the plot buffer!
    pls->termin      = ( strlen( m_mfo ) ) > 0 ? 0 : 1; // interactive device unless we are writing to memory - pretty sure this is an unused option though
    pls->graphx      = GRAPHICS_MODE; //  This indicates this is a graphics driver. PLPlot with therefore cal pltext() before outputting text, however we currently have not implemented catching that text.

    if ( text )
    {
        pls->dev_text    = 1; // want to draw text
        pls->dev_unicode = 1; // want unicode
        if ( hrshsym )
            pls->dev_hrshsym = 1;
    }
    pls->dev = (void *) this;


    // Set up physical limits of plotting device (in drawing units)
    PLFLT downscale  = (double) m_width / (double) ( PIXELS_X - 1 );
    PLFLT downscale2 = (double) m_height / (double) PIXELS_Y;
    if ( downscale < downscale2 )
        downscale = downscale2;
    plP_setphy( (PLINT) 0, (PLINT) ( m_width / downscale ),
        (PLINT) 0, (PLINT) ( m_height / downscale ) );

    // get physical device limits coordinates
    plP_gphy( &m_xmin, &m_xmax, &m_ymin, &m_ymax );

	SetSize( pls, m_width, m_height );

	if( strlen(m_mfi)>0 )
	{
		m_inputMemoryMap.create( mfi, m_inputSize, true );
		if( m_inputMemoryMap.isValid())
		{
			if( pls->plbuf_buffer )
				free( pls->plbuf_buffer );
			pls->plbuf_top = 0;
			pls->plbuf_buffer_size = 0;
			pls->plbuf_buffer = malloc( m_inputSize );
			if( pls->plbuf_buffer )
			{
				pls->plbuf_top = m_inputSize;
				pls->plbuf_buffer_size = m_inputSize;
				memcpy( pls->plbuf_buffer, m_inputMemoryMap.getBuffer(), m_inputSize );
				plreplot();
			}
		}
	}
}


//--------------------------------------------------------------------------
//  wxPLDevice::~wxPLDevice( void )
//
//  The deconstructor frees memory allocated by the device.
//--------------------------------------------------------------------------
wxPLDevice::~wxPLDevice()
{
    if ( m_font )
        delete m_font;
}


//--------------------------------------------------------------------------
//  void wxPLDevice::DrawLine( short x1a, short y1a, short x2a, short y2a )
//
//  Draw a line from (x1a, y1a) to (x2a, y2a).
//--------------------------------------------------------------------------
void wxPLDevice::DrawLine( short x1a, short y1a, short x2a, short y2a )
{
	if( !m_dc )
		return;

    x1a = (short) ( x1a / m_scalex ); y1a = (short) ( m_height - y1a / m_scaley );
    x2a = (short) ( x2a / m_scalex );        y2a = (short) ( m_height - y2a / m_scaley );

    m_dc->DrawLine( (wxCoord) x1a, (wxCoord) y1a, (wxCoord) x2a, (wxCoord) y2a );
}


//--------------------------------------------------------------------------
//  void wxPLDevice::DrawPolyline( short *xa, short *ya, PLINT npts )
//
//  Draw a poly line - coordinates are in the xa and ya arrays.
//--------------------------------------------------------------------------
void wxPLDevice::DrawPolyline( short *xa, short *ya, PLINT npts )
{
	if( !m_dc )
		return;

    wxCoord x1a, y1a, x2a, y2a;

    x2a = (wxCoord) ( xa[0] / m_scalex );
    y2a = (wxCoord) ( m_height - ya[0] / m_scaley );
    for ( PLINT i = 1; i < npts; i++ )
    {
        x1a = x2a; y1a = y2a;
        x2a = (wxCoord) ( xa[i] / m_scalex );
        y2a = (wxCoord) ( m_height - ya[i] / m_scaley );

        m_dc->DrawLine( x1a, y1a, x2a, y2a );
    }
}


//--------------------------------------------------------------------------
//  void wxPLDevice::ClearBackground( PLINT bgr, PLINT bgg, PLINT bgb,
//                                   PLINT x1, PLINT y1, PLINT x2, PLINT y2 )
//
//  Clear parts ((x1,y1) to (x2,y2)) of the background in color (bgr,bgg,bgb).
//--------------------------------------------------------------------------
void wxPLDevice::ClearBackground( PLINT bgr, PLINT bgg, PLINT bgb,
                                 PLINT x1, PLINT y1, PLINT x2, PLINT y2 )
{
	if( !m_dc )
		return;

    if ( x1 < 0 )
        x1 = 0;
    else
        x1 = (PLINT) ( x1 / m_scalex );
    if ( y1 < 0 )
        y1 = 0;
    else
        y1 = (PLINT) ( m_height - y1 / m_scaley );
    if ( x2 < 0 )
        x2 = m_width;
    else
        x2 = (PLINT) ( x2 / m_scalex );
    if ( y2 < 0 )
        y2 = m_height;
    else
        y2 = (PLINT) ( m_height - y2 / m_scaley );

    const wxPen   oldPen   = m_dc->GetPen();
    const wxBrush oldBrush = m_dc->GetBrush();

    m_dc->SetPen( *( wxThePenList->FindOrCreatePen( wxColour( bgr, bgg, bgb ), 1, wxSOLID ) ) );
    m_dc->SetBrush( wxBrush( wxColour( bgr, bgg, bgb ) ) );
    m_dc->DrawRectangle( x1, y1, x2 - x1, y2 - y1 );

    m_dc->SetPen( oldPen );
    m_dc->SetBrush( oldBrush );
}


//--------------------------------------------------------------------------
//  void wxPLDevice::FillPolygon( PLStream *pls )
//
//  Draw a filled polygon.
//--------------------------------------------------------------------------
void wxPLDevice::FillPolygon( PLStream *pls )
{
	if( !m_dc )
		return;

    wxPoint *points = new wxPoint[pls->dev_npts];
    wxCoord xoffset = 0;
    wxCoord yoffset = 0;

    for ( int i = 0; i < pls->dev_npts; i++ )
    {
        points[i].x = (int) ( pls->dev_x[i] / m_scalex );
        points[i].y = (int) ( m_height - pls->dev_y[i] / m_scaley );
    }

    if ( pls->dev_eofill )
    {
        m_dc->DrawPolygon( pls->dev_npts, points, xoffset, yoffset, wxODDEVEN_RULE );
    }
    else
    {
        m_dc->DrawPolygon( pls->dev_npts, points, xoffset, yoffset, wxWINDING_RULE );
    }
    delete[] points;
}


//--------------------------------------------------------------------------
//  void wxPLDevice::SetWidth( PLStream *pls )
//
//  Set the width of the drawing pen.
//--------------------------------------------------------------------------
void wxPLDevice::SetWidth( PLStream *pls )
{
	//Note that calls to this function before we have a DC should be honoured
	//Save a flag to grab the value from the PLStream after creation.
	if( !m_dc )
	{
		m_plstate_width = true;
		return;
	}

    m_dc->SetPen( *( wxThePenList->FindOrCreatePen( wxColour( pls->curcolor.r, pls->curcolor.g, pls->curcolor.b ),
                         pls->width > 0 ? pls->width : 1, wxSOLID ) ) );
}


//--------------------------------------------------------------------------
//  void wxPLDevice::SetColor0( PLStream *pls )
//
//  Set color from colormap 0.
//--------------------------------------------------------------------------
void wxPLDevice::SetColor( PLStream *pls )
{
	//Note that calls to this function before we have a DC should be honoured
	//Save a flag to grab the value from the PLStream after creation.
	if( !m_dc )
	{
		m_plstate_color = true;
		return;
	}

    m_dc->SetPen( *( wxThePenList->FindOrCreatePen( wxColour( pls->curcolor.r, pls->curcolor.g, pls->curcolor.b, pls->curcolor.a * 255 ),
                         pls->width > 0 ? pls->width : 1, wxSOLID ) ) );
    m_dc->SetBrush( wxBrush( wxColour( pls->curcolor.r, pls->curcolor.g, pls->curcolor.b, pls->curcolor.a * 255 ) ) );
}


//--------------------------------------------------------------------------
//  void wxPLDevice::SetExternalBuffer( void* dc )
//
//  Adds a dc to the device. In that case, the drivers doesn't provide
//  a GUI.
//--------------------------------------------------------------------------
void wxPLDevice::SetExternalBuffer( PLStream *pls, void* dc )
{
    m_dc   = (wxDC *) dc; // Add the dc to the device
	strcpy( m_mfo, "" );
	BeginPage( pls );
	if( strlen( m_mfi ) > 0 )
		plreplot();
}

//--------------------------------------------------------------------------
//  void wxPLDevice::DrawText( PLUNICODE* ucs4, int ucs4Len, bool drawText )
//
//  This is the function to draw text. Pass in a unicde string and its
//  length and set drawText to true to actually draw the text or false to
//  just get text metrics. This function will process the string for inline
//  style change escapes and newlines.
//--------------------------------------------------------------------------
void wxPLDevice::DrawText( PLUNICODE* ucs4, int ucs4Len, bool drawText )
{

	if( !m_dc )
		return;

    char utf8_string[m_max_string_length];
    char utf8[5];
    memset( utf8_string, '\0', m_max_string_length );

    // Get PLplot escape character
    char plplotEsc;
    plgesc( &plplotEsc );

    //Reset the size metrics
    m_textWidth         = 0;
    m_textHeight        = 0;
    m_superscriptHeight = 0;
    m_subscriptDepth    = 0;

    int  i = 0;
    while ( i < ucs4Len )
    {
        if ( ucs4[i] < PL_FCI_MARK )                // not a font change
        {
            if ( ucs4[i] != (PLUNICODE) plplotEsc ) // a character to display
            {
                ucs4_to_utf8( ucs4[i], utf8 );
                strncat( utf8_string, utf8,
                    sizeof ( utf8_string ) - strlen( utf8_string ) - 1 );
                i++;
                continue;
            }
            i++;
            if ( ucs4[i] == (PLUNICODE) plplotEsc ) // a escape character to display
            {
                ucs4_to_utf8( ucs4[i], utf8 );
                strncat( utf8_string, utf8,
                    sizeof ( utf8_string ) - strlen( utf8_string ) - 1 );
                i++;
                continue;
            }
            else
            {
                if ( ucs4[i] == (PLUNICODE) 'u' ) // Superscript
                {                                 // draw string so far
                    DrawTextSection( utf8_string, drawText );

                    // change font scale
                    if ( m_yOffset < -0.0001 )
                        m_fontScale *= 1.25; // Subscript scaling parameter
                    else
                        m_fontScale *= 0.8;  // Subscript scaling parameter
                    SetFont( m_fci );

                    m_yOffset += m_scaley * m_fontSize * m_fontScale / 2.;
                }
                if ( ucs4[i] == (PLUNICODE) 'd' ) // Subscript
                {                                 // draw string so far
                    DrawTextSection( utf8_string, drawText );

                    // change font scale
                    double old_fontScale = m_fontScale;
                    if ( m_yOffset > 0.0001 )
                        m_fontScale *= 1.25; // Subscript scaling parameter
                    else
                        m_fontScale *= 0.8;  // Subscript scaling parameter
                    SetFont( m_fci );

                    m_yOffset -= m_scaley * m_fontSize * old_fontScale / 2.;
                }
                if ( ucs4[i] == (PLUNICODE) '-' ) // underline
                {                                 // draw string so far
                    DrawTextSection( utf8_string, drawText );

                    m_underlined = !m_underlined;
                    SetFont( m_fci );
                }
                if ( ucs4[i] == (PLUNICODE) '+' ) // overline
                {                                 // not implemented yet
                }
                i++;
            }
        }
        else // a font change
        {
            // draw string so far
            DrawTextSection( utf8_string, drawText );

            // get new font
            m_fci = ucs4[i];
            SetFont( m_fci );
            i++;
        }
    }

    DrawTextSection( utf8_string, drawText );
}


//--------------------------------------------------------------------------
//  void wxPLDevice::DrawTextSection( char* utf8_string, bool drawText )
//
//  Draw a section of text. This should already have been processed and
//  split into sections so that the text passed in has no style changes or
//  newines.
//--------------------------------------------------------------------------
void wxPLDevice::DrawTextSection( char* utf8_string, bool drawText )
{
	if( !m_dc )
		return;

    wxCoord  w, h, d, l;

    wxString str( wxConvUTF8.cMB2WC( utf8_string ), *wxConvCurrent );

    m_dc->GetTextExtent( str, &w, &h, &d, &l );

    if ( drawText )
    {
        m_dc->DrawRotatedText( str, (wxCoord) ( m_posX - m_yOffset / m_scaley * m_sin_rot ),
            (wxCoord) ( m_height - (wxCoord) ( m_posY + m_yOffset * m_cos_rot / m_scaley ) ),
            m_rotation * 180.0 / M_PI );
        m_posX += (PLINT) ( w * m_cos_rot );
        m_posY += (PLINT) ( w * m_sin_rot );
    }

    m_textWidth += w;

    //keep track of the height of superscript text, the depth of subscript
    //text and the height of regular text
    if ( m_yOffset > 0.0001 )
    {
        //determine the height the text would have if it were full size
        double currentOffset = m_yOffset;
        double currentHeight = h;
        while ( currentOffset > 0.0001 )
        {
            currentOffset -= m_scaley * m_fontSize * m_fontScale / 2.;
            currentHeight *= 1.25;
        }
        m_textHeight = (wxCoord) m_textHeight > ( currentHeight )
                     ? m_textHeight
                     : currentHeight;
        //work out the height including superscript
        m_superscriptHeight = m_superscriptHeight > ( currentHeight + m_yOffset / m_scaley )
                            ? m_superscriptHeight
                            : static_cast<int>( ( currentHeight + m_yOffset / m_scaley ) );
    }
    else if ( m_yOffset < -0.0001 )
    {
        //determine the height the text would have if it were full size
        double currentOffset = m_yOffset;
        double currentHeight = h;
        double currentDepth  = d;
        while ( currentOffset < -0.0001 )
        {
            currentOffset += m_scaley * m_fontSize * m_fontScale * 1.25 / 2.;
            currentHeight *= 1.25;
            currentDepth  *= 1.25;
        }
        m_textHeight = (wxCoord) m_textHeight > currentHeight ? m_textHeight : currentHeight;
        //work out the additional depth for subscript note an assumption has been made
        //that the font size of (non-superscript and non-subscript) text is the same
        //along a line. Currently there is no escape to change font size mid string
        //so this should be fine
        m_subscriptDepth = (wxCoord) m_subscriptDepth > ( ( -m_yOffset / m_scaley + h + d ) - ( currentDepth + m_textHeight ) )
                         ? m_subscriptDepth
                         : ( ( -m_yOffset / m_scaley + h + d ) - ( currentDepth + m_textHeight ) );
        m_subscriptDepth = m_subscriptDepth > 0 ? m_subscriptDepth : 0;
    }
    else
        m_textHeight = (wxCoord) m_textHeight > ( h ) ? m_textHeight : h;

    memset( utf8_string, '\0', m_max_string_length );
}


//--------------------------------------------------------------------------
//  void wxPLDevice::SetFont( PLUNICODE fci )
//
//  Set font defined by fci.
//--------------------------------------------------------------------------
void wxPLDevice::SetFont( PLUNICODE fci )
{
	if( !m_dc )
		return;

    unsigned char fontFamily, fontStyle, fontWeight;

    plP_fci2hex( fci, &fontFamily, PL_FCI_FAMILY );
    plP_fci2hex( fci, &fontStyle, PL_FCI_STYLE );
    plP_fci2hex( fci, &fontWeight, PL_FCI_WEIGHT );

    if ( m_font )
        delete m_font;

    m_font = wxFont::New( (int) ( m_fontSize * m_fontScale < 4 ? 4 : m_fontSize * m_fontScale ),
        fontFamilyLookup[fontFamily],
        fontStyleLookup[fontStyle] | fontWeightLookup[fontWeight] );
    m_font->SetUnderlined( m_underlined );
    m_dc->SetFont( *m_font );
}


//--------------------------------------------------------------------------
//  void wxPLDevice::ProcessString( PLStream* pls, EscText* args )
//
//  This is the main function which processes the unicode text strings.
//  Font size, rotation and color are set, width and height of the
//  text string is determined and then the string is drawn to the canvas.
//--------------------------------------------------------------------------
void wxPLDevice::ProcessString( PLStream* pls, EscText* args )
{
	if( !m_dc )
		return;

    // Check that we got unicode, warning message and return if not
    if ( args->unicode_array_len == 0 )
    {
        printf( "Non unicode string passed to the wxWidgets driver, ignoring\n" );
        return;
    }

    // Check that unicode string isn't longer then the max we allow
    if ( args->unicode_array_len >= 500 )
    {
        printf( "Sorry, the wxWidgets drivers only handles strings of length < %d\n", 500 );
        return;
    }

    // Calculate the font size (in pixels)
    m_fontSize = pls->chrht * VIRTUAL_PIXELS_PER_MM / m_scaley * 1.3;

    // Use PLplot core routine to get the corners of the clipping rectangle
    PLINT rcx[4], rcy[4];
    difilt_clip( rcx, rcy );

    wxPoint cpoints[4];
    for ( int i = 0; i < 4; i++ )
    {
        cpoints[i].x = rcx[i] / m_scalex;
        cpoints[i].y = m_height - rcy[i] / m_scaley;
    }
    wxDCClipper clip( *m_dc, wxRegion( 4, cpoints ) );

    // calculate rotation of text
    plRotationShear( args->xform, &m_rotation, &m_shear, &m_stride );
    m_rotation -= pls->diorot * M_PI / 2.0;
    m_cos_rot   = cos( m_rotation );
    m_sin_rot   = sin( m_rotation );

    // Set font color
    m_dc->SetTextForeground( wxColour( pls->curcolor.r, pls->curcolor.g, pls->curcolor.b ) );
    m_dc->SetTextBackground( wxColour( pls->curcolor.r, pls->curcolor.g, pls->curcolor.b ) );

    PLUNICODE *lineStart     = args->unicode_array;
    int       lineLen        = 0;
    bool      lineFeed       = false;
    bool      carriageReturn = false;
    wxCoord   paraHeight     = 0;
    // Get the curent font
    m_fontScale = 1.0;
    m_yOffset   = 0.0;
    plgfci( &m_fci );
    SetFont( m_fci );
    while ( lineStart != args->unicode_array + args->unicode_array_len )
    {
        while ( lineStart + lineLen != args->unicode_array + args->unicode_array_len
                && *( lineStart + lineLen ) != (PLUNICODE) '\n' )
        {
            lineLen++;
        }
        //set line feed for the beginning of this line and
        //carriage return for the end
        lineFeed       = carriageReturn;
        carriageReturn = lineStart + lineLen != args->unicode_array + args->unicode_array_len
                         && *( lineStart + lineLen ) == (PLUNICODE) ( '\n' );
        if ( lineFeed )
            paraHeight += m_textHeight + m_subscriptDepth;

        //remember the text parameters so they can be restored
        double    startingFontScale = m_fontScale;
        double    startingYOffset   = m_yOffset;
        PLUNICODE startingFci       = m_fci;

        // determine extent of text
        m_posX = args->x / m_scalex;
        m_posY = args->y / m_scaley;

        DrawText( lineStart, lineLen, false );

        if ( lineFeed && m_superscriptHeight > m_textHeight )
            paraHeight += m_superscriptHeight - m_textHeight;

        // actually draw text, resetting the font first
        m_fontScale = startingFontScale;
        m_yOffset   = startingYOffset;
        m_fci       = startingFci;
        SetFont( m_fci );
        m_posX = (PLINT) ( args->x / m_scalex - ( args->just * m_textWidth ) * m_cos_rot - ( 0.5 * m_textHeight - paraHeight * m_lineSpacing ) * m_sin_rot ); //move to set alignment
        m_posY = (PLINT) ( args->y / m_scaley - ( args->just * m_textWidth ) * m_sin_rot + ( 0.5 * m_textHeight - paraHeight * m_lineSpacing ) * m_cos_rot );
        DrawText( lineStart, lineLen, true );  //draw text

        lineStart += lineLen;
        if ( carriageReturn )
            lineStart++;
        lineLen = 0;
    }
    //posX = args->x;
    //posY = args->y;
    //PSDrawText( args->unicode_array, args->unicode_array_len, false );

    //posX = (PLINT) ( args->x - ( ( args->just * textWidth ) * cos_rot + ( 0.5 * textHeight ) * sin_rot ) * scalex );
    //posY = (PLINT) ( args->y - ( ( args->just * textWidth ) * sin_rot - ( 0.5 * textHeight ) * cos_rot ) * scaley );
    //PSDrawText( args->unicode_array, args->unicode_array_len, true );

}

//--------------------------------------------------------------------------
//  void wxPLDevice::EndPage( PLStream* pls )
//  End the page. This is the point that we write the buffer to the memory
//  mapped file if needed
//--------------------------------------------------------------------------
void wxPLDevice::EndPage( PLStream* pls )
{
	if( strlen( m_mfo ) > 0 )
	{
		//This bit of code copies the pls->plbuf_buffer to a memeory map and
		//runs wxPlViewer which reads the buffer in and displays the plot.
		//Note that the buffer is cleared at the beginnign of each page so
		//we are transferring and displaying one page at a time.

		//create a memory map to hold the data, note that creating a new map automatically destroys the previous one
		m_outputMemoryMap.create( m_mfo, pls->plbuf_top, false );
		//check the memory map is valid
		if( !m_outputMemoryMap.isValid() )
		{
			plwarn( "Error creating memory map for wxWidget instruction transport. The page will not be displayed" );
			return;
		}
		//copy the page's buffer to the map
		memcpy( m_outputMemoryMap.getBuffer(), pls->plbuf_buffer, pls->plbuf_top );
		//Run the wxPlViewer with command line parameters telling it the location and size of the buffer
		//the console will hang until wxPlViewer exits
		wxString command;
		command << wxT( "wxPlViewer " ) << wxString( m_mfo, wxConvUTF8 ) << wxT( " " ) << pls->plbuf_top;
		system( command.mb_str() );
	}
}

//--------------------------------------------------------------------------
//  void wxPLDevice::BeginPage( PLStream* pls )
//  Clears the page and applys any state changes waiting to be applied
//--------------------------------------------------------------------------
void wxPLDevice::BeginPage( PLStream* pls )
{
	if( !m_dc )
		return;
    // clear background
    PLINT bgr, bgg, bgb;                  // red, green, blue
    plgcolbg( &bgr, &bgg, &bgb );         // get background color information
    ClearBackground( bgr, bgg, bgb );

    // Replay escape calls that come in before PLESC_DEVINIT.  All of them
    // required a DC that didn't exist yet.
    //
    if ( m_plstate_width )
		SetWidth( pls );
    if ( m_plstate_color )
		SetColor( pls );

    m_plstate_width = false;
    m_plstate_color = false;
}

//--------------------------------------------------------------------------
//  void wxPLDevice::SetSize( PLStream* pls )
//  Set the size of the page, scale parameters and the dpi
//--------------------------------------------------------------------------
void wxPLDevice::SetSize( PLStream* pls, int width, int height )
{
	// set new size and scale parameters
    m_width  = width;
    m_height = height;
    m_scalex = (PLFLT) ( m_xmax - m_xmin ) / width;
    m_scaley = (PLFLT) ( m_ymax - m_ymin ) / height;

    // set dpi
    plspage( VIRTUAL_PIXELS_PER_IN / m_scalex, VIRTUAL_PIXELS_PER_IN / m_scaley, 0, 0, 0, 0 );

	//redraw the plot if we have somewhere to draw it to
	if( m_dc )
		plreplot();
}

