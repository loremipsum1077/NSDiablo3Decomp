/* Nintendo's libcurl allocator glue (siglo/curl_nintendo_allocator.c).
 * There is no public source for this file; it is written from the DWARF
 * (names, types, line numbers) and the binary.
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* From the Nintendo SDK diagnostics header (nn/nn_Abort.h, C binding). */
void nndetailDiagAbortFormatImpl(const char *condition, const char *function,
                                 const char *fileName, int lineNumber,
                                 const char *format, ...)
  __attribute__((noreturn));

/* NNC_ABORT-style macro (name invented); the file name is the one the
 * original build recorded. */
#define CURL_SIGLO_ABORT(message)                                            \
  nndetailDiagAbortFormatImpl("NNC_ABORT", __func__,                         \
    "D:\\home\\teamcity2\\work\\sdk\\Externals\\curl\\siglo\\curl_nintendo_allocator.c", \
    __LINE__, message)

typedef struct Curl_SigloAllocatorTable
{
  void *(*m_mallocFunctionPointer)(size_t);
  /* free */
  void (*m_freeFunctionPointer)(void *);
  /* realloc */
  void *(*m_reallocFunctionPointer)(void *, size_t);
  /* strdup */
  char *(*m_strdupFunctionPointer)(const char *);
  /* calloc */
  void *(*m_callocFunctionPointer)(size_t, size_t);
} Curl_SigloAllocatorTable;
Curl_SigloAllocatorTable g_allocatorTable;


int Curl_SigloAllocatorInitializeDefaults(void)
{
  g_allocatorTable.m_mallocFunctionPointer = malloc;
  g_allocatorTable.m_freeFunctionPointer = free;
  g_allocatorTable.m_reallocFunctionPointer = realloc;
  g_allocatorTable.m_strdupFunctionPointer = strdup;
  g_allocatorTable.m_callocFunctionPointer = calloc;

  return 1;
}








int Curl_SigloAllocatorInitialize(void *(*m)(size_t), void (*f)(void *),
                                  void *(*r)(void *, size_t),
                                  char *(*s)(const char *),
                                  void *(*c)(size_t, size_t))
{
  if (m != NULL &&
      f != NULL && r != NULL && s != NULL && c != NULL)
  {
    g_allocatorTable.m_mallocFunctionPointer = m;
    g_allocatorTable.m_freeFunctionPointer = f;
    g_allocatorTable.m_reallocFunctionPointer = r;
    g_allocatorTable.m_strdupFunctionPointer = s;
    g_allocatorTable.m_callocFunctionPointer = c;

    return 1;
  }

  return 0;
}





void *Curl_SigloMalloc(size_t sizeToAllocate)
{
  return g_allocatorTable.m_mallocFunctionPointer(sizeToAllocate);
}

void Curl_SigloFree(void *pointer)
{
  g_allocatorTable.m_freeFunctionPointer(pointer);
}

void *Curl_SigloRealloc(void *ptr, size_t size)
{
  return g_allocatorTable.m_reallocFunctionPointer(ptr, size);
}

char *Curl_SigloStrdup(const char *string)
{
  return g_allocatorTable.m_strdupFunctionPointer(string);
}

void *Curl_SigloCalloc(size_t nmemb, size_t size)
{
  return g_allocatorTable.m_callocFunctionPointer(nmemb, size);
}

void *Curl_SigloAllocAligned(size_t alignment, size_t size)
{
  void *pMem;
  void **ppAligned;
  size_t offset = alignment - 1 + sizeof(void *);

  if (g_allocatorTable.m_mallocFunctionPointer == NULL)
  {
    return NULL;
  }

  /* room for the alignment and for the original pointer */
  pMem = g_allocatorTable.m_mallocFunctionPointer(offset + size);
  if (pMem == NULL)
  {
    return NULL;
  }

  ppAligned = (void **)(((size_t)pMem + offset) & ~(alignment - 1));
  ppAligned[-1] = pMem;

  return ppAligned;
}


void Curl_SigloFreeAligned(void *pointer)
{
  if (g_allocatorTable.m_freeFunctionPointer == NULL)
  {
    CURL_SIGLO_ABORT("aligned free called before initialized");
  }

  /* the original pointer is stored just before the aligned block */
  g_allocatorTable.m_freeFunctionPointer(((void **)pointer)[-1]);
}
