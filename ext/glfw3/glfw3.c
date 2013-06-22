#include "ruby.h"
#include <GLFW/glfw3.h>


typedef struct s_rb_glfw_error
{
  VALUE error_code;   // mark
  VALUE description;  // mark
} rb_glfw_error_t;


static const char *kRB_IVAR_WINDOW_INTERNAL                   = "@__internal_window";
static const char *kRB_IVAR_WINDOW_KEY_CALLBACK               = "@__key_callback";
static const char *kRB_IVAR_WINDOW_CHAR_CALLBACK              = "@__char_callback";
static const char *kRB_IVAR_WINDOW_MOUSE_BUTTON_CALLBACK      = "@__mouse_button_callback";
static const char *kRB_IVAR_WINDOW_CURSOR_POSITION_CALLBACK   = "@__cursor_position_callback";
static const char *kRB_IVAR_WINDOW_CURSOR_ENTER_CALLBACK      = "@__cursor_enter_callback";
static const char *kRB_IVAR_WINDOW_SCROLL_CALLBACK            = "@__scroll_callback";
static const char *kRB_IVAR_WINDOW_POSITION_CALLBACK          = "@__position_callback";
static const char *kRB_IVAR_WINDOW_SIZE_CALLBACK              = "@__size_callback";
static const char *kRB_IVAR_WINDOW_CLOSE_CALLBACK             = "@__close_callback";
static const char *kRB_IVAR_WINDOW_REFRESH_CALLBACK           = "@__refresh_callback";
static const char *kRB_IVAR_WINDOW_FOCUS_CALLBACK             = "@__focus_callback";
static const char *kRB_IVAR_WINDOW_ICONIFY_CALLBACK           = "@__iconify_callback";
static const char *kRB_IVAR_WINDOW_FRAMEBUFFER_SIZE_CALLBACK  = "@__framebuffer_size_callback";
static const char *kRB_CVAR_WINDOW_WINDOWS                    = "@@__windows";
static const char *kRB_CVAR_GLFW_ERROR_CALLBACK               = "@@__error_callback";
static const char *kRB_CVAR_GLFW_MONITOR_CALLBACK             = "@@__monitor_callback";


static VALUE s_glfw_module = Qundef;
static VALUE s_glfw_window_klass = Qundef;
static VALUE s_glfw_window_internal_klass = Qundef;
static VALUE s_glfw_monitor_klass = Qundef;
static VALUE s_glfw_videomode_klass = Qundef;


static void rb_glfw_error_callback(int error_code, const char *description);
static void rb_glfw_monitor_callback(GLFWmonitor *monitor, int message);


#define RB_ENABLE_CALLBACK_DEF(NAME, CALLBACK, GLFW_FUNC)                     \
static VALUE NAME (VALUE self, VALUE enabled)                                 \
{                                                                             \
  if (RTEST(enabled)) {                                                       \
    GLFW_FUNC ( rb_get_window(self), CALLBACK );                              \
  } else {                                                                    \
    GLFW_FUNC ( rb_get_window(self), NULL );                                  \
  }                                                                           \
  return self;                                                                \
}


/* GLFWAPI int glfwInit(void); */

static VALUE rb_glfw_init(VALUE self)
{
  (void)self;
  VALUE result = glfwInit() ? Qtrue : Qfalse;
  if (result == Qtrue) {
    glfwSetMonitorCallback(rb_glfw_monitor_callback);
  }
  return result;
}




/* GLFWAPI void glfwTerminate(void); */

static VALUE rb_glfw_terminate(VALUE self)
{
  glfwTerminate();
  return Qundef;
}



/* GLFWAPI void glfwGetVersion(int* major, int* minor, int* rev); */

static VALUE rb_glfw_version(VALUE self)
{
  int major = 0;
  int minor = 0;
  int revision = 0;
  glfwGetVersion(&major, &minor, &revision);
  return rb_ary_new3(3, INT2NUM(major), INT2NUM(minor), INT2NUM(revision));
}



/* GLFWAPI GLFWerrorfun glfwSetErrorCallback(GLFWerrorfun cbfun); */

static void rb_glfw_error_callback(int error_code, const char *description)
{
  VALUE lambda = rb_cvar_get(s_glfw_module, rb_intern(kRB_CVAR_GLFW_ERROR_CALLBACK));



  if (RTEST(lambda)) {
    VALUE rb_description = rb_str_new2(description);
    VALUE rb_error_code = INT2FIX(error_code);
    OBJ_FREEZE(rb_description);
    OBJ_FREEZE(rb_error_code);
    rb_funcall(lambda, rb_intern("call"), 2, rb_error_code, rb_description);
  } else {
    rb_raise(rb_eRuntimeError, "GLFW Error 0x%X: %s", error_code, description);
  }
}



/* GLFWAPI GLFWmonitor** glfwGetMonitors(int* count); */

VALUE rb_glfw_get_monitors(VALUE self)
{
  long monitor_index = 0;
  int num_monitors = 0;
  GLFWmonitor **monitors = glfwGetMonitors(&num_monitors);
  VALUE monitors_out = rb_ary_new();
  for (; num_monitors; --num_monitors, ++monitor_index) {
    VALUE monitor = Data_Wrap_Struct(s_glfw_monitor_klass, 0, 0, monitors[monitor_index]);
    rb_obj_call_init(monitor, 0, 0);
    rb_ary_push(monitors_out, monitor);
  }
  return monitors_out;
}



/* GLFWAPI GLFWmonitor* glfwGetPrimaryMonitor(void); */

VALUE rb_glfw_get_primary_monitor(VALUE self)
{
  VALUE monitor = Data_Wrap_Struct(s_glfw_monitor_klass, 0, 0, glfwGetPrimaryMonitor());
  rb_obj_call_init(monitor, 0, 0);
  return monitor;
}



/* GLFWAPI void glfwGetMonitorPos(GLFWmonitor* monitor, int* xpos, int* ypos); */

VALUE rb_monitor_position(VALUE self)
{
  GLFWmonitor *monitor;
  Data_Get_Struct(self, GLFWmonitor, monitor);
  int xpos = 0;
  int ypos = 0;
  glfwGetMonitorPos(monitor, &xpos, &ypos);
  return rb_ary_new3(2, INT2FIX(xpos), INT2FIX(ypos));
}



/* GLFWAPI void glfwGetMonitorPhysicalSize(GLFWmonitor* monitor, int* width, int* height); */

VALUE rb_monitor_physical_size(VALUE self)
{
  GLFWmonitor *monitor;
  Data_Get_Struct(self, GLFWmonitor, monitor);
  int width = 0;
  int height = 0;
  glfwGetMonitorPhysicalSize(monitor, &width, &height);
  return rb_ary_new3(2, INT2FIX(width), INT2FIX(height));
}



/* GLFWAPI const char* glfwGetMonitorName(GLFWmonitor* monitor); */


VALUE rb_monitor_name(VALUE self)
{
  GLFWmonitor *monitor;
  Data_Get_Struct(self, GLFWmonitor, monitor);
  return rb_str_new2(glfwGetMonitorName(monitor));
}



/* GLFWAPI GLFWmonitorfun glfwSetMonitorCallback(GLFWmonitorfun cbfun); */

static void rb_glfw_monitor_callback(GLFWmonitor *monitor, int message)
{
  VALUE lambda = rb_cvar_get(s_glfw_module, rb_intern(kRB_CVAR_GLFW_MONITOR_CALLBACK));
  if (RTEST(lambda)) {
    VALUE rb_monitor = Data_Wrap_Struct(s_glfw_monitor_klass, 0, 0, monitor);
    rb_obj_call_init(rb_monitor, 0, 0);
    rb_funcall(lambda, rb_intern("call"), 2, rb_monitor, INT2FIX(message));
  }
}



/* GLFWAPI const GLFWvidmode* glfwGetVideoModes(GLFWmonitor* monitor, int* count); */

static VALUE rb_videomode_width(VALUE self)
{
  GLFWvidmode *mode;
  Data_Get_Struct(self, GLFWvidmode, mode);
  return INT2FIX(mode->width);
}

static VALUE rb_videomode_height(VALUE self)
{
  GLFWvidmode *mode;
  Data_Get_Struct(self, GLFWvidmode, mode);
  return INT2FIX(mode->height);
}

static VALUE rb_videomode_red_bits(VALUE self)
{
  GLFWvidmode *mode;
  Data_Get_Struct(self, GLFWvidmode, mode);
  return INT2FIX(mode->redBits);
}

static VALUE rb_videomode_green_bits(VALUE self)
{
  GLFWvidmode *mode;
  Data_Get_Struct(self, GLFWvidmode, mode);
  return INT2FIX(mode->greenBits);
}

static VALUE rb_videomode_blue_bits(VALUE self)
{
  GLFWvidmode *mode;
  Data_Get_Struct(self, GLFWvidmode, mode);
  return INT2FIX(mode->blueBits);
}

static VALUE rb_videomode_refresh_rate(VALUE self)
{
  GLFWvidmode *mode;
  Data_Get_Struct(self, GLFWvidmode, mode);
  return INT2FIX(mode->refreshRate);
}

