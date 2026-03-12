/// @file
/// @brief Определение фабричных функций singleton-логгеров
/// @author Artemenko Anton

#include <logger_factory.hpp>

#include <app_logger.hpp>
#include <observer_logger.hpp>

namespace logger
{
namespace
{
LoggerOutputMode &GlobalOutputModeStorage()
{
    static LoggerOutputMode mode = LoggerOutputMode::Console;
    return mode;
}

LogOutput ToLogOutput(LoggerOutputMode mode)
{
    return mode == LoggerOutputMode::File ? LogOutput::File : LogOutput::Console;
}
} // namespace

void SetGlobalLogOutput(LoggerOutputMode mode)
{
    GlobalOutputModeStorage() = mode;
    GetAppLogger()->SetOutput(ToLogOutput(mode));
    GetObserverLogger()->SetOutput(ToLogOutput(mode));
}

LoggerOutputMode GetGlobalLogOutput()
{
    return GlobalOutputModeStorage();
}

std::shared_ptr<ILogger> GetAppLogger()
{
    static std::shared_ptr<ILogger> logger = [] {
        auto instance = std::make_shared<AppLogger>(ToLogOutput(GetGlobalLogOutput()));
        instance->SetLogFile("logs/app.log");
        return instance;
    }();

    logger->SetOutput(ToLogOutput(GetGlobalLogOutput()));

    return logger;
}

std::shared_ptr<ILogger> GetObserverLogger()
{
    static std::shared_ptr<ILogger> logger = [] {
        auto instance = std::make_shared<ObserverLogger>(ToLogOutput(GetGlobalLogOutput()));
        instance->SetLogFile("logs/observer.log");
        return instance;
    }();

    logger->SetOutput(ToLogOutput(GetGlobalLogOutput()));

    return logger;
}

} // namespace logger
