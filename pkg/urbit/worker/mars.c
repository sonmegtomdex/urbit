//! @file mars.c
//!
//! Main loop of a serf process.

#include "all.h"
#include "c/bile.h"
#include "c/list.h"
#include "c/path.h"
#include "vere/vere.h"
#include "vere/saga.h"
#include "vere/lock.h"
#include "vere/mars.h"

//==============================================================================
// Functions
//==============================================================================

c3_c tac_c[256];  //  tracing label

/*
::  peek=[gang (each path $%([%once @tas @tas path] [%beam @tas beam]))]
::  ovum=ovum
::
::    next steps:
::    - |mass should be a query of the serf directly
::    - add duct or vane stack for spinner
::
|$  [peek ovum]
|%
+$  task                                                ::  urth -> mars
  $%  [%live ?(%meld %pack) ~] :: XX rename
      [%exit ~]
      [%peek mil=@ peek]
      [%poke mil=@ ovum]
      [%sync %save ~]
  ==
+$  gift                                                ::  mars -> urth
  $%  [%live ~]
      [%flog cord]
      [%slog pri=@ tank]
      [%peek p=(each (unit (cask)) goof)]
      [%poke p=(each (list ovum) (list goof))]
      [%ripe [pro=%2 kel=wynn] [who=@p fake=?] eve=@ mug=@]
      [%sync eve=@ mug=@]
  ==
--
*/

/* _mars_grab(): garbage collect, checking for profiling.
*/
static void
_mars_grab(u3_noun sac)
{
  if ( u3_nul == sac) {
    //  XX review u3o_check_corrupt
    //
    if ( u3C.wag_w & (u3o_debug_ram | u3o_check_corrupt) ) {
      u3m_grab(u3_none);
    }

    return;
  }

  {
    c3_w tot_w = 0;
    FILE* fil_u;

#ifdef U3_MEMORY_LOG
    {
      u3_noun wen = u3dc("scot", c3__da, u3k(u3A->now));
      c3_c* wen_c = u3r_string(wen);

      c3_c nam_c[2048];
      snprintf(nam_c, 2048, "%s/.urb/put/mass", u3P.dir_c);

      struct stat st;
      if ( -1 == stat(nam_c, &st) ) {
        mkdir(nam_c, 0700);
      }

      c3_c man_c[2054];
      snprintf(man_c, 2053, "%s/%s-serf.txt", nam_c, wen_c);

      fil_u = fopen(man_c, "w");
      fprintf(fil_u, "%s\r\n", wen_c);

      c3_free(wen_c);
      u3z(wen);
    }
#else
    {
      fil_u = stderr;
    }
#endif

    c3_assert( u3R == &(u3H->rod_u) );
    fprintf(fil_u, "\r\n");

    tot_w += u3a_maid(fil_u, "total userspace", u3a_prof(fil_u, 0, sac));
    tot_w += u3m_mark(fil_u);
    tot_w += u3a_maid(fil_u, "space profile", u3a_mark_noun(sac));

    u3a_print_memory(fil_u, "total marked", tot_w);
    u3a_print_memory(fil_u, "free lists", u3a_idle(u3R));
    u3a_print_memory(fil_u, "sweep", u3a_sweep());

    fflush(fil_u);

#ifdef U3_MEMORY_LOG
    {
      fclose(fil_u);
    }
#endif

    u3z(sac);

    u3l_log("\n");
  }
}

/* _mars_fact(): commit a fact and enqueue its effects.
 */
static void
_mars_fact(u3_mars* mar_u, u3_noun job, u3_noun pro)
{
  {
    u3_gift gif_u = {
      .sat_e = u3_gift_fact_e,
      .eve_d = mar_u->dun_d,
    };

    u3s_jam_xeno(pro, &gif_u.len_d, &gif_u.hun_y);
    u3z(pro);

    c3_list_pushb(mar_u->gif_u, &gif_u, sizeof(gif_u));
  }

  {
    u3_atom mat = u3ke_jam(job);
    c3_w  len_w = u3r_met(3, mat);

    c3_l   mug_l = mar_u->mug_l;
    size_t len_i = sizeof(mug_l) + len_w;
    c3_y   dat_y[len_i];
    dat_y[0] = mug_l & 0xff;
    dat_y[1] = (mug_l >> 8) & 0xff;
    dat_y[2] = (mug_l >> 16) & 0xff;
    dat_y[3] = (mug_l >> 24) & 0xff;
    u3r_bytes(0, len_w, dat_y + sizeof(mug_l), mat);
    u3z(mat);

    u3_saga_commit_async(mar_u->log_u,
                         dat_y,
                         len_i);
  }

}

/* _mars_gift(): enqueue response message.
*/
static void
_mars_gift(u3_mars* mar_u, u3_noun pro)
{
  u3_gift gif_u = {
    .sat_e = u3_gift_rest_e,
    .ptr_v = NULL,
  };

  u3s_jam_xeno(pro, &gif_u.len_d, &gif_u.hun_y);
  u3z(pro);

  c3_list_pushb(mar_u->gif_u, &gif_u, sizeof(gif_u));
}

/* _mars_make_crud(): construct error-notification event.
*/
static u3_noun
_mars_make_crud(u3_noun job, u3_noun dud)
{
  u3_noun now, ovo, new;
  u3x_cell(job, &now, &ovo);

  new = u3nt(u3k(now),
             u3nt(u3_blip, c3__arvo, u3_nul),
             u3nt(c3__crud, dud, u3k(ovo)));

  u3z(job);
  return new;
}