static VALUE rb_monitor_video_modes(VALUE self)
{
  GLFWmonitor *monitor;
  Data_Get_Struct(self, GLFWmonitor, monitor);
  VALUE rb_modes = rb_ary_new();
  int num_modes = 0;
  int mode_index = 0;
  const GLFWvidmode *modes = glfwGetVideoModes(monitor, &num_modes);
  for (; num_modes; --num_modes, ++mode_index) {
    GLFWvidmode *mode = ALLOC(GLFWvidmode);
    *mode = modes[mode_index];
    VALUE rb_mode = Data_Wrap_Struct(s_glfw_videomode_klass, 0, free, mode);
    rb_obj_call_init(rb_mode, 0, 0);
    rb_ary_push(rb_modes, rb_mode);
  }
  return rb_modes;
}



/* GLFWAPI const GLFWvidmode* glfwGetVideoMode(GLFWmonitor* monitor); */

static VALUE rb_monitor_video_mode(VALUE self)
{
  GLFWmonitor *monitor;
  Data_Get_Struct(self, GLFWmonitor, monitor);
  GLFWvidmode *mode = ALLOC(GLFWvidmode);
  *mode = *glfwGetVideoMode(monitor);
  VALUE rb_mode = Data_Wrap_Struct(s_glfw_videomode_klass, 0, free, mode);
  rb_obj_call_init(rb_mode, 0, 0);
  return rb_mode;
}



/* GLFWAPI void glfwSetGamma(GLFWmonitor* monitor, float gamma); */

static VALUE rb_monitor_set_gamma(VALUE self, VALUE gamma)
{
  GLFWmonitor *monitor;
  Data_Get_Struct(self, GLFWmonitor, monitor);
  glfwSetGamma(monitor, (float)NUM2DBL(gamma));
  return self;
}



/* GLFWAPI const GLFWgammaramp* glfwGetGammaRamp(GLFWmonitor* monitor); */

#warning "No implementation for glfwGetGammaRamp bindings"



/* GLFWAPI void glfwSetGammaRamp(GLFWmonitor* monitor, const GLFWgammaramp* ramp); */

#warning "No implementation for glfwSetGammaRamp bindings"



/* GLFWAPI void glfwDefaultWindowHints(void); */

static VALUE rb_window_default_window_hints(VALUE self)
{
  glfwDefaultWindowHints();
  return self;
}



/* GLFWAPI void glfwWindowHint(int target, int hint); */

static VALUE rb_window_window_hint(VALUE self, VALUE target, VALUE hint)
{
  glfwWindowHint(NUM2INT(target), NUM2INT(hint));
  return self;
}



/* GLFWAPI GLFWwindow* glfwCreateWindow(int width, int height, const char* title, GLFWmonitor* monitor,  */

static VALUE rb_lookup_window(GLFWwindow *window)
{
  return (VALUE)glfwGetWindowUserPointer(window);
}

static GLFWwindow *rb_get_window(VALUE rb_window)
{
  GLFWwindow *window = NULL;
  if (RTEST(rb_window)) {
    ID ivar_window = rb_intern(kRB_IVAR_WINDOW_INTERNAL);
    VALUE rb_window_data = Qnil;
    if (RTEST((rb_window_data = rb_ivar_get(rb_window, ivar_window)))) {
      Data_Get_Struct(rb_window_data, GLFWwindow, window);
    }
  }
  return window;
}

// static VALUE rb_window_init(int argc, VALUE *argv, VALUE self)
// {
//   return self;
// }

static VALUE rb_window_new(int argc, VALUE *argv, VALUE self)
{
  ID ivar_window = rb_intern(kRB_IVAR_WINDOW_INTERNAL);
  VALUE rb_width, rb_height, rb_title, rb_monitor, rb_share;
  VALUE rb_window;
  VALUE rb_window_data;
  VALUE rb_windows;
  GLFWwindow *window = NULL;
  int width, height;
  const char *title = "";
  GLFWmonitor *monitor = NULL;
  GLFWwindow *share = NULL;

  // Grab arguments
  rb_scan_args(argc, argv, "23", &rb_width, &rb_height, &rb_title, &rb_monitor, &rb_share);

  width = NUM2INT(rb_width);
  height = NUM2INT(rb_height);

  if (RTEST(rb_title)) {
    if (rb_type(rb_title) != T_STRING) {
      rb_title = rb_any_to_s(rb_title);
    }
    title = StringValueCStr(rb_title);
  }

  if (RTEST(rb_monitor) && rb_obj_is_instance_of(rb_monitor, s_glfw_monitor_klass)) {
    Data_Get_Struct(rb_monitor, GLFWmonitor, monitor);
  }

  if (RTEST(rb_share) && rb_obj_is_instance_of(rb_share, s_glfw_window_klass)) {
    VALUE rb_shared_window = rb_ivar_get(rb_share, ivar_window);
    Data_Get_Struct(rb_shared_window, GLFWwindow, share);
  }

  // Create GLFW window
  window = glfwCreateWindow(width, height, title, monitor, share);

  // Allocate the window wrapper (only used to store the window pointer)
  rb_window_data = Data_Wrap_Struct(s_glfw_window_internal_klass, 0, 0, window);
  rb_obj_call_init(rb_window_data, 0, 0);

  // Allocate the window
  rb_window = rb_obj_alloc(s_glfw_window_klass);

  rb_ivar_set(rb_window, ivar_window, rb_window_data);
  rb_ivar_set(rb_window, rb_intern(kRB_IVAR_WINDOW_KEY_CALLBACK), Qnil);
  rb_ivar_set(rb_window, rb_intern(kRB_IVAR_WINDOW_CHAR_CALLBACK), Qnil);
  rb_ivar_set(rb_window, rb_intern(kRB_IVAR_WINDOW_MOUSE_BUTTON_CALLBACK), Qnil);
  rb_ivar_set(rb_window, rb_intern(kRB_IVAR_WINDOW_CURSOR_POSITION_CALLBACK), Qnil);
  rb_ivar_set(rb_window, rb_intern(kRB_IVAR_WINDOW_CURSOR_ENTER_CALLBACK), Qnil);
  rb_ivar_set(rb_window, rb_intern(kRB_IVAR_WINDOW_SCROLL_CALLBACK), Qnil);
  rb_ivar_set(rb_window, rb_intern(kRB_IVAR_WINDOW_POSITION_CALLBACK), Qnil);
  rb_ivar_set(rb_window, rb_intern(kRB_IVAR_WINDOW_SIZE_CALLBACK), Qnil);
  rb_ivar_set(rb_window, rb_intern(kRB_IVAR_WINDOW_CLOSE_CALLBACK), Qnil);
  rb_ivar_set(rb_window, rb_intern(kRB_IVAR_WINDOW_REFRESH_CALLBACK), Qnil);
  rb_ivar_set(rb_window, rb_intern(kRB_IVAR_WINDOW_FOCUS_CALLBACK), Qnil);
  rb_ivar_set(rb_window, rb_intern(kRB_IVAR_WINDOW_ICONIFY_CALLBACK), Qnil);
  rb_ivar_set(rb_window, rb_intern(kRB_IVAR_WINDOW_FRAMEBUFFER_SIZE_CALLBACK), Qnil);

  glfwSetWindowUserPointer(window, (void *)rb_window);
  rb_obj_call_init(rb_window, 0, 0);

  // Store the window so it can't go out of scope until explicitly destroyed.
  rb_windows = rb_cvar_get(self, rb_intern(kRB_CVAR_WINDOW_WINDOWS));
  rb_hash_aset(rb_windows, INT2FIX((int)window), rb_window);

  return rb_window;
}



/* GLFWAPI void glfwDestroyWindow(GLFWwindow* window); */

static VALUE rb_window_destroy(VALUE self)
{
  GLFWwindow *window = rb_get_window(self);
  if (window) {
    glfwDestroyWindow(window);
    rb_ivar_set(self, rb_intern(kRB_IVAR_WINDOW_INTERNAL), Qnil);
    VALUE rb_windows = rb_cvar_get(s_glfw_window_klass, rb_intern(kRB_CVAR_WINDOW_WINDOWS));
    rb_hash_delete(rb_windows, INT2FIX((int)window));
  }
  return self;
}



/* GLFWAPI int glfwWindowShouldClose(GLFWwindow* window); */

static VALUE rb_window_should_close(VALUE self)
{
  GLFWwindow *window = rb_get_window(self);
  return glfwWindowShouldClose(window) ? Qtrue : Qfalse;
}



/* GLFWAPI void glfwSetWindowShouldClose(GLFWwindow* window, int value); */

static VALUE rb_window_set_should_close(VALUE self, VALUE value)
{
  GLFWwindow *window = rb_get_window(self);
  glfwSetWindowShouldClose(window, RTEST(value) ? GL_TRUE : GL_FALSE);
  return self;
}



/* GLFWAPI void glfwSetWindowTitle(GLFWwindow* window, const char* title); */

static VALUE rb_window_set_title(VALUE self, VALUE title)
{
  glfwSetWindowTitle(rb_get_window(self), StringValueCStr(title));
  return title;
}



/* GLFWAPI void glfwGetWindowPos(GLFWwindow* window, int* xpos, int* ypos); */

static VALUE rb_window_get_position(VALUE self)
{
  int xpos = 0;
  int ypos = 0;
  glfwGetWindowPos(rb_get_window(self), &xpos, &ypos);
  return rb_ary_new3(2, INT2FIX(xpos), INT2FIX(ypos));
}



/* GLFWAPI void glfwSetWindowPos(GLFWwindow* window, int xpos, int ypos); */

