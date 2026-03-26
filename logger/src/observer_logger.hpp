/// @file
/// @brief Объявление логгера наблюдения за файлами
/// @author Artemenko Anton
#ifndef GUID_0710a4e6_d340_4b8f_8a3e_1134f49087ee
#define GUID_0710a4e6_d340_4b8f_8a3e_1134f49087ee

#include <thread_safe_logger.hpp>

namespace logger
{
/// @brief Логгер состояния наблюдения файлов
class ObserverLogger final : public ThreadSafeLogger
{
   public:
    /// @brief Конструктор логгера наблюдения
    /// @param[in] output Режим вывода логов
    explicit ObserverLogger(LogOutput output = LogOutput::Console);

    /// @brief Деструктор логгера наблюдения
    ~ObserverLogger() override;

   private:
    /// @brief Сформировать сообщение для записи
    /// @param[in] level Уровень логирования
    /// @param[in] message Текст сообщения
    /// @param[in] file Имя файла
    /// @param[in] line Номер строки
    /// @param[in] function Имя функции
    /// @return Отформатированное сообщение
    QString FormatMessage(LogLevel level, const QString& message, const char* file, int line,
                          const char* function) const override;
};

}  // namespace logger

#endif  // GUID_0710a4e6_d340_4b8f_8a3e_1134f49087ee
