#pragma once

//https://github.com/lordoffox/aredis/blob/master/aredis.hpp

#ifdef WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

namespace aredis
{
  struct socket_init
  {
    socket_init()
    {
      WSADATA wsaData;
      WSAStartup(MAKEWORD(2, 2), &wsaData);
    }
    ~socket_init()
    {
      WSACleanup();
    }
  };
  typedef SOCKET socket_type;
  void socket_close(socket_type fd)
  {
    closesocket(fd);
  }
}

#else
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>

namespace aredis
{
  struct socket_init{};
  typedef int socket_type;
  void socket_close(socket_type fd)
  {
    close(fd);
  }
}

#endif

#include <stdlib.h>
#include <cstdint>
#include <string>
#include <vector>

namespace aredis
{
  inline void init_socket()
  {
    static socket_init sinit;
  }

  enum resp_value_type
  {
    rrt_nil,
    rrt_error_info,
    rrt_integer_value,
    rrt_string_value,
    rrt_array_value,
  };

  enum resp_state
  {
    rs_type_read,
    rs_error_read,
    rs_error_cr,
    rs_integer_read,
    rs_integer_cr,
    rs_string_read,
    rs_string_cr,
    rs_bulk_len,
    rs_bulk_len_cr,
    rs_bulk_read,
    rs_array_read,
    rs_array_cr,
    rs_done,
  };

  struct resp_value
  {
    resp_value_type type = rrt_nil;
    union uvalue
    {
      int64_t ivalue;
      const char *  svalue;
    }values;
    int32_t len = 0;

    inline bool is_nil(){ return type == rrt_nil; }

    inline bool is_ok()
    {
      if (type == rrt_string_value && len == 2)
      {
        if (values.svalue[0] == 'O' && values.svalue[1] == 'K')
        {
          return true;
        }
      }
      return false;
    }
  };

  enum redis_code
  {
    rc_ok,
    rc_incomplete,
    rc_server_init_error,
    rc_server_disconnect,
    rc_server_connect_fail,
    rc_error_endpoint,
    rc_auth_fail,
    rc_select_db_fail,
    rc_command_error,
    rc_resp_invalid,
  };

  inline size_t i64toa(int64_t n, char * buff, size_t len)
  {
    bool sign = false;
    if (n < 0)
    {
      n = -n;
      sign = true;
    }
    size_t i = len;
    for (; n > 0 && i > 0; --i)
    {
      lldiv_t d = div(n, 10ll);
      n = d.quot;
      buff[i - 1] = (char)d.rem + '0';
    }
    if (sign)
    {
      --i;
      buff[i] = '-';
    }
    size_t slen = len - i;
    if (i > 0)
    {
      memcpy(buff, buff + i, len - i);
    }
    buff[slen] = 0;
    return slen;
  }

  struct resp_result
  {
    enum{ default_array_size = 16 };
    size_t size = 1;
    redis_code error = rc_ok;
    resp_value vals[default_array_size];
    std::vector<resp_value> dvals;
    std::string error_msg;
    resp_result()
    {
      memset(&vals[0], 0, sizeof(vals));
    }

    ~resp_result()
    {
    }

    inline void reset()
    {
      error = rc_ok;
      error_msg.clear();
      memset(&vals[0], 0, sizeof(vals));
      dvals.clear();
      size = 1;
    }

    inline resp_result& operator = (resp_result& val)
    {
      size = val.size;
      error = val.error;
      memcpy(&vals[0], &val.vals[0], sizeof(vals));
      error_msg = val.error_msg;
      dvals = val.dvals;
      return *this;
    }

    inline resp_value& value(size_t idx)
    {
      if (idx >= size)
      {
        throw std::out_of_range("resp array out of range");
      }
      if (size > default_array_size)
      {        
        return dvals[idx];
      }
      return vals[idx];
    }

    inline resp_value const& value(size_t idx) const
    {
      if (idx >= size)
      {
        throw std::out_of_range("resp array out of range");
      }
      if (size > default_array_size)
      {
        return dvals[idx];
      }
      return vals[idx];
    }

    inline std::string dump()
    {
      char ltoa[21];
      i64toa(size, ltoa, 21);
      std::string str;
      str.append(ltoa)
        .append(1, '\n');
      for (size_t i = 0; i < size; ++i)
      {
        auto& val = value(i);
        switch (val.type)
        {
        case rrt_nil:
        {
          str.append("nil\n");
          break;
        }
        case rrt_error_info:
        {
          str.append("error:")
            .append(error_msg)
            .append(1, '\n');
          break;
        }
        case rrt_integer_value:
        {
          i64toa(val.values.ivalue, ltoa, 21);
          str.append(ltoa)
            .append(1, '\n');
          break;
        }
        case rrt_string_value:
        {
          str.append(val.values.svalue, val.len)
            .append(1, '\n');
          break;
        }
        }
      }
      return str;
    }
  };