/* _mars_sure_feck(): event succeeded, send effects.
*/
static u3_noun
_mars_sure_feck(u3_mars* mar_u, c3_w pre_w, u3_noun vir)
{
  c3_o rec_o = c3n;
  c3_o pac_o = c3n;

  //  intercept |mass, observe |reset
  //
  {
    u3_noun riv = vir;
    c3_w    i_w = 0;

    while ( u3_nul != riv ) {
      u3_noun fec = u3t(u3h(riv));

      //  assumes a max of one %mass effect per event
      //
      if ( c3__mass == u3h(fec) ) {
        //  save a copy of the %mass data
        //
        mar_u->sac = u3k(u3t(fec));
        //  replace the %mass data with ~
        //
        //    For efficient transmission to daemon.
        //
        riv = u3kb_weld(u3qb_scag(i_w, vir),
                        u3nc(u3nt(u3k(u3h(u3h(riv))), c3__mass, u3_nul),
                             u3qb_slag(1 + i_w, vir)));
        u3z(vir);
        vir = riv;
        break;
      }

      //  reclaim memory from persistent caches on |reset
      //
      if ( c3__vega == u3h(fec) ) {
        rec_o = c3y;
      }

      riv = u3t(riv);
      i_w++;
    }
  }

  //  after a successful event, we check for memory pressure.
  //
  //    if we've exceeded either of two thresholds, we reclaim
  //    from our persistent caches, and notify the daemon
  //    (via a "fake" effect) that arvo should trim state
  //    (trusting that the daemon will enqueue an appropriate event).
  //    For future flexibility, the urgency of the notification is represented
  //    by a *decreasing* number: 0 is maximally urgent, 1 less so, &c.
  //
  //    high-priority: 2^22 contiguous words remaining (~8 MB)
  //    low-priority:  2^27 contiguous words remaining (~536 MB)
  //    XX maybe use 2^23 (~16 MB) and 2^26 (~268 MB?
  //
  //    XX these thresholds should trigger notifications sent to the king
  //    instead of directly triggering these remedial actions.
  //
  {
    u3_noun pri = u3_none;
    c3_w pos_w = u3a_open(u3R);
    c3_w low_w = (1 << 27);
    c3_w hig_w = (1 << 22);

    if ( (pre_w > low_w) && !(pos_w > low_w) ) {
      //  XX set flag(s) in u3V so we don't repeat endlessly?
      //
      pac_o = c3y;
      rec_o = c3y;
      pri   = 1;
    }
    else if ( (pre_w > hig_w) && !(pos_w > hig_w) ) {
      pac_o = c3y;
      rec_o = c3y;
      pri   = 0;
    }
    //  reclaim memory from persistent caches periodically
    //
    //    XX this is a hack to work two things
    //    - bytecode caches grow rapidly and can't be simply capped
    //    - we don't make very effective use of our free lists
    //
    else if ( 0 == (mar_u->dun_d % 1000ULL) ) {
      rec_o = c3y;
    }

    //  notify daemon of memory pressure via "fake" effect
    //
    if ( u3_none != pri ) {
      u3_noun cad = u3nc(u3nt(u3_blip, c3__arvo, u3_nul),
                         u3nc(c3__trim, pri));
      vir = u3nc(cad, vir);
    }
  }

  mar_u->rec_o = c3o(mar_u->rec_o, rec_o);
  mar_u->pac_o = c3o(mar_u->pac_o, pac_o);

  return vir;
}

/* _mars_poke(): attempt to compute an event. [*eve] is RETAINED.
*/
static c3_o
_mars_poke(c3_w mil_w, u3_noun* eve, u3_noun* out)
{
  c3_t tac_t = !!( u3C.wag_w & u3o_trace );
  c3_c tag_c[9];
  c3_o ret_o;

  // XX refactor tracing, avoid allocation
  //
  if ( tac_t ) {
    u3_noun wir = u3h(u3t(*eve));
    u3_noun tag = u3h(u3t(u3t(*eve)));
    c3_c* wir_c = u3m_pretty_path(wir);
    c3_w  len_w;

    u3r_bytes(0, 8, (c3_y*)tag_c, tag);
    tag_c[8] = 0;

    //  ellipses for trunctation
    //
    if ( sizeof(tac_c) <
         snprintf(tac_c, sizeof(tac_c), "poke %%%s on %s", tag_c, wir_c) )
    {
      memset(tac_c + (sizeof(tac_c) - 4), '.', 3);
      tac_c[255] = 0;
    }

    u3t_event_trace(tac_c, 'b');
    c3_free(wir_c);
  }

#ifdef U3_EVENT_TIME_DEBUG
  struct timeval b4;
  gettimeofday(&b4, 0);

  {
    u3_noun tag = u3h(u3t(u3t(*eve)));
    u3r_bytes(0, 8, (c3_y*)tag_c, tag);
    tag_c[8] = 0;

    if ( c3__belt != tag ) {
      u3l_log("mars: (%" PRIu64 ") %%%s\r\n", u3A->eve_d + 1, tag_c);
    }
  }
#endif

  {
    u3_noun pro;

    if ( c3y == (ret_o = u3v_poke_sure(mil_w, u3k(*eve), &pro)) ) {
      *out = pro;
    }
    else if ( c3__evil == u3h(pro) ) {
      *out = u3nc(pro, u3_nul);
    }
    else {
      u3_noun dud = pro;

      *eve = _mars_make_crud(*eve, u3k(dud));

      if ( c3y == (ret_o = u3v_poke_sure(mil_w, u3k(*eve), &pro)) ) {
        *out = pro;
        u3z(dud);
      }
      else {
        *out = u3nt(dud, pro, u3_nul);
      }
    }
  }

#ifdef U3_EVENT_TIME_DEBUG
  {
    c3_w      ms_w, clr_w;
    struct timeval f2, d0;
    gettimeofday(&f2, 0);
    timersub(&f2, &b4, &d0);

    ms_w  = (d0.tv_sec * 1000) + (d0.tv_usec / 1000);
    clr_w = ms_w > 1000 ? 1 : ms_w < 100 ? 2 : 3; //  red, green, yellow

    if ( clr_w != 2 ) {
      u3l_log("\x1b[3%dm%%%s (%" PRIu64 ") %4d.%02dms\x1b[0m\n",
              clr_w, tag_c, u3A->eve_d + 1, ms_w,
              (int) (d0.tv_usec % 1000) / 10);
    }
  }
#endif

  if ( tac_t ) {
    u3t_event_trace(tac_c, 'e');
  }

  return ret_o;
}

