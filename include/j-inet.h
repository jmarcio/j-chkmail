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


#ifndef __JINET_H__


char           *jinet_ntop (int, void *, char *, size_t);
int             jinet_pton (int, char *, void *);

bool            jsock_ntop(struct sockaddr *, socklen_t , char *, size_t);

int             ip_strcmp (char *, char *);

bool            get_hostname(char *, size_t);

bool            get_hostbyaddr (char *ip, char *name, int len);
bool            get_hostbyname (char *name, char *ip, int len);

char           *inet_n2p (int, void *, char *, size_t);
int             inet_p2n (int, char *, void *);

bool            get_hostbysock(struct sockaddr *sock, socklen_t slen, 
			       char *addr, size_t alen,
			       char *name, size_t nlen);

int             Ip_strcmp (char *, char *);




#define   J_SOCK_READ        1
#define   J_SOCK_WRITE       0

#define   J_SOCK_ERROR      -1
#define   J_SOCK_READY       0
#define   J_SOCK_TIMEOUT     1

int             jfd_ready (int, bool, long);

bool            sd_printf(int sd, char *format, ...);
int             sd_readln(int fd, char *buf, size_t size);

#define __JINET_H__
#endif
