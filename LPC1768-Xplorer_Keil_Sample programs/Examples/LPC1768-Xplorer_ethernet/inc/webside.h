/******************************************************************
 *****                                                        *****
 *****  Name: webside.c                                       *****
 *****  Ver.: 1.0                                             *****
 *****  Date: 07/05/2001                                      *****
 *****  Auth: Andreas Dannenberg                              *****
 *****        HTWK Leipzig                                    *****
 *****        university of applied sciences                  *****
 *****        Germany                                         *****
 *****        adannenb@et.htwk-leipzig.de                     *****
 *****  Func: example HTML-code for easyweb.c                 *****
 *****                                                        *****
 ******************************************************************/

// CodeRed - Example webpage updated

const unsigned char WebSide[] = {
"<html>\r\n"
"<head>\r\n"

"<title>easyWEB - dynamic Webside</title>\r\n"
"</head>\r\n"
"\r\n"
"<body bgcolor=\"#3030A0\" text=\"#FFFF00\">\r\n"
"<p><b><font color=\"#FFFFFF\" size=\"6\"><i>Hello World!</i></font></b></p>\r\n"
"\r\n"
"<p><b>This is a dynamic website hosted by the embedded Webserver</b> <b>easyWEB.</b></p>\r\n"
"<p><b>Hardware:</b></p>\r\n"
"<ul>\r\n"
"<li><b>lpc1768 Xplorer board from NGX Technologies. ARM M3 Cortex running EasyWeb</b></li>\r\n"
"<li><b>Embedded EMAC Ethernet Controller (New IP!)</b></li>\r\n"
"</ul>\r\n"
"<br><br>\r\n"

"<form ACTION=\" \" METHOD=POST>"
"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
"<b>LED(D4)ON&nbsp;&nbsp;<input type=\"submit\" name=\"D3\" value=\"ON\" /> &nbsp;&nbsp;&nbsp;&nbsp;"
"LED(D4)OFF&nbsp;&nbsp;<input type=\"submit\" name=\"D3\" value=\"OFF\" /></b>"
"</form>"	
"\r\n"
"\r\n"
"<br><br>\r\n"
"<table border=\"0\" width=\"500\">\r\n"
"<tr>\r\n"
"<td width=\"10%\"></td>\r\n"
"<td width=\"45%\">\r\n"
"<table bgcolor=\"silver\" border=\"5\" cellpadding=\"0\" cellspacing=\"0\" width=\"250\">\r\n"
"<tr>\r\n"
"<td>\r\n"
"<p><b><font color=\"#FFFFFF\" size=\"6\"><i>Page Count: AD1%</i></font></b></p>\r\n"
"</td>\r\n"
"</tr>\r\n"
"</table>\r\n"
"</td>\r\n"
"</tr>\r\n"
"</table>\r\n"
"<br><br><br><br>\r\n"
"<table border=\"0\" width=\"0\">\r\n"
"<tr>\r\n"
"<td width=\"0%\">&nbsp;&nbsp;&nbsp;&nbsp;</td>\r\n"
"</tr>\r\n"
"</table>\r\n"
"\r\n"
"\r\n"
"\r\n"
"\r\n"
"</body>\r\n"
"</html>\r\n"
"\r\n"};
