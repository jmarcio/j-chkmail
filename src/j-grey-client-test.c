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
#include <j-chkmail.h>


/******************************************************************************
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
#define             DEF_TUPLE         "NET,HOST,FULL"
#define             DEF_TIMES         "0,1000,0,1000"

struct conf_T
{
  bool                random;
  double              randrange;
  unsigned long       upperlimit;
  int                 count;

  char               *domain_src;
  char               *domain_dst;

  char               *ip;
  char               *hostname;
  char               *from;
  char               *to;
};

typedef struct conf_T conf_T;

#define CONF_INITIALIZER   {					\
    FALSE, 2., 0, 10000,					\
      "nowhere.foss.jose-marcio.org", "foss.jose-marcio.org",			\
      "1.1.1.1", "nowhere.foss.jose-marcio.org",			\
      "nobody@foss.jose-marcio.org", "grey@foss.jose-marcio.org"		\
}

static int          do_test(conf_T * cargs);

static void         usage(char *);

int
main(argc, argv)
     int                 argc;
     char              **argv;
{
  int                 i;

  conf_T              cargs = CONF_INITIALIZER;

  set_log_output(TRUE, TRUE);

  log_level = 7;

  /*
   ** 1. Read configuration parameters
   */
  {
    const char         *args = "hvl:c:rR:U:";
    int                 c;

    while ((c = getopt(argc, argv, args)) != -1)
    {
      switch (c)
      {
          /* help */
        case 'h':
          usage(argv[0]);
          exit(0);
          break;

          /* verbose */
        case 'v':
          log_level++;
          break;

          /* log level */
        case 'l':
          log_level = atoi(optarg);
          break;

        case 'c':
          cargs.count = atoi(optarg);
          break;

        case 'r':
          cargs.random = !cargs.random;
          break;
        case 'R':
          cargs.randrange = atof(optarg);
          break;
        case 'U':
          cargs.upperlimit = atoi(optarg);
          break;

        default:
          usage(argv[0]);
          printf("Error ... \n");
          exit(0);
      }
    }
  }

  configure("j-grey-client-test", conf_file, FALSE);

  do_test(&cargs);

  return 0;
}



/******************************************************************************
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
static int
do_test(conf_T * cargs)
{
  char               *ip = "", *from = "", *to = "", *hostname = "";
  int                 n = 0;
  char                buf[256];
  long                range;
  time_t              ti, tf;
  char                ipbuf[256];

  ip = "1.1.1.1";
  from = "nobody@foss.jose-marcio.org";
  hostname = "nowhere.foss.jose-marcio.org";
  to = "grey@foss.jose-marcio.org";

  if (cargs->upperlimit < 10)
  {
    range = cargs->randrange * cargs->count;
    range = MAX(range, 10);
  } else
    range = cargs->upperlimit;

  atexit(remote_grey_quit);

  memset(ipbuf, 0, sizeof(ipbuf));
  ti = time_ms();
  srandom(ti);
  for (n = 0; n < cargs->count; n++)
  {
    int                 r = GREY_OK;
    char               *s = NULL;
    bool                new = FALSE;
    bool                can_validate = TRUE;
    int                 rind;

    if (cargs->random) {
      rind = random() % range;

      snprintf(ipbuf, sizeof(ipbuf), "1.1.%d.%d",
	       rind % 255,
	       random() % 255);
      ip = ipbuf;
    }
    else
      rind = n;

    snprintf(buf, sizeof (buf), "grey-%d@foss.jose-marcio.org", rind);
    to = buf;

    r = remote_grey_check(ip, from, to, hostname);
    switch (r)
    {
      case GREY_OK:
        s = "OK";
        break;
      case GREY_WAIT:
        s = "WAIT";
        break;
      case GREY_ERROR:
        s = "ERROR";
        break;
      case GREY_REJECT:
        s = "REJECT";
        break;
    }
    if (r == GREY_ERROR)
    {
      MESSAGE_INFO(1,"* %7d : Error : %s %s %s %s", n, ip, hostname, from, to);
      sleep(1);
    }
  }
  tf = time_ms();

  MESSAGE_INFO(1, "Entries checked : %5d\n", n);
  MESSAGE_INFO(1, "Time elapsed    : %5d ms", tf - ti);

  return 0;
}



/******************************************************************************
 *                                                                            *
 *                                                                            *
 ******************************************************************************/
void
usage(arg)
     char               *arg;
{
  printf("Usage : %s options\n"
         "  %s\n" "  Compiled on %s\n", arg, PACKAGE, __DATE__ " " __TIME__);
}
