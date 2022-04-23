#include "Error.h"

Error::Error(const QString &text,
             const bool isCritical)
    : m_text      {text},
      m_isCritical{isCritical}
{
    
}

const QString &Error::getText() const
{
    return m_text;
}

const bool Error::isCritical() const
{
    return m_isCritical;
}

const bool Error::isValid() const
{
    if (m_text.isEmpty()) return false;
    
    return true;
}
