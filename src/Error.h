#ifndef ERROR_H
#define ERROR_H

#include <QObject>
#include <QString>

class Error
{
    Q_GADGET
    Q_PROPERTY(QString text READ getText);
    Q_PROPERTY(bool isCritical READ isCritical);
    
public:
    Error(const QString &text = QString(),
          const bool isCritical = false);
    
    const QString& getText   () const;
    const bool     isCritical() const;
    const bool     isValid   () const;
    
private:
    QString m_text;
    bool    m_isCritical;
};

//Q_DECLARE_METATYPE(Error)

#endif // ERROR_H
