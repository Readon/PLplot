[CCode (cheader_filename="plplot.h")]
namespace PlPlot {
	[CCode (cname = "pl_cmd")]
	void command( int op, void * ptr );

    [CCode (cname = "c_pl_setcontlabelformat")]
    void setcontlabelformat( int lexp, int sigdig );

    [CCode (cname = "c_pl_setcontlabelparam")]
    void setcontlabelparam( double offset, double size, double spacing, int active );
    
    [CCode (cname = "c_pladv")]
    void advance_page( int page );
    
    [CCode (cname = "c_plarc")]
    void arc( double x, double y, double a, double b, double angle1, double angle2,
            double rotate, bool fill );
             
    [CCode (cname = "c_plaxes")]
    void axes( double x0, double y0, string xopt, double xtick, int nxsub,
            string yopt, double ytick, int nysub );
    
    [CCode (cname = "c_plbin")]
    void histogram( [CCode (array_length_pos = 0)] double[] x, 
                [CCode (array_length = false)] double[] y, BinType opt );
    
    [CCode (cname = "c_plbop")]
    void new_page();
       
    [CCode (cname = "c_plbox")]
    void box( string xopt, double xtick, int nxsub,
             string yopt, double ytick, int nysub );
             
    [CCode (cname = "c_plbox3")]
    void box3d( string xopt, string xlabel, double xtick, int nxsub,
              string yopt, string ylabel, double ytick, int nysub,
              string zopt, string zlabel, double ztick, int nzsub );
    
    [CCode (cname = "c_plbtime")]
    void get_break_time_by_ctime( out int year, out int month, out int day, 
                         out int hour, out int min, out int sec, double ctime );
    
    [CCode (cname = "c_plcalc_world")]
    void get_world_by_dev_coord( double rx, double ry, out double wx, out double wy, out int window );

    [CCode (cname = "c_plclear")]
    void clear();

    [CCode (cname = "c_plcol0")]
    void set_color_map0(Color icol0);
    
    [CCode (cname = "c_plcol1")]
    void set_color_map1(Color icol1);
    
    [CCode (cname = "c_plcolorbar")]
    void color_bar( out double p_colorbar_width, out double p_colorbar_height,
              int opt, int position, double x, double y,
              double x_length, double y_length,
              int bg_color, int bb_color, int bb_style,
              double low_cap_color, double high_cap_color,
              int cont_color, double cont_width,
              [CCode (array_length_pos = 15.9)] int[] label_opts, 
              [CCode (array_length = false)] string[] labels,
              [CCode (array_length_pos = 17.9)] string[] axis_opts,
              [CCode (array_length = false)] double[] ticks, 
              [CCode (array_length = false)] int[] sub_ticks,
              [CCode (array_length = false)] int[] n_values, 
              double *values );

    [CCode (cname = "c_plconfigtime")]
    void config_time_transform( double scale, double offset1, double offset2, 
                int ccontrol, bool ifbtime_offset, 
                int year, int month, int day, int hour, int min, double sec );

    [CCode (cname = "PLTRANSFORM_callback")]
    public delegate void TransformFunc( double x, double y, out double tx, out double ty);
    [CCode (cname = "c_plcont")]
    void contour( double[,] f, int kx, int lx,
            int ky, int ly, double[] clevel, TransformFunc pltr);

    [CCode (cname = "c_plcpstrm")]
    void copy( int iplsr, bool flags );
    
    [CCode (cname = "c_plctime")]
    void get_ctime_by_break_time( int year, int month, int day, int hour, int min, double sec, out double ctime );
    
    [CCode (cname = "c_plend")]
    void finish_plot();
    
    [CCode (cname = "c_plend1")]
    void finish_current_stream_plot();
    
    [CCode (cname = "c_plenv")]    
    void setup_window( double xmin, double xmax, double ymin, double ymax,
            int just, int axis );
                
    [CCode (cname = "c_plenv0")]    
    void setup_window_with_clear( double xmin, double xmax, double ymin, double ymax,
            int just, int axis );
            
