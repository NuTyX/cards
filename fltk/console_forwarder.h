#ifndef  CONSOLE_FORWARDER_H
#define  CONSOLE_FORWARDER_H

#include <streambuf>
#include <iostream>
#include <ostream>

template <class Elem = char, class Tr = std::char_traits<Elem> >
class console_forwarder : public std::basic_streambuf<Elem, Tr>
{
    typedef void (*pfncb)(const Elem *, std::streamsize _Count);

protected:
    std::basic_ostream<Elem, Tr>  &m_stream;
    std::streambuf                *m_buf;
    pfncb                         m_cb;

public:
    console_forwarder(std::ostream &stream, pfncb cb)
        : m_stream(stream), m_cb(cb)
    {
        // redirect stream
        m_buf = m_stream.rdbuf(this);
    };

    ~console_forwarder()
    {
        // restore stream
        m_stream.rdbuf(m_buf);
    }

    // override xsputn and make it forward data to the callback function
    std::streamsize xsputn(const Elem *_Ptr, std::streamsize _Count)
    {
        m_cb(_Ptr, _Count);
        return _Count;
    }

    // override overflow and make it forward data to the callback function
    typename Tr::int_type overflow(typename Tr::int_type v)
    {
        Elem ch = Tr::to_char_type(v);
        m_cb(&ch, 1);
        return Tr::not_eof(v);
    }
};

#endif
