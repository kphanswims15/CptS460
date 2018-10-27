int strlen(char *s)
{
  int len = 0, i = 0;

  for(i = 0; s[i] != 0; i++)
  {
    len++;
  }
  return len;
}

int strcmp(char *s1, char *s2)
{
  int i;
  for (i = 0; s1[i] && s2[i]; i++)
  {
    if (s1[i] == s2[i] || (s1[i] ^ 32) == s2[i])
      continue;
    else
      break;
  }

  if (s1[i] == s2[i])
    return 0;
  if((s1[i]|32) < (s2[i]|32))
    return -1;
  return 1;
}

int _strcpy(char *dest, char *src)
{
  int i = 0;
  while(src[i] != 0)
  {
    dest[i] = src[i];
    ++i;
  }
  dest[i] = 0;
  return 1;
}

int strncpy(char *dest, char *src, int n)
{
  int i = 0;
  while(src[i] != 0 && n != 0)
  {
    dest[i] = src[i];
    ++i;
    n--;
  }
  dest[i] = 0;
  return 1;
}

int strncmp(char *s1, char *s2, int n)
{
  while(n && *s1 && (*s1 == *s2))
  {
    ++s1;
    ++s2;
    --n;
  }

  if(n == 0)
  {
    return 0;
  }
  else
  {
    return (*(unsigned char *)s1 - *(unsigned char *) s2);
  }
}

int memcpy(char *dest, char *src, int n)
{
  for (int i = 0; i < n; i++)
  {
    dest[i] ==  src[i];
  }

  return 1;
}

int _memset(char *dest, char c, int n)
{
  while(n--)
  {
    *dest++ = c;
  }
  return 1;
}
