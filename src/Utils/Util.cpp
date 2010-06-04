#include "Util.h"
#include <fstream>
#include <stdarg.h>
#include <sys/stat.h>

using namespace std;

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
  fopen_s(&outfile, "bwapi-data\\logs\\BWAI_Log.txt", "a+");
  fprintf_s(outfile, buffer);
  fprintf_s(outfile, "\n");
  fclose(outfile);
}

void clearLog()
{
  FILE *outfile;
  fopen_s(&outfile, "bwapi-data\\logs\\BWAI_Log.txt", "w");
  fclose(outfile);
}