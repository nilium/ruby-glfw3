require 'pkg-config'
require 'mkmf'

$LDFLAGS += " #{`pkg-config --static --libs glfw3`}"
$CFLAGS += " #{`pkg-config --cflags glfw3`}"

create_makefile('glfw3/glfw3')
