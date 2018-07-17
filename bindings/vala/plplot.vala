
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

    class Stream : Object{
        private int _stream = 0;
        private class int active_streams = 0;

        construct {
            active_streams ++;
        }

        ~Stream() {
            active_streams --;
            if ( active_streams != 0 )
                finish_plot();
        }

        protected virtual void set_stream() {
            set_current_stream( _stream ); 
        }
    }
}