/*
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<html>
 <head>
  <title>Index of /aduqueno.rtorrent.downloads/Bowling.2012.FRENCH.BDRip.720p.HDLight.x264.AAC-NoTag</title>
 </head>
 <body>
<h1>Index of /aduqueno.rtorrent.downloads/Bowling.2012.FRENCH.BDRip.720p.HDLight.x264.AAC-NoTag</h1>
  <table>
   <tr><th valign="top"><img src="/icons/blank.gif" alt="[ICO]"></th><th><a href="?C=N;O=D">Name</a></th><th><a href="?C=M;O=A">Last modified</a></th><th><a href="?C=S;O=A">Size</a></th><th><a href="?C=D;O=A">Description</a></th></tr>
   <tr><th colspan="5"><hr></th></tr>
<tr><td valign="top"><img src="/icons/back.gif" alt="[PARENTDIR]"></td><td><a href="/aduqueno.rtorrent.downloads/">Parent Directory</a></td><td>&nbsp;</td><td align="right">  - </td><td>&nbsp;</td></tr>
<tr><td valign="top"><img src="/icons/movie.gif" alt="[VID]"></td><td><a href="Bowling.2012.FRENCH.BDRip.720p.HDLight.x264.AAC-NoTag.mkv">Bowling.2012.FRENCH.BDRip.720p.HDLight.x264.AAC-NoTag.mkv</a></td><td align="right">2018-01-06 15:37  </td><td align="right">1.4G</td><td>&nbsp;</td></tr>
<tr><td valign="top"><img src="/icons/unknown.gif" alt="[   ]"></td><td><a href="Bowling.2012.FRENCH.BDRip.720p.HDLight.x264.AAC-NoTag.nfo">Bowling.2012.FRENCH.BDRip.720p.HDLight.x264.AAC-NoTag.nfo</a></td><td align="right">2018-01-06 15:34  </td><td align="right">3.7K</td><td>&nbsp;</td></tr>
   <tr><th colspan="5"><hr></th></tr>
</table>
</body></html>
*/


#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#define DIR ./www/images

int main(void)
{
	struct dirent *lecture;
	DIR *rep, *par;
	rep = opendir("./images" );
	char* parent;
	char buffer[128];
	//printf("########## %s ###########\n",buffer);
	while ((lecture = readdir(rep))) {
		if(!strcmp(lecture->d_name,"..")) {
			chdir("./..");
			getcwd(buffer,128);
			printf("-- Parent : %s\n",buffer);
		}
		else {
			if(strcmp(lecture->d_name,".")) {
				printf("%s\n", lecture->d_name);
			}		
		}

	}
	closedir(rep);
}
