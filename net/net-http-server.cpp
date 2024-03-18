// Compiler for PHP (aka KPHP)
// Copyright (c) 2020 LLC «V Kontakte»
// Distributed under the GPL v3 License, see LICENSE.notice.txt

#include "net/net-http-server.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common/crc32.h"
#include "common/kprintf.h"
#include "common/precise-time.h"

#include "net/net-buffers.h"
#include "net/net-connections.h"
#include "net/net-events.h"

/*
 *
 *		HTTP SERVER INTERFACE
 *
 */

#define SERVER_VERSION "nginx/0.3.33"

int http_connections;
long long http_queries, http_bad_headers, http_queries_size;

static const char *extra_http_response_headers = "";

int hts_std_wakeup(struct connection *c);
int hts_parse_execute(struct connection *c);
int hts_std_alarm(struct connection *c);
int hts_do_wakeup(struct connection *c);
int hts_init_accepted(struct connection *c);
int hts_close_connection(struct connection *c, int who);

conn_type_t ct_http_server = {.magic = CONN_FUNC_MAGIC,
                              .flags = 0,
                              .title = "http_server",
                              .accept = accept_new_connections,
                              .init_accepted = hts_init_accepted,
                              .create_outbound = NULL,
                              .run = server_read_write,
                              .reader = server_reader,
                              .writer = server_writer,
                              .close = hts_close_connection,
                              .free_buffers = free_connection_buffers,
                              .parse_execute = hts_parse_execute,
                              .init_outbound = server_failed,
                              .connected = server_failed,
                              .wakeup = hts_std_wakeup,
                              .alarm = hts_std_alarm,
                              .ready_to_write = NULL,
                              .check_ready = NULL,
                              .wakeup_aio = NULL,
                              .data_received = NULL,
                              .data_sent = NULL,
                              .ancillary_data_received = NULL,
                              .flush = NULL,
                              .crypto_init = NULL,
                              .crypto_free = NULL,
                              .crypto_encrypt_output = NULL,
                              .crypto_decrypt_input = NULL,
                              .crypto_needed_output_bytes = NULL};

enum http_query_parse_state {
  htqp_start,
  htqp_readtospace,
  htqp_readtocolon,
  htqp_readint,
  htqp_skipspc,
  htqp_skiptoeoln,
  htqp_skipspctoeoln,
  htqp_eoln,
  htqp_wantlf,
  htqp_wantlastlf,
  htqp_linestart,
  htqp_fatal,
  htqp_done
};

int hts_default_execute(struct connection *c, int op);

struct http_server_functions default_http_server = {.info = NULL,
                                                    .execute = hts_default_execute,
                                                    .ht_wakeup = hts_do_wakeup,
                                                    .ht_alarm = hts_do_wakeup,
                                                    .ht_close = NULL};

int hts_default_execute(struct connection *c, int op) {
  struct hts_data *D = HTS_DATA(c);

  if (op != htqt_empty) {
    netw_queries++;
    if (op != htqt_get) {
      netw_update_queries++;
    }
  }

  switch (op) {

    case htqt_empty:
      break;

    case htqt_get:
    case htqt_post:
    case htqt_head:
    case htqt_options:

    default:
      D->query_flags |= QF_ERROR;
      break;
  }

  assert(advance_skip_read_ptr(&c->In, D->header_size) == D->header_size);
  return D->data_size >= 0 ? -413 : -501;
}

int hts_init_accepted(struct connection *c __attribute__((unused))) { // TODO: this unused generated by script, recheck it.
  http_connections++;
  return 0;
}

int hts_close_connection(struct connection *c, int who) {
  tvkprintf(net_connections, 3, "server close http conn %d\n", c->fd);
  http_connections--;

  if (HTS_FUNC(c)->ht_close != NULL) {
    HTS_FUNC(c)->ht_close(c, who);
  }

  return server_close_connection(c, who);
}

