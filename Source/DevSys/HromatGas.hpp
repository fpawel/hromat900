//------------------------------------------------------------------------------
#ifndef HROMAT_GAS_HEADER_INCLUDED_______
#define HROMAT_GAS_HEADER_INCLUDED_______
//------------------------------------------------------------------------------
#define VCL_IOSTREAM
#include <iostream>
#include <system.hpp>
#include <inifiles.hpp>


TIniFile* IniGas();

//------------------------------------------------------------------------------
/**
*       ������������ � ������ ���������� �����������
*/
//------------------------------------------------------------------------------
namespace Gas
{
		
//------------------------------------------------------------------------------
    struct Item
    {
	    explicit Item
        (
    	    double h0_,double h1_,
   		    double M_,
   		    double Upgs0_,double Upgs1_,
   		    double Ux0_,double Ux1_,
            double sqrtb_,
            double Uh0_,
            double min_, double max_,
            const AnsiString& name_,
            const AnsiString& caption_,
            int captionPriority_,
            int sensorId_
        );
        Item( const Item& obj );
        Item operator=( const Item& obj );
        double
	        //��������� �������� ������� �������� ���������� (������/������)
            h1, h0,
            //�������� ����� ����������
   	        M,
            //������������ ����������� ���������� ��������������� ����������
   	        Upgs0, Upgs1,
            //������������ ����������� ���������� ��������������� ����������� ���������
   	        Ux0, Ux1,
            //����������� b
	        sqrtb,
            Uh0,
            min, max, sensorId;

            AnsiString name, caption ;
            int reportnum;

        AnsiString Format1() const
        {
            return caption.IsEmpty() ? name : AnsiString().sprintf(
                "%s (%s)", caption, name);
        }
        AnsiString FormatOrders() const
        {
            return AnsiString().sprintf("%g-%g", min, max );
        }

    }; // struct Item
//------------------------------------------------------------------------------
    
    unsigned Count();                   // ���������� �����������
    unsigned GrauatorsCount();          // ���������� ���������� �����������
    AnsiString Formula(unsigned);       // ������� �����������    
    const Item* GetItems();
    int GetIdx(const AnsiString& formula);              // ������� ��������� �� ��� �����
    int GetReportOrder(const AnsiString& formula);      // ������� ��������� � ������ �� ��� �����

    const Item* GetSortedItems();

    bool comp(const AnsiString& l, const AnsiString& r) {
            int i1 = Gas::GetIdx(l), i2 = Gas::GetIdx(r);
            if(i1==-1 || i2==-1) return true;
            const Gas::Item &it1 = Gas::GetItems()[i1], &it2 = Gas::GetItems()[i2];
            return it1.reportnum<it2.reportnum;
        }

    struct Comp{
        bool operator()(const AnsiString& l, const AnsiString& r) {

            return comp(l,r);
        }
    } ;

}; // namespace Gas
//------------------------------------------------------------------------------

#endif