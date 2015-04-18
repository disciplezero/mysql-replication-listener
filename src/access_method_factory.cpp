/*
Copyright (c) 2003, 2011, 2013, Oracle and/or its affiliates. All rights
reserved.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of
the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
02110-1301  USA
*/
#include "access_method_factory.h"
#include "file_driver.h"

using mysql::system::Binary_log_driver;
using mysql::system::Binlog_file_driver;

static Binary_log_driver *parse_file_url(const char *body, size_t length)
{
  /* Find the beginning of the file name */
  if (strncmp(body, "//", 2) != 0)
    return 0;

  /*
    Since we don't support host information yet, there should be a
    slash after the initial "//".
   */
  if (body[2] != '/')
    return 0;

  return new Binlog_file_driver(body + 2);
}

/**
   URI parser information.
 */
struct Parser {
  const char* protocol;
  Binary_log_driver *(*parser)(const char *body, size_t length);
};

/**
   Array of schema names and matching parsers.
*/
static Parser url_parser[] = {
  { "file",  parse_file_url },
};

Binary_log_driver *
mysql::system::create_transport(const char *url)
{
  const char *pfx = strchr(url, ':');
  if (pfx == 0)
    return NULL;
  for (int i = 0 ; i < sizeof(url_parser)/sizeof(*url_parser) ; ++i)
  {
    const char *proto = url_parser[i].protocol;
    if (strncmp(proto, url, strlen(proto)) == 0)
      return (*url_parser[i].parser)(pfx+1, strlen(pfx+1));
  }
  return NULL;
}
