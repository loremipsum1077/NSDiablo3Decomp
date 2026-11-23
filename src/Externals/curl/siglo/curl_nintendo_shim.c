/* Nintendo's libcurl C runtime shim (siglo/curl_nintendo_shim.c).
 * There is no public source for this file; it is written from the DWARF
 * (names, types, line numbers) and the binary. */

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

/* From nnc/os/os_Tick.h, nnc/socket and nn/nn_Abort.h (C bindings). */
typedef int64_t nnosTick;
typedef struct nnsocketResolverOption nnsocketResolverOption;
struct hostent; struct stat;
nnosTick nnosGetSystemTick(void);
int64_t nnosGetSystemTickFrequency(void);
int64_t nnosConvertTickToNanoSeconds(nnosTick tick);
struct hostent *nnsocketGetHostByNameWithOptions(const char *name,
    const nnsocketResolverOption *options, size_t optionCount);
void *Curl_SigloMalloc(size_t sizeToAllocate);
void Curl_SigloFree(void *pointer);
void nndetailDiagAbortFormatImpl(const char *condition, const char *function,
    const char *fileName, int lineNumber, const char *format, ...)
  __attribute__((noreturn));
#define NNC_ABORT(...) nndetailDiagAbortFormatImpl("NNC_ABORT", __func__, \
  "D:\\home\\teamcity2\\work\\sdk\\Externals\\curl\\siglo\\curl_nintendo_shim.c", \
  __LINE__, __VA_ARGS__)
time_t nnCurlShim_time(time_t *timep)
{
    nnosTick tick;
    int64_t tickFrequency;
    time_t rc;
    tick = nnosGetSystemTick();
    tickFrequency = nnosGetSystemTickFrequency();
    rc = tick / tickFrequency;

    if (timep != NULL)
    {
        *timep = rc;
    }

    return rc;
}


struct timeval nnCurlShim_get_timeval(struct timeval *ptime)
{
    struct timeval now;

    nnosTick tick = nnosGetSystemTick();
    int64_t tickFrequency = nnosGetSystemTickFrequency();

    now.tv_sec = tick / tickFrequency;
    now.tv_usec = (nnosConvertTickToNanoSeconds(tick) / 1000) % 1000000;

    if (ptime != NULL)
    {
        *ptime = now;
    }

    return now;
}

struct hostent *nnCurlShim_gethostbyname(const char *name, nnsocketResolverOption *options, size_t resolverOptionCount)
{
    return nnsocketGetHostByNameWithOptions(name, options, resolverOptionCount);
}

/* the C function a shim stands in for: its name without "nnCurlShim_" */
#define SHIM_NAME (__func__ + sizeof("nnCurlShim_") - 1)


// NON_MATCHING: ptr and total swap callee-saved registers (x19/x20); same instructions otherwise
size_t nnCurlShim_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{

    /* only the standard streams are supported */

    size_t rc;
    size_t total;
    char *stringToPrint;

    if (stream != stdout && stream != stderr)
    {
        NNC_ABORT("unsupported function (%s) called",
                  SHIM_NAME);
    }




    total = size * nmemb;
    if (total != 0)
    {
        /* copy to a terminated string */


        stringToPrint = Curl_SigloMalloc(total + 1);
        if (stringToPrint == NULL)
        {
            NNC_ABORT("unsupported");
        }



        memset(stringToPrint, 0, total + 1); memcpy(stringToPrint, ptr, total);

        Curl_SigloFree(stringToPrint);
    }
    rc = total;
    return rc;
}



int nnCurlShim_fputs(const char *s, FILE *stream)
{
    int rc = -1;





    if (stream != stdout && stream != stderr)
    {
        NNC_ABORT("unsupported function (%s) called",
                  SHIM_NAME);
    }

    if (s != NULL)
    {




        rc = (int)strlen(s);
    }

    return rc;
}


int nnCurlShim_fflush(FILE *fp)
{
    int rc = -1;





    if (fp != stdout && fp != stderr)
    {
        /* only the standard streams are supported */



        NNC_ABORT("unsupported function (%s) called",
                  SHIM_NAME);
    }
    rc = 0;
    return rc;
}
























int nnCurlShim_fprintf(FILE *stream, const char *format, ...)
{








    int rc = -1;
    va_list ap;

    if (stream != stdout && stream != stderr)
    {
        NNC_ABORT("unsupported function (%s) called",
                  SHIM_NAME);
    }





    va_start(ap, format);

    va_end(ap);

    return rc;
}


int nnCurlShim_vfprintf(FILE *stream, const char *format, va_list args)
{
    int rc = -1;




    if (stream != stdout && stream != stderr)
    {
        NNC_ABORT("unsupported function (%s) called",
                  SHIM_NAME);
    }







    return rc;
}





































int nnCurlShim_fclose(FILE *fp)
{
    int rc = -1;





    NNC_ABORT("unsupported function (%s) called",
              SHIM_NAME);

    return rc;
}


FILE *nnCurlShim_fopen(const char *path, const char *mode)
{
    FILE *rc = NULL;





    NNC_ABORT("unsupported function (%s) called",
              SHIM_NAME);

    return rc;
}


size_t nnCurlShim_fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    int rc = -1;





    NNC_ABORT("unsupported function (%s) called",
              SHIM_NAME);

    return rc;
}


int nnCurlShim_fseek(FILE *stream, long offset, int whence)
{
    int rc = -1;





    NNC_ABORT("unsupported function (%s) called",
              SHIM_NAME);

    return rc;
}


char *nnCurlShim_fgets(char *s, int size, FILE *stream)
{
    char *rc = NULL;





    NNC_ABORT("unsupported function (%s) called",
              SHIM_NAME);

    return rc;
}
































int nnCurlShim_stat(const char *path, struct stat *buf)
{
    int rc = -1;





    NNC_ABORT("unsupported function (%s) called",
              SHIM_NAME);

    return rc;
}
