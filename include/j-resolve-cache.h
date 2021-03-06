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
 *  Creation     : Tue Dec 20 22:45:42 CET 2005
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


#ifndef J_RESOLVE_CACHE_H

#define        RESOLVE_CACHE_RW           0
#define        RESOLVE_CACHE_WR           0
#define        RESOLVE_CACHE_RD           1

bool           resolve_cache_init(char *dbdir, int rwmode);

bool           resolve_cache_check(char *prefix, char *key, char *value, size_t size);

bool           resolve_cache_add(char *prefix, char *key, char *value);

bool           resolve_cache_times(time_t dt_sync, time_t dt_check, time_t dt_expire);

void           resolve_cache_log_enable(bool enable);

# define J_RESOLVE_CACHE_H    1
#endif             /* J_RESOLVE_CACHE_H */
