// pllegend()
//
// Copyright (C) 2010  Hezekiah M. Carty
// Copyright (C) 2010  Alan W. Irwin
//
// This file is part of PLplot.
//
// PLplot is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Library Public License as published
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

//! @file
//!
//! Legend routines.
//!
#include "plplotP.h"

//--------------------------------------------------------------------------
//! Obtain ratio of world to mm coordinates in both x and y.
//!
//! @param x_world_per_mm : pointer to PLFLT containing x ratio after call
//! @param y_world_per_mm : pointer to PLFLT containing y ratio after call
//!

static void get_world_per_mm( PLFLT *x_world_per_mm, PLFLT *y_world_per_mm )
{
    // Normalized viewport limits
    PLFLT vxmin, vxmax, vymin, vymax;
    // Size of subpage in mm
    PLFLT mxmin, mxmax, mymin, mymax;
    // Viewport limits in world coordinates
    PLFLT wxmin, wxmax, wymin, wymax;
    plgvpd( &vxmin, &vxmax, &vymin, &vymax );
    plgspa( &mxmin, &mxmax, &mymin, &mymax );
    plgvpw( &wxmin, &wxmax, &wymin, &wymax );
    *x_world_per_mm = ( wxmax - wxmin ) / ( ( vxmax - vxmin ) * ( mxmax - mxmin ) );
    *y_world_per_mm = ( wymax - wymin ) / ( ( vymax - vymin ) * ( mymax - mymin ) );
}

//--------------------------------------------------------------------------
//! Obtain character or symbol height in (y) world coordinates.
//!
//! @param ifcharacter : TRUE obtain character height, FALSE obtain symbol
//! height
//! @return character or symbol height
//!

static PLFLT get_character_or_symbol_height( PLBOOL ifcharacter )
{
    // Character height in mm
    PLFLT default_mm, char_height_mm;
    PLFLT x_world_per_mm, y_world_per_mm;

    if ( ifcharacter )
    {
        plgchr( &default_mm, &char_height_mm );
    }
    else
    {
        default_mm     = plsc->symdef;
        char_height_mm = plsc->symht;
    }
    get_world_per_mm( &x_world_per_mm, &y_world_per_mm );
    return ( char_height_mm * y_world_per_mm );
}

#define normalized_to_world_x( nx )    ( ( xmin ) + ( nx ) * ( ( xmax ) - ( xmin ) ) )
#define normalized_to_world_y( ny )    ( ( ymin ) + ( ny ) * ( ( ymax ) - ( ymin ) ) )

//--------------------------------------------------------------------------
//! Plot discrete legend using lines, symbols, cmap0 colors, or cmap1
//! colors.
//!
//! @param opt : ORed option flags PL_LEGEND_BACKGROUND and
//! PL_LEGEND_TEXT_LEFT controlling overall legend.
//! @param plot_width : width in normalized subpage units of plotted area
//! (lines, symbols, or coloured area) in legend.
//! @param text_offset : offset of text area from plot area in units
//! of character width.
//!
// N.B. the total width of the legend is made up of plplot_width +
//   text_offset (converted to normalized viewport coordinates) + width
//   of the longest string.  The latter quantity is calculated internally
//   using plstrl and converted to normalized viewport coordinates.
//
// x, y: Normalized position of the upper-left corner of the
//   legend in the viewport.
// nlegend: Number of legend entries
// text_colors: Color map 0 indices of the colors to use for label text
// text: text string for each legend entry
// cmap0_colors: cmap0 color index for each legend entry
// nsymbols: number of points/symbols to be drawn for each plot_width
// symbols: Symbol to draw for each legend entry.

void
c_pllegend( PLINT opt, PLFLT plot_width,
            PLFLT x, PLFLT y, PLINT bg_color,
            PLINT *opt_array, PLINT nlegend,
            PLFLT text_offset, PLFLT text_scale, PLFLT text_spacing,
            PLINT text_justification, PLINT *text_colors, char **text,
            PLINT *line_colors, PLINT *line_styles, PLINT *line_widths,
            PLINT *nsymbols, PLINT *symbol_colors,
            PLFLT *symbol_scales, PLINT *symbols,
            PLINT *cmap0_colors, PLFLT *cmap1_colors,
            PLINT *cmap_patterns, PLFLT *cmap_scales )

