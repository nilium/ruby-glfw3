ruby-glfw3
==========

[GLFW 3] bindings gem for Ruby 2.x.

To install the gem, you'll need GLFW 3 built and installed where pkg-config can
find it. You may also want to grab some [OpenGL bindings] as well.

[OpenGL bindings]: https://github.com/nilium/ruby-opengl
[GLFW3]: https://github.com/glfw/glfw

Once that's taken care of, you can install it simply by building and installing
the gem, like so:

    $ gem build glfw3.gemspec
    $ gem install glfw3-0.0.1.gemspec

After that, write a quick script to toy with it. For example:

    require 'glfw3'
    require 'opengl'

    # Initialize GLFW 3
    Glfw.init

    # Create a window
    window = Glfw::Window.new(800, 600, "Foobar")

    # Set some callbacks
    window.set_key_callback {
      |window, key, code, action, mods|
      window.should_close = true if key == Glfw::KEY_ESCAPE
    }

    window.set_close_callback {
      |window|
      window.should_close = true
    }

    # Make the window's context current
    window.make_context_current
    loop {
      # And do stuff
      Glfw.wait_events
      Gl.glClear(Gl::GL_COLOR_BUFFER_BIT | Gl::GL_DEPTH_BUFFER_BIT)
      window.swap_buffers
      break if window.should_close?
    }

    # Explicitly destroy the window when done with it.
    window.destroy


License
-------

ruby-glw3 is licensed under a simplified BSD license because it seems the most
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

    The views and conclusions contained in the software and documentation are those
    of the authors and should not be interpreted as representing official policies,
    either expressed or implied, of the FreeBSD Project.

