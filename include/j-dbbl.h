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

#ifndef __JDBBL_H_

#define  BL_ORACLE              1
#define  BL_PATMATCH            2
#define  BL_BADRCPT             3
#define  BL_THROTTLE            4


bool                db_open_blacklist();
bool                db_check_blacklist(char *);
bool                db_close_blacklist();


/* dynamic or static blacklist */

typedef struct db_map_T
{
  char                why[32];
  char                key[32];

  int                 weight;
  time_t              date;
  char                ipres[24];
  char                msg[64];
} db_map_T;

bool                 db_map_check(char *, char *, char *, char *, size_t);
bool                 db_map_add(char *, char *, char *, char *);

bool                 db_map_open(char *bl);
bool                 db_map_close(char *bl);
bool                 db_map_close_all(void);

#define __JDBBL_H_
#endif