/* _mars_work(): perform a task.
*/
static c3_o
_mars_work(u3_mars* mar_u, u3_noun jar)
{
  u3_noun tag, dat, pro;

  if ( c3n == u3r_cell(jar, &tag, &dat) ) {
    fprintf(stderr, "mars: fail a\r\n");
    u3z(jar);
    return c3n;
  }

  switch ( tag ) {
    default: {
      fprintf(stderr, "mars: fail b\r\n");
      u3z(jar);
      return c3n;
    }

    case c3__poke: {
      u3_noun tim, job;
      c3_w  mil_w, pre_w;

      if ( (c3n == u3r_cell(dat, &tim, &job)) ||
           (c3n == u3r_safe_word(tim, &mil_w)) )
      {
        fprintf(stderr, "mars: poke fail\r\n");
        u3z(jar);
        return c3n;
      }

      //  XX better timestamps
      //
      {
        u3_noun now;
        struct timeval tim_u;
        gettimeofday(&tim_u, 0);

        now   = u3_time_in_tv(&tim_u);
        job = u3nc(now, u3k(job));
      }
      u3z(jar);

      pre_w = u3a_open(u3R);
      mar_u->sen_d++;

      if ( c3y == _mars_poke(mil_w, &job, &pro) ) {
        mar_u->dun_d = mar_u->sen_d;
        mar_u->mug_l = u3r_mug(u3A->roc);
        mar_u->mut_o = c3y;

        pro = _mars_sure_feck(mar_u, pre_w, pro);

        _mars_fact(mar_u, job, u3nt(c3__poke, c3y, pro));
      }
      else {
        mar_u->sen_d = mar_u->dun_d;
        u3z(job);
        _mars_gift(mar_u, u3nt(c3__poke, c3n, pro));
      }

      c3_assert( mar_u->dun_d == u3A->eve_d );
    } break;

    case c3__peek: {
      u3_noun tim, sam, pro;
      c3_w  mil_w;

      if ( (c3n == u3r_cell(dat, &tim, &sam)) ||
           (c3n == u3r_safe_word(tim, &mil_w)) )
      {
        u3z(jar);
        return c3n;
      }

      u3k(sam); u3z(jar);
      _mars_gift(mar_u, u3nc(c3__peek, u3v_soft_peek(mil_w, sam)));
    } break;

    case c3__sync: {
      u3_noun nul;

      if (  (c3n == u3r_p(dat, c3__save, &nul))
         || (u3_nul != nul) )
      {
        u3z(jar);
        return c3n;
      }

      mar_u->sat_e = u3_mars_save_e;
    } break;

    //  $%  [%live ?(%meld %pack) ~] :: XX rename
    //
    case c3__live: {
      u3_noun com, nul;

      if ( (c3n == u3r_cell(dat, &com, &nul)) ||
           (u3_nul != nul) )
      {
        u3z(jar);
        return c3n;
      }

      switch ( com ) {
        default: {
          u3z(jar);
          return c3n;
        }

        case c3__pack: {
          u3z(jar);
          u3a_print_memory(stderr, "mars: pack: gained", u3m_pack());
        } break;

        case c3__meld: {
          u3z(jar);
          u3u_meld();
        } break;
      }

      _mars_gift(mar_u, u3nc(c3__live, u3_nul));
    } break;

    case c3__exit: {
      u3z(jar);
      mar_u->sat_e = u3_mars_exit_e;
    } break;
  }

  return c3y;
}

/* _mars_post(): update mars state post-task.
*/
void
_mars_post(u3_mars* mar_u)
{
  if ( c3y == mar_u->rec_o ) {
    u3m_reclaim();
    mar_u->rec_o = c3n;
  }

  //  XX this runs on replay too, |mass s/b elsewhere
  //
  if ( c3y == mar_u->mut_o ) {
    _mars_grab(mar_u->sac);
    mar_u->sac   = u3_nul;
    mar_u->mut_o = c3n;
  }

  if ( c3y == mar_u->pac_o ) {
    u3a_print_memory(stderr, "mars: pack: gained", u3m_pack());
    u3l_log("\n");
    mar_u->pac_o = c3n;
  }
}

/* _mars_damp_file(): write sampling-profiler output.
*/
static void
_mars_damp_file(void)
{
  if ( u3C.wag_w & u3o_debug_cpu ) {
    FILE* fil_u;

    {
      u3_noun wen = u3dc("scot", c3__da, u3k(u3A->now));
      c3_c* wen_c = u3r_string(wen);

      c3_c nam_c[2048];
      snprintf(nam_c, 2048, "%s/.urb/put/profile", u3P.dir_c);

      struct stat st;
      if ( -1 == stat(nam_c, &st) ) {
        c3_mkdir(nam_c, 0700);
      }

      c3_c man_c[2054];
      snprintf(man_c, 2053, "%s/%s.txt", nam_c, wen_c);

      fil_u = c3_fopen(man_c, "w");

      c3_free(wen_c);
      u3z(wen);
    }

    u3t_damp(fil_u);

    {
      fclose(fil_u);
    }
  }
}

