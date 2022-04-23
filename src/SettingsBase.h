#ifndef SETTINGSBASE_H
#define SETTINGSBASE_H

#include <QSettings>
#include <QStandardPaths>

class SettingsBase
{
public:
    SettingsBase()          = default;
    virtual ~SettingsBase() = default;
    
    virtual bool save       () const = 0;
    virtual bool load       ()       = 0;
    virtual void setDefaults()       = 0;
    
protected:
    virtual QString getSettingsPath() const = 0;
    
    QString getSettingsDir() const;
    
    bool getQtSettingsObject(std::unique_ptr<QSettings> &settings) const;
    
//    bool writeToSettings(const QString  &paramName,
//                         const QVariant &paramValue);
//    bool readFromSettings(const QString &paramName,
//                          QVariant &paramValue);
};

#endif // SETTINGSBASE_H
