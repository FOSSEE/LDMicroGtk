void OutputDebugString(char* str)
{

}

double GetTickCount(void) 
{
//   timespec now;
//   clock_gettime()
//   if (clock_gettime(CLOCK_MONOTONIC, &now))
//     return 0;
  return 10.2;//now.tv_sec * 1000.0 + now.tv_nsec / 1000000.0;
}