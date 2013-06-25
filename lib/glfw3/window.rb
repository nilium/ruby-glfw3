require 'glfw3/glfw3'

module Glfw; end

#
# A GLFW window. These contain a context, optionally one shared with other
# windows and generate events. Each window maintains its own event callbacks.
#
# Wraps GLFWwindow and its associated functions, for the most part.
#
#
# === Event Callbacks
#
# All window event callbacks' first argument is the window that generated the
# event. These event callbacks all receive the same arguments as their GLFW C
# counterparts, so refer to the
# {GLFW 3 documentation}[http://www.glfw.org/docs/3.0/group__input.html]
# for that.
#
#
# === User Data
#
# If you need to associate a particular object or value with a window, you can
# use its #user_data attribute to store and retreive an arbitrary object for
# the window. If you need to store multiple values with the window, you might
# set its user data object to a Hash.
#
class Glfw::Window

  #
  # User data attribute, can be set to any arbitrary object. Used to associate
  # any object with the window for later retrieval.
  #
  attr_accessor :user_data



  alias_method :should_close?, :get_should_close
  alias_method :should_close=, :set_should_close

  alias_method :cursor_pos, :get_cursor_pos

  def cursor_pos=(xy)
    set_cursor_pos(*xy)
  end

  alias_method :move, :set_position
  alias_method :resize, :set_size

  alias_method :position, :get_position
  alias_method :size, :get_size

  def position=(xy)
    set_position(*xy)
  end

  def size=(wh)
    set_size(*wh)
  end

  #
  # Returns an array of all allocated GLFW windows.
  #
  # call-seq:
  #     windows -> [Glfw::Window, ...]
  #
  def self.windows
    @@__windows.values
  end

  #
  # Gets the X position of the window in screen space. See also #position.
  #
  def x
    position[0]
  end

  #
  # Gets the Y position of the window in screen space. See also #position.
  #
  def y
    position[1]
  end

  #
  # Gets the width of the window. See also #size.
  #
  def width
    size[0]
  end

  #
  # Gets the width of the window. See also #size.
  #
  def height
    size[1]
  end

  def key_callback=(func)
    @__key_callback = func
    set_key_callback__(!func.nil?)
  end

  def set_key_callback(&block)
    self.key_callback = lambda(&block)
  end

  def char_callback=(func)
    @__char_callback = func
    set_char_callback__(!func.nil?)
  end

  def set_char_callback(&block)
    self.char_callback = lambda(&block)
  end

  def mouse_button_callback(func)
    @__mouse_button_callback = func
    set_mouse_button_callback__(!func.nil?)
  end

  def set_mouse_button_callback(&block)
    self.mouse_button_callback = lambda(&block)
  end

  def cursor_position_callback(func)
    @__cursor_position_callback = func
    set_cursor_position_callback__(!func.nil?)
  end

  def set_cursor_position_callback(&block)
    self.cursor_position_callback = lambda(&block)
  end

  def cursor_enter_callback(func)
    @__cursor_enter_callback = func
    set_cursor_enter_callback__(!func.nil?)
  end

  def set_cursor_enter_callback(&block)
    self.cursor_enter_callback = lambda(&block)
  end

  def scroll_callback(func)
    @__scroll_callback = func
    set_scroll_callback__(!func.nil?)
  end

  def set_scroll_callback(&block)
    self.scroll_callback = lambda(&block)
  end

  def position_callback=(func)
    @__position_callback = func
    set_position_callback__(!func.nil?)
  end

  def set_position_callback(&block)
    self.position_callback = lambda(&block)
  end

  def size_callback=(func)
    @__size_callback = func
    set_size_callback__(!func.nil?)
  end

  def set_size_callback(&block)
    self.size_callback = lambda(&block)
  end

  def close_callback=(func)
    @__close_callback = func
    set_close_callback__(!func.nil?)
  end

  def set_close_callback(&block)
    self.close_callback = lambda(&block)
  end

  def refresh_callback=(func)
    @__refresh_callback = func
    set_refresh_callback__(!func.nil?)
  end

  def set_refresh_callback(&block)
    self.refresh_callback = lambda(&block)
  end

  def focus_callback=(func)
    @__focus_callback = func
    set_focus_callback__(!func.nil?)
  end

  def set_focus_callback(&block)
    self.focus_callback = lambda(&block)
  end

  def iconify_callback=(func)
    @__iconify_callback = func
    set_iconify_callback__(!func.nil?)
  end

  def set_iconify_callback(&block)
    self.iconify_callback = lambda(&block)
  end

  def framebuffer_size_callback(func)
    @__framebuffer_size_callback = func
    set_framebuffer_size_callback__(!func.nil?)
  end

  def set_framebuffer_size_callback(&block)
    self.framebuffer_size_callback = lambda(&block)
  end


end