static inline const char *http_get_error_msg_text(int *code) {
  /* the most frequent case */
  if (*code == 200) {
    return "OK";
  }
  switch (*code) {
    /* python generated from old array */
    case 201:
      return "Created";
    case 202:
      return "Accepted";
    case 204:
      return "No Content";
    case 206:
      return "Partial Content";
    case 301:
      return "Moved Permanently";
    case 302:
      return "Found";
    case 303:
      return "See Other";
    case 304:
      return "Not Modified";
    case 307:
      return "Temporary Redirect";
    case 400:
      return "Bad Request";
    case 403:
      return "Forbidden";
    case 404:
      return "Not Found";
    case 405:
      return "Method Not Allowed";
    case 406:
      return "Not Acceptable";
    case 408:
      return "Request Timeout";
    case 411:
      return "Length Required";
    case 413:
      return "Request Entity Too Large";
    case 414:
      return "Request-URI Too Long";
    case 418:
      return "I'm a teapot";
    case 422:
      return "Unprocessable Entity";
    case 429:
      return "Too Many Requests";
    case 431:
      return "Request Header Fields Too Large";
    case 501:
      return "Not Implemented";
    case 502:
      return "Bad Gateway";
    case 503:
      return "Service Unavailable";
    case 504:
      return "Gateway timeout";
    default:
      *code = 500;
  }
  return "Internal Server Error";
}

static char error_text_pattern[] = "<html>\r\n"
                                   "<head><title>%d %s</title></head>\r\n"
                                   "<body bgcolor=\"white\">\r\n"
                                   "<center><h1>%d %s</h1></center>\r\n"
                                   "<hr><center>" SERVER_VERSION "</center>\r\n"
                                   "</body>\r\n"
                                   "</html>\r\n";

int format_http_error_page(int code, char *buff, size_t buff_size) {
  const char *error_message = http_get_error_msg_text(&code);
  return snprintf(buff, buff_size, error_text_pattern, code, error_message, code, error_message);
}

int write_http_error(struct connection *c, int code) {
  if (code == 204) {
    write_basic_http_header(c, code, 0, -1, nullptr, nullptr);
    return 0;
  } else {
    const size_t buff_size = 1024;
    static char buff[buff_size];
    int len = format_http_error_page(code, buff, buff_size);
    write_basic_http_header(c, code, 0, len, nullptr, nullptr);
    return write_out(&c->Out, buff, len);
  }
}

