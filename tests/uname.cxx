#include <sys/utsname.h>
#include <cstdio>
#include <cstdlib>

int main()
{
    int i = 1;
    struct utsname *buf=(utsname*)malloc(sizeof (utsname));
    if(!buf)
        return i;
    i = uname(buf);
    if (i)
        return i;
    printf("1. Machine Hardware: %s\n", buf->machine);
    printf("2. Hostname: %s\n", buf->nodename);
    printf("3. OS name: %s\n", buf->sysname);
    printf("4. Kernel Release: %s\n", buf->release);
    printf("5. Kernel Version: %s\n", buf->version);
    printf("6. Domain name: %s\n", buf->domainname);
    printf("-> Size of utsname->Version: %u\n",sizeof (buf->version));
    printf("-> 6 x %u is the size of utsname: %u\n",
           sizeof (buf->version),
           sizeof (utsname));
    free(buf);
    return i;
}