    [CCode (cname = "c_pleop")]
    void finish_current_page();    
    
    [CCode (cname = "c_plerrx")]
    void xerror( [CCode (array_length_pos = 0)] double[] xmin, 
                [CCode (array_length = false)] double[] xmax, 
                [CCode (array_length = false)] double[] y );

    [CCode (cname = "c_plerry")]
    void yerror( [CCode (array_length_pos = 0)] double[] x, 
                [CCode (array_length = false)] double[] ymin, 
                [CCode (array_length = false)] double[] ymax );
    
    [CCode (cname = "c_plfamadv")]
    void advance_family();

    [CCode (cname = "c_plfill")]
    void polygon_filled( [CCode (array_length_pos = 0)] double[] x, 
                [CCode (array_length = false)] double[] y );

    [CCode (cname = "c_plfill3")]
    void polygon_filled3d( [CCode (array_length_pos = 0)] double[] x, 
                [CCode (array_length = false)] double[] y, 
                [CCode (array_length = false)] double[] z );

    [CCode (cname = "c_plflush")]
    void flush();

    [CCode (cname = "c_plfont")]
    void set_font(int ifont);
    
    [CCode (cname = "c_plfontld")]
    void load_font(int fnt);
    
    [CCode (cname = "c_plgchr")]
    void get_char_size( out double p_def, out double p_ht );
    
    [CCode (cname = "c_plgcmap1_range")]
    void get_map1_range( out double min_color, out double max_color );

    [CCode (cname = "c_plgcol0")]
    void get_map0_rgb ( int icol0, out int r, out int g, out int b );

    [CCode (cname = "c_plgcol0a")]
    void get_map0_rgba ( int icol0, out int r, out int g, out int b, out double alpha );

    [CCode (cname = "c_plgcolbg")]
    void get_bg_rgb ( out int r, out int g, out int b );

    [CCode (cname = "c_plgcolbga")]
    void get_bg_rgba ( out int r, out int g, out int b, out double alpha );
    
    [CCode (cname = "c_plgcompression")]
    void get_compression ( out int compression );

    [CCode (cname = "c_plgdev")]
    void get_current_dev ( out string p_dev );

    [CCode (cname = "c_plgdidev")]
    void get_current_dev_param ( out double p_mar, out double p_aspect, out double p_jx, out double p_jy );
    
    [CCode (cname = "c_plgdiori")]    
    void get_orientation( out double p_rot );

    [CCode (cname = "c_plgdiplt")]    
    void get_plot_param( out double p_xmin, out double p_ymin, out double p_xmax, out double p_ymax );

    [CCode (cname = "c_plgdrawmode")]
    int get_draw_mode();
    
    [CCode (cname = "c_plgfam")]
    void get_family( out int p_fam, out int p_num, out int p_bmax );
    
    [CCode (cname = "c_plgfci")]    
    void get_font_character( out uint32 p_fci );
    
    [CCode (cname = "c_plgfnam")]    
    void get_font_name( out string fnam );
    
    [CCode (cname = "c_plgfont")]    
    void get_current_font_param( out int p_family, out int p_style, out int p_weight );

    [CCode (cname = "c_plglevel")]    
    void get_run_level( out RunLevel p_level );

    [CCode (cname = "c_plgpage")] 
    void get_page_param( out double p_xp, out double p_yp,
        out int p_xleng, out int p_yleng, out int p_xoff, out int p_yoff );

    [CCode (cname = "c_plgra")]    
    void switch_to_graphic();
    
    [CCode (cname = "c_plgradient")]    
    void gradient( [CCode (array_length_pos = 0)] double[] x, 
        [CCode (array_length = false)]double[] y, double angle );
    
    [CCode (cname = "c_plgriddata")]  
    void irregular_sampled_data( [CCode (array_length = false)] double[] x, 
                [CCode (array_length = false)] double[] y, 
                double[] z, double[] xg, double[] yg,
                [CCode (array_length = false)] double[,] zg, 
                int type, double data );

