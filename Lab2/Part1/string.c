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
