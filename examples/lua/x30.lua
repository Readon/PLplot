--[[
  Alpha color values demonstration.

  Copyright (C) 2008 Hazen Babcock


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
  
  This example will only really be interesting when used with devices that 
  support or alpha (or transparency) values, such as the cairo device family.
--]]


-- initialise Lua bindings to PLplot
if string.sub(_VERSION,1,7)=='Lua 5.0' then
	lib=loadlib('./plplotluac.so','luaopen_plplotluac') or loadlib('plplotluac.dll','luaopen_plplotluac')
	assert(lib)()
else
	require('plplotluac')
end
pl=plplotluac

red   = {  0, 255,   0,   0 }
green = {  0,   0, 255,   0 }
blue  = {  0,   0,   0, 255 }
alpha = {  1,   1,   1,   1 }

px = { 0.1, 0.5, 0.5, 0.1 }
py = { 0.1, 0.1, 0.5, 0.5 }

pos = { 0, 1 }
rcoord = { 1, 1 }
gcoord = { 0, 0 }
bcoord = { 0, 0 }
acoord = { 0, 1 }
rev = { false, false }

clevel = {}

--  plparseopts (&argc, argv, PL_PARSE_FULL);

pl.plinit()
pl.plscmap0n(4)
pl.plscmap0a(red, green, blue, alpha, 4)

--   Page 1:
--
--   This is a series of red, green and blue rectangles overlaid
--   on each other with gradually increasing transparency.

-- Set up the window
pl.pladv(0)
pl.plvpor(0, 1, 0, 1)
pl.plwind(0, 1, 0, 1)
pl.plcol0(0)
pl.plbox("", 1, 0, "", 1, 0)

-- Draw the boxes
for i = 0, 8 do
  icol = math.mod(i, 3) + 1

  -- Get a color, change its transparency and
  -- set it as the current color.
  r, g, b, a = pl.plgcol0a(icol)
  pl.plscol0a(icol, r, g, b, 1-i/9)
  pl.plcol0(icol)

  -- Draw the rectangle
  plfill(px, py)

  -- Shift the rectangles coordinates
  for j = 1, 4 do
    px[j] = px[j] + 0.5/9
    py[j] = py[j] + 0.5/9
  end
end

--   Page 2:

--   This is a bunch of boxes colored red, green or blue with a single 
--   large (red) box of linearly varying transparency overlaid. The
--   overlaid box is completely transparent at the bottom and completely
--   opaque at the top.

-- Set up the window
pladv(0)
plvpor(0.1, 0.9, 0.1, 0.9)
plwind(0.0, 1.0, 0.0, 1.0)

-- Draw the boxes. There are 25 of them drawn on a 5 x 5 grid.
for i = 0, 4 do
  -- Set box X position
  px[1] = 0.05 + 0.2 * i
  px[2] = px[1] + 0.1
  px[3] = px[2]
  px[4] = px[1]

  -- We don't want the boxes to be transparent, so since we changed 
  -- the colors transparencies in the first example we have to change
  -- the transparencies back to completely opaque.
  icol = mod(i, 3) + 1
  r, g, b, a = plgcol0a(icol)
  plscol0a(icol, r, g, b, 1)
  plcol0(icol)
  
  for j = 0, 4 do
    -- Set box y position and draw the box.
    py[1] = 0.05 + 0.2 * j
    py[2] = py[1]
    py[3] = py[1] + 0.1
    py[4] = py[3]
    plfill(px, py)
  end
end

-- The overlaid box is drawn using plshades with a color map that is 
-- the same color but has a linearly varying transparency.

-- Create the color map with 128 colors and use plscmap1la to initialize
-- the color values with a linear varying transparency (or alpha)
plscmap1n(128)
plscmap1la(1, 2, pos, rcoord, gcoord, bcoord, acoord, rev)

-- Create a 2 x 2 array that contains the z values (0.0 to 1.0) that will
-- used for the shade plot. plshades will use linear interpolation to
-- calculate the z values of all the intermediate points in this array.
--[[plAlloc2dGrid(&z, 2, 2);
z[0][0] = 0.0;
z[1][0] = 0.0;
z[0][1] = 1.0;
z[1][1] = 1.0;

/* Set the color levels array. These levels are also between 0.0 and 1.0 */
for(i=0;i<101;i++){
  clevel[i] = 0.01 * (PLFLT)i;
}

/* Draw the shade plot with zmin = 0.0, zmax = 1.0 and x and y coordinate ranges */
/* such that it fills the entire plotting area. */
plshades(z, 2, 2, NULL, 0.0, 1.0, 0.0, 1.0, clevel, 101, 0, -1, 2, plfill, 1, NULL, NULL);

plFree2dGrid(z,2,2);--]]

plend()