    [CCode (cname = "c_plgspa")]
    void get_subpage_param ( out double xmin, out double xmax, out double ymin, out double ymax );

    [CCode (cname = "c_plgstrm")]
    void get_current_stream( out int p_strm );
    
    [CCode (cname = "c_plgver")]
    void get_version( out string p_ver );

    [CCode (cname = "c_plgvpd")]
    void get_viewport_in_device( out double p_xmin, out double p_xmax, out double p_ymin, out double p_ymax );

    [CCode (cname = "c_plgvpw")]
    void get_viewport_in_world( out double p_xmin, out double p_xmax, out double p_ymin, out double p_ymax );

    [CCode (cname = "c_plgxax")]    
    void get_xaxis( out int p_digmax, out int p_digits );

    [CCode (cname = "c_plgyax")]    
    void get_yaxis( out int p_digmax, out int p_digits );

    [CCode (cname = "c_plgzax")]    
    void get_zaxis( out int p_digmax, out int p_digits );  
    
    [CCode (cname = "c_plhist")]    
    void unbined_histogram( [CCode (array_length_pos = 0)] double[] data, double datmin, double datmax,
            int nbin, int opt );

    [CCode (cname = "c_plhlsrgb")]
    void hls_to_rgb( double h, double l, double s, out double p_r, out double p_g, out double p_b );

    [CCode (cname = "c_plimage")]
    void autocolor_maxtrix_in_map1(double[,] idata, 
            double xmin, double xmax, double ymin, double ymax, double zmin, double zmax,
            double Dxmin, double Dxmax, double Dymin, double Dymax); 

    [CCode (cname = "c_plimagefr")]
    void maxtrix_in_map1(double[,] idata,
            double xmin, double xmax, double ymin, double ymax, double zmin, double zmax,
            double valuemin, double valuemax, TransformFunc pltr); 
    
    [CCode (cname = "c_plinit")]
    void init();
    
    [CCode (cname = "c_pljoin")]
    void line_simple( double x1, double y1, double x2, double y2 );

    [CCode (cname = "c_pllab")]
    void label( string xlabel, string ylabel, string tlabel );

    [CCode (cname = "c_pllegend")]
    void legend( out double p_legend_width, out double p_legend_height,
                int opt, int position, double x, double y, double plot_width,
                int bg_color, int bb_color, int bb_style,
                int nrow, int ncolumn,
                [CCode (array_length_pos = 12.9)] int[] opt_array,
                double text_offset, double text_scale, double text_spacing,
                double text_justification,
                [CCode (array_length = false)] int[] text_colors, 
                [CCode (array_length = false)] string[] text,
                [CCode (array_length = false)] int[] box_colors, 
                [CCode (array_length = false)] int[] box_patterns,
                [CCode (array_length = false)] double[] box_scales, 
                [CCode (array_length = false)] double[] box_line_widths,
                [CCode (array_length = false)] int[] line_colors, 
                [CCode (array_length = false)] int[] line_styles,
                [CCode (array_length = false)] double[] line_widths,
                [CCode (array_length = false)] int[] symbol_colors, 
                [CCode (array_length = false)] double[] symbol_scales,
                [CCode (array_length = false)] int[] symbol_numbers, 
                [CCode (array_length = false)] string[] symbols );

    [CCode (cname = "c_pllightsource")]
    void light_source( double x, double y, double z );
    
    [CCode (cname = "c_plline")]
    void line( [CCode (array_length_pos = 0)] double[] x, 
                [CCode (array_length = false)] double[] y );

    [CCode (cname = "c_plpath")]
    void path( int n, double x1, double y1, double x2, double y2 );

    [CCode (cname = "c_plline3")]
    void line3d([CCode (array_length_pos = 0)] double[] x, 
                [CCode (array_length = false)] double[] y, 
                [CCode (array_length = false)] double[] z );
    
    [CCode (cname = "c_pllsty")]
    void line_style(int lin);

