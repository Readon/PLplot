/* 

	Bar chart demo.
*/

import plplot;
import std.string;

/*--------------------------------------------------------------------------*\
 * main
 *
 * Does a simple bar chart, using color fill.  If color fill is
 * unavailable, pattern fill is used instead (automatic).
\*--------------------------------------------------------------------------*/
int main( char[][] args )
{
  char[] string;
  PLFLT[10] y0;

  /* Parse and process command line arguments */
  char*[] c_args = new char*[args.length];
  foreach( size_t i, char[] arg; args ) {
    c_args[i] = toStringz(arg);
  }
  int argc = c_args.length;
  plparseopts( &argc, cast(char**)c_args, PL_PARSE_FULL );

  /* Initialize plplot */
  plinit();

  pladv( 0 );
  plvsta();
  plwind( 1980.0, 1990.0, 0.0, 35.0 );
  plbox( "bc", 1.0, 0, "bcnv", 10.0, 0 );
  plcol0( 2 );
  pllab( "Year", "Widget Sales (millions)", "#frPLplot Example 12" );

  y0[] = [5.0, 15.0, 12.0, 24.0, 28.0, 30.0, 20.0, 8.0, 12.0, 3.0];

  for( size_t i=0; i<10; i++ ) {
  	plcol0( i+1 );
    plpsty( 0 );
    plfbox( (1980.+i), y0[i] );
    string = format( "%.0f", y0[i] );
    plptex( (1980.+i+.5), (y0[i]+1.), 1.0, 0.0, .5, toStringz(string) );
    string = format( "%d", 1980+i );
    plmtex( "b", 1.0, ((i+1)*.1-.05), 0.5, toStringz(string) );
  }

  /* Don't forget to call plend() to finish off! */
  plend();
  return 0;
}


void plfbox( PLFLT x0, PLFLT y0 )
{
  PLFLT[4] x = [x0, x0, x0+1.0, x0+1.0];;
  PLFLT[4] y = [0.0, y0, y0, 0.0];

  plfill( 4, cast(PLFLT*)x, cast(PLFLT*)y );
  plcol0( 1 );
  pllsty( 1 );
  plline( 4, cast(PLFLT*)x, cast(PLFLT*)y );
}