{
    // Viewport world-coordinate limits
    PLFLT xmin, xmax, ymin, ymax;
    // Legend position
    PLFLT plot_x, plot_x_end, plot_x_world, plot_x_end_world;
    PLFLT plot_y, plot_y_world;
    PLFLT text_x, text_y, text_x_world, text_y_world;
    // Character height (world coordinates)
    PLFLT character_height, character_width, symbol_width;
    // y-position of the current legend entry
    PLFLT ty, dty;
    // Positions of the legend entries
    PLFLT dxs, *xs, *ys, xl[2], yl[2], xcmap[4], ycmap[4];
    PLINT i, j;
    // Active attributes to be saved and restored afterward.
    PLINT col0_save         = plsc->icol0,
          line_style_save   = plsc->line_style,
          line_width_save   = plsc->width,
          pattern_save      = plsc->patt;
    PLFLT text_scale_save   = plsc->chrht / plsc->chrdef,
          symbol_scale_save = plsc->symht / plsc->symdef;
    // Saved world coordinates of viewport.
    PLFLT xwmin_save, xwmax_save, ywmin_save, ywmax_save;
    // Saved normalized coordinates of viewport.
    PLFLT xdmin_save, xdmax_save, ydmin_save, ydmax_save;

    PLFLT x_world_per_mm, y_world_per_mm, text_width0 = 0., text_width;
    PLFLT total_width_border, total_width, total_height;

    PLINT some_lines   = 0, some_symbols = 0, some_cmaps = 0;
    PLINT max_nsymbols = 0;

    plgvpd( &xdmin_save, &xdmax_save, &ydmin_save, &ydmax_save );
    plgvpw( &xwmin_save, &xwmax_save, &ywmin_save, &ywmax_save );
    // viewport corresponds to sub-page so that all legends will
    // be clipped at sub-page boundaries.
    plvpor( 0., 1., 0., 1. );
    plwind( 0., 1., 0., 1. );

    plschr( 0., text_scale );

    for ( i = 0; i < nlegend; i++ )
    {
        if ( opt_array[i] & PL_LEGEND_LINE )
            some_lines = 1;
        if ( opt_array[i] & PL_LEGEND_SYMBOL )
        {
            max_nsymbols = MAX( max_nsymbols, nsymbols[i] );
            some_symbols = 1;
        }
        if ( opt_array[i] & ( PL_LEGEND_CMAP0 | PL_LEGEND_CMAP1 ) )
            some_cmaps = 1;
    }

    plgvpw( &xmin, &xmax, &ymin, &ymax );

    // World coordinates for legend plots
    plot_x           = x;
    plot_y           = y;
    plot_x_end       = plot_x + plot_width;
    plot_x_world     = normalized_to_world_x( plot_x );
    plot_y_world     = normalized_to_world_y( plot_y );
    plot_x_end_world = normalized_to_world_x( plot_x_end );

    // Get character height and width in world coordinates
    character_height = get_character_or_symbol_height( TRUE );
    character_width  = character_height * fabs( ( xmax - xmin ) / ( ymax - ymin ) );
    // Get world-coordinate positions of the start of the legend text
    text_x       = plot_x_end;
    text_y       = plot_y;
    text_x_world = normalized_to_world_x( text_x ) +
                   text_offset * character_width;
    text_y_world = normalized_to_world_y( text_y );

    // Calculate maximum width of text area (first in mm, then converted
    // to x world coordinates) including text_offset area.
    for ( i = 0; i < nlegend; i++ )
    {
        text_width0 = MAX( text_width0, plstrl( text[i] ) );
    }
    get_world_per_mm( &x_world_per_mm, &y_world_per_mm );
    text_width0 = x_world_per_mm * text_width0;
    text_width  = text_width0 + text_offset * character_width;
    // make small border area where only the background is plotted
    // for left and right of legend.  0.4 seems to be a reasonable factor
    // that gives a good-looking result.
    total_width_border = 0.4 * character_width;
    total_width        = 2. * total_width_border + text_width + ( xmax - xmin ) * plot_width;
    total_height       = nlegend * text_spacing * character_height;

    if ( opt & PL_LEGEND_BACKGROUND )
    {
        PLFLT xbg[4] = {
            plot_x_world,
            plot_x_world,
            plot_x_world + total_width,
            plot_x_world + total_width,
        };
        PLFLT ybg[4] = {
            plot_y_world,
            plot_y_world - total_height,
            plot_y_world - total_height,
            plot_y_world,
        };
        plpsty( 0 );
        plcol0( bg_color );
        plfill( 4, xbg, ybg );
        plcol0( col0_save );
    }

    if ( opt & PL_LEGEND_TEXT_LEFT )
    {
        // text area on left, plot area on right.
        text_x_world      = plot_x_world;
        plot_x_world     += text_width;
        plot_x_end_world += text_width;
    }
    // adjust border after background is drawn.
    plot_x_world     += total_width_border;
    plot_x_end_world += total_width_border;
    text_x_world     += total_width_border;

    if ( some_cmaps )
    {
        xcmap[0] = plot_x_world;
        xcmap[1] = plot_x_world;
        xcmap[2] = plot_x_end_world;
        xcmap[3] = plot_x_end_world;
    }

    if ( some_lines )
    {
        xl[0] = plot_x_world;
        xl[1] = plot_x_end_world;
    }

    if ( some_symbols )
    {
        max_nsymbols = MAX( 2, max_nsymbols );
        if ( ( ( xs = (PLFLT *) malloc( max_nsymbols * sizeof ( PLFLT ) ) ) == NULL ) ||
             ( ( ys = (PLFLT *) malloc( max_nsymbols * sizeof ( PLFLT ) ) ) == NULL ) )
        {
            plexit( "pllegend: Insufficient memory" );
        }

        // Get symbol width in world coordinates if symbols are plotted to
        // adjust ends of line of symbols.
        // AWI, no idea why must use 0.5 factor to get ends of symbol lines
        // to line up approximately correctly with plotted legend lines.
        // Factor should be unity.
        symbol_width = 0.5 * get_character_or_symbol_height( FALSE ) *
                       fabs( ( xmax - xmin ) / ( ymax - ymin ) );
    }

    dty = text_spacing * character_height;
    ty  = text_y_world + 0.5 * dty;
    // Draw each legend entry
    for ( i = 0; i < nlegend; i++ )
    {
        // y position of text, lines, symbols, and/or centre of cmap0 box.
        ty = ty - dty;
        // Label/name for the legend
        plcol0( text_colors[i] );
        plptex( text_x_world + text_justification * text_width0, ty, 0.1, 0.0, text_justification, text[i] );

        if ( opt_array[i] & PL_LEGEND_CMAP0 )
        {
            plcol0( cmap0_colors[i] );
            plpsty( cmap_patterns[i] );
            ycmap[0] = ty + 0.5 * dty * cmap_scales[i];
            ycmap[1] = ty - 0.5 * dty * cmap_scales[i];
            ycmap[2] = ty - 0.5 * dty * cmap_scales[i];
            ycmap[3] = ty + 0.5 * dty * cmap_scales[i];
            plfill( 4, xcmap, ycmap );
        }
        if ( opt_array[i] & PL_LEGEND_CMAP1 )
        {
            plcol1( cmap1_colors[i] );
            plpsty( cmap_patterns[i] );
            ycmap[0] = ty + 0.5 * dty * cmap_scales[i];
            ycmap[1] = ty - 0.5 * dty * cmap_scales[i];
            ycmap[2] = ty - 0.5 * dty * cmap_scales[i];
            ycmap[3] = ty + 0.5 * dty * cmap_scales[i];
            plfill( 4, xcmap, ycmap );
        }
        if ( opt_array[i] & PL_LEGEND_LINE )
        {
            plcol0( line_colors[i] );
            pllsty( line_styles[i] );
            plwid( line_widths[i] );
            yl[0] = ty;
            yl[1] = ty;
            plline( 2, xl, yl );
            pllsty( line_style_save );
            plwid( line_width_save );
        }

        if ( opt_array[i] & PL_LEGEND_SYMBOL )
        {
            plcol0( symbol_colors[i] );
            plssym( 0., symbol_scales[i] );
            dxs = ( plot_x_end_world - plot_x_world - symbol_width ) / (double) ( MAX( nsymbols[i], 2 ) - 1 );
            for ( j = 0; j < nsymbols[i]; j++ )
            {
                xs[j] = plot_x_world + 0.5 * symbol_width + dxs * (double) j;
                ys[j] = ty;
            }
            plpoin( nsymbols[i], xs, ys, symbols[i] );
        }
    }
    if ( some_symbols )
    {
        free( xs );
        free( ys );
    }

    // Restore
    plcol0( col0_save );
    plschr( 0., text_scale_save );
    plssym( 0., symbol_scale_save );
    plpsty( pattern_save );
    plvpor( xdmin_save, xdmax_save, ydmin_save, ydmax_save );
    plwind( xwmin_save, xwmax_save, ywmin_save, ywmax_save );

    return;
}