/* _mars_flush(): send pending gifts.
*/
static void
_mars_flush(u3_mars* mar_u)
{
top:
  {
    u3_gift* gif_u = c3_lode_data(c3_list_peekf(mar_u->gif_u));

    //  XX gather i/o
    //
    while (  gif_u
          && (  (u3_gift_rest_e == gif_u->sat_e)
             || (gif_u->eve_d <= u3_saga_last_commit(mar_u->log_u)) ) )
    {
      u3_newt_send(mar_u->out_u, gif_u->len_d, gif_u->hun_y);

      c3_free(c3_list_popf(mar_u->gif_u));
      gif_u = c3_lode_data(c3_list_peekf(mar_u->gif_u));
    }
  }

  if (  (u3_mars_work_e != mar_u->sat_e)
     && (u3_saga_last_commit(mar_u->log_u) == mar_u->dun_d) )
  {
    if ( u3_mars_save_e == mar_u->sat_e ) {
      if ( u3_saga_needs_rollover(mar_u->log_u)
          && !u3_saga_rollover(mar_u->log_u) )
      {
        fprintf(stderr, "mars: failed to rollover to new epoch\r\n");
      }
      u3e_save();
      mar_u->sav_u.eve_d = mar_u->dun_d;
      _mars_gift(mar_u,
        u3nt(c3__sync, u3i_chub(mar_u->dun_d), mar_u->mug_l));
      mar_u->sat_e = u3_mars_work_e;
      goto top;
    }
    else if ( u3_mars_exit_e == mar_u->sat_e ) {
      u3e_save();
      u3_lock_release(mar_u->dir_u);
      u3_saga_close(mar_u->log_u);
      u3s_cue_xeno_done(mar_u->sil_u);
      u3t_trace_close();
      _mars_damp_file();

      c3_free(mar_u->dir_u);
      for ( size_t idx_i = 0; idx_i < c3_list_len(mar_u->gif_u); idx_i++ ) {
        c3_free(c3_list_popf(mar_u->gif_u));
      }
      c3_free(mar_u->gif_u);
      c3_free(mar_u->log_u);
      c3_free(mar_u);

      // XX exit cb ?
      exit(0);
    }
  }
}

/* _mars_step_trace(): initialize or rotate trace file.
*/
static void
_mars_step_trace(const c3_c* dir_c)
{
  if ( u3C.wag_w & u3o_trace ) {
    if ( u3_Host.tra_u.con_w == 0  && u3_Host.tra_u.fun_w == 0 ) {
      u3t_trace_open(dir_c);
    }
    else if ( u3_Host.tra_u.con_w >= 100000 ) {
      u3t_trace_close();
      u3t_trace_open(dir_c);
    }
  }
}

c3_o
u3_mars_kick(u3_mars* mar_u, c3_d len_d, c3_y* hun_y)
{
  c3_o ret_o = c3n;

  _mars_step_trace(c3_path_str(mar_u->dir_u));

  //  XX optimize for stateless tasks w/ peek-next
  //
  if ( u3_mars_work_e == mar_u->sat_e ) {
    u3_weak jar = u3s_cue_xeno_with(mar_u->sil_u, len_d, hun_y);

    //  parse errors are fatal
    //
    if (  (u3_none == jar)
       || (c3n == _mars_work(mar_u, jar)) )
    {
      fprintf(stderr, "mars: bad\r\n");
      //  XX error cb?
      //
      exit(1);
    }

    _mars_post(mar_u);

    ret_o = c3y;
  }

  _mars_flush(mar_u);

  return ret_o;
}

/* _mars_disk_cb(): mars commit result callback.
*/
static void
_mars_timer_cb(uv_timer_t* tim_u)
{
  u3_mars* mar_u = tim_u->data;

  if ( mar_u->dun_d > mar_u->sav_u.eve_d ) {
    mar_u->sat_e = u3_mars_save_e;
  }

  _mars_flush(mar_u);
}

//! Callback invoked upon completion of successful commit to event log.
static c3_t
_saga_commit_cb(void* ptr_v, c3_d ide_d, c3_t suc_t)
{
  u3_mars* mar_u = ptr_v;

  if ( !suc_t ) {
    //  XX better
    //
    fprintf(stderr, "mars: commit fail\r\n");
    exit(1);
  }

  _mars_flush(mar_u);
  return 1;
}

//! Callback invoked for each event during event log replay. See u3_saga_play
//! for description of parameters.
static c3_t
_saga_replay_cb(void*        ptr_v,
                c3_d         cur_d,
                c3_d         las_d,
                c3_y* const  byt_y,
                const size_t byt_i)
{
  static c3_t fir_t = 1;
  if ( fir_t ) {
    if ( las_d == cur_d ) {
      u3l_log("play: event %" PRIu64 "\r\n", las_d);
    }
    else {
      u3l_log("play: events %" PRIu64 "-%" PRIu64 "\r\n", cur_d, las_d);
    }
    u3l_log("---------------- playback starting ----------------\r\n");
  }

  u3_mars* mar_u = ptr_v;

  mar_u->sen_d = cur_d;

  c3_assert(sizeof(c3_l) < byt_i);
  c3_l mug_l = byt_y[0] ^ (byt_y[1] << 8) ^ (byt_y[2] << 16) ^ (byt_y[3] << 24);
  u3_noun evt = u3ke_cue(u3i_bytes(byt_i - 4, byt_y + 4));

  c3_w    pre_w = u3a_open(u3R);
  u3_noun vir;
  if ( c3n == u3v_poke_sure(0, evt, &vir) ) {
    u3z(vir);
    fprintf(stderr, "play (%" PRIu64 "): failed\r\n", cur_d);
    return 0;
  }
  u3z(_mars_sure_feck(mar_u, pre_w, vir));

  // (4)
  mar_u->mug_l = u3r_mug(u3A->roc);
  if ( mug_l && mar_u->mug_l != mug_l ) {
    fprintf(stderr,
            "play: mug mismatch on event %" PRIu64
            ": expected %08x, actual %08x\r\n",
            mar_u->sen_d,
            mug_l,
            mar_u->mug_l);
  }

  mar_u->dun_d = mar_u->sen_d;

  // XX refactor |mass
  // XX get batch size from args
  static const c3_d bat_d = 500;
  if ( 0 == cur_d % bat_d || las_d == cur_d ) {
    u3z(mar_u->sac);
    mar_u->sac = u3_nul;
    _mars_post(mar_u);
    u3l_log("play (%" PRIu64 "): done\r\n", mar_u->dun_d);
  }

  if ( las_d == cur_d ) {
    u3l_log("---------------- playback complete ----------------\r\n");
    fir_t = 1;
  }
  else {
    fir_t = 0;
  }
  return 1;
}

