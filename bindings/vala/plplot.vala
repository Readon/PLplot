
namespace PlPlot{
    public enum StreamId { 
        Next,
        Current,
        Specific
    }
    
    public enum Color {
        Black,        Red,         Yellow,        Green,
        Cyan,         Pink,        Tan,           Grey,
        DarkRed,      DeepBlue,    Purple,        LightCyan,
        LightBlue,    Orchid,      Mauve,         White
    }
    
    public enum BinType{
        Default,
        Centered,
        NoExpand,
        NoEmpty
    }

    public enum RunLevel{
        UnInitialized,
        Initialized,
        ViewPortDefined,
        WorldCoordDefined
    }
	
	public enum Command{
		SetRGB = 1,			// obsolete
		AllocNCol,  		// obsolete
		SetLPB,		  		// obsolete
		Expose,				// handle window expose
		Resize,				// handle window resize
		Redraw,				// handle window redraw
		Text,				// switch to text screen
		Graph,				// switch to graphics screen
		Fill,  				// fill polygon
		DI,					// handle DI command
		Flush,				// flush output
		EH,					// handle Window events
		GetC,				// get cursor position
		SWin,				// set window parameters
		DoubleBuffering,	// configure double buffering
		XORMod,				// set xor mode
		SetCompression,		// AFR: set compression
		Clear,				// RL: clear graphics region
		Dash,               // RL: draw dashed line
		HasText,			// driver draws text
		Image,				// handle image
		ImageOPS,			// plimage related operations
		PL2DevCol,			// convert PLColor to device color
		Dev2PLCol,			// convert device color to PLColor
		SetBGFG,            // set BG, FG colors
		DevInit,			// alternate device initialization
		GetBackend,			// get used backend of (wxWidgets) driver - no longer used
		BeginText,			// get ready to draw a line of text
		TextChar,           // render a character of text
		ControlChar,        // handle a text control character (super/subscript, etc.)
		EndText,            // finish a drawing a line of text
		StartRasterize,     // start rasterized rendering
		EndRasterize,       // end rasterized rendering
		ARC,                // render an arc
		Gradient,  			// render a gradient
		ModeSet,			// set drawing mode
		ModeGet,            // get drawing mode
		FixAspect,          // set or unset fixing the aspect ratio of the plot
		ImportBuffer,       // set the contents of the buffer to a specified byte string
		AppendBuffer,       // append the given byte string to the buffer
		FlushRemainingBuffer // flush the remaining buffer e.g. after new data was appended
	}

    public class Stream : Object{
        private int _stream = 0;
        private class int active_streams = 0;

        construct {
            make_stream(out _stream);
            active_streams ++;
        }

        public Stream.with_options(string? driver, string? file, 
                int xwindow_count, int ywindow_count, 
                int r=0, int g=0, int b=0 ){
            Object();        

            if ( driver != null )
                PlPlot.set_current_dev( driver );
            if ( file != null )
                PlPlot.set_output_file_name( file );

            PlPlot.set_subpage( xwindow_count, ywindow_count );
            PlPlot.set_bg( r, g, b );
        }

        ~Stream() {
            PlPlot.set_current_stream( _stream );
            PlPlot.finish_current_stream_plot();

            active_streams --;
            if ( active_streams != 0 )
                PlPlot.finish_plot();
        }
		
		public void init()
		{
            PlPlot.set_current_stream( _stream );			
            PlPlot.init();
			PlPlot.get_current_stream(out _stream);
		}
		
		public void command(int op, void * ptr)
		{
            PlPlot.set_current_stream( _stream );
            PlPlot.command( op, ptr );
		}

        public void advance_page( int page )
        {
            PlPlot.set_current_stream( _stream );
            PlPlot.advance_page( page );
        }

        public void arc( double x, double y, double a, double b, 
						double angle1, double angle2,
						double rotate, bool fill )
        {
            PlPlot.set_current_stream( _stream );
            PlPlot.arc( x, y, a, b, angle1, angle2, rotate, fill );
        }
        
        public void vector (double[,] u, double[,] v, 
							double scale, TransformFunc pltr)
        {
            PlPlot.set_current_stream( _stream );
            PlPlot.vector (u, v, scale, pltr);
        }

        public void set_arrow_style_for_vector(double[] arrowx, 
                double[] arrowy, bool fill)
        {
            PlPlot.set_current_stream( _stream );
            PlPlot.set_arrow_style_for_vector(arrowx, arrowy, fill);
        }
        