  struct resp_parser
  {
    enum integer_sign
    {
      is_init,
      is_pos,
      is_neg
    };
    std::string buff;
    bool is_array = false;
    integer_sign isign = is_init;
    size_t want_count = 1;
    size_t read_count = 0;
    size_t read_pos = 0;
    resp_state current_state = rs_type_read;
    resp_result res;

    inline void data(const char * data, size_t len)
    {
      buff.append(data, len);
    }

    inline void clear()
    {
      reset();
      buff.clear();
    }

    inline resp_value& current_value()
    {
      if (res.size <= resp_result::default_array_size)
      {
        return res.vals[read_count];
      }
      return res.dvals[read_count];
    }

    inline redis_code parser_value_end()
    {
      ++read_count;
      if (read_count == want_count)
      {
        current_state = rs_done;
        for (size_t i = 0; i < res.size; ++i)
        {
          auto& val = res.value(i);
          if (val.type == rrt_string_value)
          {
            val.values.svalue = buff.data() + val.values.ivalue;
          }
        }
      }
      else
      {
        current_state = rs_type_read;
      }
      return rc_ok;
    }

    inline redis_code parser_simple_string_end()
    {
      if (read_pos < buff.size())
      {
        if (buff[read_pos++] == '\n')
        {
          auto& val = current_value();
          val.len = read_pos - (size_t)val.values.ivalue - 2;
          return parser_value_end();
        }
        else
        {
          return parser_simple_string();
        }
      }
      return rc_incomplete;
    }

    inline redis_code parser_simple_string()
    {
      for (; read_pos < buff.size();)
      {
        if (buff[read_pos++] == '\r')
        {
          current_state = rs_string_cr;
          return parser_simple_string_end();
        }
      }
      return rc_incomplete;
    }

    inline redis_code parser_error_string_end()
    {
      if (read_pos < buff.size())
      {
        if (buff[read_pos++] == '\n')
        {
          auto& val = current_value();
          val.len = read_pos - (size_t)val.values.ivalue - 2;
          res.error_msg.assign(buff.data() + val.values.ivalue, val.len);
          return parser_value_end();
        }
        else
        {
          return parser_error_string();
        }
      }
      return rc_incomplete;
    }

    inline redis_code parser_error_string()
    {
      for (; read_pos < buff.size();)
      {
        if (buff[read_pos++] == '\r')
        {
          res.error = rc_command_error;
          current_state = rs_error_cr;
          return parser_error_string_end();
        }
      }
      return rc_incomplete;
    }

    inline redis_code parser_integer_end()
    {
      if (read_pos < buff.size())
      {
        if (buff[read_pos++] == '\n')
        {
          if (isign == is_neg)
          {
            auto& val = current_value();
            val.values.ivalue = -val.values.ivalue;
          }
          isign = is_init;
          return parser_value_end();
        }
      }
      return rc_incomplete;
    }

    inline redis_code parser_integer()
    {
      for (; read_pos < buff.size();)
      {
        char c = buff[read_pos++];
        switch (c)
        {
        case '-':
        {
          if (isign == is_init)
          {
            isign = is_neg;
            break;
          }
          return rc_resp_invalid;
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
          if (isign == is_init)
          {
            isign = is_pos;
            break;
          }
          auto& val = current_value();
          val.values.ivalue *= 10;
          val.values.ivalue += (c - '0');
          break;
        }
        case '\r':
        {
          current_state = rs_integer_cr;
          return parser_integer_end();
        }
        default:
        {
          return rc_resp_invalid;
        }
        }
      }
      return rc_incomplete;
    }

    inline redis_code parser_bulk_string()
    {
      size_t remain = buff.size() - read_pos;
      auto& val = current_value();
      int32_t len = val.len;
      if (len < 0)
      {
        val.len = 0;
        val.type = rrt_nil;
        return parser_value_end();
      }
      if (remain >= (size_t)len)
      {
        if (buff[read_pos + len] == '\r' && buff[read_pos + len + 1] == '\n')
        {
          read_pos += (len + 2);
          return parser_value_end();
        }
        return rc_resp_invalid;
      }
      return rc_incomplete;
    }

    inline redis_code parser_bulk_end()
    {
      if (read_pos < buff.size())
      {
        if (buff[read_pos++] == '\n')
        {
          current_state = rs_bulk_read;
          current_value().values.ivalue = read_pos;
          return parser_bulk_string();
        }
      }
      return rc_incomplete;
    }

