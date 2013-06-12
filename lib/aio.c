/** \file aio.c
  \class aio class for asynch non-blocking IO, via libuv.

  For POSIX unbuffered fileio \see file.c open,read,write,seek calls on fd
  and lib/buffile.c for buffered IO.

 (c) 2013 perl11.org */
#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include "p2.h"

PN aio_vt, aio_tcp_vt, aio_udp_vt, aio_loop_vt, aio_tty_vt, aio_pipe_vt, aio_poll_vt, aio_prepare_vt,
  aio_idle_vt, aio_async_vt, aio_timer_vt, aio_fs_poll_vt, aio_signal_vt, aio_fs_event_vt, aio_mutex_vt,
  aio_rwlock_vt, aio_sem_vt, aio_cond_vt, aio_barrier_vt, aio_check_vt,
  aio_handle_vt, aio_process_vt, aio_stream_vt, aio_req_vt, aio_connect_vt, aio_write_vt, aio_shutdown_vt,
  aio_udp_send_vt, aio_fs_vt, aio_work_vt, aio_getaddrinfo_vt, aio_cpu_info_vt, aio_interface_address_vt;

#define DEF_AIO_INIT(T) \
  static PN aio_##T##_init(Potion *P, PN cl, PN self) {                 \
    int r;                                                              \
    uv_##T##_t *handle = (uv_##T##_t*)PN_DATA(self);                    \
    r = uv_##T##_init(handle);                                          \
    if (!r) return potion_io_error(P, _XSTR(t) "_init");                \
    return self;                                                        \
  }
#define DEF_AIO_INIT_LOOP(T) \
  static PN aio_##T##_init(Potion *P, PN cl, PN self, PN loop) {        \
    int r;                                                              \
    uv_##T##_t *handle = (uv_##T##_t*)PN_DATA(self);                    \
    if (!loop) loop = (PN)uv_default_loop();                            \
    else loop = PN_DATA(loop);                                          \
    r = uv_##T##_init((uv_loop_t*)loop, handle);                        \
    if (!r) return potion_io_error(P, _XSTR(t) "_init");                \
    return self;                                                        \
  }

/**\memberof aio_tcp
   initialize aio_tcp
   \param loop   PNAioLoop to uv_loop_t*, defaults to uv_default_loop()
   \see http://nikhilm.github.io/uvbook/networking.html#tcp
   \returns self */
DEF_AIO_INIT_LOOP(tcp)
/**\memberof aio_udp
   initialize aio_udp
   \param loop   PNAioLoop to uv_loop_t*, defaults to uv_default_loop()
   \see http://nikhilm.github.io/uvbook/networking.html#udp
   \returns self */
DEF_AIO_INIT_LOOP(udp)
DEF_AIO_INIT_LOOP(prepare)
DEF_AIO_INIT_LOOP(check)
DEF_AIO_INIT_LOOP(idle)
DEF_AIO_INIT_LOOP(timer)
DEF_AIO_INIT_LOOP(fs_poll)
DEF_AIO_INIT_LOOP(signal)
DEF_AIO_INIT(cond)
DEF_AIO_INIT(mutex)
DEF_AIO_INIT(rwlock)

static PN aio_tty_init(Potion *P, PN cl, PN self, PN loop, PN file, PN readable) {
  int r;
  uv_tty_t *handle = (uv_tty_t*)PN_DATA(self);
  if (!loop) loop = (PN)uv_default_loop();
  else loop = PN_DATA(loop);
  r = uv_tty_init((uv_loop_t*)loop, handle, PN_DATA(file), PN_NUM(readable));
  if (!r) return potion_io_error(P, "tty_init");
  return self;
}
static PN aio_pipe_init(Potion *P, PN cl, PN self, PN loop, PN ipc) {
  int r;
  uv_pipe_t *handle = (uv_pipe_t*)PN_DATA(self);
  if (!loop) loop = (PN)uv_default_loop();
  else loop = PN_DATA(loop);
  r = uv_pipe_init((uv_loop_t*)loop, handle, PN_NUM(ipc));
  if (!r) return potion_io_error(P, "pipe_init");
  return self;
}
static PN aio_poll_init(Potion *P, PN cl, PN self, PN loop, PN fd) {
  int r;
  uv_poll_t *handle = (uv_poll_t*)PN_DATA(self);
  if (!loop) loop = (PN)uv_default_loop();
  else loop = PN_DATA(loop);
  r = uv_poll_init((uv_loop_t*)loop, handle, PN_NUM(fd));
  if (!r) return potion_io_error(P, "poll_init");
  return self;
}
static PN aio_barrier_init(Potion *P, PN cl, PN self, PN count) {
  int r;
  uv_barrier_t *handle = (uv_barrier_t*)PN_DATA(self);
  r = uv_barrier_init(handle, PN_NUM(count));
  if (!r) return potion_io_error(P, "barrier_init");
  return self;
}
static PN aio_sem_init(Potion *P, PN cl, PN self, PN value) {
  int r;
  uv_sem_t *handle = (uv_sem_t*)PN_DATA(self);
  r = uv_sem_init(handle, PN_NUM(value));
  if (!r) return potion_io_error(P, "sem_init");
  return self;
}
static PN aio_fs_event_init(Potion *P, PN cl, PN self, PN loop, PN filename, PN cb, PN flags) {
  int r;
  uv_fs_event_t *handle = (uv_fs_event_t*)PN_DATA(self);
  if (!loop) loop = (PN)uv_default_loop();
  else loop = PN_DATA(loop);
  r = uv_fs_event_init((uv_loop_t*)loop, handle, PN_STR_PTR(filename), (uv_fs_event_cb)PN_DATA(cb), PN_NUM(flags));
  if (!r) return potion_io_error(P, "fs_event_init");
  return self;
}
static PN aio_async_init(Potion *P, PN cl, PN self, PN loop, PN cb) {
  int r;
  uv_async_t *handle = (uv_async_t*)PN_DATA(self);
  if (!loop) loop = (PN)uv_default_loop();
  else loop = PN_DATA(loop);
  r = uv_async_init((uv_loop_t*)loop, handle, (uv_async_cb)PN_DATA(cb));
  if (!r) return potion_io_error(P, "async_init");
  return self;
}

