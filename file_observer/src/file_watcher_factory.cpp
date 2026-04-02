/// @file
/// @brief Реализация фабрики watcher'ов
/// @author Artemenko Anton

#include <file_watcher_factory.hpp>
#include <src/polling_file_watcher.hpp>

namespace file_observer
{
template <>
std::unique_ptr<IFileWatcher> CreateFileWatcher<PollingWatcherTag>(std::shared_ptr<logger::ILogger> logger)
{
    constexpr int kDefaultIntervalMs = 1000;
    return std::make_unique<PollingFileWatcher>(kDefaultIntervalMs, std::move(logger));
}

}  // namespace file_observer