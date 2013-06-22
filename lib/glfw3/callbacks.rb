require 'glfw3/glfw3'

module Glfw
  @@error_callback__ = nil
  @@monitor_callback__ = nil

  #
  # Gets the current error callback object.
  #
  # call-seq:
  #     error_callback -> obj or nil
  #
  def self.error_callback
    @@error_callback
  end

  #
  # Sets the current error callback object to the given value. Presumably a
  # Proc or some other object, but one that implements a call(...) function.
  #
  # If set to nil, any GLFW error will raise a RuntimeError exception.
  #
  # The error callback is expected to take two arguments: the error code and a
  # description of the error. For example:
  #
  #     Glfw.error_callback = lambda { |error_code, description|
  #       raise RuntimeError, "GLFW Error #{error_code}: #{description}"
  #     }
  #
  def self.error_callback=(lambda)
    @@error_callback = lambda
  end

  #
  # Sets the current error callback to a Proc generated from the provided block.
  #
  def self.set_error_callback(&block)
    self.error_callback = lambda(&block)
  end

  #
  # Gets the current monitor callback object.
  #
  # call-seq:
  #     monitor_callback -> obj or nil
  #
  def self.monitor_callback
    @@monitor_callback
  end

  #
  # Sets the current monitor callback object to the given object. Presumably a
  # Proc or some other object, but one that implements a call(...) function.
  #
  # If set to nil, the callback is disabled.
  #
  # The monitor callback is expected to take two arguments: the monitor and an
  # event (one of either Glfw::CONNECTED or Glfw::DISCONNECTED). For example:
  #
  #     Glfw.monitor_callback = lambda { |monitor, event|
  #       # ...
  #     }
  #
  def self.monitor_callback=(lambda)
    @@monitor_callback = lambda
  end

  #
  # Sets the current monitor callback to a Proc generated from the provided
  # block.
  #
  def self.set_monitor_callback(&block)
    self.monitor_callback = lambda(&block)
  end
end
