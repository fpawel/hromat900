#ifndef MY_HROMAT_TRANSFER_REPORT_CONTEXT_INDEXES_HEADER_DEFINED______
#define MY_HROMAT_TRANSFER_REPORT_CONTEXT_INDEXES_HEADER_DEFINED______

/**
*		�������� ������ � ��������� ��������� ����������
*/
namespace RPT_TRNSFR_CNTXT
{
    enum
    {
	    BEG,        // ����� ��������� ���������
        END,        // ��������� ��������� ���������
        ITFC,       // ���������� ��������� ������ ����� ( �����������/�������� ���������� )
        ERR,        // ������
        START_THREAD,
        STOP_THREAD,
        THREAD_OK,
        HROMAT_BUSY,
        COUNT
    };
    const char* const caption[COUNT] =
    {
        "����� �������������",
        "��������� �������������",
        "����� �����",
        "������ ",
        "����� ������",
        "��������� ������",
        " ",
        "����������� �����"
    };
};

#endif