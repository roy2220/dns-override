#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>

#include <subhook.h>


static subhook_t getaddrinfo_hook;
static subhook_t gethostbyname_hook;
static subhook_t fopen_hook;

static void init(void) __attribute__((constructor));
static void fin(void) __attribute__((destructor));
static int fake_getaddrinfo(const char *, const char *, const struct addrinfo *
                            , struct addrinfo **);
static struct hostent *fake_gethostbyname(const char *);
static FILE *fake_fopen(const char *, const char *);


static void
init(void)
{
    getaddrinfo_hook = subhook_new(getaddrinfo, fake_getaddrinfo, SUBHOOK_OPTION_64BIT_OFFSET);
    gethostbyname_hook = subhook_new(gethostbyname, fake_gethostbyname
                                     , SUBHOOK_OPTION_64BIT_OFFSET);
    fopen_hook = subhook_new(fopen, fake_fopen, SUBHOOK_OPTION_64BIT_OFFSET);
    subhook_install(getaddrinfo_hook);
    subhook_install(gethostbyname_hook);
}


static void
fin(void)
{
    subhook_remove(getaddrinfo_hook);
    subhook_remove(gethostbyname_hook);
    subhook_free(getaddrinfo_hook);
    subhook_free(gethostbyname_hook);
}


static int
fake_getaddrinfo(const char *node, const char *service, const struct addrinfo *hints
                 , struct addrinfo **res)
{
    int (*real_getaddrinfo)(const char *, const char *, const struct addrinfo *
                            , struct addrinfo **);
    int result;
    real_getaddrinfo = subhook_get_trampoline(getaddrinfo_hook);

    if (real_getaddrinfo == NULL) {
        subhook_remove(getaddrinfo_hook);
        real_getaddrinfo = getaddrinfo;
    }

    subhook_install(fopen_hook);
    result = real_getaddrinfo(node, service, hints, res);
    subhook_remove(fopen_hook);

    if (real_getaddrinfo == getaddrinfo) {
        subhook_install(getaddrinfo_hook);
    }

    return result;
}


static struct hostent *
fake_gethostbyname(const char *name)
{
    struct hostent *(*real_gethostbyname)(const char *);
    struct hostent *result;
    real_gethostbyname = subhook_get_trampoline(gethostbyname_hook);

    if (real_gethostbyname == NULL) {
        subhook_remove(gethostbyname_hook);
        real_gethostbyname = gethostbyname;
    }

    subhook_install(fopen_hook);
    result = real_gethostbyname(name);
    subhook_remove(fopen_hook);

    if (real_gethostbyname == gethostbyname) {
        subhook_install(gethostbyname_hook);
    }

    return result;
}


static FILE *
fake_fopen(const char *path, const char *mode)
{
    FILE *(*real_fopen)(const char *, const char *);
    FILE *result;
    real_fopen = subhook_get_trampoline(fopen_hook);

    if (real_fopen == NULL) {
        subhook_remove(fopen_hook);
        real_fopen = fopen;
    }

    if (strcmp(path, "/etc/resolv.conf") == 0) {
        const char *path_resconf;
        path_resconf = getenv("PATH_RESCONF");

        if (path_resconf == NULL) {
            result = real_fopen("/etc/resolv.conf", mode);
        } else {
            result = real_fopen(path_resconf, mode);
        }
    } else if (strcmp(path, "/etc/hosts") == 0) {
        const char *path_hosts;
        path_hosts = getenv("PATH_HOSTS");

        if (path_hosts == NULL) {
            result = real_fopen("/etc/hosts", mode);
        } else {
            result = real_fopen(path_hosts, mode);
        }
    } else {
        result = real_fopen(path, mode);
    }

    if (real_fopen == fopen) {
        subhook_install(fopen_hook);
    }

    return result;
}