    inline redis_code parser_bulk_len()
    {
      for (; read_pos < buff.size();)
      {
        char c = buff[read_pos++];
        switch (c)
        {
        case '-':
        {
          auto& val = current_value();
          val.len = -1;
          break;
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
          auto& val = current_value();
          if (val.len >= 0)
          {
            val.len *= 10;
            val.len += (c - '0');
          }
          break;
        }
        case '\r':
        {
          current_state = rs_bulk_len_cr;
          return parser_bulk_end();
        }
        default:
        {
          return rc_resp_invalid;
        }
        }
      }
      return rc_incomplete;
    }

    inline redis_code parser_array_end()
    {
      if (read_pos < buff.size())
      {
        if (buff[read_pos++] == '\n')
        {
          current_state = rs_type_read;
          return rc_ok;
        }
      }
      return rc_incomplete;
    }

    inline redis_code parser_array_len()
    {
      for (; read_pos < buff.size();)
      {
        char c = buff[read_pos++];
        switch (c)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
          auto& val = current_value();
          val.len *= 10;
          val.len += (c - '0');
          break;
        }
        case '\r':
        {
          current_state = rs_array_cr;
          want_count = current_value().len;
          res.size = want_count;
          if (res.size > resp_result::default_array_size)
          {
            res.dvals.resize(res.size);
          }
          return parser_array_end();
        }
        default:
        {
          res.error_msg = "invalid resp";
          return rc_resp_invalid;
        }
        }
      }
      return rc_incomplete;
    }

    inline redis_code parser_type()
    {
      if (read_pos >= buff.size())
      {
        return rc_incomplete;
      }
      char ty = buff[read_pos++];
      switch (ty)
      {
      case '+':
      {
        current_state = rs_string_read;
        auto& val = current_value();
        val.type = rrt_string_value;
        val.values.ivalue = read_pos;
        return parser_simple_string();
      }
      case '-':
      {
        current_state = rs_error_read;
        auto& val = current_value();
        val.type = rrt_error_info;
        val.values.ivalue = read_pos;
        return parser_error_string();
      }
      case ':':
      {
        auto& val = current_value();
        val.type = rrt_integer_value;
        current_state = rs_integer_read;
        return parser_integer();
      }
      case '$':
      {
        current_state = rs_bulk_len;
        auto& val = current_value();
        val.type = rrt_string_value;
        return parser_bulk_len();
      }
      case '*':
      {
        if (is_array)
        {
          res.error_msg = "invalid resp";
          return rc_resp_invalid;
        }
        is_array = true;
        current_state = rs_array_read;
        return parser_array_len();
      }
      }
      res.error_msg = "invalid resp";
      return rc_resp_invalid;
    }

    inline void reset()
    {
      buff.erase(0, read_pos);
      is_array = false;
      want_count = 1;
      read_count = 0;
      read_pos = 0;
      current_state = rs_type_read;
      res.reset();
    }

    inline redis_code parser()
    {
      if (current_state == rs_done)
      {
        reset();
      }
      while (want_count > read_count)
      {
        redis_code rc;
        switch (current_state)
        {
        case rs_type_read:
        {
          rc = parser_type();
          break;
        }
        case rs_error_read:
        {
          rc = parser_error_string();
          break;
        }
        case rs_error_cr:
        {
          rc = parser_error_string_end();
          break;
        }
        case rs_integer_read:
        {
          rc = parser_integer();
          break;
        }
        case rs_integer_cr:
        {
          rc = parser_integer_end();
          break;
        }
        case rs_string_read:
        {
          rc = parser_simple_string();
          break;
        }
        case rs_string_cr:
        {
          rc = parser_simple_string_end();
          break;
        }
        case rs_bulk_len:
        {
          rc = parser_bulk_len();
          break;
        }
        case rs_bulk_len_cr:
        {
          rc = parser_bulk_end();
          break;
        }
        case rs_bulk_read:
        {
          rc = parser_bulk_string();
          break;
        }
        case rs_array_read:
        {
          rc = parser_array_len();
          break;
        }
        case rs_array_cr:
        {
          rc = parser_array_end();
          break;
        }
        case rs_done:
        {
          break;
        }
        default:
        {
          res.error = rc_resp_invalid;
          res.error_msg = "invalid resp";
          return rc_resp_invalid;
        }
        }
        if (rc != rc_ok)
        {
          res.error = rc;
          return rc;
        }
        if (current_state == rs_done)
        {
          res.error = rc_ok;
          break;
        }
      };
      return rc_ok;
    }
  };

  struct redis_command
  {
    enum{ head_size = 20 };
    uint32_t arg_count = 0;
    size_t head_pos = head_size;
    char ltoa_buff[21];
    std::string buff;

    redis_command()
    {
      buff.reserve(2048);
      buff.resize(head_size);
    }

    inline void reset()
    {
      arg_count = 0;
      head_pos = head_size;
      buff.resize(head_size);
    }

    inline redis_command& arg(const char * val , size_t len = 0)
    {
      ++arg_count;
      if (val == nullptr)
      {
        buff.append("$0\r\n\r\n");
      }
      else
      {
        if (len == 0)
        {
          len = std::strlen(val);
        }
        i64toa(len, ltoa_buff, 21);
        buff.append(1, '$')
          .append(ltoa_buff)
          .append("\r\n")
          .append(val)
          .append("\r\n");
      }
      return *this;
    }

    inline redis_command& arg(int64_t val)
    {
      std::string v = std::to_string(val);
      return arg(v.data(), v.length());
    }

    inline redis_command& arg(std::string const& val)
    {      
      return arg(val.data(),val.length());
    }

    inline void end()
    {
      if (arg_count > 0)
      {
        size_t slen = i64toa(arg_count, ltoa_buff, 21);
        size_t pos = head_size - slen - 3;
        head_pos = pos;
        buff[pos++] = '*';
        for (size_t i = 0; i < slen; ++i)
        {
          buff[pos++] = ltoa_buff[i];
        }
        buff[pos++] = '\r';
        buff[pos++] = '\n';
      }
    }

    inline redis_command& args()
    {
      return *this;
    }

    template<typename head, typename ...targs>
    inline redis_command& args(head const& h, targs const&... targs_)
    {
      arg(h);
      return args(targs_...);
    }

    template<typename ...targs>
    inline redis_command& cmd(const char * cmd, targs const&... targs_)
    {
      reset();
      arg(cmd);
      return args(targs_...);
    }

    template<typename ...targs>
    inline redis_command& cmd(std::string const& cmd, targs const&... targs_)
    {
      reset();
      arg(cmd.c_str());
      return args(targs_...);
    }

    inline std::string dump()
    {
      std::string str;
      str.reserve(buff.length() * 2);
      for (size_t i = head_pos; i < buff.length(); ++i)
      {
        char c = buff[i];
        if (c == '\r')
        {
          str.append(1, '\\');
          str.append(1, 'r');
        }
        else if (c == '\n')
        {
          str.append(1, '\\');
          str.append(1, 'n');
        }
        else
        {
          str.append(1, c);
        }
      }
      return str;
    }
  };

  struct batch_command
  {
    std::string buff;
    size_t count = 0;
    inline bool add(redis_command& cmd)
    {
      
      if (cmd.arg_count > 0)
      {
        ++count;
        cmd.end();
        buff.append(cmd.buff.data() + cmd.head_pos,
          cmd.buff.length() - cmd.head_pos);
        return true;
      }
      return false;
    }

    inline void clear()
    {
      buff.clear();
      count = 0;
    }
  };

  struct redis_conn
  {
    redis_conn()
    {
      init_socket();
    }

    ~redis_conn()
    {
      close();
    }

    inline bool connect(const char * host = "127.0.0.1" , int port = 6379 , char * password = nullptr , int db = 0)
    {
      if (retry > 3) retry = 3;
      this->retry = retry;
      char int_buff[21];
      i64toa(port, int_buff, 21);
      addrinfo *result = nullptr;
      addrinfo hints;
      memset(&hints, 0, sizeof(hints));
      hints.ai_family = 0;
      hints.ai_flags = 0;
      hints.ai_protocol = IPPROTO_TCP;
      hints.ai_socktype = SOCK_STREAM;
      int ret = getaddrinfo(host, int_buff, &hints, &result);
      if (ret != 0)
      {
        parser.res.error = rc_error_endpoint;
        parser.res.error_msg = "endpoint error";
        return false;
      }
      addrinfo *ptr = result;
      for (; ptr != nullptr;ptr = ptr->ai_next)
      {
        if (ptr->ai_family == AF_INET || ptr->ai_family == AF_INET6)
          break;
      }
      if (ptr == nullptr)
      {
        parser.res.error = rc_error_endpoint;
        parser.res.error_msg = "endpoint error";
        return false;
      }
      memcpy((char *)&serv_addr, (char*)ptr->ai_addr, ptr->ai_addrlen);
      auto_connect_buff.clear();
      if (password != nullptr && password[0] != 0)
      {
        do_auth = true;
        int len = std::strlen(password);
        i64toa(len, int_buff, 21);
        auto_connect_buff.append("*2\r\n$4\r\nauth\r\n$")
          .append(int_buff)
          .append("\r\n")
          .append(password)
          .append("\r\n");
      }
      if (db > 0)
      {
        do_switch = true;
        int len = i64toa(db, int_buff, 21);
        std::string slen = std::to_string(len);
        auto_connect_buff.append("*2\r\n$6\r\nselect\r\n$")
          .append(slen)
          .append("\r\n")
          .append(int_buff)
          .append("\r\n");
      }
      return connect_impl();
    }

    inline bool reply(resp_result& res)
    {
      return get_reply(res);
    }

    inline bool check_conn()
    {
      if (init == false)
      {
        parser.res.error = rc_server_init_error;
        parser.res.error_msg = "server init error";
        return false;
      }
      if (sockfd == 0)
      {
        if (connect_impl() == false)
        {
          return false;
        }
      }
      return true;
    }

    inline bool command(redis_command& cmd)
    {
      if (check_conn() == false)
      {
        return false;
      }
      if (cmd.arg_count == 0)
      {
        parser.res.error = rc_command_error;
        parser.res.error_msg = "command error";
        return false;
      }
      cmd.end();
      size_t do_count = retry;
      int ret = ::send(sockfd, cmd.buff.data() + cmd.head_pos,
        cmd.buff.length() - cmd.head_pos, 0);
      if (ret < 0)
      {
        parser.res.error = rc_server_disconnect;
        parser.res.error_msg = "server disconnect";
        close();
        return false;
      }
      return true;
    }

    inline bool command(batch_command const& cmd)
    {
      if (check_conn() == false)
      {
        return false;
      }
      if (cmd.buff.empty())
      {
        parser.res.error = rc_command_error;
        parser.res.error_msg = "command error";
        return false;
      }
      size_t do_count = retry;
      int ret = ::send(sockfd, cmd.buff.data(), cmd.buff.length(), 0);
      if (ret < 0)
      {
        parser.res.error = rc_server_disconnect;
        parser.res.error_msg = "server disconnect";
        close();
        return false;
      }
      return true;
    }

    inline redis_code error() const{ return parser.res.error; }

    inline std::string const& error_msg() const{ return parser.res.error_msg; }

  private:
    bool do_auth = false;
    bool do_switch = false;
    bool init = false;
    size_t retry = 0;
    socket_type sockfd = 0;
    sockaddr serv_addr;
    resp_parser parser;
    std::string auto_connect_buff;

    inline void close()
    {
      if (sockfd)
      {
        socket_close(sockfd);
        sockfd = 0;
      }
    }

    inline bool connect_impl()
    {
      close();
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (::connect(sockfd, &serv_addr, sizeof(serv_addr)) < 0)
      {
        parser.res.error = rc_server_connect_fail;
        parser.res.error_msg = "could not connect to server";
        close();
        return false;
      }
      if (!auto_connect_buff.empty())
      {
        int ret = ::send(sockfd, auto_connect_buff.data(), auto_connect_buff.length(), 0);
        if (ret < 0)
        {
          parser.res.error = rc_server_disconnect;
          parser.res.error_msg = "server disconnect";
          close();
          return false;
        }
        parser.clear();
        resp_result res;
        if (do_auth)
        {
          if (get_reply(res) == false)
          {
            return false;
          }
          if (!res.value(0).is_ok())
          {
            parser.res.error = rc_auth_fail;
            parser.res.error_msg = "auth failed";
            return false;
          }
        }
        if (do_switch)
        {
          if (get_reply(res) == false)
          {
            return false;
          }
          if (!res.value(0).is_ok())
          {
            parser.res.error = rc_select_db_fail;
            parser.res.error_msg = "select db failed";
            return false;
          }
        }
      }
      init = true;
      return true;
    }

    inline bool get_reply(resp_result& res)
    {
      char buffer[65536];
      if (sockfd)
      {
        parser.reset();
        do 
        {
          int ret = ::recv(sockfd, buffer, 65536, 0);
          if (ret <= 0)
          {
            close();
            parser.res.error = rc_server_disconnect;
            parser.res.error_msg = "server disconnect";
            break;
          }
          else
          {
            parser.data(buffer, ret);
            redis_code rc = parser.parser();
            if (rc == rc_incomplete)
            {
              continue;
            }
            parser.res.error = rc;
            if (rc == rc_ok)
            {
              res = parser.res;
              return true;
            }
          }
        } while (0);
      }
      res.error = parser.res.error;
      res.error_msg = parser.res.error_msg;
      res.size = 0;
      return false;
    }
  };
}