static VALUE rb_window_set_position(VALUE self, VALUE x, VALUE y)
{
  glfwSetWindowPos(rb_get_window(self), NUM2INT(x), NUM2INT(y));
  return self;
}



/* GLFWAPI void glfwGetWindowSize(GLFWwindow* window, int* width, int* height); */

static VALUE rb_window_get_size(VALUE self)
{
  int width = 0;
  int height = 0;
  glfwGetWindowSize(rb_get_window(self), &width, &height);
  return rb_ary_new3(2, INT2FIX(width), INT2FIX(height));
}



/* GLFWAPI void glfwSetWindowSize(GLFWwindow* window, int width, int height); */

static VALUE rb_window_set_size(VALUE self, VALUE width, VALUE height)
{
  glfwSetWindowSize(rb_get_window(self), NUM2INT(width), NUM2INT(height));
  return self;
}



/* GLFWAPI void glfwGetFramebufferSize(GLFWwindow* window, int* width, int* height); */

static VALUE rb_window_get_framebuffer_size(VALUE self)
{
  int width = 0;
  int height = 0;
  glfwGetFramebufferSize(rb_get_window(self), &width, &height);
  return rb_ary_new3(2, INT2FIX(width), INT2FIX(height));
}



/* GLFWAPI void glfwIconifyWindow(GLFWwindow* window); */

static VALUE rb_window_iconify(VALUE self)
{
  glfwIconifyWindow(rb_get_window(self));
  return self;
}



/* GLFWAPI void glfwRestoreWindow(GLFWwindow* window); */

static VALUE rb_window_restore(VALUE self)
{
  glfwRestoreWindow(rb_get_window(self));
  return self;
}



/* GLFWAPI void glfwShowWindow(GLFWwindow* window); */

static VALUE rb_window_show(VALUE self)
{
  glfwShowWindow(rb_get_window(self));
  return self;
}



/* GLFWAPI void glfwHideWindow(GLFWwindow* window); */

static VALUE rb_window_hide(VALUE self)
{
  glfwHideWindow(rb_get_window(self));
  return self;
}



/* GLFWAPI GLFWmonitor* glfwGetWindowMonitor(GLFWwindow* window); */

static VALUE rb_window_get_monitor(VALUE self)
{
  GLFWmonitor *monitor = glfwGetWindowMonitor(rb_get_window(self));
  VALUE rb_monitor = Data_Wrap_Struct(s_glfw_monitor_klass, 0, 0, monitor);
  rb_obj_call_init(rb_monitor, 0, 0);
  return rb_monitor;
}



/* GLFWAPI int glfwGetWindowAttrib(GLFWwindow* window, int attrib); */





/* GLFWAPI GLFWwindowposfun glfwSetWindowPosCallback(GLFWwindow* window, GLFWwindowposfun cbfun); */

static void rb_window_window_position_callback(GLFWwindow *window, int x, int y)
{
  VALUE rb_window = rb_lookup_window(window);
  VALUE rb_func = rb_ivar_get(rb_window, rb_intern(kRB_IVAR_WINDOW_POSITION_CALLBACK));
  rb_funcall(rb_func, rb_intern("call"), 3, rb_window, INT2FIX(x), INT2FIX(y));
}

RB_ENABLE_CALLBACK_DEF(rb_window_set_window_position_callback, rb_window_window_position_callback, glfwSetWindowPosCallback);




/* GLFWAPI GLFWwindowsizefun glfwSetWindowSizeCallback(GLFWwindow* window, GLFWwindowsizefun cbfun); */

static void rb_window_window_size_callback(GLFWwindow *window, int width, int height)
{
  VALUE rb_window = rb_lookup_window(window);
  VALUE rb_func = rb_ivar_get(rb_window, rb_intern(kRB_IVAR_WINDOW_SIZE_CALLBACK));
  rb_funcall(rb_func, rb_intern("call"), 3, rb_window, INT2FIX(width), INT2FIX(height));
}

RB_ENABLE_CALLBACK_DEF(rb_window_set_window_size_callback, rb_window_window_size_callback, glfwSetWindowSizeCallback);



/* GLFWAPI GLFWwindowclosefun glfwSetWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun cbfun); */

static void rb_window_close_callback(GLFWwindow *window)
{
  VALUE rb_window = rb_lookup_window(window);
  VALUE rb_func = rb_ivar_get(rb_window, rb_intern(kRB_IVAR_WINDOW_CLOSE_CALLBACK));
  rb_funcall(rb_func, rb_intern("call"), 1, rb_window);
}

RB_ENABLE_CALLBACK_DEF(rb_window_set_close_callback, rb_window_close_callback, glfwSetWindowCloseCallback);




/* GLFWAPI GLFWwindowrefreshfun glfwSetWindowRefreshCallback(GLFWwindow* window, GLFWwindowrefreshfun cbfun); */

static void rb_window_refresh_callback(GLFWwindow *window)
{
  VALUE rb_window = rb_lookup_window(window);
  VALUE rb_func = rb_ivar_get(rb_window, rb_intern(kRB_IVAR_WINDOW_REFRESH_CALLBACK));
  rb_funcall(rb_func, rb_intern("call"), 1, rb_window);
}

RB_ENABLE_CALLBACK_DEF(rb_window_set_refresh_callback, rb_window_refresh_callback, glfwSetWindowRefreshCallback);



/* GLFWAPI GLFWwindowfocusfun glfwSetWindowFocusCallback(GLFWwindow* window, GLFWwindowfocusfun cbfun); */

static void rb_window_focus_callback(GLFWwindow *window, int focused)
{
  VALUE rb_window = rb_lookup_window(window);
  VALUE rb_func = rb_ivar_get(rb_window, rb_intern(kRB_IVAR_WINDOW_FOCUS_CALLBACK));
  rb_funcall(rb_func, rb_intern("call"), 2, rb_window, focused ? Qtrue : Qfalse);
}

RB_ENABLE_CALLBACK_DEF(rb_window_set_focus_callback, rb_window_focus_callback, glfwSetWindowFocusCallback);



/* GLFWAPI GLFWwindowiconifyfun glfwSetWindowIconifyCallback(GLFWwindow* window, GLFWwindowiconifyfun cbfun); */

static void rb_window_iconify_callback(GLFWwindow *window, int iconified)
{
  VALUE rb_window = rb_lookup_window(window);
  VALUE rb_func = rb_ivar_get(rb_window, rb_intern(kRB_IVAR_WINDOW_ICONIFY_CALLBACK));
  rb_funcall(rb_func, rb_intern("call"), 2, rb_window, iconified ? Qtrue : Qfalse);
}

RB_ENABLE_CALLBACK_DEF(rb_window_set_iconify_callback, rb_window_iconify_callback, glfwSetWindowIconifyCallback);



/* GLFWAPI GLFWframebuffersizefun glfwSetFramebufferSizeCallback(GLFWwindow* window, GLFWframebuffersizefun cbfun) */


static void rb_window_fbsize_callback(GLFWwindow *window, int width, int height)
{
  VALUE rb_window = rb_lookup_window(window);
  VALUE rb_func = rb_ivar_get(rb_window, rb_intern(kRB_IVAR_WINDOW_FRAMEBUFFER_SIZE_CALLBACK));
  rb_funcall(rb_func, rb_intern("call"), 3, rb_window, INT2FIX(width), INT2FIX(height));
}

RB_ENABLE_CALLBACK_DEF(rb_window_set_fbsize_callback, rb_window_fbsize_callback, glfwSetFramebufferSizeCallback);



/* GLFWAPI void glfwPollEvents(void); */

static VALUE rb_glfw_poll_events(VALUE self)
{
  glfwPollEvents();
  return self;
}



/* GLFWAPI void glfwWaitEvents(void); */

static VALUE rb_glfw_wait_events(VALUE self)
{
  glfwWaitEvents();
  return self;
}



/* GLFWAPI int glfwGetInputMode(GLFWwindow* window, int mode); */

static VALUE rb_window_get_input_mode(VALUE self, VALUE mode)
{
  return INT2FIX(glfwGetInputMode(rb_get_window(self), NUM2INT(mode)));
}



/* GLFWAPI void glfwSetInputMode(GLFWwindow* window, int mode, int value); */

static VALUE rb_window_set_input_mode(VALUE self, VALUE mode, VALUE value)
{
  glfwSetInputMode(rb_get_window(self), NUM2INT(mode), NUM2INT(value));
  return self;
}



/* GLFWAPI int glfwGetKey(GLFWwindow* window, int key); */

static VALUE rb_window_get_key(VALUE self, VALUE key)
{
  return INT2FIX(glfwGetKey(rb_get_window(self), NUM2INT(key)));
}



/* GLFWAPI int glfwGetMouseButton(GLFWwindow* window, int button); */

static VALUE rb_window_get_mouse_button(VALUE self, VALUE button)
{
  return INT2FIX(glfwGetMouseButton(rb_get_window(self), NUM2INT(button)));
}



/* GLFWAPI void glfwGetCursorPos(GLFWwindow* window, double* xpos, double* ypos); */

static VALUE rb_window_get_cursor_pos(VALUE self)
{
  double xpos = 0;
  double ypos = 0;
  glfwGetCursorPos(rb_get_window(self), &xpos, &ypos);
  return rb_ary_new3(2, rb_float_new(xpos), rb_float_new(ypos));
}



