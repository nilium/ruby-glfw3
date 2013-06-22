require 'glfw3/glfw3'

module Glfw
  @@error_callback__ = nil
  @@monitor_callback__ = nil

  def self.error_callback
    @@error_callback
  end

  def self.error_callback=(lambda)
    @@error_callback = lambda
  end

  def self.set_error_callback(&block)
    self.error_callback = lambda(&block)
  end

  def self.monitor_callback
    @@monitor_callback
  end

  def self.monitor_callback=(lambda)
    @@monitor_callback = lambda
  end

  def self.set_monitor_callback(&block)
    self.monitor_callback = lambda(&block)
  end
end