//! Callback invoked for each boot event during boot sequence replay. Create
//! single event out of boot events over repeated calls to _saga_boot_cb() and
//! attempt to boot once the single event has been assembled. `ptr_v` is used to
//! store a noun pointer that should initially be set to `u3_nul`. See
//! u3_saga_play for description of parameters.
static c3_t
_saga_boot_cb(void*        ptr_v,
              c3_d         cur_d,
              c3_d         las_d,
              c3_y* const  byt_y,
              const size_t len_i)
{
  u3_noun* evt = ptr_v;
  if ( u3_nul == *evt ) {
    u3l_log("--------------- bootstrap starting ----------------\r\n");
  }
  *evt = u3nc(u3ke_cue(u3i_bytes(len_i - 4, byt_y + 4)), *evt);

  if ( cur_d < las_d ) {
    return 1;
  }

  *evt       = u3kb_flop(*evt);
  c3_o suc_o = u3v_boot(*evt);
  *evt       = u3_nul;
  if ( c3n == suc_o ) {
    fprintf(stderr, "boot: failed to evaluate boot event\r\n");
    return 0;
  }
  u3l_log("--------------- bootstrap complete ----------------\r\n");

  u3e_save();

  return 1;
}

/* _mars_sign_init(): initialize daemon signal handlers
*/
static void
_mars_sign_init(u3_mars* mar_u)
{
  //  handle SIGINFO (if available)
  //
#ifdef SIGINFO
  {
    u3_usig* sig_u;

    sig_u = c3_malloc(sizeof(u3_usig));
    uv_signal_init(u3L, &sig_u->sil_u);

    sig_u->sil_u.data = mar_u;

    sig_u->num_i = SIGINFO;
    sig_u->nex_u = u3_Host.sig_u;
    u3_Host.sig_u = sig_u;
  }
#endif

  //  handle SIGUSR1 (fallback for SIGINFO)
  //
  {
    u3_usig* sig_u;

    sig_u = c3_malloc(sizeof(u3_usig));
    uv_signal_init(u3L, &sig_u->sil_u);

    sig_u->sil_u.data = mar_u;

    sig_u->num_i = SIGUSR1;
    sig_u->nex_u = u3_Host.sig_u;
    u3_Host.sig_u = sig_u;
  }
}

/* _mars_sign_cb: signal callback.
*/
static void
_mars_sign_cb(uv_signal_t* sil_u, c3_i num_i)
{
  u3_mars* mar_u = sil_u->data;

  switch ( num_i ) {
    default: {
      u3l_log("\r\nmars: mysterious signal %d\r\n", num_i);
    } break;

    //  fallthru if defined
    //
#ifdef SIGINFO
    case SIGINFO:
#endif
    case SIGUSR1: {
      //  XX add u3_mars_slog()
      //
      u3_saga_info(mar_u->log_u);
    } break;
  }
}

/* _mars_sign_move(): enable daemon signal handlers
*/
static void
_mars_sign_move(void)
{
  u3_usig* sig_u;

  for ( sig_u = u3_Host.sig_u; sig_u; sig_u = sig_u->nex_u ) {
    uv_signal_start(&sig_u->sil_u, _mars_sign_cb, sig_u->num_i);
  }
}

/* _mars_sign_hold(): disable daemon signal handlers
*/
static void
_mars_sign_hold(void)
{
  u3_usig* sig_u;

  for ( sig_u = u3_Host.sig_u; sig_u; sig_u = sig_u->nex_u ) {
    uv_signal_stop(&sig_u->sil_u);
  }
}

/* _mars_sign_close(): dispose daemon signal handlers
*/
static void
_mars_sign_close(void)
{
  u3_usig* sig_u;

  for ( sig_u = u3_Host.sig_u; sig_u; sig_u = sig_u->nex_u ) {
    uv_close((uv_handle_t*)&sig_u->sil_u, (uv_close_cb)free);
  }
}