int hts_parse_execute(struct connection *c) {
  tvkprintf(net_connections, 3, "server start processing http conn %d\n", c->fd);
  struct hts_data *D = HTS_DATA(c);
  char *ptr, *ptr_s, *ptr_e;
  int len;
  long long tt;

  while (c->status == conn_expect_query || c->status == conn_reading_query) {
    len = nbit_ready_bytes(&c->Q);
    ptr = ptr_s = static_cast<char *>(nbit_get_ptr(&c->Q));
    ptr_e = ptr + len;
    if (len <= 0) {
      break;
    }

    while (ptr < ptr_e && c->parse_state != htqp_done) {
      tvkprintf(net_connections, 4, "server parse conn %d in state %d\n", c->fd, c->parse_state);
      switch (c->parse_state) {
        case htqp_start:
          // fprintf (stderr, "htqp_start: ptr=%p (%.8s), hsize=%d, qf=%d, words=%d\n", ptr, ptr, D->header_size, D->query_flags, D->query_words);
          memset(D, 0, sizeof(*D));
          D->query_type = htqt_none;
          D->data_size = -1;
          c->parse_state = htqp_readtospace;
          /* fallthrough */

        case htqp_readtospace:
          // fprintf (stderr, "htqp_readtospace: ptr=%p (%.8s), hsize=%d, qf=%d, words=%d\n", ptr, ptr, D->header_size, D->query_flags, D->query_words);
          while (ptr < ptr_e && ((unsigned)*ptr > ' ')) {
            if (D->wlen < 15) {
              D->word[D->wlen] = *ptr;
            }
            D->wlen++;
            ptr++;
          }
          if (D->wlen > MAX_HTTP_HEADER_QUERY_WORD_SIZE) {
            if (D->query_words == 1) {
              D->extra_int = 414;
            }
            c->parse_state = htqp_fatal;
            break;
          }
          if (ptr == ptr_e) {
            break;
          }
          c->parse_state = htqp_skipspc;
          D->query_words++;
          if (D->query_words == 1) {
            D->query_type = htqt_error;
            if (D->wlen == 3 && !memcmp(D->word, "GET", 3)) {
              D->query_type = htqt_get;
            } else if (D->wlen == 4) {
              if (!memcmp(D->word, "HEAD", 4)) {
                D->query_type = htqt_head;
              } else if (!memcmp(D->word, "POST", 4)) {
                D->query_type = htqt_post;
              }
            } else if (D->wlen == 7 && !memcmp(D->word, "OPTIONS", 7)) {
              D->query_type = htqt_options;
            }
            if (D->query_type == htqt_error) {
              c->parse_state = htqp_skiptoeoln;
              D->query_flags |= QF_ERROR;
            }
          } else if (D->query_words == 2) {
            D->uri_offset = D->header_size;
            D->uri_size = D->wlen;
            if (!D->wlen) {
              c->parse_state = htqp_skiptoeoln;
              D->query_flags |= QF_ERROR;
            }
          } else if (D->query_words == 3) {
            c->parse_state = htqp_skipspctoeoln;
            if (D->wlen != 0) {
              /* HTTP/x.y */
              if (D->wlen != 8) {
                c->parse_state = htqp_skiptoeoln;
                D->query_flags |= QF_ERROR;
              } else {
                if (!memcmp(D->word, "HTTP/1.0", 8)) {
                  D->http_ver = HTTP_V10;
                } else if (!memcmp(D->word, "HTTP/1.1", 8)) {
                  D->http_ver = HTTP_V11;
                } else {
                  c->parse_state = htqp_skiptoeoln;
                  D->query_flags |= QF_ERROR;
                }
              }
            } else {
              D->http_ver = HTTP_V09;
            }
          } else {
            assert(D->query_flags & (QF_HOST | QF_CONNECTION));
            if (D->wlen) {
              if (D->query_flags & QF_HOST) {
                D->host_offset = D->header_size;
                D->host_size = D->wlen;
              } else if (D->wlen == 10 && !strncasecmp(D->word, "keep-alive", 10)) {
                D->query_flags |= QF_KEEPALIVE;
              }
            }
            D->query_flags &= ~(QF_HOST | QF_CONNECTION);
            c->parse_state = htqp_skipspctoeoln;
          }
          D->header_size += D->wlen;
          break;

        case htqp_skipspc:
        case htqp_skipspctoeoln:
          // fprintf (stderr, "htqp_skipspc[toeoln]: ptr=%p (%.8s), hsize=%d, qf=%d, words=%d\n", ptr, ptr, D->header_size, D->query_flags, D->query_words);
          while (D->header_size < MAX_HTTP_HEADER_SIZE && ptr < ptr_e && (*ptr == ' ' || (*ptr == '\t' && D->query_words >= 8))) {
            D->header_size++;
            ptr++;
          }
          if (D->header_size >= MAX_HTTP_HEADER_SIZE) {
            c->parse_state = htqp_fatal;
            break;
          }
          if (ptr == ptr_e) {
            break;
          }
          if (c->parse_state == htqp_skipspctoeoln) {
            c->parse_state = htqp_eoln;
            break;
          }
          if (D->query_words < 3) {
            D->wlen = 0;
            c->parse_state = htqp_readtospace;
          } else {
            assert(D->query_words >= 4);
            if (D->query_flags & QF_DATASIZE) {
              if (D->data_size != -1) {
                c->parse_state = htqp_skiptoeoln;
                D->query_flags |= QF_ERROR;
              } else {
                c->parse_state = htqp_readint;
                D->data_size = 0;
              }
            } else if (D->query_flags & (QF_HOST | QF_CONNECTION)) {
              D->wlen = 0;
              c->parse_state = htqp_readtospace;
            } else {
              c->parse_state = htqp_skiptoeoln;
            }
          }
          break;

        case htqp_readtocolon:
          // fprintf (stderr, "htqp_readtocolon: ptr=%p (%.8s), hsize=%d, qf=%d, words=%d\n", ptr, ptr, D->header_size, D->query_flags, D->query_words);
          while (ptr < ptr_e && *ptr != ':' && *ptr > ' ') {
            if (D->wlen < 15) {
              D->word[D->wlen] = *ptr;
            }
            D->wlen++;
            ptr++;
          }
          if (D->wlen > MAX_HTTP_HEADER_KEY_SIZE) {
            c->parse_state = htqp_fatal;
            break;
          }
          if (ptr == ptr_e) {
            break;
          }

          if (*ptr != ':') {
            D->header_size += D->wlen;
            c->parse_state = htqp_skiptoeoln;
            D->query_flags |= QF_ERROR;
            break;
          }

          ptr++;

          if (D->wlen == 4 && !strncasecmp(D->word, "host", 4)) {
            D->query_flags |= QF_HOST;
          } else if (D->wlen == 10 && !strncasecmp(D->word, "connection", 10)) {
            D->query_flags |= QF_CONNECTION;
          } else if (D->wlen == 14 && !strncasecmp(D->word, "content-length", 14)) {
            D->query_flags |= QF_DATASIZE;
          } else {
            D->query_flags &= ~(QF_HOST | QF_DATASIZE | QF_CONNECTION);
          }

          D->header_size += D->wlen + 1;
          c->parse_state = htqp_skipspc;
          break;

        case htqp_readint:
          // fprintf (stderr, "htqp_readint: ptr=%p (%.8s), hsize=%d, qf=%d, words=%d\n", ptr, ptr, D->header_size, D->query_flags, D->query_words);

          tt = D->data_size;
          while (ptr < ptr_e && *ptr >= '0' && *ptr <= '9') {
            if (tt >= 0x7fffffffL / 10) {
              D->query_flags |= QF_ERROR;
              c->parse_state = htqp_skiptoeoln;
              break;
            }
            tt = tt * 10 + (*ptr - '0');
            ptr++;
            D->header_size++;
            D->query_flags &= ~QF_DATASIZE;
          }

          D->data_size = tt;
          if (ptr == ptr_e) {
            break;
          }

          if (D->query_flags & QF_DATASIZE) {
            D->query_flags |= QF_ERROR;
            c->parse_state = htqp_skiptoeoln;
          } else {
            c->parse_state = htqp_skipspctoeoln;
          }
          break;

        case htqp_skiptoeoln:
          // fprintf (stderr, "htqp_skiptoeoln: ptr=%p (%.8s), hsize=%d, qf=%d, words=%d\n", ptr, ptr, D->header_size, D->query_flags, D->query_words);

          while (D->header_size < MAX_HTTP_HEADER_SIZE && ptr < ptr_e && (*ptr != '\r' && *ptr != '\n')) {
            D->header_size++;
            ptr++;
          }
          if (D->header_size >= MAX_HTTP_HEADER_SIZE) {
            c->parse_state = htqp_fatal;
            break;
          }
          if (ptr == ptr_e) {
            break;
          }

          c->parse_state = htqp_eoln;
          /* fallthrough */
        case htqp_eoln:

          if (ptr == ptr_e) {
            break;
          }
          if (*ptr == '\r') {
            ptr++;
            D->header_size++;
          }
          c->parse_state = htqp_wantlf;
          /* fallthrough */
        case htqp_wantlf:
          // fprintf (stderr, "htqp_wantlf: ptr=%p (%.8s), hsize=%d, qf=%d, words=%d\n", ptr, ptr, D->header_size, D->query_flags, D->query_words);

          if (ptr == ptr_e) {
            break;
          }
          if (++D->query_words < 8) {
            D->query_words = 8;
            if (D->query_flags & QF_ERROR) {
              c->parse_state = htqp_fatal;
              break;
            }
          }

          if (D->http_ver <= HTTP_V09) {
            c->parse_state = htqp_wantlastlf;
            break;
          }

          if (*ptr != '\n') {
            D->query_flags |= QF_ERROR;
            c->parse_state = htqp_skiptoeoln;
            break;
          }

          ptr++;
          D->header_size++;

          c->parse_state = htqp_linestart;
          /* fallthrough */
        case htqp_linestart:
          // fprintf (stderr, "htqp_linestart: ptr=%p (%.8s), hsize=%d, qf=%d, words=%d\n", ptr, ptr, D->header_size, D->query_flags, D->query_words);

          if (ptr == ptr_e) {
            break;
          }

          if (!D->first_line_size) {
            D->first_line_size = D->header_size;
          }

          if (*ptr == '\r') {
            ptr++;
            D->header_size++;
            c->parse_state = htqp_wantlastlf;
            break;
          }
          if (*ptr == '\n') {
            c->parse_state = htqp_wantlastlf;
            break;
          }

          if (D->query_flags & QF_ERROR) {
            c->parse_state = htqp_skiptoeoln;
          } else {
            D->wlen = 0;
            c->parse_state = htqp_readtocolon;
          }
          break;

        case htqp_wantlastlf:
          // fprintf (stderr, "htqp_wantlastlf: ptr=%p (%.8s), hsize=%d, qf=%d, words=%d\n", ptr, ptr, D->header_size, D->query_flags, D->query_words);

          if (ptr == ptr_e) {
            break;
          }
          if (*ptr != '\n') {
            c->parse_state = htqp_fatal;
            break;
          }
          ptr++;
          D->header_size++;

          if (!D->first_line_size) {
            D->first_line_size = D->header_size;
          }

          c->parse_state = htqp_done;

        case htqp_done:
          // fprintf (stderr, "htqp_done: ptr=%p (%.8s), hsize=%d, qf=%d, words=%d\n", ptr, ptr, D->header_size, D->query_flags, D->query_words);
          break;

        case htqp_fatal:
          // fprintf (stderr, "htqp_fatal: ptr=%p (%.8s), hsize=%d, qf=%d, words=%d\n", ptr, ptr, D->header_size, D->query_flags, D->query_words);
          D->query_flags |= QF_ERROR;
          c->parse_state = htqp_done;
          break;

        default:
          assert(0);
      }
    }

    len = ptr - ptr_s;
    nbit_advance(&c->Q, len);

    if (c->parse_state == htqp_done) {
      if (D->header_size >= MAX_HTTP_HEADER_SIZE) {
        if (D->extra_int == 0) {
          D->extra_int = 431;
        }
        D->query_flags |= QF_ERROR;
      }
      if (!(D->query_flags & QF_ERROR)) {
        c->status = conn_running;
        if (!HTS_FUNC(c)->execute) {
          HTS_FUNC(c)->execute = hts_default_execute;
        }
        int res;
        if (D->query_type == htqt_post && D->data_size < 0) {
          assert(advance_skip_read_ptr(&c->In, D->header_size) == D->header_size);
          res = -411;
        } else if (D->query_type != htqt_post && D->data_size > 0) {
          res = -413;
        } else {
          res = HTS_FUNC(c)->execute(c, D->query_type);
        }
        http_queries++;
        http_queries_size += D->header_size + D->data_size;
        if (res > 0) {
          c->status = conn_reading_query;
          return res; // need more bytes
        } else if (res < 0) {
          if (res == SKIP_ALL_BYTES) {
            assert(advance_skip_read_ptr(&c->In, D->header_size) == D->header_size);
            if (D->data_size > 0) {
              len = advance_skip_read_ptr(&c->In, D->data_size);
              if (len < D->data_size) {
                c->parse_state = htqp_start;
                if (c->status == conn_running) {
                  c->status = conn_expect_query;
                }
                return len - D->data_size;
              }
            }
          } else {
            if (res == -413) {
              D->query_flags &= ~QF_KEEPALIVE;
            }
            write_http_error(c, -res);
            D->query_flags &= ~QF_ERROR;
          }
        }
        if (c->status == conn_running) {
          c->status = conn_expect_query;
        }

        // assert ((c->pending_queries && (c->status == conn_wait_net || c->status == conn_wait_aio)) || (!c->pending_queries && c->status ==
        // conn_expect_query));
        assert(c->status == conn_wait_net || (c->pending_queries && c->status == conn_wait_aio) || (!c->pending_queries && c->status == conn_expect_query));

      } else {
        // fprintf (stderr, "[parse error]\n");
        assert(advance_skip_read_ptr(&c->In, D->header_size) == D->header_size);
        c->status = conn_expect_query;
        http_bad_headers++;
      }
      if (D->query_flags & QF_ERROR) {
        assert(c->status == conn_expect_query);
        D->query_flags &= ~QF_KEEPALIVE;
        write_http_error(c, D->extra_int ? D->extra_int : 400);
        D->extra_int = 0;
      }
      if (c->status == conn_expect_query && !(D->query_flags & QF_KEEPALIVE)) {
        c->status = conn_write_close;
        c->parse_state = -1;
        return 0;
      }
      if (c->status != conn_wait_aio) {
        c->parse_state = htqp_start;
      }
      nbit_set(&c->Q, &c->In);
    }
  }
  if (c->status == conn_reading_query || c->status == conn_wait_aio) {
    return NEED_MORE_BYTES;
  }
  return 0;
}

