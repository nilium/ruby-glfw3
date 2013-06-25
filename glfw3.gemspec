# This file is part of ruby-glfw3.
# Copyright (c) 2013 Noel Raymond Cower. All rights reserved.
# See COPYING for license details.

Gem::Specification.new { |s|
  s.name        = 'glfw3'
  s.version     = '0.3.0'
  s.date        = '2013-06-21'
  s.summary     = 'GLFW3'
  s.description = 'GLFW 3 bindings for Ruby 2.x'
  s.authors     = [ 'Noel Raymond Cower' ]
  s.email       = 'ncower@gmail.com'
  s.files       = Dir.glob('lib/**/*.rb') +
                  Dir.glob('ext/**/*.{c,rb}') +
                  [ 'COPYING', 'README.md' ]
  s.extensions << 'ext/glfw3/extconf.rb'
  s.homepage    = 'https://github.com/nilium/ruby-glfw3'
  s.license     = 'Simplified BSD'
  s.has_rdoc    = true
  s.extra_rdoc_files = [
      'ext/glfw3/glfw3.c',
      'README.md',
      'COPYING'
  ]
  s.rdoc_options << '--title' << 'core-opengl -- OpenGL Core Profile Bindings' <<
                    '--main' << 'README.md' <<
                    '--line-numbers'
}