/* u3_mars_init(): init mars, replay if necessary.
*/
u3_mars*
u3_mars_init(c3_c* dir_c, u3_moat* inn_u, u3_mojo* out_u, c3_d eve_d)
{
  u3_mars* mar_u;
  {
    mar_u  = c3_calloc(sizeof(*mar_u));
    *mar_u = (u3_mars){
      .dir_u = c3_path_fv(1, dir_c),
      .sen_d = u3A->eve_d,
      .dun_d = u3A->eve_d,
      .pac_o = c3n,
      .rec_o = c3n,
      .mut_o = c3n,
      .sac   = u3_nul,
      .inn_u = inn_u,
      .out_u = out_u,
      .sat_e = u3_mars_work_e,
      .gif_u = c3_list_init(),
      .xit_f = NULL,
    };
  }

  //  start signal handlers
  //
  _mars_sign_init(mar_u);
  _mars_sign_move();

  //  wire up signal controls
  //
  u3C.sign_hold_f = _mars_sign_hold;
  u3C.sign_move_f = _mars_sign_move;

  u3_lock_acquire(mar_u->dir_u);

  c3_path* const dir_u = c3_path_fv(3, c3_path_str(mar_u->dir_u), ".urb", "log");
  { // XX load/set secrets.
    if ( eve_d && eve_d <= mar_u->dun_d ) {
      u3l_log("mars: replay to %" PRIu64 " already done (at %" PRIu64 ")\r\n",
              eve_d,
              mar_u->dun_d);
      goto free_mars;
    }

    try_saga(mar_u->log_u = u3_saga_open(dir_u, c3n, &mar_u->met_u),
             goto free_mars,
             "failed to open event log at %s",
             c3_path_str(dir_u));

    // Play boot events if necessary.
    if ( 0 == mar_u->dun_d && u3_saga_needs_bootstrap(mar_u->log_u) ) {
      u3_noun evt = u3_nul;
      try_saga(u3_saga_replay(mar_u->log_u,
                              mar_u->dun_d,
                              mar_u->met_u.lif_w,
                              _saga_boot_cb,
                              &evt),
               goto free_event_log,
               "failed to evaluate boot sequence");
      mar_u->sen_d = mar_u->dun_d = mar_u->met_u.lif_w;
    }

    try_saga(u3_saga_replay(mar_u->log_u,
                            mar_u->dun_d,
                            eve_d,
                            _saga_replay_cb,
                            mar_u),
             goto free_event_log,
             "failed to replay event log");

    if ( u3_saga_last_commit(mar_u->log_u) > mar_u->dun_d ) {
      goto free_event_log;
    }

    u3_saga_set_async_ctx(mar_u->log_u, u3L, _saga_commit_cb, mar_u);
  }

  // These must be set after the replay in order to adhere to the invariant
  // that all noun references be roots when calling the u3_saga interface.
  mar_u->mug_l = u3r_mug(u3A->roc);
  mar_u->sil_u = u3s_cue_xeno_init();

  { // Send ready status message. XX version negotation.
    c3_d    len_d;
    c3_y*   hun_y;
    u3_noun wyn = u3_nul;
    u3_noun msg
      = u3nq(c3__ripe,
             u3nc(2, wyn),
             u3nc(u3i_chubs(2, mar_u->met_u.who_d), mar_u->met_u.fak_o),
             u3nc(u3i_chub(mar_u->dun_d), mar_u->mug_l));

    u3s_jam_xeno(msg, &len_d, &hun_y);
    u3_newt_send(mar_u->out_u, len_d, hun_y);
    u3z(msg);
  }

  // XX check return, make interval configurable.
  uv_timer_init(u3L, &(mar_u->sav_u.tim_u));
  uv_timer_start(&(mar_u->sav_u.tim_u), _mars_timer_cb, 120000, 120000);

  mar_u->sav_u.eve_d      = mar_u->dun_d;
  mar_u->sav_u.tim_u.data = mar_u;

  c3_path_free(dir_u);
  goto succeed;

free_event_log:
  u3_saga_close(mar_u->log_u);
  c3_free(mar_u->log_u);
free_mars:
  c3_path_free(dir_u);
  c3_path_free(mar_u->dir_u);
  c3_free(mar_u->gif_u);
  c3_free(mar_u);
fail:
  return NULL;

succeed:
  return mar_u;
}

/* _mars_wyrd_card(): construct %wyrd.
*/
static u3_noun
_mars_wyrd_card(c3_m nam_m, c3_w ver_w, c3_l sev_l)
{
  //  XX ghetto (scot %ta)
  //
  u3_noun ver = u3nt(c3__vere, u3i_string("~." URBIT_VERSION), u3_nul);
  // u3_noun sen = u3dc("scot", c3__uv, sev_l); //  lol no
  u3_noun sen = u3i_string("0v1s.vu178");
  u3_noun kel;

  //  special case versions requiring the full stack
  //
  if (  ((c3__zuse == nam_m) && (418 == ver_w))
     || ((c3__lull == nam_m) && (329 == ver_w))
     || ((c3__arvo == nam_m) && (240 == ver_w)) )
  {
    kel = u3nl(u3nc(c3__zuse, 418),
               u3nc(c3__lull, 329),
               u3nc(c3__arvo, 240),
               u3nc(c3__hoon, 140),
               u3nc(c3__nock, 4),
               u3_none);
  }
  //  XX speculative!
  //
  else {
    kel = u3nc(nam_m, u3i_word(ver_w));
  }

  return u3nt(c3__wyrd, u3nc(sen, ver), kel);
}

