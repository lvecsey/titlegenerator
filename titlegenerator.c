/*
    titlegenerator - create a 1920x1080 title slide for video production
    Copyright (C) 2020  Lester Vecsey

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "fcgi_stdio.h"

#include <stdlib.h>
#include <unistd.h>

#include <fcgiapp.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <errno.h>

#include <string.h>

int fill_substring(char *out_buf, char *in_buf) {

  char *rptr;
  char *wptr;

  rptr = in_buf;
  wptr = out_buf;

  while (rptr[0]) {

    if (rptr[0] == '+') {
      wptr[0] = ' ';
      rptr++;
      wptr++;
      continue;
    }

    wptr[0] = rptr[0];

    rptr++;
    wptr++;
    
  }

  wptr[0] = 0;
  
  return 0;
  
}

int main(int argc, char *argv[]) {

  long int count;

  char title_buf[80];

  char title_clean[80];  

  long int fontsize;

  int retval;

  char template[7];
  
  count = 0;

    while(FCGI_Accept() >= 0) {

      char *env_QS;

      env_QS = getenv("QUERY_STRING");
      
      printf("Content-type: image/png\r\n"
	     "\r\n");

      retval = sscanf(env_QS, "fontsize=%ld&title=%s", &fontsize, title_buf);

      if (retval == 2) {

	char cmd[255];

	int out_fd;
	
	retval = fill_substring(title_clean, title_buf);
	fprintf(stderr, "Rendering fontsize %ld and text %s\n", fontsize, title_clean);

	memset(template, 'X', 6);
	template[6] = 0;

	retval = mkstemp(template);
	
	retval = sprintf(cmd, "convert -size 1920x1080 -background black -fill white -font ./cmr10.ttf -pointsize %ld -gravity Center caption:'%s' /tmp/output_tg%s.png", fontsize, title_clean, template);

	{

	  struct stat buf;
	  int fd;
	  void *m;

	  char in_fn[80];
	  
	  retval = system(cmd);

	  retval = sprintf(in_fn, "/tmp/output_tg%s.png", template);
	  
	  fd = open(in_fn, O_RDWR);
	  retval = fstat(fd, &buf);
	  m = mmap(NULL, buf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	  if (m == MAP_FAILED) {
	    retval = close(fd);
	    continue;
	  }
	  
	  retval = fwrite(m, buf.st_size, 1, stdout);
	  
	}
	  
      }

      
    }

    return 0;
    
}