int hts_std_wakeup(struct connection *c) {
  tvkprintf(net_connections, 3, "server standard http wakeup on conn %d\n", c->fd);
  if (c->status == conn_wait_net || c->status == conn_wait_aio) {
    c->status = conn_expect_query;
    HTS_FUNC(c)->ht_wakeup(c);
  }
  if (c->Out.total_bytes > 0) {
    c->flags |= C_WANTWR;
  }
  if (c->status != conn_wait_net && c->status != conn_wait_aio) {
    c->generation = ++conn_generation;
    c->pending_queries = 0;
  }
  return 0;
}

int hts_std_alarm(struct connection *c) {
  tvkprintf(net_connections, 3, "server standard http alarm on conn %d\n", c->fd);
  HTS_FUNC(c)->ht_alarm(c);
  if (c->Out.total_bytes > 0) {
    c->flags |= C_WANTWR;
  }
  c->generation = ++conn_generation;
  c->pending_queries = 0;
  return 0;
}

int hts_do_wakeup(struct connection *c __attribute__((unused))) { // TODO: this unused generated by script, recheck it.
  // struct hts_data *D = HTS_DATA(c);
  assert(0);
  return 0;
}

/*
 *
 *		USEFUL HTTP FUNCTIONS
 *
 */

char now_date_string[] = "Thu, 01 Jan 1970 00:00:00 GMT";
int now_date_utime;

