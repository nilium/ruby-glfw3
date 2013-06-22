require 'glfw3/glfw3'

module Glfw; end

class Glfw::Window

  def windows
    @__windows.values
  end

  def x
    position[0]
  end

  def y
    position[1]
  end

  def width
    size[0]
  end

  def height
    size[1]
  end

  def key_callback=(func)
    @__key_callback = func
    puts "Setting key callback #{!func.nil?}"
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