    [CCode (cname = "PLMAPFORM_callback", has_target=false, delegate_target=false)]
    public delegate void MapFormFunc( int n, ref double[] x, ref double[] y);
    [CCode (cname = "c_plmap")]
    void shapefile_data( MapFormFunc mapform, string name,
             double minx, double maxx, double miny, double maxy );
    
    [CCode (cname = "c_plmapline")]
    void shapefile_data_line( MapFormFunc mapform, string name,
            double minx, double maxx, double miny, double maxy, 
            int[] plotentries);
            
    [CCode (cname = "c_plmapstring")]
    void shapefile_data_string( MapFormFunc mapform, string name, string text,
                double minx, double maxx, double miny, double maxy,
                int[] plotentries);

    [CCode (cname = "c_plmaptex")]
    void shapefile_data_text( MapFormFunc mapform, string name, 
                double dx, double dy, double just, string text,
                double minx, double maxx, double miny, double maxy, 
                int plotentry);

    [CCode (cname = "c_plmapfill")]
    void shapefile_data_polygon( MapFormFunc mapform, string name,
            double minx, double maxx, double miny, double maxy, 
            int[] plotentries);

    [CCode (cname = "c_plmeridians")]
    void meridians( MapFormFunc mapform, double dlong, double dlat,
            double minlong, double maxlong, double minlat, double maxlat );

    [CCode (cname = "c_plmesh")]
    void mesh( [CCode (array_length_pos = 3.1)] double[] x, 
                [CCode (array_length_pos = 3.2)] double[] y, 
                [CCode (array_length = false)] double[,] z, int opt );
                
    [CCode (cname = "c_plmeshc")]
    void mesh_with_contour( [CCode (array_length_pos = 3.1)] double[] x, 
                [CCode (array_length_pos = 3.2)] double[] y, 
                [CCode (array_length = false)] double[,] z,
                int opt, double[] clevel );
    
    [CCode (cname = "c_plmkstrm")]    
    void make_stream( out int p_strm );

    [CCode (cname = "c_plmtex")]    
    void text( string side, double disp, double pos, double just,
            string text );
    
    [CCode (cname = "c_plmtex3")]    
    void text3d( string side, double disp, double pos, double just,
            string text );

    [CCode (cname = "c_plot3d")]      
    void plot3d( [CCode (array_length_pos = 3.1)] double[] x, 
                [CCode (array_length_pos = 3.2)] double[] y, 
                [CCode (array_length = false)] double[,] z,
                int opt, bool side ); 

    [CCode (cname = "c_plot3dc")]      
    void plot3d_with_contour( [CCode (array_length_pos = 3.1)] double[] x, 
                [CCode (array_length_pos = 3.2)] double[] y, 
                [CCode (array_length = false)] double[,] z,
                int opt, double[] clevel );

    [CCode (cname = "c_plot3dcl")]      
    void plot3d_limit_with_contour( [CCode (array_length_pos = 3.1)] double[] x, 
                [CCode (array_length_pos = 3.2)] double[] y, 
                [CCode (array_length = false)] double[,] z,
                int opt, double[] clevel, 
                int indexxmin, [CCode (array_length_pos = 6.9)] int[] indexymin,
                [CCode (array_length = false)] int[] indexymax );
    
    [CCode (cname = "c_plparseopts")] 
    int parse_opts([CCode (array_length_pos = 0)] ref string[] argv, int mode );

    [CCode (cname = "c_plpat")]     
    void set_fill_pattern( [CCode (array_length_pos = 0)] int[] inc, 
                [CCode (array_length = false)] int[] del );
    
    [CCode (cname = "c_plpoin")] 
    void glyph( [CCode (array_length_pos = 0)] double[] x, 
                [CCode (array_length = false)] double[] y, int code );
    
    [CCode (cname = "c_plpoin3")] 
    void glyph3d( [CCode (array_length_pos = 0)] double[] x, 
                [CCode (array_length = false)] double[] y, 
                [CCode (array_length = false)] double[] z, int code );
    
