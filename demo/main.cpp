#include "tweeners/builder.hpp"
#include "tweeners/easing.hpp"
#include "tweeners/system.hpp"

#include <SDL2/SDL.h>

#include <memory>
#include <vector>

template< typename F >
class at_scope_exit
{
public:
  explicit at_scope_exit( F function )
    : m_function( std::move( function ) )
  {

  }

  ~at_scope_exit()
  {
    m_function();
  }

  at_scope_exit( const at_scope_exit& ) = delete;
  at_scope_exit( at_scope_exit&& ) = delete;

  at_scope_exit& operator=( const at_scope_exit& ) = delete;
  at_scope_exit operator=( at_scope_exit&& ) = delete;
  
private:
  F m_function;
};

struct easing_function
{
  easing_function( const char* n, std::function< float( float ) > f )
    : name( n ),
      function( std::move( f ) )
  {

  }
  
  const char* name;
  std::function< float( float ) > function;
};

class easing_explorer
{
public:
  easing_explorer( int screen_width, int screen_height, SDL_Rect& dot );

  void update( int delay_ms );

  void next_easing_x();
  void next_easing_y();
  void previous_easing_x();
  void previous_easing_y();

private:
  struct variable
  {
    const char* name;
    const int min;
    const int max;
    std::size_t current_easing;
    bool need_change;
    int& value;
    tweeners::system::id_type slot;
  };
  
private:
  void previous_easing( std::size_t& index ) const;
  void next_easing( std::size_t& index ) const;
  
  void insert_slot( variable& v );
  void replace_slot_if_needed( variable& v );

private:
  static constexpr int s_margin = 100;
  variable m_x;
  variable m_y;
  const std::vector< easing_function > m_easing_functions;

  tweeners::system m_system;
};

#define register_easing( function )                                     \
  easing_function( #function, &tweeners::easing::function<> ),          \
    easing_function                                                     \
    ( #function "_out",                                                 \
      []( float t ) -> float                                            \
      {                                                                 \
        return                                                          \
          tweeners::easing::ease_out( t, &tweeners::easing::function<> ); \
      }                                                                 \
      ),                                                                \
  easing_function                                                       \
    ( #function "_in_out",                                              \
      []( float t ) -> float                                            \
      {                                                                 \
        return                                                          \
          tweeners::easing::ease_in_out( t, &tweeners::easing::function<> ); \
      }                                                                 \
    )

easing_explorer::easing_explorer
( int screen_width, int screen_height, SDL_Rect& dot )
  : m_x
    {
      "x",
      s_margin - dot.w / 2,
      screen_width - s_margin - dot.w / 2,
      0,
      false,
      dot.x
    },
    m_y
    {
      "y",
      screen_height - s_margin - dot.h / 2,
      s_margin - dot.h / 2,
      0,
      false,
      dot.y
    },
    m_easing_functions
    ( {
        easing_function( "none", &tweeners::easing::none<> ),
        easing_function( "linear", &tweeners::easing::linear<> ),
        register_easing( sine ),
        register_easing( quad ),
        register_easing( cubic ),
        register_easing( quart ),
        register_easing( quint ),
        register_easing( circ ),
        register_easing( expo ),
        register_easing( elastic ),
        register_easing( bounce ),
        register_easing( back )
    } )
{
  insert_slot( m_x );
  insert_slot( m_y );
}
#undef register_easing

void easing_explorer::update( int delay_ms )
{
  m_system.update( delay_ms );
}

void easing_explorer::next_easing_x()
{
  m_x.need_change = true;
  next_easing( m_x.current_easing );

  printf
    ( "X-axis easing function set to %s.\n",
      m_easing_functions[ m_x.current_easing ].name );
}

void easing_explorer::next_easing_y()
{
  m_y.need_change = true;
  next_easing( m_y.current_easing );

  printf
    ( "Y-axis easing function set to %s.\n",
      m_easing_functions[ m_y.current_easing ].name );
}

void easing_explorer::previous_easing_x()
{
  m_x.need_change = true;
  previous_easing( m_x.current_easing );

  printf
    ( "X-axis easing function set to %s.\n",
      m_easing_functions[ m_x.current_easing ].name );
}

void easing_explorer::previous_easing_y()
{
  m_y.need_change = true;
  previous_easing( m_y.current_easing );

  printf
    ( "Y-axis easing function set to %s.\n",
      m_easing_functions[ m_y.current_easing ].name );
}

void easing_explorer::previous_easing( std::size_t& index ) const
{
  if ( index == 0 )
    index = m_easing_functions.size() - 1;
  else
    --index;
}

void easing_explorer::next_easing( std::size_t& index ) const
{
  ++index;

  if ( index == m_easing_functions.size() )
    index = 0;
}

void easing_explorer::insert_slot( variable& v )
{
  constexpr int duration( 2000 );

  v.slot =
    tweeners::builder()
    .range_transform
    ( v.min, v.max, duration, v.value,
      m_easing_functions[ v.current_easing ].function )
    .on_done
    ( std::bind
      ( &easing_explorer::replace_slot_if_needed, this, std::ref( v ) ) )
    .build( m_system );
  
  m_system.play_in_sequence( v.slot, v.slot );
}

void easing_explorer::replace_slot_if_needed( variable& v )
{
  if ( !v.need_change )
    return;

  v.need_change = false;
  m_system.remove_slot( v.slot );
  insert_slot( v );
}

static void update_scene
( SDL_Renderer* renderer, easing_explorer& explorer, const SDL_Rect& dot )
{
  SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0 );
  SDL_RenderClear( renderer );
  SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
  SDL_RenderFillRect( renderer, &dot );
  SDL_RenderPresent( renderer );

  constexpr int delay_ms( 16 );
  SDL_Delay( delay_ms );
  explorer.update( delay_ms );
}