/* GLFWAPI void glfwSetCursorPos(GLFWwindow* window, double xpos, double ypos); */

static VALUE rb_window_set_cursor_pos(VALUE self, VALUE x, VALUE y)
{
  glfwSetCursorPos(rb_get_window(self), NUM2DBL(x), NUM2DBL(y));
  return self;
}



/* GLFWAPI GLFWkeyfun glfwSetKeyCallback(GLFWwindow* window, GLFWkeyfun cbfun); */

static void rb_window_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  VALUE rb_window = rb_lookup_window(window);
  VALUE rb_func = rb_ivar_get(rb_window, rb_intern(kRB_IVAR_WINDOW_KEY_CALLBACK));
  rb_funcall(rb_func, rb_intern("call"), 5, rb_window, INT2FIX(key), INT2FIX(scancode), INT2FIX(action), INT2FIX(mods));
}

RB_ENABLE_CALLBACK_DEF(rb_window_set_key_callback, rb_window_key_callback, glfwSetKeyCallback);



/* GLFWAPI GLFWcharfun glfwSetCharCallback(GLFWwindow* window, GLFWcharfun cbfun); */

static void rb_window_char_callback(GLFWwindow *window, unsigned int code)
{
  VALUE rb_window = rb_lookup_window(window);
  VALUE rb_func = rb_ivar_get(rb_window, rb_intern(kRB_IVAR_WINDOW_CHAR_CALLBACK));
  rb_funcall(rb_func, rb_intern("call"), 2, rb_window, UINT2NUM(code));
}

RB_ENABLE_CALLBACK_DEF(rb_window_set_char_callback, rb_window_char_callback, glfwSetCharCallback);




/* GLFWAPI GLFWmousebuttonfun glfwSetMouseButtonCallback(GLFWwindow* window, GLFWmousebuttonfun cbfun); */

static void rb_window_mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
  VALUE rb_window = rb_lookup_window(window);
  VALUE rb_func = rb_ivar_get(rb_window, rb_intern(kRB_IVAR_WINDOW_MOUSE_BUTTON_CALLBACK));
  rb_funcall(rb_func, rb_intern("call"), 4, rb_window, INT2FIX(button), INT2FIX(action), INT2FIX(mods));
}

RB_ENABLE_CALLBACK_DEF(rb_window_set_mouse_button_callback, rb_window_mouse_button_callback, glfwSetMouseButtonCallback);



/* GLFWAPI GLFWcursorposfun glfwSetCursorPosCallback(GLFWwindow* window, GLFWcursorposfun cbfun); */

static void rb_window_cursor_position_callback(GLFWwindow *window, double x, double y)
{
  VALUE rb_window = rb_lookup_window(window);
  VALUE rb_func = rb_ivar_get(rb_window, rb_intern(kRB_IVAR_WINDOW_CURSOR_POSITION_CALLBACK));
  rb_funcall(rb_func, rb_intern("call"), 4, rb_window, rb_float_new(x), rb_float_new(y));
}

RB_ENABLE_CALLBACK_DEF(rb_window_set_cursor_position_callback, rb_window_cursor_position_callback, glfwSetCursorPosCallback);



/* GLFWAPI GLFWcursorenterfun glfwSetCursorEnterCallback(GLFWwindow* window, GLFWcursorenterfun cbfun); */

static void rb_window_cursor_enter_callback(GLFWwindow *window, int entered)
{
  VALUE rb_window = rb_lookup_window(window);
  VALUE rb_func = rb_ivar_get(rb_window, rb_intern(kRB_IVAR_WINDOW_CURSOR_ENTER_CALLBACK));
  rb_funcall(rb_func, rb_intern("call"), 2, rb_window, entered ? Qtrue : Qfalse);
}

RB_ENABLE_CALLBACK_DEF(rb_window_set_cursor_enter_callback, rb_window_cursor_enter_callback, glfwSetCursorEnterCallback);



/* GLFWAPI GLFWscrollfun glfwSetScrollCallback(GLFWwindow* window, GLFWscrollfun cbfun); */

static void rb_window_scroll_callback(GLFWwindow *window, double x, double y)
{
  VALUE rb_window = rb_lookup_window(window);
  VALUE rb_func = rb_ivar_get(rb_window, rb_intern(kRB_IVAR_WINDOW_SCROLL_CALLBACK));
  rb_funcall(rb_func, rb_intern("call"), 4, rb_window, rb_float_new(x), rb_float_new(y));
}

RB_ENABLE_CALLBACK_DEF(rb_window_set_scroll_callback, rb_window_scroll_callback, glfwSetScrollCallback);




/* GLFWAPI int glfwJoystickPresent(int joy); */

static VALUE rb_glfw_joystick_present(VALUE self, VALUE joystick)
{
  return glfwJoystickPresent(NUM2INT(joystick)) ? Qtrue : Qfalse;
}



/* GLFWAPI const float* glfwGetJoystickAxes(int joy, int* count); */

static VALUE rb_glfw_get_joystick_axes(VALUE self, VALUE joystick)
{
  VALUE rb_axes = Qnil;
  int num_axes = 0;
  const float *axes = glfwGetJoystickAxes(NUM2INT(joystick), &num_axes);
  if (num_axes > 0) {
    rb_axes = rb_ary_new();
    for (; num_axes; --num_axes, ++axes) {
      rb_ary_push(rb_axes, rb_float_new(*axes));
    }
  }
  return rb_axes;
}



/* GLFWAPI const unsigned char* glfwGetJoystickButtons(int joy, int* count); */

static VALUE rb_glfw_get_joystick_buttons(VALUE self, VALUE joystick)
{
  VALUE rb_buttons = Qnil;
  int num_buttons = 0;
  const unsigned char *buttons = glfwGetJoystickButtons(NUM2INT(joystick), &num_buttons);
  if (num_buttons > 0) {
    rb_buttons = rb_ary_new();
    for (; num_buttons; --num_buttons, ++buttons) {
      rb_ary_push(rb_buttons, rb_float_new(INT2FIX((int)*buttons)));
    }
  }
  return rb_buttons;
}



/* GLFWAPI const char* glfwGetJoystickName(int joy); */

static VALUE rb_glfw_get_joystick_name(VALUE self, VALUE joystick)
{
  const char *joy_name = glfwGetJoystickName(NUM2INT(joystick));
  if (joy_name) {
    return rb_str_new2(joy_name);
  } else {
    return Qnil;
  }
}



/* GLFWAPI void glfwSetClipboardString(GLFWwindow* window, const char* string); */

static VALUE rb_window_set_clipboard_string(VALUE self, VALUE string)
{
  glfwSetClipboardString(rb_get_window(self), StringValueCStr(string));
  return string;
}



/* GLFWAPI const char* glfwGetClipboardString(GLFWwindow* window); */

static VALUE rb_window_get_clipboard_string(VALUE self)
{
  return rb_str_new2(glfwGetClipboardString(rb_get_window(self)));
}



/* GLFWAPI double glfwGetTime(void); */

static VALUE rb_glfw_get_time(VALUE self)
{
  return rb_float_new(glfwGetTime());
}



/* GLFWAPI void glfwSetTime(double time); */

static VALUE rb_glfw_set_time(VALUE self, VALUE time_)
{
  glfwSetTime(NUM2DBL(time_));
  return time_;
}



/* GLFWAPI void glfwMakeContextCurrent(GLFWwindow* window); */

static VALUE rb_window_make_context_current(VALUE self)
{
  glfwMakeContextCurrent(rb_get_window(self));
  return self;
}



/* GLFWAPI GLFWwindow* glfwGetCurrentContext(void); */

static VALUE rb_window_get_current_context(VALUE self)
{
  GLFWwindow *window = glfwGetCurrentContext();
  if (window) {
    return (VALUE)glfwGetWindowUserPointer(window);
  } else {
    return Qnil;
  }
}



/* GLFWAPI void glfwSwapBuffers(GLFWwindow* window); */

static VALUE rb_window_swap_buffers(VALUE self)
{
  glfwSwapBuffers(rb_get_window(self));
  return self;
}



/* GLFWAPI void glfwSwapInterval(int interval); */

static VALUE rb_glfw_swap_interval(VALUE self)
{
  return self;
}



/* GLFWAPI int glfwExtensionSupported(const char* extension); */

static VALUE rb_glfw_extension_supported(VALUE self)
{
  return self;
}