    [CCode (cname = "c_plpoly3")] 
    void polygon3d( int n, double[] x, double[] y, double[] z, bool[] draw, bool ifcc );

    [CCode (cname = "c_plprec")]     
    void set_precision( int setp, int prec );

    [CCode (cname = "c_plpsty")]     
    void set_area_fill_pattern( int patt );

    [CCode (cname = "c_plptex")]     
    void text_in_world( double x, double y, double dx, double dy, double just, string text );

    [CCode (cname = "c_plptex3")]     
    void text3d_in_world( double wx, double wy, double wz, double dx, double dy, double dz,
            double sx, double sy, double sz, double just, string text );

    [CCode (cname = "c_plrandd")]
    double rand(); 

    [CCode (cname = "c_plreplot")]
    void replot(); 

    [CCode (cname = "c_plrgbhls")]
    void rgb_to_hls( double r, double g, double b, out double p_h, out double p_l, out double p_s ); 

    [CCode (cname = "c_plschr")]
    void set_character_size( double def, double scale );

    [CCode (cname = "c_plscmap0")]    
    void set_map0( [CCode (array_length_pos = 5)] int[] r, 
                   [CCode (array_length = false)] int[] g, 
                   [CCode (array_length = false)] int[] b);

    [CCode (cname = "c_plscmap0a")]    
    void set_map0_rgba( [CCode (array_length_pos = 5)] int[] r, 
                        [CCode (array_length = false)] int[] g, 
                        [CCode (array_length = false)] int[] b, 
                        [CCode (array_length = false)] double[] alpha );

    [CCode (cname = "c_plscmap0n")]    
    void set_map0_number( int ncol0 );

    [CCode (cname = "c_plscmap1")]    
    void set_map1( [CCode (array_length_pos = 5)] int[] r, 
                   [CCode (array_length = false)] int[] g, 
                   [CCode (array_length = false)] int[] b);

    [CCode (cname = "c_plscmap1a")]    
    void set_map1_rgba( [CCode (array_length_pos = 5)] int[] r, 
                        [CCode (array_length = false)] int[] g, 
                        [CCode (array_length = false)] int[] b, 
                        [CCode (array_length = false)] double[] alpha);

    [CCode (cname = "c_plscmap1l")]    
    void set_map1_linear( bool itype, int npts, double[] intensity,
            double[] coord1, double[] coord2, double[] coord3, bool[] alt_hue_path );

    [CCode (cname = "c_plscmap1la")]    
    void set_map1_linear_alpha( bool itype, int npts, double[] intensity,
            double[] coord1, double[] coord2, double[] coord3, double[] alpha, bool[] alt_hue_path );

    [CCode (cname = "c_plscmap1n")]    
    void set_map1_number( int ncol1 );

    [CCode (cname = "c_plscmap1_range")]    
    void set_map1_range( double min_color, double max_color );

    [CCode (cname = "c_plscol0")]    
    void set_map0_by_id( int icol0, int r, int g, int b );

    [CCode (cname = "c_plscol0a")]    
    void set_map0_rgba_by_id( int icol0, int r, int g, int b, double alpha );

    [CCode (cname = "c_plscolbg")]    
    void set_bg( int r, int g, int b );

    [CCode (cname = "c_plscolbga")]    
    void set_bg_rgba( int r, int g, int b, double alpha );

    [CCode (cname = "c_plscolor")]    
    void color_enable( int color );

    [CCode (cname = "c_plscompression")]    
    void set_compression( int compression );

    [CCode (cname = "c_plsdev")]
    void set_current_dev ( string p_dev );

    [CCode (cname = "c_plsdidev")]
    void set_current_dev_param ( double mar, double aspect, double jx, double jy );

    [CCode (cname = "c_plsdimap")]
    void set_transform_metafile( int dimxmin, int dimxmax, int dimymin, int dimymax,
            double dimxpmm, double dimypmm );

