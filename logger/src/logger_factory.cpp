/// @file
/// @brief Определение фабричных функций singleton-логгеров
/// @author Artemenko Anton

#include <app_logger.hpp>
#include <logger_factory.hpp>
#include <observer_logger.hpp>

namespace logger
{
namespace
{
template <typename TLoggerImpl>
std::shared_ptr<ILogger> CreateAndConfigureLogger(const QString& logPath)
{
    auto instance = std::make_shared<TLoggerImpl>();
    instance->SetSettings(LoggerSettings(logPath, LogLevel::Debug, LogOutput::Console));
    return instance;
}
}  // namespace

template <>
std::shared_ptr<ILogger> GetLogger<AppLoggerTag>()
{
    static std::shared_ptr<ILogger> logger = [] { return CreateAndConfigureLogger<AppLogger>("logs/app.log"); }();

    return logger;
}

template <>
std::shared_ptr<ILogger> GetLogger<ObserverLoggerTag>()
{
    static std::shared_ptr<ILogger> logger = [] {
        return CreateAndConfigureLogger<ObserverLogger>("logs/observer.log");
    }();

    return logger;
}

}  // namespace logger
