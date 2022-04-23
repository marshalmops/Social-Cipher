#include "SettingsBase.h"

QString SettingsBase::getSettingsDir() const
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
}

bool SettingsBase::getQtSettingsObject(std::unique_ptr<QSettings> &settings) const
{
    auto settingsPath = getSettingsPath();
    
    if (settingsPath.isEmpty()) return false;
    
    settings = std::make_unique<QSettings>(settingsPath, QSettings::IniFormat);
    
    return true;
}

//bool SettingsBase::writeToSettings(const QString  &paramName,
//                                   const QVariant &paramValue)
//{
//    auto settingsPath = getSettingsPath();
    
//    if (settingsPath.isEmpty()) return false;
    
//    QSettings settings{settingsPath};
    
//    settings.setValue(paramName, paramValue);
//    settings.sync();
    
//    return true;
//}

//bool SettingsBase::readFromSettings(const QString &paramName,
//                                    QVariant      &paramValue)
//{
//    auto settingsPath = getSettingsPath();
    
//    if (settingsPath.isEmpty()) return false;
    
//    QSettings settings{settingsPath};
    
//    paramValue = settings.value(paramName);
    
//    if (!paramValue.isValid()) return false;
    
//    return true;
//}