static char months[] = "JanFebMarAprMayJunJulAugSepOctNovDecGlk";
static char dows[] = "SunMonTueWedThuFriSatEar";

int dd[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void gen_http_date(char date_buffer[29], int time) {
  int day, mon, year, hour, min, sec, xd, i, dow;
  if (time < 0)
    time = 0;
  sec = time % 60;
  time /= 60;
  min = time % 60;
  time /= 60;
  hour = time % 24;
  time /= 24;
  dow = (time + 4) % 7;
  xd = time % (365 * 3 + 366);
  time /= (365 * 3 + 366);
  year = time * 4 + 1970;
  if (xd >= 365) {
    year++;
    xd -= 365;
    if (xd >= 365) {
      year++;
      xd -= 365;
      if (xd >= 366) {
        year++;
        xd -= 366;
      }
    }
  }
  if (year & 3) {
    dd[1] = 28;
  } else {
    dd[1] = 29;
  }

  for (i = 0; i < 12; i++) {
    if (xd < dd[i]) {
      break;
    }
    xd -= dd[i];
  }

  day = xd + 1;
  mon = i;
  assert(day >= 1 && day <= 31 && mon >= 0 && mon <= 11 && year >= 1970 && year <= 2039);

  snprintf(date_buffer, 30, "%.3s, %.2d %.3s %d %.2d:%.2d:%.2d GMT", dows + dow * 3, day, months + mon * 3, year, hour, min, sec);
}

int gen_http_time(char *date_buffer, int *time) {
  char dow[4];
  char month[4];
  char tz[16];
  int i, year, mon, day, hour, min, sec;
  int argc = sscanf(date_buffer, "%3s, %d %3s %d %d:%d:%d %15s", dow, &day, month, &year, &hour, &min, &sec, tz);
  if (argc != 8) {
    return (argc > 0) ? -argc : -8;
  }
  for (mon = 0; mon < 12; mon++) {
    if (!memcmp(months + mon * 3, month, 3)) {
      break;
    }
  }
  if (mon == 12) {
    return -11;
  }
  if (year < 1970 || year > 2039) {
    return -12;
  }
  if (hour < 0 || hour >= 24) {
    return -13;
  }
  if (min < 0 || min >= 60) {
    return -14;
  }
  if (sec < 0 || sec >= 60) {
    return -15;
  }
  if (strcmp(tz, "GMT")) {
    return -16;
  }
  int d = (year - 1970) * 365 + ((year - 1969) >> 2) + (day - 1);
  if (!(year & 3) && mon >= 2) {
    d++;
  }
  dd[1] = 28;
  for (i = 0; i < mon; i++) {
    d += dd[i];
  }
  *time = (((d * 24 + hour) * 60 + min) * 60) + sec;
  return 0;
}

char *cur_http_date() {
  if (now_date_utime != now) {
    gen_http_date(now_date_string, now_date_utime = now);
  }
  return now_date_string;
}

int get_http_header(const char *qHeaders, const int qHeadersLen, char *buffer, int b_len, const char *arg_name, const int arg_len) {
  const char *where = qHeaders;
  const char *where_end = where + qHeadersLen;
  while (where < where_end) {
    const char *start = where;
    while (where < where_end && (*where != ':' && *where != '\n')) {
      ++where;
    }
    if (where == where_end) {
      buffer[0] = 0;
      return -1;
    }
    if (*where == ':') {
      if (arg_len == where - start && !strncasecmp(arg_name, start, arg_len)) {
        where++;
        while (where < where_end && (*where == 9 || *where == 32)) {
          where++;
        }
        start = where;
        while (where < where_end && *where != '\r' && *where != '\n') {
          ++where;
        }
        while (where > start && (where[-1] == ' ' || where[-1] == 9)) {
          where--;
        }
        b_len--;
        if (where - start < b_len) {
          b_len = where - start;
        }
        memcpy(buffer, start, b_len);
        buffer[b_len] = 0;
        return b_len;
      }
      ++where;
    }
    while (where < where_end && *where != '\n') {
      ++where;
    }
    if (where < where_end) {
      ++where;
    }
  }
  buffer[0] = 0;
  return -1;
}

static char header_pattern[] = "HTTP/1.1 %d %s\r\n"
                               "Server: " SERVER_VERSION "\r\n"
                               "Date: %s\r\n"
                               "Content-Type: %.256s\r\n"
                               "Connection: %s\r\n%.1024s%.4096s";

int write_basic_http_header(struct connection *c, int code, int date, int len, const char *add_header, const char *content_type) {
  struct hts_data *D = HTS_DATA(c);

  if (D->http_ver >= HTTP_V10 || D->http_ver == 0) {
    const size_t buff_size = 4096;
    static char buff[buff_size], date_buff[32];
    char *ptr = buff;
    const char *error_message = http_get_error_msg_text(&code);
    if (date) {
      gen_http_date(date_buff, date);
    }
    ptr += snprintf(ptr, buff_size - 64, header_pattern, code, error_message, date ? date_buff : cur_http_date(), content_type ? content_type : "text/html",
                    (D->query_flags & QF_KEEPALIVE) ? "keep-alive" : "close",
                    (D->query_flags & QF_EXTRA_HEADERS) && extra_http_response_headers ? extra_http_response_headers : "", add_header ?: "");
    D->query_flags &= ~QF_EXTRA_HEADERS;
    assert(ptr < buff + buff_size - 64);
    if (len >= 0) {
      ptr += snprintf(ptr, 30, "Content-Length: %d\r\n", len);
    }

    ptr += snprintf(ptr, 3, "\r\n");

    return write_out(&c->Out, buff, ptr - buff);
  }

  return 0;
}

/*
 *
 *		END (HTTP SERVER)
 *
 */
