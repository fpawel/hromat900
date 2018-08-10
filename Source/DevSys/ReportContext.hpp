#ifndef MY_HROMAT_TRANSFER_REPORT_CONTEXT_INDEXES_HEADER_DEFINED______
#define MY_HROMAT_TRANSFER_REPORT_CONTEXT_INDEXES_HEADER_DEFINED______

/**
*		контекст отчёта о состоянии менеджера трансферов
*/
namespace RPT_TRNSFR_CNTXT
{
    enum
    {
	    BEG,        // старт обработки трансфера
        END,        // окончание обработки трансфера
        ITFC,       // изменилось состояние канала связи ( установлено/оборвано соединение )
        ERR,        // ошибка
        START_THREAD,
        STOP_THREAD,
        THREAD_OK,
        HROMAT_BUSY,
        COUNT
    };
    const char* const caption[COUNT] =
    {
        "Старт приёмопередачи",
        "Окончание приёмопередачи",
        "Канал связи",
        "Ошибка ",
        "Старт опроса",
        "Окончание опроса",
        " ",
        "Хроматограф занят"
    };
};

#endif