    [CCode (cname = "c_plsdiori")]
    void set_orientation(double rot);

    [CCode (cname = "c_plsdiplt")]
    void set_plot_param( double p_xmin, double p_ymin, double p_xmax, double p_ymax );

    [CCode (cname = "c_plsdiplz")]
    void set_plot_zoom( double xmin, double ymin, double xmax, double ymax );

    [CCode (cname = "c_plsdrawmode")]
    void set_draw_mode(int mode);

    [CCode (cname = "c_plseed")]
    void seed(uint seed);

    [CCode (cname = "c_plsesc")]
    void escape_char(uint8 char);

    [CCode (cname = "c_plsetopt")]
    void set_options(string opt, string optarg);

    [CCode (cname = "c_plsfam")]
    void set_family( int fam, int num, int bmax );

    [CCode (cname = "c_plsfci")]
    void set_font_character( uint32 p_fci );
    
    [CCode (cname = "c_plsfnam")]    
    void set_output_file_name( string fnam );
    
    [CCode (cname = "c_plsfont")]    
    void set_current_font_param( int p_family, int p_style, int p_weight );
    
    
    [CCode (cname = "PLDEFINED_callback", has_target=false, delegate_target=false)]
    public delegate int DefinedFunc(double x, double y);
    [CCode (cname = "PLFILL_callback", has_target=false, delegate_target=false)]
    public delegate void FillFunc(int n, double[] x, double[] y);
    [CCode (cname = "c_plshade")]    
    void shade( double[,] a, DefinedFunc defined,
               double xmin, double xmax, double ymin, double ymax,
               double shade_min, double shade_max,
               int sh_cmap, double sh_color, double sh_width,
               int min_color, double min_width,
               int max_color, double max_width,
               FillFunc fill, bool rectangular,
               TransformFunc pltr );

    [CCode (cname = "c_plshades")]    
    void shades( double[,] a, DefinedFunc defined,
                double xmin, double xmax, double ymin, double ymax,
                double[] clevel, double fill_width,
                int cont_color, double cont_width,
                FillFunc fill, bool rectangular,
                TransformFunc pltr );
   
    [CCode (cname = "PLLABEL_FUNC_callback")]
    public delegate void LabelFunc( int axis, double value, out string label, int length);
    [CCode (cname = "c_plslabelfunc")]
    void set_label_function( LabelFunc label_func );

    [CCode (cname = "c_plsmaj")]
    void set_major_tick_len( double def, double scale );

    [CCode (cname = "c_plsmem")]
    void set_memory([CCode (array_length_pos=0)]uint8[,] plotmem );

    [CCode (cname = "c_plsmema")]
    void set_memory_rgba( int maxx, int maxy, [CCode (array_length = false)]uint8[] plotmem );

    [CCode (cname = "c_plsmin")]
    void set_minor_tick_len( double def, double scale );

    [CCode (cname = "c_plsori")]
    void set_orientation_int( int ori );

    [CCode (cname = "c_plspage")]
    void set_page_param( double xp, double yp,
            int xleng, int yleng, int xoff, int yoff );

    [CCode (cname = "c_plspal0")]
    void set_map0_palette( string filename );

    [CCode (cname = "c_plspal1")]
    void set_map1_palette( string filename, bool interpolate );

    [CCode (cname = "c_plspause")]
    void set_pause( bool pause );

    [CCode (cname = "c_plsstrm")]
    void set_current_stream( int strm );

    [CCode (cname = "c_plssub")]
    void set_subpage( int nx, int ny );

    [CCode (cname = "c_plssym")]
    void set_symbol_size( double def, double scale );

    [CCode (cname = "c_plstar")]
    void init_plot( int nx, int ny );

    [CCode (cname = "c_plstart")]
    void init_plot_on_dev( string devname, int nx, int ny );

    [CCode (cname = "c_plstransform")]
    void set_transform( TransformFunc coordinate_transform );

    [CCode (cname = "c_plstring")]
    void plot_string([CCode (array_length_pos = 0)]double[] x, 
                [CCode (array_length = false)]double[] y, string text);

