#include "Util.h"
#include <fstream>
#include <stdarg.h>
#include <sys/stat.h>
char buffer[1024];
void log(const char* text, ...)
{
  const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];

  va_list ap;
  va_start(ap, text);
  vsnprintf_s(buffer, BUFFER_SIZE, BUFFER_SIZE, text, ap);
  va_end(ap);

  FILE *outfile;
  if (fopen_s(&outfile, "bwapi-data\\logs\\BWSAL.log", "a+")==0)
  {
    fprintf_s(outfile, buffer);
    fclose(outfile);
  }
}