/*
 *
 * j-chkmail - Mail Server Filter for sendmail
 *
 * Copyright (c) 2001-2017 - Jose-Marcio Martins da Cruz
 *
 *  Auteur     : Jose Marcio Martins da Cruz
 *               jose.marcio.mc@gmail.org
 *
 *  Historique :
 *  Creation     : janvier 2002
 *
 * This program is free software, but with restricted license :
 *
 * - j-chkmail is distributed only to registered users
 * - j-chkmail license is available only non-commercial applications,
 *   this means, you can use j-chkmail if you make no profit with it.
 * - redistribution of j-chkmail in any way : binary, source in any
 *   media, is forbidden
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * More details about j-chkmail license can be found at j-chkmail
 * web site : http://foss.jose-marcio.org
 */

#include <j-sys.h>

#include "j-chkmail.h"


/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */

#define      SZ_NAME       64

#ifndef DTCLEANUP
#define DTCLEANUP    600
#endif


typedef struct OpenConn_T
{
#if 0
  JSOCKADDR_T         addr;
#endif
  char                ip[SZ_IP];
  char                name[SZ_NAME];
  int                 nb;
  time_t              update;
}
OpenConn_T;


/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */

static struct
{
  bool                ok;
  time_t              last;
  int                 nb;

  pthread_mutex_t     mutex;

  JBT_T               db_open;

}
hdata =
{
FALSE, (time_t) 0, 0, PTHREAD_MUTEX_INITIALIZER, JBT_INITIALIZER};

#define DATA_LOCK() \
  if (pthread_mutex_lock(&hdata.mutex) != 0) { \
    LOG_SYS_ERROR("pthread_mutex_lock"); \
  }

#define DATA_UNLOCK() \
  if (pthread_mutex_unlock(&hdata.mutex) != 0) { \
    LOG_SYS_ERROR("pthread_mutex_unlock"); \
  }



/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
static int
connopen_cmp(a, b)
     void               *a;
     void               *b;
{
  OpenConn_T         *ta = (OpenConn_T *) a;
  OpenConn_T         *tb = (OpenConn_T *) b;

  if ((ta == NULL) || (tb == NULL))
  {
    if (ta == tb)
      return 0;
    if (ta == NULL)
      return -1;
    else
      return 1;
  }

  return (strcmp(ta->ip, tb->ip));
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
static              bool
connopen_init()
{
  if (hdata.ok)
    return TRUE;

  DATA_LOCK();

  if (!hdata.ok)
  {
    if (jbt_init(&hdata.db_open, sizeof (OpenConn_T), connopen_cmp))
    {
      hdata.ok = TRUE;
      hdata.last = time(NULL);
    } else
      LOG_MSG_ERROR("Can't initialize db_open");
  }

  DATA_UNLOCK();

  return hdata.ok;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
void
connopen_reset()
{
  DATA_LOCK();

  jbt_destroy(&hdata.db_open);
  hdata.nb = 0;

  DATA_UNLOCK();
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
int
connopen_check_host(ip, name, nb)
     char               *ip;
     char               *name;
     int                 nb;
{
  OpenConn_T          p;
  OpenConn_T         *ptr = NULL;
  int                 res = 0;
  time_t              now = time(NULL);

#if 0 && defined(OS_FREEBSD)
  return 0;
#endif

  if ((ip == NULL) || (strlen(ip) == 0))
    return res;

  if (!connopen_init())
    return res;

  DATA_LOCK();

  name = STRNULL(name, "NONAME");
  if (strlen(name) == 0)
    name = "NONAME";

  /* update open connections table... */
  memset(&p, 0, sizeof (p));
  strlcpy(p.ip, ip, sizeof (p.ip));

  ptr = jbt_get(&hdata.db_open, &p);

  if (ptr != NULL)
  {
    if (nb != 0)
    {
      ptr->nb += nb;
      ptr->update = now;
      hdata.nb += nb;
    }
    res = ptr->nb;
  } else
  {
    if (nb > 0)
    {
      strlcpy(p.ip, ip, sizeof (p.ip));
      strlcpy(p.name, name, sizeof (p.name));

      p.nb = nb;
      p.update = now;
      if (jbt_add(&hdata.db_open, &p))
      {
        res = p.nb;
        hdata.nb += nb;
      } else
        LOG_MSG_ERROR("Error adding new leaf to db");
    }
  }

  DATA_UNLOCK();

  if ((hdata.last + DTCLEANUP / 2 < now) &&
      ((hdata.last + DTCLEANUP < now) || (jbt_count(&hdata.db_open) > NB_BTCLEANUP)))
    connopen_clean_table();

  return res;
}


/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
static time_t       tcleanup = (time_t) 0;

static              bool
select_function(vp, arg)
     void               *vp;
     void               *arg;
{
  OpenConn_T         *p = (OpenConn_T *) vp;
  time_t              now;

#if 1
  now = tcleanup;
#else
  now = time(NULL);
#endif

  if (p == NULL)
    return FALSE;

  if ((p->nb > 0) || (p->update + DTCLEANUP > now))
    return TRUE;
  return FALSE;
}

bool
connopen_clean_table()
{
  time_t              now = time(NULL);

  if (!connopen_init())
    return FALSE;

  DATA_LOCK();

  tcleanup = now;

  if ((hdata.last + DTCLEANUP / 2 < now) &&
      ((hdata.last + DTCLEANUP < now) || (jbt_count(&hdata.db_open) > NB_BTCLEANUP)))
  {

#if 1
    JBT_T               tmp = JBT_INITIALIZER;

    MESSAGE_INFO(19, "connopen_clean_table : before  : %d nodes",
                 jbt_count(&hdata.db_open));

    if (jbt_init(&tmp, sizeof (OpenConn_T), connopen_cmp))
    {
      if (jbt_cpy(&tmp, &hdata.db_open, select_function, NULL))
      {
        jbt_destroy(&hdata.db_open);
        hdata.db_open = tmp;
      } else
        LOG_MSG_ERROR("Can't copy btrees...");
    } else
#else
    if (!jbt_cleanup(&hdata.db_open, connopen_cmp, NULL))
#endif
      LOG_MSG_ERROR("Can't initialize temporary btree");

    MESSAGE_INFO(19, "connopen_clean_table : after   : %d nodes",
                 jbt_count(&hdata.db_open));

    hdata.last = now;
  }

  DATA_UNLOCK();

  return TRUE;
}

/* ****************************************************************************
 *                                                                            *
 *                                                                            *
 **************************************************************************** */
int                 logfd = -1;

static int
log_rec(void *data, void *param)
{
  OpenConn_T         *p = (OpenConn_T *) data;

  if (p->nb > 0)
  {
    if (logfd < 0)
      MESSAGE_INFO(10, "  %-17s : %3d : %s", p->ip, p->nb, p->name);
    else
      FD_PRINTF(logfd, "  %-17s : %3d : %s\n", p->ip, p->nb, p->name);
    return 1;
  }
  return 0;
}

void
connopen_print_table(fd)
     int                 fd;
{
  int                 nb = 0;

  if (!connopen_init())
    return;

  DATA_LOCK();

  logfd = fd;
  if (logfd < 0)
    MESSAGE_INFO(10, "*** Open connections :");
  else
    FD_PRINTF(logfd, "*** Open connections :\n");

  nb = jbt_browse(&hdata.db_open, log_rec, NULL);

  if (logfd < 0)
    MESSAGE_INFO(10, "    %d entries on database", nb);
  else
    FD_PRINTF(logfd, "    %d entries on database\n", nb);
  fd = -1;

  DATA_UNLOCK();
}
