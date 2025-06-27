#pragma once

#include <mutex>

#include <IConfigStorage/IConfigStorage.hpp>

class AppConfig : public IConfigStorage
{
public:
    AppConfig(std::string, std::map<std::string, sdbus::Variant>);

    std::map<std::string, sdbus::Variant> getAllParameters() const override;
    
    void setParameter(const std::string&, const sdbus::Variant&) override;
    
    [[nodiscard]] inline std::string getAppName() const override
    {
        return app_name_;
    }

private:
    mutable std::mutex mutex_;
    std::string app_name_;
    std::map<std::string, sdbus::Variant> parameters_;
};
