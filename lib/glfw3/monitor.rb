require 'glfw3/glfw3'

module Glfw ; end

class Glfw::Monitor
  alias_method :gamma_ramp=, :set_gamma_ramp
  alias_method :gamma_ramp, :get_gamma_ramp
end
