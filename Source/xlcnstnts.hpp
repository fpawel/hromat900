//------------------------------------------------------------------------------
#ifndef MY_EXCEL_NAMES_HEADER_INCLUDED____
#define MY_EXCEL_NAMES_HEADER_INCLUDED____
//------------------------------------------------------------------------------
namespace Excel
{
//------------------------------------------------------------------------------
typedef enum Constants
{
  xlAll = 0xFFFFEFF8, 
  xlAutomatic = 0xFFFFEFF7, 
  xlBoth = 1, 
  xlCenter = 0xFFFFEFF4, 
  xlChecker = 9, 
  xlCircle = 8, 
  xlCorner = 2, 
  xlCrissCross = 16, 
  xlCross = 4, 
  xlDiamond = 2, 
  xlDistributed = 0xFFFFEFEB, 
  xlDoubleAccounting = 5, 
  xlFixedValue = 1, 
  xlFormats = 0xFFFFEFE6, 
  xlGray16 = 17, 
  xlGray8 = 18, 
  xlGrid = 15, 
  xlHigh = 0xFFFFEFE1, 
  xlInside = 2, 
  xlJustify = 0xFFFFEFDE, 
  xlLightDown = 13, 
  xlLightHorizontal = 11, 
  xlLightUp = 14, 
  xlLightVertical = 12, 
  xlLow = 0xFFFFEFDA, 
  xlManual = 0xFFFFEFD9, 
  xlMinusValues = 3, 
  xlModule = 0xFFFFEFD3, 
  xlNextToAxis = 4, 
  xlNone = 0xFFFFEFD2, 
  xlNotes = 0xFFFFEFD0, 
  xlOff = 0xFFFFEFCE, 
  xlOn = 1, 
  xlPercent = 2, 
  xlPlus = 9, 
  xlPlusValues = 2, 
  xlSemiGray75 = 10, 
  xlShowLabel = 4, 
  xlShowLabelAndPercent = 5, 
  xlShowPercent = 3, 
  xlShowValue = 2, 
  xlSimple = 0xFFFFEFC6, 
  xlSingle = 2, 
  xlSingleAccounting = 4, 
  xlSolid = 1, 
  xlSquare = 1, 
  xlStar = 5, 
  xlStError = 4, 
  xlToolbarButton = 2, 
  xlTriangle = 3, 
  xlGray25 = 0xFFFFEFE4, 
  xlGray50 = 0xFFFFEFE3, 
  xlGray75 = 0xFFFFEFE2, 
  xlBottom = 0xFFFFEFF5, 
  xlLeft = 0xFFFFEFDD, 
  xlRight = 0xFFFFEFC8, 
  xlTop = 0xFFFFEFC0, 
  xl3DBar = 0xFFFFEFFD, 
  xl3DSurface = 0xFFFFEFF9, 
  xlBar = 2, 
  xlColumn = 3, 
  xlCombination = 0xFFFFEFF1, 
  xlCustom = 0xFFFFEFEE, 
  xlDefaultAutoFormat = 0xFFFFFFFF, 
  xlMaximum = 2, 
  xlMinimum = 4, 
  xlOpaque = 3, 
  xlTransparent = 2, 
  xlBidi = 0xFFFFEC78, 
  xlLatin = 0xFFFFEC77, 
  xlContext = 0xFFFFEC76, 
  xlLTR = 0xFFFFEC75, 
  xlRTL = 0xFFFFEC74, 
  xlFullScript = 1, 
  xlPartialScript = 2, 
  xlMixedScript = 3, 
  xlMixedAuthorizedScript = 4, 
  xlVisualCursor = 2, 
  xlLogicalCursor = 1, 
  xlSystem = 1, 
  xlPartial = 3, 
  xlHindiNumerals = 3, 
  xlBidiCalendar = 3, 
  xlGregorian = 2, 
  xlComplete = 4, 
  xlScale = 3, 
  xlClosed = 3, 
  xlColor1 = 7, 
  xlColor2 = 8, 
  xlColor3 = 9, 
  xlConstants = 2, 
  xlContents = 2, 
  xlBelow = 1, 
  xlCascade = 7, 
  xlCenterAcrossSelection = 7, 
  xlChart4 = 2, 
  xlChartSeries = 17, 
  xlChartShort = 6, 
  xlChartTitles = 18, 
  xlClassic1 = 1, 
  xlClassic2 = 2, 
  xlClassic3 = 3, 
  xl3DEffects1 = 13, 
  xl3DEffects2 = 14, 
  xlAbove = 0, 
  xlAccounting1 = 4, 
  xlAccounting2 = 5, 
  xlAccounting3 = 6, 
  xlAccounting4 = 17, 
  xlAdd = 2, 
  xlDebugCodePane = 13, 
  xlDesktop = 9, 
  xlDirect = 1, 
  xlDivide = 5, 
  xlDoubleClosed = 5, 
  xlDoubleOpen = 4, 
  xlDoubleQuote = 1, 
  xlEntireChart = 20, 
  xlExcelMenus = 1, 
  xlExtended = 3, 
  xlFill = 5, 
  xlFirst = 0, 
  xlFloating = 5, 
  xlFormula = 5, 
  xlGeneral = 1, 
  xlGridline = 22, 
  xlIcons = 1, 
  xlImmediatePane = 12, 
  xlInteger = 2, 
  xlLast = 1, 
  xlLastCell = 11, 
  xlList1 = 10, 
  xlList2 = 11, 
  xlList3 = 12, 
  xlLocalFormat1 = 15, 
  xlLocalFormat2 = 16, 
  xlLong = 3, 
  xlLotusHelp = 2, 
  xlMacrosheetCell = 7, 
  xlMixed = 2, 
  xlMultiply = 4, 
  xlNarrow = 1, 
  xlNoDocuments = 3, 
  xlOpen = 2, 
  xlOutside = 3, 
  xlReference = 4, 
  xlSemiautomatic = 2, 
  xlShort = 1, 
  xlSingleQuote = 2, 
  xlStrict = 2, 
  xlSubtract = 3, 
  xlTextBox = 16, 
  xlTiled = 1, 
  xlTitleBar = 8, 
  xlToolbar = 1, 
  xlVisible = 12, 
  xlWatchPane = 11, 
  xlWide = 3, 
  xlWorkbookTab = 6, 
  xlWorksheet4 = 1, 
  xlWorksheetCell = 3, 
  xlWorksheetShort = 5, 
  xlAllExceptBorders = 6, 
  xlLeftToRight = 2, 
  xlTopToBottom = 1, 
  xlVeryHidden = 2, 
  xlDrawingObject = 14
} Constants;
//------------------------------------------------------------------------------
typedef enum XlHAlign
{
    xlHAlignCenter = (int)0xFFFFEFF4,
    xlHAlignCenterAcrossSelection = 7,
    xlHAlignDistributed = (int)0xFFFFEFEB,
    xlHAlignFill = 5,
    xlHAlignGeneral = 1,
    xlHAlignJustify = (int)0xFFFFEFDE,
    xlHAlignLeft = (int)0xFFFFEFDD,
    xlHAlignRight = (int)0xFFFFEFC8
} XlHAlign;
//------------------------------------------------------------------------------
typedef enum XlVAlign
{
    xlVAlignBottom = (int)0xFFFFEFF5,
    xlVAlignCenter = (int)0xFFFFEFF4,
    xlVAlignDistributed = (int)0xFFFFEFEB,
    xlVAlignJustify = (int)0xFFFFEFDE,
    xlVAlignTop = (int)0xFFFFEFC0
} XlVAlign;
//------------------------------------------------------------------------------
typedef enum XlBordersIndex
{
  xlInsideHorizontal = 12,
  xlInsideVertical = 11,
  xlDiagonalDown = 5,
  xlDiagonalUp = 6,
  xlEdgeBottom = 9,
  xlEdgeLeft = 7,
  xlEdgeRight = 10,
  xlEdgeTop = 8
} XlBordersIndex;
//------------------------------------------------------------------------------
typedef enum XlLineStyle
{
  xlContinuous = 1,
  xlDash = 0xFFFFEFED,
  xlDashDot = 4,
  xlDashDotDot = 5,
  xlDot = 0xFFFFEFEA,
  xlDouble = 0xFFFFEFE9,
  xlSlantDashDot = 13,
  xlLineStyleNone = 0xFFFFEFD2
} XlLineStyle;
//------------------------------------------------------------------------------
typedef enum XlBorderWeight
{
  xlHairline = 1,
  xlMedium = 0xFFFFEFD6,
  xlThick = 4,
  xlThin = 2
} XlBorderWeight;
//------------------------------------------------------------------------------
typedef enum XlColorIndex
{
  xlColorIndexAutomatic = 0xFFFFEFF7,
  xlColorIndexNone = 0xFFFFEFD2
} XlColorIndex;

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
}; // namespace Excel
//------------------------------------------------------------------------------

#endif