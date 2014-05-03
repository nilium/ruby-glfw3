ruby-glfw3
==========

    $ gem install glfw3

Intro
-----

[GLFW 3] bindings gem for Ruby 2.x.

To install the gem, you'll need GLFW 3 built and installed where pkg-config can
find it. You may also want to grab some [OpenGL bindings] as well.

[OpenGL bindings]: https://github.com/nilium/ruby-opengl
[GLFW 3]: https://github.com/glfw/glfw

Once that's taken care of, you can install it simply by installing the gem via RubyGems.org using

    $ gem install glfw3

Or by building and installing the gem, making note of the version so you know
the gem package to install, like so:

    $ gem build glfw3.gemspec
    $ gem install glfw3-VERSION-HERE.gemspec

After that, write a quick script to toy with it. For example:

    require 'glfw3'
    require 'opengl-core'

    # Initialize GLFW 3
    Glfw.init

    # Create a window
    window = Glfw::Window.new(800, 600, "Foobar")

    # Set some callbacks
    window.set_key_callback do |window, key, code, action, mods|
      window.should_close = true if key == Glfw::KEY_ESCAPE
    end

    window.set_close_callback do |window|
      window.should_close = true
    end

    # Make the window's context current
    window.make_context_current
    loop do
      # And do stuff
      Glfw.wait_events
      GL.glClear(GL::GL_COLOR_BUFFER_BIT | GL::GL_DEPTH_BUFFER_BIT)
      window.swap_buffers
      break if window.should_close?
    end

    # Explicitly destroy the window when done with it.
    window.destroy


License
-------

ruby-glfw3 is licensed under a simplified BSD license because it seems the most
reasonable. If there's a problem with that, let me know.

    Copyright (c) 2013, Noel Raymond Cower <ncower@gmail.com>.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer. 
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution. 

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