#if 0
static PN
aio_tcp_connect(Potion *P, PN cl, PN self) {
  uv_tcp_t client;
  return PN_INT(uv_tcp_init(&connect_req, &client, addr, connect_cb));
}

static void
aio_shutdown_cb(Potion *P, PN cl, pn_aio self, int status) {
  uv_stream_t* stream = ((uv_shutdown_t*)req)->handle;
  potion_send(stream, shutdown_cb);
  free(req);
}
#endif

/**
   allocate a req or handle struct and return the object
 */
static PN
aio_new(Potion *P, PN cl, PN self, PN type) {
#define DEF_AIO_OBJ(T)                                                  \
  if (type == PN_STR(_XSTR(T))) {                                       \
    struct PNData *data = potion_data_alloc(P, sizeof(uv_##T##_t));     \
    data->vt = aio_##T##_vt;                                            \
    return (PN)data;                                                    \
  } else

  DEF_AIO_OBJ(loop)
  DEF_AIO_OBJ(async)
  DEF_AIO_OBJ(check)
  DEF_AIO_OBJ(fs_event)
  DEF_AIO_OBJ(fs_poll)
  DEF_AIO_OBJ(handle)
  DEF_AIO_OBJ(idle)
  DEF_AIO_OBJ(pipe)
  DEF_AIO_OBJ(poll)
  DEF_AIO_OBJ(prepare)
  DEF_AIO_OBJ(process)
  DEF_AIO_OBJ(stream)
  DEF_AIO_OBJ(tcp)
  DEF_AIO_OBJ(timer)
  DEF_AIO_OBJ(tty)
  DEF_AIO_OBJ(udp)
  DEF_AIO_OBJ(signal)
  DEF_AIO_OBJ(req)
  DEF_AIO_OBJ(connect)
  DEF_AIO_OBJ(write)
  DEF_AIO_OBJ(shutdown)
  DEF_AIO_OBJ(udp_send)
  DEF_AIO_OBJ(fs)
  DEF_AIO_OBJ(work)
  DEF_AIO_OBJ(getaddrinfo)
  DEF_AIO_OBJ(cpu_info)
  DEF_AIO_OBJ(interface_address)
  DEF_AIO_OBJ(mutex)
  DEF_AIO_OBJ(rwlock)
  DEF_AIO_OBJ(sem)
  DEF_AIO_OBJ(cond)
  DEF_AIO_OBJ(barrier)
  {}
#undef DEF_AIO_OBJ
}
#define DEF_AIO_NEW(T)                                                  \
  static PN aio_##T##_new(Potion *P, PN cl, PN self) {               \
    struct PNData *data = potion_data_alloc(P, sizeof(uv_##T##_t));     \
    data->vt = aio_##T##_vt;                                            \
    return (PN)data;                                                    \
}
DEF_AIO_NEW(loop)
DEF_AIO_NEW(async)
DEF_AIO_NEW(check)
DEF_AIO_NEW(fs_event)
DEF_AIO_NEW(fs_poll)
DEF_AIO_NEW(handle)
DEF_AIO_NEW(idle)
DEF_AIO_NEW(pipe)
DEF_AIO_NEW(poll)
DEF_AIO_NEW(prepare)
DEF_AIO_NEW(process)
DEF_AIO_NEW(stream)
DEF_AIO_NEW(tcp)
DEF_AIO_NEW(timer)
DEF_AIO_NEW(tty)
DEF_AIO_NEW(udp)
DEF_AIO_NEW(signal)
DEF_AIO_NEW(req)
DEF_AIO_NEW(connect)
DEF_AIO_NEW(write)
DEF_AIO_NEW(shutdown)
DEF_AIO_NEW(udp_send)
DEF_AIO_NEW(fs)
DEF_AIO_NEW(work)
DEF_AIO_NEW(getaddrinfo)
DEF_AIO_NEW(cpu_info)
DEF_AIO_NEW(interface_address)
DEF_AIO_NEW(mutex)
DEF_AIO_NEW(rwlock)
DEF_AIO_NEW(sem)
DEF_AIO_NEW(cond)
DEF_AIO_NEW(barrier)
#undef DEF_AIO_NEW

/**\memberof aio
   \deprecated
   initialize aio types
   \param loop   PNAioLoop to uv_loop_t*, defaults to uv_default_loop()
   \see http://nikhilm.github.io/uvbook/networking.html#tcp
   \returns self */
static PN
aio_init(Potion *P, PN cl, PN self, PN loop) {
  PN type = potion_send(PN_VTABLE(self), PN_name);
  if (type == PN_STR("aio_tcp"))
    return aio_tcp_init(P, cl, self, loop);
  else
  if (type == PN_STR("aio_udp"))
    return aio_udp_init(P, cl, self, loop);
}

void Potion_Init_aio(Potion *P) {
  aio_vt = potion_type_new2(P, PN_TUSER, PN_VTABLE(PN_TUSER), PN_STR("aio"));
  potion_method(aio_vt, "init", aio_init, "loop=o");
  potion_type_constructor_is(aio_vt, PN_FUNC(aio_new, "type=S"));
  //potion_class_method(aio_vt, "new", potion_aio_new, "type=S");
#define DEF_AIO_VT(T)                                                            \
  aio_##T##_vt = potion_type_new2(P, PN_TUSER, aio_vt, PN_STR("aio_" _XSTR(t))); \
  potion_type_constructor_is(aio_##T##_vt, PN_FUNC(aio_##T##_new, 0));           \
  potion_method(aio_vt, "new", aio_##T##_new, 0);
#define DEF_AIO_INIT_VT(T,args) \
  DEF_AIO_VT(T); potion_method(aio_##T##_vt, "init", aio_##T##_init, args);

  DEF_AIO_INIT_VT(tcp, "loop=o");
  DEF_AIO_INIT_VT(udp, "loop=o");
  DEF_AIO_VT(loop);
  DEF_AIO_INIT_VT(tty,  "loop=o,file=o,readable=N");
  DEF_AIO_INIT_VT(pipe, "loop=o,ipc=N");
  DEF_AIO_INIT_VT(poll, "loop=o,fd=N");
  DEF_AIO_INIT_VT(prepare, "loop=o");
  DEF_AIO_INIT_VT(check, "loop=o");
  DEF_AIO_INIT_VT(idle,  "loop=o");
  DEF_AIO_INIT_VT(async, "loop=o,cb=o");
  DEF_AIO_INIT_VT(timer, "loop=o");
  DEF_AIO_INIT_VT(fs_poll, "loop=o");
  DEF_AIO_INIT_VT(signal, "loop=o");
  DEF_AIO_INIT_VT(fs_event, "loop=o,filename=S,cb=o,flags=N");
  DEF_AIO_INIT_VT(mutex, 0);
  DEF_AIO_INIT_VT(rwlock, 0);
  DEF_AIO_INIT_VT(sem, "value=N");
  DEF_AIO_INIT_VT(cond, 0);
  DEF_AIO_INIT_VT(barrier, "count=N");
  DEF_AIO_VT(handle);
  DEF_AIO_VT(process);
  DEF_AIO_VT(stream);
  DEF_AIO_VT(req);
  DEF_AIO_VT(connect);
  DEF_AIO_VT(write);
  DEF_AIO_VT(shutdown);
  DEF_AIO_VT(udp_send);
  DEF_AIO_VT(fs);
  DEF_AIO_VT(work);
  DEF_AIO_VT(getaddrinfo);
  DEF_AIO_VT(cpu_info);
  DEF_AIO_VT(interface_address);

#undef DEF_AIO_VT
#undef DEF_AIO_INIT_VT

#if 0
  potion_method(aio_tcp_vt, "connect", aio_tcp_connect, "req=o,addr=S|connect_cb=&");
  potion_method(aio_tcp_vt, "connect6", aio_tcp_connect6, "req=o,addr=S|connect_cb=&");
  potion_method(aio_udp_vt, "connect", aio_udp_connect, "req=o,addr=S|connect_cb=&");
  potion_method(aio_pipe_vt, "open", aio_pipe_open, "file=o");
  potion_method(aio_pipe_vt, "bind", aio_pipe_bind, "name=S");
  potion_method(aio_pipe_vt, "connect", aio_pipe_connect, "req=o,name=S|connect_cb=&");
  potion_method(aio_pipe_vt, "pending_instances", aio_pipe_pending_instances, "count=N");
#endif
}