    [CCode (cname = "c_plstring3")]
    void plot_string3d([CCode (array_length_pos = 0)]double[] x, 
                [CCode (array_length = false)]double[] y, 
                [CCode (array_length = false)]double[] z, 
                string text);

    [CCode (cname = "c_plstripa")]
    void strip_add_point(int id, int pen, int x, int y);

    // arrays colline styline legline only contain constant 4 entities.
    [CCode (cname = "c_plstripc")]    
    void strip(out int id, string xspec, string yspec,
            double xmin, double xmax, double xjump, double ymin, double ymax,
            double xlpos, double ylpos,
            bool y_ascl, bool acc,
            int colbox, int collab,
            [CCode (array_length = false)]int[] colline, 
            [CCode (array_length = false)]int[] styline, 
            [CCode (array_length = false)]string[] legline,
            string labx, string laby, string labtop);

    [CCode (cname = "c_plstripd")]
    void delete_strip(int id);

    [CCode (cname = "c_plstyl")]
    void set_line_style([CCode (array_length_pos = 0)] int[] mark,[CCode (array_length = false)] int[] space);

    [CCode (cname = "c_plsurf3d")]
    void suface3d([CCode (array_length_pos = 3.1)] double[] x, 
                [CCode (array_length_pos = 3.2)] double[] y, 
                [CCode (array_length = false)] double[,] z,
                int opt, double[] clevel);

    [CCode (cname = "c_plsurf3dl")]
    void suface3d_limit([CCode (array_length_pos = 3.1)] double[] x, 
                [CCode (array_length_pos = 3.2)] double[] y, 
                [CCode (array_length = false)] double[,] z,
                int opt, double[] clevel,
                int indexxmin, [CCode (array_length_pos = 6.9)] int[] indexymin,
                [CCode (array_length = false)] int[] indexymax);

    [CCode (cname = "c_plsvect")]
    void set_arrow_style_for_vector([CCode (array_length = false)] double[] arrowx, 
                double[] arrowy, bool fill);

    [CCode (cname = "c_plsvpa")]
    void set_viewport(double xmin, double xmax, double ymin, double ymax);

    [CCode (cname = "c_plsxax")]
    void set_xaxis(int digmax, int digits);

    [CCode (cname = "c_plsyax")]
    void set_yaxis(int digmax, int digits);

    [CCode (cname = "c_plsym")]
    void glyph_on_pos([CCode (array_length_pos = 0)]double[] x,[CCode (array_length = false)] double[] y, int code);

    [CCode (cname = "c_plszax")]
    void set_zaxis(int digmax, int digits);

    [CCode (cname = "c_pltext")]
    void switch_to_text();

    [CCode (cname = "c_pltimefmt")]
    void set_time_format(string fmt);

    [CCode (cname = "c_plvasp")]
    void select_viewport_by_ratio (double aspect);

    [CCode (cname = "c_plvect")]
    void vector ([CCode (array_length = false)] double[,] u, double[,] v, 
                double scale, TransformFunc pltr);

    [CCode (cname = "c_plvpas")]
    void select_viewport_by_coord_ratio (double xmin, double xmax, double ymin, double ymax, 
            double aspect);

    [CCode (cname = "c_plvpor")]
    void select_viewport_by_normal_subpage(double xmin, double xmax, double ymin, double ymax);

    [CCode (cname = "c_plvsta")]
    void select_viewport();

    [CCode (cname = "c_plw3d")]
    void set_3dto2d_transform(double basex, double basey, double height, double xmin,
            double xmax, double ymin, double ymax, double zmin,
            double zmax, double alt, double az);
    
    [CCode (cname = "c_plwidth")]
    void set_pen_width(double width);
    
    [CCode (cname = "c_plwind")]
    void select_window(double xmin, double xmax, double ymin, double ymax);
    
    [CCode (cname = "c_plxormod")]
    void enable_xormode(bool mode, out bool status);
}