        public void axes( double x0, double y0, string xopt, double xtick, 
							int nxsub, string yopt, double ytick, int nysub )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.axes( x0, y0, xopt, xtick, nxsub, yopt, ytick, nysub );
        }
        
        public void histogram( double[] x, double[] y, BinType opt )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.histogram( x, y, opt );
        }

        public void new_page()
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.new_page();
        }

        public void box( string xopt, double xtick, int nxsub,
						string yopt, double ytick, int nysub )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.box( xopt, xtick, nxsub, yopt, ytick, nysub );
        }

        public void box3d( string xopt, string xlabel, double xtick, int nxsub,
							string yopt, string ylabel, double ytick, int nysub,
							string zopt, string zlabel, double ztick, int nzsub )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.box3d( xopt, xlabel, xtick, nxsub, yopt, ylabel, ytick, nysub,
					zopt, zlabel, ztick, nzsub );
        }

        public void get_break_time_by_ctime( out int year, out int month, out int day, 
										out int hour, out int min, out int sec, double ctime )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_break_time_by_ctime( out year, out month, out day, out hour, out min, out sec, ctime );
        }

        public void get_world_by_dev_coord( double rx, double ry,
											out double wx, out double wy,
											out int window )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_world_by_dev_coord( rx, ry, out wx, out wy, out window );
        }

        public void clear()
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.clear();
        }

        public void set_color_map0(Color icol0)
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_color_map0( icol0 );
        }
    
    //TODO: col
    
        public void set_color_map1(Color icol1)
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_color_map1( icol1 );
        }

    //TODO: col1

        public void config_time_transform( double scale, double offset1, double offset2, 
								int ccontrol, bool ifbtime_offset, 
								int year, int month, int day, int hour, int min, double sec )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.config_time_transform( scale, offset1, offset2, ccontrol, ifbtime_offset, 
								year, month, day, hour, min, sec );
        }

        public void contour( double[,] f, int kx, int lx, int ky, int ly,
							double[] clevel, TransformFunc pltr)
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.contour( f, kx, lx, ky, ly, clevel, pltr);
        }

    //TODO: fcont

        public void copy( int iplsr, bool flags )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.copy( iplsr, flags );
        }

        public void get_ctime_by_break_time( int year, int month, int day,
											int hour, int min, double sec, out double ctime )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_ctime_by_break_time( year, month, day, hour, min, sec, out ctime );
        }

    //TODO: did2pc

    //TODO: dip2dc

        public void setup_window( double xmin, double xmax, double ymin,
									double ymax, int just, int axis )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.setup_window( xmin, xmax, ymin, ymax, just, axis );
        }

        public void setup_window_with_clear( double xmin, double xmax, 
											double ymin, double ymax,
											int just, int axis )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.setup_window_with_clear( xmin, xmax, ymin, ymax, just, axis );
        }

        public void finish_current_page()
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.finish_current_page();
        }

        public void xerror( double[] xmin, double[] xmax, double[] y )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.xerror( xmin, xmax, y );
        }

        public void yerror( double[] x, double[] ymin, double[] ymax )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.yerror( x, ymin, ymax );
        }

        public void advance_family()
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.advance_family();
        }

        public void polygon_filled( double[] x, double[] y )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.polygon_filled( x, y );
        }

        public void polygon_filled3d( double[] x, double[] y, double[] z )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.polygon_filled3d( x, y, z );
        }

        public void flush()
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.flush();
        }

        public void set_font(int ifont)
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_font(ifont);
        }

        public void load_font(int fnt)
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.load_font(fnt);
        }

        public void get_char_size( out double p_def, out double p_ht )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_char_size( out p_def, out p_ht );
        }

        public void get_map0_rgb ( int icol0, out int r, out int g, out int b )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_map0_rgb ( icol0, out r, out g, out b );
        }

        public void get_map0_rgba ( int icol0, out int r, out int g, out int b, out double alpha )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_map0_rgba ( icol0, out r, out g, out b, out alpha );
        }

        public void get_bg_rgb ( out int r, out int g, out int b )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_bg_rgb ( out r, out g, out b );
        }

        public void get_bg_rgba ( out int r, out int g, out int b, out double alpha )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_bg_rgba ( out r, out g, out b, out alpha );
        }

        public void get_compression ( out int compression )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_compression ( out compression );
        }

        public void get_current_dev_param ( out double p_mar, out double p_aspect,
											out double p_jx, out double p_jy )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_current_dev_param ( out p_mar, out p_aspect, out p_jx, out p_jy );
        }

        public void get_orientation( out double p_rot )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_orientation ( out p_rot );
        }

        public void get_plot_param( out double p_xmin, out double p_ymin,
			out double p_xmax, out double p_ymax )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_plot_param( out p_xmin, out p_ymin, out p_xmax, out p_ymax );
        }

        public void get_font_character( out uint32 p_fci )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_font_character( out p_fci );
        }
		
		public void get_family( out int p_fam, out int p_num, out int p_bmax )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_family( out p_fam, out p_num, out p_bmax );
        }
		
		public void get_font_name( out string fnam )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_font_name( out fnam );
        }
		
		public void get_current_font_param( out int p_family, out int p_style, out int p_weight )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_current_font_param( out p_family, out p_style, out p_weight );
        }
		
		public void get_run_level( out RunLevel p_level )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_run_level( out p_level );
        }
		
		public void get_page_param( out double p_xp, out double p_yp,
									out int p_xleng, out int p_yleng,
									out int p_xoff, out int p_yoff )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_page_param( out p_xp, out p_yp, out p_xleng, out p_yleng, out p_xoff, out p_yoff );
        }
		
		public void switch_to_graphic()
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.switch_to_graphic();
        }
		
		public void gradient( double[] x, double[] y, double angle )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.gradient( x, y, angle );
        }
		
		public void irregular_sampled_data( double[] x, double[] y, double[] z,
											double[] xg, double[] yg,
											double[,] zg, int type, double data )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.irregular_sampled_data( x, y, z, xg, yg, zg, type, data );
        }
		
		public void get_subpage_param ( out double xmin, out double xmax,
										out double ymin, out double ymax )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_subpage_param ( out xmin, out xmax, out ymin, out ymax );
        }
		
		public void get_version( out string p_ver )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_version( out p_ver );
        }
		
		public void get_viewport_in_device( out double p_xmin, out double p_xmax,
											out double p_ymin, out double p_ymax )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_viewport_in_device(out p_xmin, out p_xmax, out p_ymin, out p_ymax );
        }
		
		public void get_viewport_in_world( out double p_xmin, out double p_xmax,
											out double p_ymin, out double p_ymax )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_viewport_in_world( out p_xmin, out p_xmax, out p_ymin, out p_ymax );
        }
		
		public void get_xaxis( out int p_digmax, out int p_digits )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_xaxis( out p_digmax, out p_digits );
        }
		
		public void get_yaxis( out int p_digmax, out int p_digits )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_yaxis( out p_digmax, out p_digits );
        }

		public void get_zaxis( out int p_digmax, out int p_digits )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_zaxis( out p_digmax, out p_digits );
        }
		
		public void unbined_histogram( double[] data, double datmin, double datmax,
										int nbin, int opt )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.unbined_histogram( data, datmin, datmax, nbin, opt );
        }
		
		public void get_current_stream( out int p_strm )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_current_stream( out p_strm );
        }
		
		public void line_simple( double x1, double y1, double x2, double y2 )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.line_simple( x1, y1, x2, y2 );
        }
		
		public void label( string xlabel, string ylabel, string tlabel )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.label( xlabel, ylabel, tlabel );
        }
		
		public void legend( out double p_legend_width, out double p_legend_height,
							int opt, int position, double x, double y, double plot_width,
							int bg_color, int bb_color, int bb_style,
							int nrow, int ncolumn,
							int[] opt_array,
							double text_offset, double text_scale, double text_spacing,
							double text_justification,
							int[] text_colors, string[] text,
							int[] box_colors, int[] box_patterns,
							double[] box_scales, double[] box_line_widths,
							int[] line_colors, int[] line_styles,
							double[] line_widths,
							int[] symbol_colors, double[] symbol_scales,
							int[] symbol_numbers, string[] symbols )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.legend( out p_legend_width, out p_legend_height, opt, position, x, y, plot_width, bg_color,
				bb_color, bb_style, nrow, ncolumn, opt_array, text_offset, text_scale, text_spacing, text_justification, text_colors, text, box_colors, box_patterns, box_scales, box_line_widths, line_colors, line_styles, line_widths,
				symbol_colors, symbol_scales, symbol_numbers, symbols );
        }
		
		public void color_bar( out double p_colorbar_width, out double p_colorbar_height,
							  int opt, int position, double x, double y,
							  double x_length, double y_length,
							  int bg_color, int bb_color, int bb_style,
							  double low_cap_color, double high_cap_color,
							  int cont_color, double cont_width,
							  int[] label_opts, string[] labels,
							  string[] axis_opts, double[] ticks, int[] sub_ticks,
							  int[] n_values, double* values )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.color_bar( out p_colorbar_width, out p_colorbar_height, opt, position, x, y,
						x_length, y_length, bg_color, bb_color, bb_style,
						low_cap_color, high_cap_color, cont_color, cont_width,
                        label_opts, labels, 
                        axis_opts,	ticks, sub_ticks, 
                        n_values, values );
        }
		
		public void light_source( double x, double y, double z )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.light_source( x, y, z );
        }
		
		public void line( double[] x, double[] y )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.line( x, y );
        }
		
		public void line3d( double[] x, double[] y, double[] z )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.line3d( x, y, z );
        }
		
		public void line_style(int lin)
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.line_style(lin);
        }
		
		public void shapefile_data( MapFormFunc mapform, string name,
									double minx, double maxx, double miny, double maxy )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.shapefile_data( mapform, name, minx, maxx, miny, maxy );
        }
		
		public void shapefile_data_line( MapFormFunc mapform, string name,
										double minx, double maxx, double miny, double maxy, 
										int[] plotentries)
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.shapefile_data_line(mapform, name, minx, maxx, miny, maxy,
								plotentries);
        }
		
		public void shapefile_data_string( MapFormFunc mapform, string name, string text,
										double minx, double maxx, double miny, double maxy,
										int[] plotentries)
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.shapefile_data_string( mapform, name, text, minx, maxx, miny, maxy,
									plotentries);
        }
		
		public void shapefile_data_text( MapFormFunc mapform, string name, 
										double dx, double dy, double just, string text,
										double minx, double maxx, double miny, double maxy, 
										int plotentry)
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.shapefile_data_text( mapform, name, dx, dy, just, text,
									minx, maxx, miny, maxy, plotentry);
        }
		
		public void shapefile_data_polygon( MapFormFunc mapform, string name,
											double minx, double maxx, double miny, double maxy, 
											int[] plotentries)
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.shapefile_data_polygon( mapform, name, minx, maxx, miny, maxy,
									plotentries);
        }
		
		public void meridians( MapFormFunc mapform, double dlong, double dlat,
								double minlong, double maxlong, double minlat, double maxlat )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.meridians( mapform, dlong, dlat, minlong, maxlong, minlat, maxlat );
        }
		
		public void mesh( double[] x, double[] y, double[,] z, int opt )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.mesh( x, y, z, opt );
        }
		
		public void mesh_with_contour( double[] x, double[] y, double[,] z,
										int opt, double[] clevel )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.mesh_with_contour( x, y, z, opt, clevel );
        }
		
		public void text( string side, double disp, double pos, double just, string text )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.text( side, disp, pos, just, text );
        }
		
		public void text3d( string side, double disp, double pos, double just, string text )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.text3d( side, disp, pos, just, text );
        }
		
		public void suface3d(double[] x, double[] y, double[,] z, 
								int opt, double[] clevel)
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.suface3d(x, y, z, opt, clevel);
        }
		
		public void suface3d_limit(double[] x, double[] y, double[,] z,
									int opt, double[] clevel,
									int indexxmin, int[] indexymin, int[] indexymax)
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.suface3d_limit(x, y, z, opt, clevel, 
							indexxmin, indexymin, indexymax);
        }
		
		public void plot3d( double[] x, double[] y, double[,] z,
							int opt, bool side )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.plot3d( x, y, z, opt, side );
        }
		
		public void plot3d_with_contour( double[] x, double[] y, double[,] z,
										int opt, double[] clevel )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.plot3d_with_contour( x, y, z, opt, clevel );
        }
		
		public void plot3d_limit_with_contour( double[] x, double[] y, double[,] z,
					int opt, double[] clevel,
					int indexxmin, int[] indexymin, int[] indexymax )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.plot3d_limit_with_contour( x, y, z, opt, clevel,  
										indexxmin, indexymin, indexymax );
        }
		
		public int parse_opts(ref string[] argv, int mode )
        {
            PlPlot.set_current_stream( _stream );            
            return PlPlot.parse_opts(ref argv, mode );
        }
		
		public void set_fill_pattern( int[] inc, int[] del )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_fill_pattern( inc, del );
        }
		
		public void path( int n, double x1, double y1, double x2, double y2 )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.path( n, x1, y1, x2, y2 );
        }
		
		public void glyph( double[] x, double[] y, int code )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.glyph( x, y, code );
        }
		
		public void glyph3d( double[] x, double[] y, double[] z, int code )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.glyph3d( x, y, z, code );
        }
		
	//TODO: poly3
		
		public void set_precision( int setp, int prec )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_precision( setp, prec );
        }
		
		public void set_area_fill_pattern( int patt )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_area_fill_pattern( patt );
        }
		
		public void text_in_world( double x, double y, double dx, double dy,
									double just, string text )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.text_in_world( x, y, dx, dy, just, text );
        }
		
		public void text3d_in_world( double wx, double wy, double wz,
									double dx, double dy, double dz,
									double sx, double sy, double sz,
									double just, string text )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.text3d_in_world( wx, wy, wz, dx, dy, dz, sx, sy, sz, just, text );
        }
		
	//TODO: translatecursor
		
		public void replot()
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.replot();
        }
		
		public void set_character_size( double def, double scale )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_character_size( def, scale );
        }
		
		public void set_map0_number( int ncol0 )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_map0_number( ncol0 );
        }
		
		public void set_map1_number( int ncol1 )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_map1_number( ncol1 );
        }
		
		public void set_map1_range( double min_color, double max_color )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_map1_range( min_color, max_color );
        }
		
		public void get_map1_range( out double min_color, out double max_color )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_map1_range( out min_color, out max_color );
        }
		
		public void set_map0( int[] r, int[] g, int[] b )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_map0( r, g, b );
        }
		
		public void set_map0_rgba( int[] r, int[] g, int[] b, double[] alpha )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_map0_rgba( r, g, b, alpha );
        }
		
		public void set_map1( int[] r, int[] g, int[] b )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_map1( r, g, b );
        }
		
		public void set_map1_rgba( int[] r, int[] g, int[] b, double[] alpha )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_map1_rgba( r, g, b, alpha );
        }
		
	//TODO: scmap1l
	
	//TODO: scmap1la
		
		public void set_map0_by_id( int icol0, int r, int g, int b )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_map0_by_id( icol0, r, g, b );
        }
		
		public void set_map0_rgba_by_id( int icol0, int r, int g, int b, double alpha )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_map0_rgba_by_id( icol0, r, g, b, alpha );
        }
		
		public void set_bg( int r, int g, int b )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_bg( r, g, b );
        }
		
		public void set_bg_rgba( int r, int g, int b, double alpha )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_bg_rgba( r, g, b, alpha );
        }
		
		public void color_enable( int color )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.color_enable( color );
        }
		
		public void set_compression( int compression )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_compression( compression );
        }
		
		public void set_current_dev ( string p_dev )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_current_dev ( p_dev );
        }

		public void get_current_dev ( out string p_dev )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.get_current_dev ( out p_dev );
        }
		
		public void set_current_dev_param ( double mar, double aspect, double jx, double jy )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_current_dev_param ( mar, aspect, jx, jy );
        }
		
		public void set_transform_metafile( int dimxmin, int dimxmax, int dimymin, int dimymax,
											double dimxpmm, double dimypmm )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_transform_metafile( dimxmin, dimxmax, dimymin, dimymax, dimxpmm, dimypmm );
        }
		
		public void set_orientation(double rot)
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_orientation(rot);
        }
		
		public void set_plot_param( double p_xmin, double p_ymin, double p_xmax, double p_ymax )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_plot_param( p_xmin, p_ymin, p_xmax, p_ymax );
        }
		
		public void set_plot_zoom( double xmin, double ymin, double xmax, double ymax )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_plot_zoom( xmin, ymin, xmax, ymax );
        }
		
		public void escape_char(uint8 char)
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.escape_char(char);
        }
		
		public void setcontlabelparam( double offset, double size, double spacing, int active )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.setcontlabelparam( offset, size, spacing, active );
        }
		
		public void setcontlabelformat( int lexp, int sigdig )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.setcontlabelformat( lexp, sigdig );
        }
		
		public void set_family( int fam, int num, int bmax )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_family( fam, num, bmax );
        }
		
		public void set_font_character( uint32 p_fci )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_font_character( p_fci );
        }

		public void set_output_file_name( string fnam )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_output_file_name( fnam );
        }
		
	//TODO: sdevdata	
	
		public void set_current_font_param( int p_family, int p_style, int p_weight )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_current_font_param( p_family, p_style, p_weight );
        }
		
		public void shade( double[,] a, DefinedFunc defined,
						   double xmin, double xmax, double ymin, double ymax,
						   double shade_min, double shade_max,
						   int sh_cmap, double sh_color, double sh_width,
						   int min_color, double min_width,
						   int max_color, double max_width,
						   FillFunc fill, bool rectangular,
						   TransformFunc pltr )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.shade( a, defined, xmin, xmax, ymin, ymax, shade_min, shade_max,
					sh_cmap, sh_color, sh_width, min_color, min_width,
					max_color, max_width, fill, rectangular, pltr );
        }
		
		public void shades( double[,] a, DefinedFunc defined,
							double xmin, double xmax, double ymin, double ymax,
							double[] clevel, double fill_width,
							int cont_color, double cont_width, FillFunc fill,
							bool rectangular, TransformFunc pltr )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.shades( a, defined, xmin, xmax, ymin, ymax, clevel, fill_width,
						cont_color, cont_width, fill, rectangular, pltr );
        }
		
	//TODO: shade	if()fshade	else()fshade
	
	//TODO: shade1
	
	//TODO: fshade
	
		public void set_label_function( LabelFunc label_func )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_label_function( label_func );
        }
		
		public void set_major_tick_len( double def, double scale )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_major_tick_len( def, scale );
        }
		
		public void set_memory( uint8[,] plotmem )
        {
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_memory( plotmem );
        }
		
		public void set_memory_rgba( int maxx, int maxy, uint8[] plotmem )
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_memory_rgba( maxx, maxy, plotmem );
        }
		
		public void set_minor_tick_len( double def, double scale )
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_minor_tick_len( def, scale );
        }
		
		public void set_orientation_int( int ori )
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_orientation_int( ori );
        }
		
		public void set_page_param( double xp, double yp, int xleng, int yleng,
									int xoff, int yoff )
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_page_param( xp, yp, xleng, yleng, xoff, yoff );
        }
		
		public void set_map0_palette( string filename )
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_map0_palette( filename );
        }
		
		public void set_map1_palette( string filename, bool interpolate )
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_map1_palette( filename, interpolate );
        }
		
		public void set_pause( bool pause )
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_pause( pause );
        }
		
		public void set_current_stream( int strm )
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_current_stream( strm );
        }
		
		public void set_subpage( int nx, int ny )
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_subpage( nx, ny );
        }
		
		public void set_symbol_size( double def, double scale )
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_symbol_size( def, scale );
        }
		
		public void init_plot( int nx, int ny )
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.init_plot( nx, ny );
        }
		
		public void init_plot_on_dev( string devname, int nx, int ny )
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.init_plot_on_dev( devname, nx, ny );
        }
		
		public void set_transform( TransformFunc coordinate_transform )
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_transform( coordinate_transform );
        }
		
		public void plot_string(double[] x, double[] y, string text)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.plot_string(x, y, text);
        }
		
		public void plot_string3d(double[] x, double[] y, double[] z, string text)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.plot_string3d(x, y, z, text);
        }
		
		public void strip(out int id, string xspec, string yspec,
						double xmin, double xmax, double xjump, double ymin, double ymax,
						double xlpos, double ylpos,
						bool y_ascl, bool acc,
						int colbox, int collab,
						int[] colline, int[] styline, string[] legline,
						string labx, string laby, string labtop)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.strip(out id, xspec, yspec, xmin, xmax, xjump, ymin, ymax,
					xlpos, ylpos, y_ascl, acc, colbox, collab,
					colline, styline, legline, labx, laby, labtop);
        }
		
		public void strip_add_point(int id, int pen, int x, int y)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.strip_add_point(id, pen, x, y);
        }
		
		public void delete_strip(int id)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.delete_strip(id);
        }
		
		public void autocolor_maxtrix_in_map1(double[,] idata,
            double xmin, double xmax, double ymin, double ymax, double zmin, double zmax,
            double Dxmin, double Dxmax, double Dymin, double Dymax)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.autocolor_maxtrix_in_map1(idata, xmin, xmax, ymin, ymax, zmin, zmax,
										Dxmin, Dxmax, Dymin, Dymax);
        }
		
		public void maxtrix_in_map1(double[,] idata, 
            double xmin, double xmax, double ymin, double ymax, double zmin, double zmax,
            double valuemin, double valuemax, TransformFunc pltr)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.maxtrix_in_map1(idata, xmin, xmax, ymin, ymax, zmin, zmax,
							valuemin, valuemax, pltr);
        }
		
		public void set_line_style(int[] mark, int[] space)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_line_style(mark, space);
        }
		
		public void set_viewport(double xmin, double xmax, double ymin, double ymax)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_viewport(xmin, xmax, ymin, ymax);
        }
		
		public void set_xaxis(int digmax, int digits)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_xaxis(digmax, digits);
        }
		
	//TODO: sxwin
		
		public void set_yaxis(int digmax, int digits)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_yaxis(digmax, digits);
        }
		
		public void glyph_on_pos(double[] x, double[] y, int code)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.glyph_on_pos(x, y, code);
        }
		
		public void set_zaxis(int digmax, int digits)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_zaxis(digmax, digits);
        }
		
		public void switch_to_text()
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.switch_to_text();
        }
		
		public void set_time_format(string fmt)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_time_format(fmt);
        }
		
		public void select_viewport_by_ratio (double aspect)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.select_viewport_by_ratio (aspect);
        }
		
		public void select_viewport_by_coord_ratio (double xmin, double xmax,
													double ymin, double ymax, 
													double aspect)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.select_viewport_by_coord_ratio (xmin, xmax, ymin, ymax, aspect);
        }
		
		public void select_viewport_by_normal_subpage(double xmin, double xmax,
														double ymin, double ymax)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.select_viewport_by_normal_subpage(xmin, xmax, ymin, ymax);
        }
		
		public void select_viewport()
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.select_viewport();
        }
		
		public void set_3dto2d_transform(double basex, double basey, double height,
										double xmin, double xmax,
										double ymin, double ymax,
										double zmin, double zmax,
										double alt, double az)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_3dto2d_transform(basex, basey, height,
								xmin, xmax, ymin, ymax, zmin, zmax, alt, az);
        }
		
		public void set_pen_width(double width)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_pen_width(width);
        }
		
		public void select_window(double xmin, double xmax, double ymin, double ymax)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.select_window(xmin, xmax, ymin, ymax);
        }
		
		public void enable_xormode(bool mode, out bool status)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.enable_xormode(mode, out status);
        }
		
		public void seed(uint seed)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.seed(seed);
        }
		
		public double rand()
		{
            PlPlot.set_current_stream( _stream );            
            return PlPlot.rand();
        }
		
	//TODO: gFileDevs
	
	//TODO: sKeyEH
	
	//TODO: sError
	
	//TODO: sexit
	
	//TODO: f2eval2
	
	//TODO: f2eval
	
	//TODO: f2evalr
	
	//TODO: ClearOpts
	
	//TODO: ResetOpts
	
	//TODO: MergeOpts
	
	//TODO: SetUsage
	
		public void set_options(string opt, string optarg)
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.set_options(opt, optarg);
        }

	//TODO: OptUsage
	
	//TODO：gfile
	
	//TODO: sfile
	
	//TODO: gesc
	
	//TODO: cmd
	
	//TODO: FindName
	
	//TODO: FindCommand
	
	//TODO: GetName
	
	//TODO: GetInt
	
	//TODO: GetFlt
	
	//TODO: Static2dGrid
	
	//TODO: Alloc2dGrid
	
	//TODO: Free2dGrid
	
	//TODO: MinMax2dGrid
	
		public void hls_to_rgb( double h, double l, double s,
								out double p_r, out double p_g, out double p_b )
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.hls_to_rgb( h, l, s, out p_r, out p_g, out p_b );
        }
		
		public void rgb_to_hls( double r, double g, double b, 
								out double p_h, out double p_l, out double p_s )
		{
            PlPlot.set_current_stream( _stream );            
            PlPlot.rgb_to_hls( r, g, b, out p_h, out p_l, out p_s );
        }
		
	//TODO: GetCursor
	
	//  #ifdef PL_DEPRECATED
	//  	// Deprecated version using PLINT instead of bool		
	//  	public void set_arrow_style_for_vector(double[] arrowx, double[] arrowy,
	//  											int npts, bool fill)
	//  	{
    //          PlPlot.set_current_stream( _stream );            
    //          PlPlot.set_arrow_style_for_vector(arrowx, arrowy, npts, fill);
    //      }
		
	//  	// Deprecated version using PLINT not bool
	//  	public void set_arrow_style_for_vector(double[] arrowx, double[] arrowy,
	//  											int npts, bool fill)
	//  	{
    //          PlPlot.set_current_stream( _stream );            
    //          PlPlot.copy( iplsr, flags );
    //      }
		
	//  	// Deprecated version using PLINT not bool
	//  	public void plot3d( double[] x, double[] y, double[,] z,
	//  						int nx, int ny, int opt, bool side )
	//  	{
    //          PlPlot.set_current_stream( _stream );            
    //          PlPlot.plot3d( x, y, z, nx, ny, opt, side );
    //      }
		
	//  	//TODO: poly3
		
	//  	//TODO: scmap1l
		
	//  	// Deprecated version using PLINT instead of bool
	//  	public void shade( double[,] a, int nx, int ny, DefinedFunc defined,
	//  					   double xmin, double xmax, double ymin, double ymax,
	//  					   double shade_min, double shade_max,
	//  					   int sh_cmap, double sh_color, double sh_width,
	//  					   int min_color, double min_width,
	//  					   int max_color, double max_width,
	//  					   FillFunc fill, bool rectangular,
	//  					   TransformFunc pltr )
	//  	{
    //          PlPlot.set_current_stream( _stream );            
    //          PlPlot.shade( a, nx, ny, defined, xmin, xmax, ymin, ymax, shade_min, shade_max,
	//  			sh_cmap, sh_color, sh_width, min_color, min_width, max_color, max_width,
	//  			fill, rectangular, pltr );
    //      }
		
	//  	// Deprecated version using PLINT instead of bool
	//  	public void shades( double[,] a, int nx, int ny, DefinedFunc defined,
	//  						double xmin, double xmax, double ymin, double ymax,
	//  						double[] clevel, int nlevel, double fill_width,
	//  						int cont_color, double cont_width,
	//  						FillFunc fill, bool rectangular,
	//  						TransformFunc pltr )
	//  	{
    //          PlPlot.set_current_stream( _stream );            
	//  		PlPlot.shades( a, nx, ny, defined, min, xmax, ymin, ymax, clevel, nlevel, fill_width,
    //              cont_color, cont_width, fill, rectangular, pltr );
    //      }

	//  	// Deprecated version using PLINT not bool
	//  	public void shade( double[,] a, int nx, int ny, DefinedFunc defined,
	//  					   double xmin, double xmax, double ymin, double ymax,
	//  					   double shade_min, double shade_max,
	//  					   int sh_cmap, double sh_color, double sh_width,
	//  					   int min_color, double min_width,
	//  					   int max_color, double max_width,
	//  					   FillFunc fill, bool rectangular,
	//  					   TransformFunc pltr )
	//  	{
	//  		int nx, ny;
	//  		d.elements( nx, ny );
			
    //          PlPlot.set_current_stream( _stream );            
	//  		PlPlot.shade( a, nx, ny, defined, xmin, xmax, ymin, ymax, shade_min, shade_max,
	//  			sh_cmap, sh_color, sh_width, min_color, min_width, max_color, max_width,
	//  			fill, rectangular, pltr );
    //      }
		
		
	//  	//TODO: fshade
		
	//  	// Deprecated version using PLINT not bool
	//  	public void set_pause( bool pause )
	//  	{
    //          PlPlot.set_current_stream( _stream );            
	//  		PlPlot.set_pause( pause );
    //      }
		
	//  	// Deprecated version using PLINT not bool
	//  	public void strip(out int id, string xspec, string yspec,
    //          double xmin, double xmax, double xjump, double ymin, double ymax,
    //          double xlpos, double ylpos,
    //          bool y_ascl, bool acc,
    //          int colbox, int collab,
    //          int[] colline, int[] styline, string[] legline,
    //          string labx, string laby, string labtop)
	//  	{
    //          PlPlot.set_current_stream( _stream );            
	//  		PlPlot.strip(id, xspec, yspec, xmin, xmax, xjump, ymin, ymax, 
	//  				xlpos, ylpos, y_ascl, acc, colbox, collab,
	//  				colline, styline, legline, labx, laby, labtop);
    //      }
		
	//  	// Deprecated version using PLINT not bool
	//  	public void enable_xormode(bool mode, out bool status)
	//  	{
    //          PlPlot.set_current_stream( _stream );            
	//  		PlPlot.enable_xormode(mode, status);
    //      }
		
    //  #endif //PL_DEPRECATED
    }
    
}