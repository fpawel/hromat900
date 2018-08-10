#ifndef ANALYZE_DEFINITIONS_HEADER_INCLUDED_______
#define ANALYZE_DEFINITIONS_HEADER_INCLUDED_______
//------------------------------------------------------------------------------
#include <vector>
namespace AnalyzeHromatogram
{
    typedef struct
	{
		std::vector<std::vector<double> > conc;
    	unsigned count;
        TDateTime dateMin, dateMax;
        bool okrug;  
		
		std::vector<std::pair<TDateTime>,double> IzobutConc;
		
	} Tag;
};
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