/* _mars_sift_pill(): extract boot formulas and module/userspace ova from pill
*/
static c3_o
_mars_sift_pill(u3_noun  pil,
                u3_noun* bot,
                u3_noun* mod,
                u3_noun* use)
{
  u3_noun pil_p, pil_q;

  if ( c3n == u3r_cell(pil, &pil_p, &pil_q) ) {
    return c3n;
  }

  {
    //  XX use faster cue
    //
    u3_noun pro = u3m_soft(0, u3ke_cue, u3k(pil_p));
    u3_noun mot, tag, dat;

    if (  (c3n == u3r_trel(pro, &mot, &tag, &dat))
       || (u3_blip != mot) )
    {
      u3m_p("mot", u3h(pro));
      fprintf(stderr, "boot: failed: unable to parse pill\r\n");
      return c3n;
    }

    if ( c3y == u3r_sing_c("ivory", tag) ) {
      fprintf(stderr, "boot: failed: unable to boot from ivory pill\r\n");
      return c3n;
    }
    else if ( c3__pill != tag ) {
      if ( c3y == u3a_is_atom(tag) ) {
        u3m_p("pill", tag);
      }
      fprintf(stderr, "boot: failed: unrecognized pill\r\n");
      return c3n;
    }

    {
      u3_noun typ;
      c3_c* typ_c;

      if ( c3n == u3r_qual(dat, &typ, bot, mod, use) ) {
        fprintf(stderr, "boot: failed: unable to extract pill\r\n");
        return c3n;
      }

      if ( c3y == u3a_is_atom(typ) ) {
        c3_c* typ_c = u3r_string(typ);
        fprintf(stderr, "boot: parsing %%%s pill\r\n", typ_c);
        c3_free(typ_c);
      }
    }

    u3k(*bot); u3k(*mod); u3k(*use);
    u3z(pro);
  }

  //  optionally replace filesystem in userspace
  //
  if ( u3_nul != pil_q ) {
    c3_w  len_w = 0;
    u3_noun ova = *use;
    u3_noun new = u3_nul;
    u3_noun ovo, tag;

    while ( u3_nul != ova ) {
      ovo = u3h(ova);
      tag = u3h(u3t(ovo));

      if (  (c3__into == tag)
         || (  (c3__park == tag)
            && (c3__base == u3h(u3t(u3t(ovo)))) ) )
      {
        c3_assert( 0 == len_w );
        len_w++;
        ovo = u3t(pil_q);
      }

      new = u3nc(u3k(ovo), new);
      ova = u3t(ova);
    }

    c3_assert( 1 == len_w );

    u3z(*use);
    *use = u3kb_flop(new);
  }

  u3z(pil);

  return c3y;
}

/* _mars_boot_make(): construct boot sequence
*/
static c3_o
_mars_boot_make(u3_boot_opts* inp_u,
                u3_noun         com,
                u3_noun*        ova,
                u3_meta*      met_u)
{
  u3_noun pil, ven, mor, who;

  //  parse boot command
  //
  if ( c3n == u3r_trel(com, &pil, &ven, &mor) ) {
    fprintf(stderr, "boot: invalid command\r\n");
    return c3n;
  }

  //  parse boot event
  //
  {
    u3_noun tag, dat;

    if ( c3n == u3r_cell(ven, &tag, &dat) ) {
      return c3n;
    }

    switch ( tag ) {
      default: {
        fprintf(stderr, "boot: unknown boot event\r\n");
        u3m_p("tag", tag);
        return c3n;
      }

      case c3__fake: {
        met_u->fak_o = c3y;
        who          = dat;
      } break;

      case c3__dawn: {
        met_u->fak_o = c3n;

        if ( c3n == u3r_cell(dat, &who, 0) ) {
          return c3n;
        }
      } break;
    }
  }

  //  validate and extract identity
  //
  if (  (c3n == u3a_is_atom(who))
     || (1 < u3r_met(7, who)) )
  {
    fprintf(stderr, "boot: invalid identity\r\n");
    u3m_p("who", who);
    return c3n;
  }

  u3r_chubs(0, 2, met_u->who_d, who);
  met_u->ver_w = elo_ver_w;

  {
    u3_noun bot, mod, use;

    //  parse pill
    //
    if ( c3n == _mars_sift_pill(u3k(pil), &bot, &mod, &use) ) {
      return c3n;
    }

    met_u->lif_w = u3qb_lent(bot);

    //  break symmetry in the module sequence
    //
    //    version negotation, verbose, identity, entropy
    //
    {
      u3_noun cad, wir = u3nt(u3_blip, c3__arvo, u3_nul);

      cad = u3nc(c3__wack, u3i_words(16, inp_u->eny_w));
      mod = u3nc(u3nc(u3k(wir), cad), mod);

      cad = u3nc(c3__whom, u3k(who));
      mod = u3nc(u3nc(u3k(wir), cad), mod);

      cad = u3nt(c3__verb, u3_nul, inp_u->veb_o);
      mod = u3nc(u3nc(u3k(wir), cad), mod);

      cad = _mars_wyrd_card(inp_u->ver_u.nam_m,
                            inp_u->ver_u.ver_w,
                            inp_u->sev_l);
      mod = u3nc(u3nc(wir, cad), mod);  //  transfer [wir]
    }

    //  prepend legacy boot event to the userspace sequence
    //
    //    XX do something about this wire
    //
    {
      u3_noun wir = u3nq(c3__d, c3__term, '1', u3_nul);
      u3_noun cad = u3nt(c3__boot, inp_u->lit_o, u3k(ven));
      use = u3nc(u3nc(wir, cad), use);
    }

    //  add props before/after the userspace sequence
    //
    {
      u3_noun pre = u3_nul;
      u3_noun aft = u3_nul;

      while ( u3_nul != mor ) {
        u3_noun mot = u3h(mor);

        switch ( u3h(mot) ) {
          case c3__prop: {
            u3_noun ter, met, ves;

            if ( c3n == u3r_trel(u3t(mot), &met, &ter, &ves) ) {
              //  XX fatal error?
              //
              u3m_p("invalid prop", u3t(mot));
              break;
            }

            if ( c3__fore == ter ) {
              u3m_p("prop: fore", met);
              pre = u3kb_weld(pre, u3k(ves));
            }
            else if ( c3__hind == ter ) {
              u3m_p("prop: hind", met);
              aft = u3kb_weld(aft, u3k(ves));
            }
            else {
              //  XX fatal error?
              //
              u3m_p("unrecognized prop tier", ter);
            }
          } break;

          //  XX fatal error?
          //
          default: u3m_p("unrecognized boot sequence enhancement", u3h(mot));
        }

        mor = u3t(mor);
      }

      use = u3kb_weld(pre, u3kb_weld(use, aft));
    }

    //  timestamp events, cons list
    //
    {
      u3_noun now = u3_time_in_tv(&inp_u->tim_u);
      u3_noun bit = u3qc_bex(48);       //  1/2^16 seconds
      u3_noun eve = u3kb_flop(bot);

      {
        u3_noun  lit = u3kb_weld(mod, use);
        u3_noun i, t = lit;

        while ( u3_nul != t ) {
          u3x_cell(t, &i, &t);
          now = u3ka_add(now, u3k(bit));
          eve = u3nc(u3nc(u3k(now), u3k(i)), eve);
        }

        u3z(lit);
      }

      *ova = u3kb_flop(eve);
      u3z(now); u3z(bit);
    }
  }

  u3z(com);

  return c3y;
}

