/// @file
/// @brief Фабрика для создания IFileWatcher
/// @author Artemenko Anton

#ifndef GUID_7a1f3c52_91d4_4f0b_8c2e_abc123456789
#define GUID_7a1f3c52_91d4_4f0b_8c2e_abc123456789

#include <ifile_watcher.hpp>
#include <memory>

namespace logger
{
class ILogger;
}

namespace file_observer
{

/// @brief Тег polling-реализации
struct PollingWatcherTag;

/// @brief Фабрика создания watcher'ов
/// @tparam TWatcherTag Тег типа watcher'а
/// @param[in] logger указатель на логгер
/// @return Уникальный указатель на watcher
template <typename TWatcherTag>
std::unique_ptr<IFileWatcher> CreateFileWatcher(int kDefaultIntervalMs, std::shared_ptr<logger::ILogger> logger);

/// @brief Специализация для polling watcher
/// @param[in] logger указатель на логгер
/// @return Уникальный указатель на watcher
template <>
std::unique_ptr<IFileWatcher> CreateFileWatcher<PollingWatcherTag>(int kDefaultIntervalMs,
                                                                   std::shared_ptr<logger::ILogger> logger);
}  // namespace file_observer

#endif  // GUID_7a1f3c52_91d4_4f0b_8c2e_abc123456789