static void run_scene( SDL_Renderer* renderer )
{
  SDL_Rect dot;

  dot.w = 10;
  dot.h = 10;

  easing_explorer explorer( 640, 480, dot );
  
  while ( true )
    {
      SDL_Event event;
  
      while ( SDL_PollEvent( &event ) )
        {
          switch( event.type )
            {
            case SDL_QUIT:
              return;
            case SDL_KEYUP:
              {
                const SDL_KeyboardEvent key( event.key );

                switch ( event.key.keysym.sym )
                  {
                  case SDLK_ESCAPE:
                    return;
                  case SDLK_LEFT:
                    {
                      explorer.previous_easing_x();
                      break;
                    }
                  case SDLK_RIGHT:
                    {
                      explorer.next_easing_x();
                      break;
                    }
                  case SDLK_DOWN:
                    {
                      explorer.previous_easing_y();
                      break;
                    }
                  case SDLK_UP:
                    {
                      explorer.next_easing_y();
                      break;
                    }
                  }
              }
            }
        }
      
      update_scene( renderer, explorer, dot );
    }
}

int main()
{
  if ( SDL_Init(SDL_INIT_VIDEO) != 0 )
    {
      fprintf( stderr, "Failed to initialize the SDL: '%s'\n", SDL_GetError() );
      return 1;
    }

  const at_scope_exit quit_sdl( &SDL_Quit );
  
  const std::unique_ptr< SDL_Window, decltype( &SDL_DestroyWindow ) > window
    ( SDL_CreateWindow( "Tweener demo", 0, 0, 640, 480, SDL_WINDOW_SHOWN ),
      &SDL_DestroyWindow );
  
  if ( window == nullptr )
    {
      fprintf( stderr, "Failed to create the window: '%s'\n", SDL_GetError() );
      return 1;
    }

  const std::unique_ptr< SDL_Renderer, decltype( &SDL_DestroyRenderer ) >
    renderer
    ( SDL_CreateRenderer
      ( window.get(), -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC ),
      &SDL_DestroyRenderer );
  
  if ( renderer == nullptr )
    {
      fprintf
        ( stderr, "Failed to create the renderer: '%s'\n", SDL_GetError() );
      return 1;
    }

  run_scene( renderer.get() );

  return 0;
}