//! ```
//!  $=  com
//!  $:  pill=[p=@ q=(unit ovum)]
//!      $=  vent
//!      $%  [%fake p=ship]
//!          [%dawn p=seed]
//!      ==
//!      more=(list ovum)
//!  ==
//!  ```
c3_o
u3_mars_boot(const c3_c* dir_c, u3_noun com)
{
  c3_o suc_o = c3n;

  u3_boot_opts inp_u;
  {
    inp_u.veb_o       = __( u3C.wag_w & u3o_verbose );
    inp_u.lit_o       = c3n; // unimplemented in arvo
    // XX source kelvin from args?
    inp_u.ver_u.nam_m = c3__zuse;
    inp_u.ver_u.ver_w = 418;

    gettimeofday(&inp_u.tim_u, 0);
    c3_rand(inp_u.eny_w);
    u3_noun now = u3_time_in_tv(&inp_u.tim_u);
    inp_u.sev_l = u3r_mug(now);
    u3z(now);
  }

  u3_meta met_u;
  u3_noun ova;
  if ( c3n == _mars_boot_make(&inp_u, com, &ova, &met_u) ) {
    fprintf(stderr, "boot: preparation failed\r\n");
    goto end;
  }

  // Create <pier>.
  c3_path* pax_u = c3_path_fv(1, dir_c);
  mkdir(c3_path_str(pax_u), 0700);

  // Create <pier>/.urb.
  c3_path_push(pax_u, ".urb");
  mkdir(c3_path_str(pax_u), 0700);

  // Create <pier>/.urb/put.
  c3_path_push(pax_u, "put");
  mkdir(c3_path_str(pax_u), 0700);
  c3_path_pop(pax_u);

  // Create <pier>/.urb/get
  c3_path_push(pax_u, "get");
  mkdir(c3_path_str(pax_u), 0700);
  c3_path_pop(pax_u);

  // Create <pier>.urb/log
  c3_path_push(pax_u, "log");
  u3_saga* log_u = u3_saga_new(pax_u, &met_u);

  c3_path_pop(pax_u);
  c3_path_pop(pax_u);
  u3_lock_acquire(pax_u);

  { // Commit boot events to the event log.
    u3_noun i, t = ova;
    while ( u3_nul != t ) {
      u3x_cell(t, &i, &t);

      // Serialize boot event noun into buffer.
      u3_atom mat   = u3qe_jam(i);
      c3_w    len_w = u3r_met(3, mat);
      c3_y    dat_y[4 + len_w];
      // First four bytes are for mug, and the mug for boot events is 0.
      memset(dat_y, 0, sizeof(c3_w));
      u3r_bytes(0, len_w, dat_y + 4, mat);
      u3z(mat);

      if ( !u3_saga_commit_sync(log_u, dat_y, 4 + len_w) ) {
        fprintf(stderr, "boot: failed to commit boot event to event log\r\n");
        goto free_event_log;
      }
    }
    u3z(ova);
  }

  _mars_step_trace(dir_c);

  u3_noun evt = u3_nul;
  // Create single event out of boot events and attempt to boot.
  if ( !u3_saga_replay(log_u, 0, met_u.lif_w, _saga_boot_cb, &evt) ) {
    goto free_event_log;
  }
  suc_o = c3y;

free_event_log:
  u3_saga_close(log_u);
  c3_free(log_u);
  u3_lock_release(pax_u);
  c3_path_free(pax_u);

end:
  return suc_o;
}

void
u3_mars_grab(void)
{
  u3_noun sac = u3_nul;

  c3_assert( u3R == &(u3H->rod_u) );

  {
    u3_noun sam, gon;

    {
      u3_noun pax = u3nc(c3__whey, u3_nul);
      u3_noun lyc = u3nc(u3_nul, u3_nul);
      sam = u3nt(lyc, c3n, u3nq(c3__once, u3_blip, u3_blip, pax));
    }

    gon = u3m_soft(0, u3v_peek, sam);

    {
      u3_noun tag, dat, val;
      u3x_cell(gon, &tag, &dat);

      if (  (u3_blip == tag)
         && (u3_nul  != dat)
         && (c3y == u3r_pq(u3t(dat), c3__omen, 0, &val))
         && (c3y == u3r_p(val, c3__mass, &sac)) )
      {
        u3k(sac);
      }
    }

    u3z(gon);
  }

  fprintf(stderr, "mars: measuring memory:\r\n");

  if ( u3_nul != sac ) {
    _mars_grab(sac);
  }
  else {
    u3a_print_memory(stderr, "total marked", u3m_mark(stderr));
    u3a_print_memory(stderr, "free lists", u3a_idle(u3R));
    u3a_print_memory(stderr, "sweep", u3a_sweep());
    fprintf(stderr, "\r\n");
  }

  fflush(stderr);
}