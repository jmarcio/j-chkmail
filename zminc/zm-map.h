/*
 *
 * j-chkmail - Mail Server Filter for sendmail
 *
 * Copyright (c) 2001-2017 - Jose-Marcio Martins da Cruz
 *
 *  Auteur       : Jose Marcio Martins da Cruz
 *                 jose.marcio.mc@gmail.org
 *
 *  Historique   :
 *  Creation     : Mon Dec 19 13:53:17 CET 2005
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


#ifndef J_MAP_H

typedef struct MAP_T
{
  int            signature;
  pthread_mutex_t mutex;
  char          *name;
  int            rdonly;
  size_t         cache_size;
  ZMDB_T          db;
  ZMDBENV_T     *env;
} MAP_T;

#define MAP_INITIALIZER  {SIGNATURE, PTHREAD_MUTEX_INITIALIZER, NULL, 0, 0, \
      ZMDB_INITIALIZER, NULL}


bool           map_init(MAP_T * map);
bool           map_ok(MAP_T * map);

bool           map_open(MAP_T * map, ZMDBENV_T *env, char *name, int rdonly, size_t cache_size);
bool           map_close(MAP_T * map);
bool           map_reopen(MAP_T * map);

bool           map_lookup(MAP_T * map, char *key, char *value, size_t size);
bool           map_add(MAP_T * map, char *key, char *value, size_t size);
bool           map_delete(MAP_T * map, char *key);

#define        MAP_BROWSE_CONTINUE    0
#define        MAP_BROWSE_STOP        1
#define        MAP_BROWSE_DELETE      2

typedef int    (*MAP_BROWSE_F) (char *, char *, void *);

bool           map_browse(MAP_T * map, MAP_BROWSE_F func, void *arg, char *key, size_t ksz, time_t tmax);

bool           map_lock(MAP_T * map);
bool           map_unlock(MAP_T * map);

bool           map_flush(MAP_T * map);

# define J_MAP_H    1
#endif             /* J_MAP_H */
