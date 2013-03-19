#ifndef QDEBUGSTREAM_H
#define QDEBUGSTREAM_H

#include <iostream>
#include <streambuf>
#include <string>

#include "qtextedit.h"

class QOutStream : public std::basic_streambuf<char>
{
public:
    QOutStream(std::ostream &stream, QTextEdit* text_edit,
               const QColor& _color = QColor(0,0,0))
        : m_stream(stream)
    {
        color = _color;
        log_window = text_edit;
        m_old_buf = stream.rdbuf();
        stream.rdbuf(this);
    }
    ~QOutStream()
    {
        // output anything that is left
        if (!m_string.empty())
            log_window->append(m_string.c_str());

        m_stream.rdbuf(m_old_buf);
    }

protected:
    virtual int_type overflow(int_type v)
    {
        if (v == '\n')
        {
            log_window->append(m_string.c_str());
            m_string.erase(m_string.begin(), m_string.end());
        }
        else
            m_string += v;

        return v;
    }

    virtual std::streamsize xsputn(const char *p, std::streamsize n)
    {
        log_window->setTextColor(color);

        m_string.append(p, p + n);

        int pos = 0;
        while (pos != (int)std::string::npos)
        {
            pos = m_string.find('\n');
            if (pos != (int)std::string::npos)
            {
                std::string tmp(m_string.begin(), m_string.begin() + pos);
                log_window->append(tmp.c_str());
                m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
            }
        }

        return n;
    }

private:
    std::ostream &m_stream;
    std::streambuf *m_old_buf;
    std::string m_string;

    QTextEdit* log_window;
    QColor color;
};

#endif