void Init_glfw3(void)
{
  s_glfw_module = rb_define_module("Glfw");
  s_glfw_monitor_klass = rb_define_class_under(s_glfw_module, "Monitor", rb_cObject);
  s_glfw_window_klass = rb_define_class_under(s_glfw_module, "Window", rb_cObject);
  s_glfw_window_internal_klass = rb_define_class_under(s_glfw_window_klass, "InternalWindow", rb_cObject);
  s_glfw_videomode_klass = rb_define_class_under(s_glfw_module, "VideoMode", rb_cObject);

  /* Glfw::Monitor */
  rb_define_singleton_method(s_glfw_monitor_klass, "monitors", rb_glfw_get_monitors, 0);
  rb_define_singleton_method(s_glfw_monitor_klass, "primary_monitor", rb_glfw_get_primary_monitor, 0);
  rb_define_method(s_glfw_monitor_klass, "name", rb_monitor_name, 0);
  rb_define_method(s_glfw_monitor_klass, "position", rb_monitor_position, 0);
  rb_define_method(s_glfw_monitor_klass, "physical_size", rb_monitor_physical_size, 0);
  rb_define_method(s_glfw_monitor_klass, "video_modes", rb_monitor_video_modes, 0);
  rb_define_method(s_glfw_monitor_klass, "video_mode", rb_monitor_video_mode, 0);
  rb_define_method(s_glfw_monitor_klass, "set_gamma", rb_monitor_set_gamma, 1);

  /* Glfw::VideoMode */
  rb_define_method(s_glfw_videomode_klass, "width", rb_videomode_width, 0);
  rb_define_method(s_glfw_videomode_klass, "height", rb_videomode_height, 0);
  rb_define_method(s_glfw_videomode_klass, "red_bits", rb_videomode_red_bits, 0);
  rb_define_method(s_glfw_videomode_klass, "green_bits", rb_videomode_green_bits, 0);
  rb_define_method(s_glfw_videomode_klass, "blue_bits", rb_videomode_blue_bits, 0);
  rb_define_method(s_glfw_videomode_klass, "refresh_rate", rb_videomode_refresh_rate, 0);

  /* Glfw::Window */
  rb_define_singleton_method(s_glfw_window_klass, "new", rb_window_new, -1);
  rb_define_singleton_method(s_glfw_window_klass, "window_hint", rb_window_window_hint, 2);
  rb_define_singleton_method(s_glfw_window_klass, "default_window_hints", rb_window_default_window_hints, 0);
  // rb_define_method(s_glfw_window_klass, "initialize", rb_window_init, -1);
  rb_define_method(s_glfw_window_klass, "destroy", rb_window_destroy, 0);
  rb_define_method(s_glfw_window_klass, "should_close?", rb_window_should_close, 0);
  rb_define_method(s_glfw_window_klass, "should_close=", rb_window_set_should_close, 1);
  rb_define_method(s_glfw_window_klass, "set_should_close", rb_window_set_should_close, 1);
  rb_define_method(s_glfw_window_klass, "current_context", rb_window_get_current_context, 0);
  rb_define_method(s_glfw_window_klass, "make_context_current", rb_window_make_context_current, 0);
  rb_define_method(s_glfw_window_klass, "swap_buffers", rb_window_swap_buffers, 0);
  rb_define_method(s_glfw_window_klass, "title=", rb_window_set_title, 1);
  rb_define_method(s_glfw_window_klass, "position", rb_window_get_position, 0);
  rb_define_method(s_glfw_window_klass, "move", rb_window_set_position, 2);
  rb_define_method(s_glfw_window_klass, "size", rb_window_get_size, 0);
  rb_define_method(s_glfw_window_klass, "resize", rb_window_set_size, 2);
  rb_define_method(s_glfw_window_klass, "framebuffer_size", rb_window_get_framebuffer_size, 0);
  rb_define_method(s_glfw_window_klass, "iconify", rb_window_iconify, 0);
  rb_define_method(s_glfw_window_klass, "restore", rb_window_restore, 0);
  rb_define_method(s_glfw_window_klass, "show", rb_window_show, 0);
  rb_define_method(s_glfw_window_klass, "hide", rb_window_hide, 0);
  rb_define_method(s_glfw_window_klass, "monitor", rb_window_get_monitor, 0);
  rb_define_method(s_glfw_window_klass, "set_window_position_callback__", rb_window_set_window_position_callback, 1);
  rb_define_method(s_glfw_window_klass, "set_window_size_callback__", rb_window_set_window_size_callback, 1);
  rb_define_method(s_glfw_window_klass, "set_close_callback__", rb_window_set_close_callback, 1);
  rb_define_method(s_glfw_window_klass, "set_refresh_callback__", rb_window_set_refresh_callback, 1);
  rb_define_method(s_glfw_window_klass, "set_focus_callback__", rb_window_set_focus_callback, 1);
  rb_define_method(s_glfw_window_klass, "set_iconify_callback__", rb_window_set_iconify_callback, 1);
  rb_define_method(s_glfw_window_klass, "set_fbsize_callback__", rb_window_set_fbsize_callback, 1);
  rb_define_method(s_glfw_window_klass, "get_input_mode", rb_window_get_input_mode, 1);
  rb_define_method(s_glfw_window_klass, "set_input_mode", rb_window_set_input_mode, 2);
  rb_define_method(s_glfw_window_klass, "key", rb_window_get_key, 1);
  rb_define_method(s_glfw_window_klass, "mouse_button", rb_window_get_mouse_button, 1);
  rb_define_method(s_glfw_window_klass, "cursor_pos", rb_window_get_cursor_pos, 0);
  rb_define_method(s_glfw_window_klass, "set_cursor_pos", rb_window_set_cursor_pos, 2);
  rb_define_method(s_glfw_window_klass, "set_key_callback__", rb_window_set_key_callback, 1);
  rb_define_method(s_glfw_window_klass, "set_char_callback__", rb_window_set_char_callback, 1);
  rb_define_method(s_glfw_window_klass, "set_mouse_button_callback__", rb_window_set_mouse_button_callback, 1);
  rb_define_method(s_glfw_window_klass, "set_cursor_position_callback__", rb_window_set_cursor_position_callback, 1);
  rb_define_method(s_glfw_window_klass, "set_cursor_enter_callback__", rb_window_set_cursor_enter_callback, 1);
  rb_define_method(s_glfw_window_klass, "set_scroll_callback__", rb_window_set_scroll_callback, 1);
  rb_define_method(s_glfw_window_klass, "clipboard_string=", rb_window_set_clipboard_string, 1);
  rb_define_method(s_glfw_window_klass, "clipboard_string", rb_window_get_clipboard_string, 0);
  rb_cvar_set(s_glfw_window_klass, rb_intern(kRB_CVAR_WINDOW_WINDOWS), rb_hash_new());

  /* Glfw */
  rb_cvar_set(s_glfw_module, rb_intern(kRB_CVAR_GLFW_ERROR_CALLBACK), Qnil);
  rb_cvar_set(s_glfw_module, rb_intern(kRB_CVAR_GLFW_MONITOR_CALLBACK), Qnil);
  rb_define_singleton_method(s_glfw_module, "version", rb_glfw_version, 0);
  rb_define_singleton_method(s_glfw_module, "terminate", rb_glfw_terminate, 0);
  rb_define_singleton_method(s_glfw_module, "init", rb_glfw_init, 0);
  rb_define_singleton_method(s_glfw_module, "poll_events", rb_glfw_poll_events, 0);
  rb_define_singleton_method(s_glfw_module, "wait_events", rb_glfw_wait_events, 0);
  rb_define_singleton_method(s_glfw_module, "joystick_present", rb_glfw_joystick_present, 1);
  rb_define_singleton_method(s_glfw_module, "joystick_axes", rb_glfw_get_joystick_axes, 1);
  rb_define_singleton_method(s_glfw_module, "joystick_buttons", rb_glfw_get_joystick_buttons, 1);
  rb_define_singleton_method(s_glfw_module, "joystick_name", rb_glfw_get_joystick_name, 1);
  rb_define_singleton_method(s_glfw_module, "time", rb_glfw_get_time, 0);
  rb_define_singleton_method(s_glfw_module, "time=", rb_glfw_set_time, 1);
  rb_define_singleton_method(s_glfw_module, "swap_interval=", rb_glfw_swap_interval, 1);
  rb_define_singleton_method(s_glfw_module, "extension_supported?", rb_glfw_extension_supported, 1);

  // DEFINE ALL THE THINGS
  rb_const_set(s_glfw_module, rb_intern("VERSION_MAJOR"), INT2FIX(GLFW_VERSION_MAJOR));
  rb_const_set(s_glfw_module, rb_intern("VERSION_MINOR"), INT2FIX(GLFW_VERSION_MINOR));
  rb_const_set(s_glfw_module, rb_intern("VERSION_REVISION"), INT2FIX(GLFW_VERSION_REVISION));
  rb_const_set(s_glfw_module, rb_intern("RELEASE"), INT2FIX(GLFW_RELEASE));
  rb_const_set(s_glfw_module, rb_intern("PRESS"), INT2FIX(GLFW_PRESS));
  rb_const_set(s_glfw_module, rb_intern("REPEAT"), INT2FIX(GLFW_REPEAT));
  rb_const_set(s_glfw_module, rb_intern("KEY_UNKNOWN"), INT2FIX(GLFW_KEY_UNKNOWN));
  rb_const_set(s_glfw_module, rb_intern("KEY_SPACE"), INT2FIX(GLFW_KEY_SPACE));
  rb_const_set(s_glfw_module, rb_intern("KEY_APOSTROPHE"), INT2FIX(GLFW_KEY_APOSTROPHE));
  rb_const_set(s_glfw_module, rb_intern("KEY_COMMA"), INT2FIX(GLFW_KEY_COMMA));
  rb_const_set(s_glfw_module, rb_intern("KEY_MINUS"), INT2FIX(GLFW_KEY_MINUS));
  rb_const_set(s_glfw_module, rb_intern("KEY_PERIOD"), INT2FIX(GLFW_KEY_PERIOD));
  rb_const_set(s_glfw_module, rb_intern("KEY_SLASH"), INT2FIX(GLFW_KEY_SLASH));
  rb_const_set(s_glfw_module, rb_intern("KEY_0"), INT2FIX(GLFW_KEY_0));
  rb_const_set(s_glfw_module, rb_intern("KEY_1"), INT2FIX(GLFW_KEY_1));
  rb_const_set(s_glfw_module, rb_intern("KEY_2"), INT2FIX(GLFW_KEY_2));
  rb_const_set(s_glfw_module, rb_intern("KEY_3"), INT2FIX(GLFW_KEY_3));
  rb_const_set(s_glfw_module, rb_intern("KEY_4"), INT2FIX(GLFW_KEY_4));
  rb_const_set(s_glfw_module, rb_intern("KEY_5"), INT2FIX(GLFW_KEY_5));
  rb_const_set(s_glfw_module, rb_intern("KEY_6"), INT2FIX(GLFW_KEY_6));
  rb_const_set(s_glfw_module, rb_intern("KEY_7"), INT2FIX(GLFW_KEY_7));
  rb_const_set(s_glfw_module, rb_intern("KEY_8"), INT2FIX(GLFW_KEY_8));
  rb_const_set(s_glfw_module, rb_intern("KEY_9"), INT2FIX(GLFW_KEY_9));
  rb_const_set(s_glfw_module, rb_intern("KEY_SEMICOLON"), INT2FIX(GLFW_KEY_SEMICOLON));
  rb_const_set(s_glfw_module, rb_intern("KEY_EQUAL"), INT2FIX(GLFW_KEY_EQUAL));
  rb_const_set(s_glfw_module, rb_intern("KEY_A"), INT2FIX(GLFW_KEY_A));
  rb_const_set(s_glfw_module, rb_intern("KEY_B"), INT2FIX(GLFW_KEY_B));
  rb_const_set(s_glfw_module, rb_intern("KEY_C"), INT2FIX(GLFW_KEY_C));
  rb_const_set(s_glfw_module, rb_intern("KEY_D"), INT2FIX(GLFW_KEY_D));
  rb_const_set(s_glfw_module, rb_intern("KEY_E"), INT2FIX(GLFW_KEY_E));
  rb_const_set(s_glfw_module, rb_intern("KEY_F"), INT2FIX(GLFW_KEY_F));
  rb_const_set(s_glfw_module, rb_intern("KEY_G"), INT2FIX(GLFW_KEY_G));
  rb_const_set(s_glfw_module, rb_intern("KEY_H"), INT2FIX(GLFW_KEY_H));
  rb_const_set(s_glfw_module, rb_intern("KEY_I"), INT2FIX(GLFW_KEY_I));
  rb_const_set(s_glfw_module, rb_intern("KEY_J"), INT2FIX(GLFW_KEY_J));
  rb_const_set(s_glfw_module, rb_intern("KEY_K"), INT2FIX(GLFW_KEY_K));
  rb_const_set(s_glfw_module, rb_intern("KEY_L"), INT2FIX(GLFW_KEY_L));
  rb_const_set(s_glfw_module, rb_intern("KEY_M"), INT2FIX(GLFW_KEY_M));
  rb_const_set(s_glfw_module, rb_intern("KEY_N"), INT2FIX(GLFW_KEY_N));
  rb_const_set(s_glfw_module, rb_intern("KEY_O"), INT2FIX(GLFW_KEY_O));
  rb_const_set(s_glfw_module, rb_intern("KEY_P"), INT2FIX(GLFW_KEY_P));
  rb_const_set(s_glfw_module, rb_intern("KEY_Q"), INT2FIX(GLFW_KEY_Q));
  rb_const_set(s_glfw_module, rb_intern("KEY_R"), INT2FIX(GLFW_KEY_R));
  rb_const_set(s_glfw_module, rb_intern("KEY_S"), INT2FIX(GLFW_KEY_S));
  rb_const_set(s_glfw_module, rb_intern("KEY_T"), INT2FIX(GLFW_KEY_T));
  rb_const_set(s_glfw_module, rb_intern("KEY_U"), INT2FIX(GLFW_KEY_U));
  rb_const_set(s_glfw_module, rb_intern("KEY_V"), INT2FIX(GLFW_KEY_V));
  rb_const_set(s_glfw_module, rb_intern("KEY_W"), INT2FIX(GLFW_KEY_W));
  rb_const_set(s_glfw_module, rb_intern("KEY_X"), INT2FIX(GLFW_KEY_X));
  rb_const_set(s_glfw_module, rb_intern("KEY_Y"), INT2FIX(GLFW_KEY_Y));
  rb_const_set(s_glfw_module, rb_intern("KEY_Z"), INT2FIX(GLFW_KEY_Z));
  rb_const_set(s_glfw_module, rb_intern("KEY_LEFT_BRACKET"), INT2FIX(GLFW_KEY_LEFT_BRACKET));
  rb_const_set(s_glfw_module, rb_intern("KEY_BACKSLASH"), INT2FIX(GLFW_KEY_BACKSLASH));
  rb_const_set(s_glfw_module, rb_intern("KEY_RIGHT_BRACKET"), INT2FIX(GLFW_KEY_RIGHT_BRACKET));
  rb_const_set(s_glfw_module, rb_intern("KEY_GRAVE_ACCENT"), INT2FIX(GLFW_KEY_GRAVE_ACCENT));
  rb_const_set(s_glfw_module, rb_intern("KEY_WORLD_1"), INT2FIX(GLFW_KEY_WORLD_1));
  rb_const_set(s_glfw_module, rb_intern("KEY_WORLD_2"), INT2FIX(GLFW_KEY_WORLD_2));
  rb_const_set(s_glfw_module, rb_intern("KEY_ESCAPE"), INT2FIX(GLFW_KEY_ESCAPE));
  rb_const_set(s_glfw_module, rb_intern("KEY_ENTER"), INT2FIX(GLFW_KEY_ENTER));
  rb_const_set(s_glfw_module, rb_intern("KEY_TAB"), INT2FIX(GLFW_KEY_TAB));
  rb_const_set(s_glfw_module, rb_intern("KEY_BACKSPACE"), INT2FIX(GLFW_KEY_BACKSPACE));
  rb_const_set(s_glfw_module, rb_intern("KEY_INSERT"), INT2FIX(GLFW_KEY_INSERT));
  rb_const_set(s_glfw_module, rb_intern("KEY_DELETE"), INT2FIX(GLFW_KEY_DELETE));
  rb_const_set(s_glfw_module, rb_intern("KEY_RIGHT"), INT2FIX(GLFW_KEY_RIGHT));
  rb_const_set(s_glfw_module, rb_intern("KEY_LEFT"), INT2FIX(GLFW_KEY_LEFT));
  rb_const_set(s_glfw_module, rb_intern("KEY_DOWN"), INT2FIX(GLFW_KEY_DOWN));
  rb_const_set(s_glfw_module, rb_intern("KEY_UP"), INT2FIX(GLFW_KEY_UP));
  rb_const_set(s_glfw_module, rb_intern("KEY_PAGE_UP"), INT2FIX(GLFW_KEY_PAGE_UP));
  rb_const_set(s_glfw_module, rb_intern("KEY_PAGE_DOWN"), INT2FIX(GLFW_KEY_PAGE_DOWN));
  rb_const_set(s_glfw_module, rb_intern("KEY_HOME"), INT2FIX(GLFW_KEY_HOME));
  rb_const_set(s_glfw_module, rb_intern("KEY_END"), INT2FIX(GLFW_KEY_END));
  rb_const_set(s_glfw_module, rb_intern("KEY_CAPS_LOCK"), INT2FIX(GLFW_KEY_CAPS_LOCK));
  rb_const_set(s_glfw_module, rb_intern("KEY_SCROLL_LOCK"), INT2FIX(GLFW_KEY_SCROLL_LOCK));
  rb_const_set(s_glfw_module, rb_intern("KEY_NUM_LOCK"), INT2FIX(GLFW_KEY_NUM_LOCK));
  rb_const_set(s_glfw_module, rb_intern("KEY_PRINT_SCREEN"), INT2FIX(GLFW_KEY_PRINT_SCREEN));
  rb_const_set(s_glfw_module, rb_intern("KEY_PAUSE"), INT2FIX(GLFW_KEY_PAUSE));
  rb_const_set(s_glfw_module, rb_intern("KEY_F1"), INT2FIX(GLFW_KEY_F1));
  rb_const_set(s_glfw_module, rb_intern("KEY_F2"), INT2FIX(GLFW_KEY_F2));
  rb_const_set(s_glfw_module, rb_intern("KEY_F3"), INT2FIX(GLFW_KEY_F3));
  rb_const_set(s_glfw_module, rb_intern("KEY_F4"), INT2FIX(GLFW_KEY_F4));
  rb_const_set(s_glfw_module, rb_intern("KEY_F5"), INT2FIX(GLFW_KEY_F5));
  rb_const_set(s_glfw_module, rb_intern("KEY_F6"), INT2FIX(GLFW_KEY_F6));
  rb_const_set(s_glfw_module, rb_intern("KEY_F7"), INT2FIX(GLFW_KEY_F7));
  rb_const_set(s_glfw_module, rb_intern("KEY_F8"), INT2FIX(GLFW_KEY_F8));
  rb_const_set(s_glfw_module, rb_intern("KEY_F9"), INT2FIX(GLFW_KEY_F9));
  rb_const_set(s_glfw_module, rb_intern("KEY_F10"), INT2FIX(GLFW_KEY_F10));
  rb_const_set(s_glfw_module, rb_intern("KEY_F11"), INT2FIX(GLFW_KEY_F11));
  rb_const_set(s_glfw_module, rb_intern("KEY_F12"), INT2FIX(GLFW_KEY_F12));
  rb_const_set(s_glfw_module, rb_intern("KEY_F13"), INT2FIX(GLFW_KEY_F13));
  rb_const_set(s_glfw_module, rb_intern("KEY_F14"), INT2FIX(GLFW_KEY_F14));
  rb_const_set(s_glfw_module, rb_intern("KEY_F15"), INT2FIX(GLFW_KEY_F15));
  rb_const_set(s_glfw_module, rb_intern("KEY_F16"), INT2FIX(GLFW_KEY_F16));
  rb_const_set(s_glfw_module, rb_intern("KEY_F17"), INT2FIX(GLFW_KEY_F17));
  rb_const_set(s_glfw_module, rb_intern("KEY_F18"), INT2FIX(GLFW_KEY_F18));
  rb_const_set(s_glfw_module, rb_intern("KEY_F19"), INT2FIX(GLFW_KEY_F19));
  rb_const_set(s_glfw_module, rb_intern("KEY_F20"), INT2FIX(GLFW_KEY_F20));
  rb_const_set(s_glfw_module, rb_intern("KEY_F21"), INT2FIX(GLFW_KEY_F21));
  rb_const_set(s_glfw_module, rb_intern("KEY_F22"), INT2FIX(GLFW_KEY_F22));
  rb_const_set(s_glfw_module, rb_intern("KEY_F23"), INT2FIX(GLFW_KEY_F23));
  rb_const_set(s_glfw_module, rb_intern("KEY_F24"), INT2FIX(GLFW_KEY_F24));
  rb_const_set(s_glfw_module, rb_intern("KEY_F25"), INT2FIX(GLFW_KEY_F25));
  rb_const_set(s_glfw_module, rb_intern("KEY_KP_0"), INT2FIX(GLFW_KEY_KP_0));
  rb_const_set(s_glfw_module, rb_intern("KEY_KP_1"), INT2FIX(GLFW_KEY_KP_1));
  rb_const_set(s_glfw_module, rb_intern("KEY_KP_2"), INT2FIX(GLFW_KEY_KP_2));
  rb_const_set(s_glfw_module, rb_intern("KEY_KP_3"), INT2FIX(GLFW_KEY_KP_3));
  rb_const_set(s_glfw_module, rb_intern("KEY_KP_4"), INT2FIX(GLFW_KEY_KP_4));
  rb_const_set(s_glfw_module, rb_intern("KEY_KP_5"), INT2FIX(GLFW_KEY_KP_5));
  rb_const_set(s_glfw_module, rb_intern("KEY_KP_6"), INT2FIX(GLFW_KEY_KP_6));
  rb_const_set(s_glfw_module, rb_intern("KEY_KP_7"), INT2FIX(GLFW_KEY_KP_7));
  rb_const_set(s_glfw_module, rb_intern("KEY_KP_8"), INT2FIX(GLFW_KEY_KP_8));
  rb_const_set(s_glfw_module, rb_intern("KEY_KP_9"), INT2FIX(GLFW_KEY_KP_9));
  rb_const_set(s_glfw_module, rb_intern("KEY_KP_DECIMAL"), INT2FIX(GLFW_KEY_KP_DECIMAL));
  rb_const_set(s_glfw_module, rb_intern("KEY_KP_DIVIDE"), INT2FIX(GLFW_KEY_KP_DIVIDE));
  rb_const_set(s_glfw_module, rb_intern("KEY_KP_MULTIPLY"), INT2FIX(GLFW_KEY_KP_MULTIPLY));
  rb_const_set(s_glfw_module, rb_intern("KEY_KP_SUBTRACT"), INT2FIX(GLFW_KEY_KP_SUBTRACT));
  rb_const_set(s_glfw_module, rb_intern("KEY_KP_ADD"), INT2FIX(GLFW_KEY_KP_ADD));
  rb_const_set(s_glfw_module, rb_intern("KEY_KP_ENTER"), INT2FIX(GLFW_KEY_KP_ENTER));
  rb_const_set(s_glfw_module, rb_intern("KEY_KP_EQUAL"), INT2FIX(GLFW_KEY_KP_EQUAL));
  rb_const_set(s_glfw_module, rb_intern("KEY_LEFT_SHIFT"), INT2FIX(GLFW_KEY_LEFT_SHIFT));
  rb_const_set(s_glfw_module, rb_intern("KEY_LEFT_CONTROL"), INT2FIX(GLFW_KEY_LEFT_CONTROL));
  rb_const_set(s_glfw_module, rb_intern("KEY_LEFT_ALT"), INT2FIX(GLFW_KEY_LEFT_ALT));
  rb_const_set(s_glfw_module, rb_intern("KEY_LEFT_SUPER"), INT2FIX(GLFW_KEY_LEFT_SUPER));
  rb_const_set(s_glfw_module, rb_intern("KEY_RIGHT_SHIFT"), INT2FIX(GLFW_KEY_RIGHT_SHIFT));
  rb_const_set(s_glfw_module, rb_intern("KEY_RIGHT_CONTROL"), INT2FIX(GLFW_KEY_RIGHT_CONTROL));
  rb_const_set(s_glfw_module, rb_intern("KEY_RIGHT_ALT"), INT2FIX(GLFW_KEY_RIGHT_ALT));
  rb_const_set(s_glfw_module, rb_intern("KEY_RIGHT_SUPER"), INT2FIX(GLFW_KEY_RIGHT_SUPER));
  rb_const_set(s_glfw_module, rb_intern("KEY_MENU"), INT2FIX(GLFW_KEY_MENU));
  rb_const_set(s_glfw_module, rb_intern("KEY_LAST"), INT2FIX(GLFW_KEY_LAST));
  rb_const_set(s_glfw_module, rb_intern("MOD_SHIFT"), INT2FIX(GLFW_MOD_SHIFT));
  rb_const_set(s_glfw_module, rb_intern("MOD_CONTROL"), INT2FIX(GLFW_MOD_CONTROL));
  rb_const_set(s_glfw_module, rb_intern("MOD_ALT"), INT2FIX(GLFW_MOD_ALT));
  rb_const_set(s_glfw_module, rb_intern("MOD_SUPER"), INT2FIX(GLFW_MOD_SUPER));
  rb_const_set(s_glfw_module, rb_intern("MOUSE_BUTTON_1"), INT2FIX(GLFW_MOUSE_BUTTON_1));
  rb_const_set(s_glfw_module, rb_intern("MOUSE_BUTTON_2"), INT2FIX(GLFW_MOUSE_BUTTON_2));
  rb_const_set(s_glfw_module, rb_intern("MOUSE_BUTTON_3"), INT2FIX(GLFW_MOUSE_BUTTON_3));
  rb_const_set(s_glfw_module, rb_intern("MOUSE_BUTTON_4"), INT2FIX(GLFW_MOUSE_BUTTON_4));
  rb_const_set(s_glfw_module, rb_intern("MOUSE_BUTTON_5"), INT2FIX(GLFW_MOUSE_BUTTON_5));
  rb_const_set(s_glfw_module, rb_intern("MOUSE_BUTTON_6"), INT2FIX(GLFW_MOUSE_BUTTON_6));
  rb_const_set(s_glfw_module, rb_intern("MOUSE_BUTTON_7"), INT2FIX(GLFW_MOUSE_BUTTON_7));
  rb_const_set(s_glfw_module, rb_intern("MOUSE_BUTTON_8"), INT2FIX(GLFW_MOUSE_BUTTON_8));
  rb_const_set(s_glfw_module, rb_intern("MOUSE_BUTTON_LAST"), INT2FIX(GLFW_MOUSE_BUTTON_LAST));
  rb_const_set(s_glfw_module, rb_intern("MOUSE_BUTTON_LEFT"), INT2FIX(GLFW_MOUSE_BUTTON_LEFT));
  rb_const_set(s_glfw_module, rb_intern("MOUSE_BUTTON_RIGHT"), INT2FIX(GLFW_MOUSE_BUTTON_RIGHT));
  rb_const_set(s_glfw_module, rb_intern("MOUSE_BUTTON_MIDDLE"), INT2FIX(GLFW_MOUSE_BUTTON_MIDDLE));
  rb_const_set(s_glfw_module, rb_intern("JOYSTICK_1"), INT2FIX(GLFW_JOYSTICK_1));
  rb_const_set(s_glfw_module, rb_intern("JOYSTICK_2"), INT2FIX(GLFW_JOYSTICK_2));
  rb_const_set(s_glfw_module, rb_intern("JOYSTICK_3"), INT2FIX(GLFW_JOYSTICK_3));
  rb_const_set(s_glfw_module, rb_intern("JOYSTICK_4"), INT2FIX(GLFW_JOYSTICK_4));
  rb_const_set(s_glfw_module, rb_intern("JOYSTICK_5"), INT2FIX(GLFW_JOYSTICK_5));
  rb_const_set(s_glfw_module, rb_intern("JOYSTICK_6"), INT2FIX(GLFW_JOYSTICK_6));
  rb_const_set(s_glfw_module, rb_intern("JOYSTICK_7"), INT2FIX(GLFW_JOYSTICK_7));
  rb_const_set(s_glfw_module, rb_intern("JOYSTICK_8"), INT2FIX(GLFW_JOYSTICK_8));
  rb_const_set(s_glfw_module, rb_intern("JOYSTICK_9"), INT2FIX(GLFW_JOYSTICK_9));
  rb_const_set(s_glfw_module, rb_intern("JOYSTICK_10"), INT2FIX(GLFW_JOYSTICK_10));
  rb_const_set(s_glfw_module, rb_intern("JOYSTICK_11"), INT2FIX(GLFW_JOYSTICK_11));
  rb_const_set(s_glfw_module, rb_intern("JOYSTICK_12"), INT2FIX(GLFW_JOYSTICK_12));
  rb_const_set(s_glfw_module, rb_intern("JOYSTICK_13"), INT2FIX(GLFW_JOYSTICK_13));
  rb_const_set(s_glfw_module, rb_intern("JOYSTICK_14"), INT2FIX(GLFW_JOYSTICK_14));
  rb_const_set(s_glfw_module, rb_intern("JOYSTICK_15"), INT2FIX(GLFW_JOYSTICK_15));
  rb_const_set(s_glfw_module, rb_intern("JOYSTICK_16"), INT2FIX(GLFW_JOYSTICK_16));
  rb_const_set(s_glfw_module, rb_intern("JOYSTICK_LAST"), INT2FIX(GLFW_JOYSTICK_LAST));
  rb_const_set(s_glfw_module, rb_intern("NOT_INITIALIZED"), INT2FIX(GLFW_NOT_INITIALIZED));
  rb_const_set(s_glfw_module, rb_intern("NO_CURRENT_CONTEXT"), INT2FIX(GLFW_NO_CURRENT_CONTEXT));
  rb_const_set(s_glfw_module, rb_intern("INVALID_ENUM"), INT2FIX(GLFW_INVALID_ENUM));
  rb_const_set(s_glfw_module, rb_intern("INVALID_VALUE"), INT2FIX(GLFW_INVALID_VALUE));
  rb_const_set(s_glfw_module, rb_intern("OUT_OF_MEMORY"), INT2FIX(GLFW_OUT_OF_MEMORY));
  rb_const_set(s_glfw_module, rb_intern("API_UNAVAILABLE"), INT2FIX(GLFW_API_UNAVAILABLE));
  rb_const_set(s_glfw_module, rb_intern("VERSION_UNAVAILABLE"), INT2FIX(GLFW_VERSION_UNAVAILABLE));
  rb_const_set(s_glfw_module, rb_intern("PLATFORM_ERROR"), INT2FIX(GLFW_PLATFORM_ERROR));
  rb_const_set(s_glfw_module, rb_intern("FORMAT_UNAVAILABLE"), INT2FIX(GLFW_FORMAT_UNAVAILABLE));
  rb_const_set(s_glfw_module, rb_intern("FOCUSED"), INT2FIX(GLFW_FOCUSED));
  rb_const_set(s_glfw_module, rb_intern("ICONIFIED"), INT2FIX(GLFW_ICONIFIED));
  rb_const_set(s_glfw_module, rb_intern("RESIZABLE"), INT2FIX(GLFW_RESIZABLE));
  rb_const_set(s_glfw_module, rb_intern("VISIBLE"), INT2FIX(GLFW_VISIBLE));
  rb_const_set(s_glfw_module, rb_intern("DECORATED"), INT2FIX(GLFW_DECORATED));
  rb_const_set(s_glfw_module, rb_intern("RED_BITS"), INT2FIX(GLFW_RED_BITS));
  rb_const_set(s_glfw_module, rb_intern("GREEN_BITS"), INT2FIX(GLFW_GREEN_BITS));
  rb_const_set(s_glfw_module, rb_intern("BLUE_BITS"), INT2FIX(GLFW_BLUE_BITS));
  rb_const_set(s_glfw_module, rb_intern("ALPHA_BITS"), INT2FIX(GLFW_ALPHA_BITS));
  rb_const_set(s_glfw_module, rb_intern("DEPTH_BITS"), INT2FIX(GLFW_DEPTH_BITS));
  rb_const_set(s_glfw_module, rb_intern("STENCIL_BITS"), INT2FIX(GLFW_STENCIL_BITS));
  rb_const_set(s_glfw_module, rb_intern("ACCUM_RED_BITS"), INT2FIX(GLFW_ACCUM_RED_BITS));
  rb_const_set(s_glfw_module, rb_intern("ACCUM_GREEN_BITS"), INT2FIX(GLFW_ACCUM_GREEN_BITS));
  rb_const_set(s_glfw_module, rb_intern("ACCUM_BLUE_BITS"), INT2FIX(GLFW_ACCUM_BLUE_BITS));
  rb_const_set(s_glfw_module, rb_intern("ACCUM_ALPHA_BITS"), INT2FIX(GLFW_ACCUM_ALPHA_BITS));
  rb_const_set(s_glfw_module, rb_intern("AUX_BUFFERS"), INT2FIX(GLFW_AUX_BUFFERS));
  rb_const_set(s_glfw_module, rb_intern("STEREO"), INT2FIX(GLFW_STEREO));
  rb_const_set(s_glfw_module, rb_intern("SAMPLES"), INT2FIX(GLFW_SAMPLES));
  rb_const_set(s_glfw_module, rb_intern("SRGB_CAPABLE"), INT2FIX(GLFW_SRGB_CAPABLE));
  rb_const_set(s_glfw_module, rb_intern("REFRESH_RATE"), INT2FIX(GLFW_REFRESH_RATE));
  rb_const_set(s_glfw_module, rb_intern("CLIENT_API"), INT2FIX(GLFW_CLIENT_API));
  rb_const_set(s_glfw_module, rb_intern("CONTEXT_VERSION_MAJOR"), INT2FIX(GLFW_CONTEXT_VERSION_MAJOR));
  rb_const_set(s_glfw_module, rb_intern("CONTEXT_VERSION_MINOR"), INT2FIX(GLFW_CONTEXT_VERSION_MINOR));
  rb_const_set(s_glfw_module, rb_intern("CONTEXT_REVISION"), INT2FIX(GLFW_CONTEXT_REVISION));
  rb_const_set(s_glfw_module, rb_intern("CONTEXT_ROBUSTNESS"), INT2FIX(GLFW_CONTEXT_ROBUSTNESS));
  rb_const_set(s_glfw_module, rb_intern("OPENGL_FORWARD_COMPAT"), INT2FIX(GLFW_OPENGL_FORWARD_COMPAT));
  rb_const_set(s_glfw_module, rb_intern("OPENGL_DEBUG_CONTEXT"), INT2FIX(GLFW_OPENGL_DEBUG_CONTEXT));
  rb_const_set(s_glfw_module, rb_intern("OPENGL_PROFILE"), INT2FIX(GLFW_OPENGL_PROFILE));
  rb_const_set(s_glfw_module, rb_intern("OPENGL_API"), INT2FIX(GLFW_OPENGL_API));
  rb_const_set(s_glfw_module, rb_intern("OPENGL_ES_API"), INT2FIX(GLFW_OPENGL_ES_API));
  rb_const_set(s_glfw_module, rb_intern("NO_ROBUSTNESS"), INT2FIX(GLFW_NO_ROBUSTNESS));
  rb_const_set(s_glfw_module, rb_intern("NO_RESET_NOTIFICATION"), INT2FIX(GLFW_NO_RESET_NOTIFICATION));
  rb_const_set(s_glfw_module, rb_intern("LOSE_CONTEXT_ON_RESET"), INT2FIX(GLFW_LOSE_CONTEXT_ON_RESET));
  rb_const_set(s_glfw_module, rb_intern("OPENGL_ANY_PROFILE"), INT2FIX(GLFW_OPENGL_ANY_PROFILE));
  rb_const_set(s_glfw_module, rb_intern("OPENGL_CORE_PROFILE"), INT2FIX(GLFW_OPENGL_CORE_PROFILE));
  rb_const_set(s_glfw_module, rb_intern("OPENGL_COMPAT_PROFILE"), INT2FIX(GLFW_OPENGL_COMPAT_PROFILE));
  rb_const_set(s_glfw_module, rb_intern("CURSOR"), INT2FIX(GLFW_CURSOR));
  rb_const_set(s_glfw_module, rb_intern("STICKY_KEYS"), INT2FIX(GLFW_STICKY_KEYS));
  rb_const_set(s_glfw_module, rb_intern("STICKY_MOUSE_BUTTONS"), INT2FIX(GLFW_STICKY_MOUSE_BUTTONS));
  rb_const_set(s_glfw_module, rb_intern("CURSOR_NORMAL"), INT2FIX(GLFW_CURSOR_NORMAL));
  rb_const_set(s_glfw_module, rb_intern("CURSOR_HIDDEN"), INT2FIX(GLFW_CURSOR_HIDDEN));
  rb_const_set(s_glfw_module, rb_intern("CURSOR_DISABLED"), INT2FIX(GLFW_CURSOR_DISABLED));
  rb_const_set(s_glfw_module, rb_intern("CONNECTED"), INT2FIX(GLFW_CONNECTED));
  rb_const_set(s_glfw_module, rb_intern("DISCONNECTED"), INT2FIX(GLFW_DISCONNECTED));

  glfwSetErrorCallback(rb_glfw_error_callback);
}

